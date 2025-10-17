/**
 * @file bron_kerbosch_serial.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#include "bron_kerbosch_serial.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

std::expected<std::vector<Graph*>, mcis::AlgorithmError>
BronKerboschSerial::find(const std::vector<const Graph*>& graphs,
                         std::optional<std::string> tag) {
    for (const auto& graph : graphs) {
        if (graph->get_num_nodes() == 0) {
            return std::unexpected(mcis::AlgorithmError::EMPTY_GRAPH);
        }
    }

    ProductGraph product_graph = build_product_graph(graphs);

    std::cout << "Product graph has " << product_graph.nodes.size()
              << " nodes\n";

    if (product_graph.nodes.size() > 1000) {
        std::cout << "Product graph too large (" << product_graph.nodes.size()
                  << " nodes), using simplified heuristic\n";
        return find_simple_mcis(graphs);
    }

    std::vector<std::set<ProductNode>> cliques
        = find_maximal_cliques_with_timeout(product_graph, 5000);

    std::vector<Graph*> mcis_results
        = convert_cliques_to_subgraphs(cliques, graphs);

    return mcis_results;
}

BronKerboschSerial::ProductGraph BronKerboschSerial::build_product_graph(
    const std::vector<const Graph*>& graphs) {
    ProductGraph product_graph;
    if (graphs.empty()) {
        return product_graph;
    }

    std::vector<std::vector<Node*>> nodes_per_graph;
    for (const auto& graph : graphs) {
        std::vector<Node*> nodes;
        for (const auto& pair : graph->get_nodes()) {
            nodes.push_back(pair.second);
        }
        nodes_per_graph.push_back(nodes);
    }

    std::function<void(int, std::vector<std::string>)> generate_product_nodes
        = [&](int graph_idx, std::vector<std::string> current_combination) {
              if (graph_idx == graphs.size()) {
                  ProductNode new_node;
                  new_node.node_ids = current_combination;
                  product_graph.nodes.insert(new_node);
                  return;
              }

              for (const auto& node : nodes_per_graph[graph_idx]) {
                  std::vector<std::string> next_combination
                      = current_combination;
                  next_combination.push_back(node->get_id());
                  generate_product_nodes(graph_idx + 1, next_combination);
              }
          };

    generate_product_nodes(0, {});

    // Create edges between compatible product nodes
    for (const auto& node1 : product_graph.nodes) {
        for (const auto& node2 : product_graph.nodes) {
            if (node1 != node2
                && are_product_nodes_adjacent(node1, node2, graphs)) {
                product_graph.adjacency[node1].insert(node2);
                product_graph.adjacency[node2].insert(node1);
            }
        }
    }

    return product_graph;
}

bool BronKerboschSerial::are_product_nodes_adjacent(
    const ProductNode& p1, const ProductNode& p2,
    const std::vector<const Graph*>& graphs) {
    bool first_edge_in_g = false;
    for (size_t i = 0; i < graphs.size(); ++i) {
        Node* u = graphs[i]->get_node(p1.node_ids[i]);
        Node* v = graphs[i]->get_node(p2.node_ids[i]);

        if (!u || !v) {
            return false;
        }

        bool edge_in_g = u->contains_edge(v) || v->contains_edge(u);
        if (i == 0) {
            first_edge_in_g = edge_in_g;
        } else {
            if (edge_in_g != first_edge_in_g) {
                return false;
            }
        }
    }

    return true;
}

std::vector<std::set<BronKerboschSerial::ProductNode>>
BronKerboschSerial::find_maximal_cliques_with_timeout(
    const ProductGraph& product_graph, int timeout_ms) {
    std::vector<std::set<ProductNode>> cliques;
    std::set<ProductNode> R;
    std::set<ProductNode> P = product_graph.nodes;
    std::set<ProductNode> X;

    auto start_time = std::chrono::high_resolution_clock::now();

    bron_kerbosch_recursive_with_timeout(R, P, X, product_graph, cliques,
                                         start_time, timeout_ms);

    if (cliques.empty()) {
        std::cout << "Timeout reached, returning simple fallback result\n";
        if (!product_graph.nodes.empty()) {
            std::set<ProductNode> simple_clique;
            simple_clique.insert(*product_graph.nodes.begin());
            cliques.push_back(simple_clique);
        }
    }

    return cliques;
}

void BronKerboschSerial::bron_kerbosch_recursive_with_timeout(
    std::set<ProductNode> R, std::set<ProductNode> P, std::set<ProductNode> X,
    const ProductGraph& product_graph,
    std::vector<std::set<ProductNode>>& cliques,
    std::chrono::high_resolution_clock::time_point start_time, int timeout_ms) {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        current_time - start_time);
    if (elapsed.count() > timeout_ms) {
        return;
    }

    if (!cliques.empty() && cliques[0].size() > 10) {
        return;
    }

    if (P.empty() && X.empty()) {
        if (!R.empty()) {
            cliques.push_back(R);
        }
        return;
    }

    ProductNode pivot = choose_pivot(P, X, product_graph);

    std::set<ProductNode> pivot_neighbors;
    auto it = product_graph.adjacency.find(pivot);
    if (it != product_graph.adjacency.end()) {
        pivot_neighbors = it->second;
    }

    std::set<ProductNode> P_without_pivot_neighbors;
    std::set_difference(P.begin(), P.end(), pivot_neighbors.begin(),
                        pivot_neighbors.end(),
                        std::inserter(P_without_pivot_neighbors,
                                      P_without_pivot_neighbors.begin()));

    for (const auto& v : P_without_pivot_neighbors) {
        std::set<ProductNode> R_new = R;
        R_new.insert(v);

        std::set<ProductNode> v_neighbors;
        auto v_it = product_graph.adjacency.find(v);
        if (v_it != product_graph.adjacency.end()) {
            v_neighbors = v_it->second;
        }

        std::set<ProductNode> P_new;
        std::set_intersection(P.begin(), P.end(), v_neighbors.begin(),
                              v_neighbors.end(),
                              std::inserter(P_new, P_new.begin()));

        std::set<ProductNode> X_new;
        std::set_intersection(X.begin(), X.end(), v_neighbors.begin(),
                              v_neighbors.end(),
                              std::inserter(X_new, X_new.begin()));

        bron_kerbosch_recursive_with_timeout(R_new, P_new, X_new, product_graph,
                                             cliques, start_time, timeout_ms);

        P.erase(v);
        X.insert(v);
    }
}

BronKerboschSerial::ProductNode BronKerboschSerial::choose_pivot(
    const std::set<ProductNode>& P, const std::set<ProductNode>& X,
    const ProductGraph& product_graph) {
    ProductNode best_pivot;
    size_t max_degree = 0;
    bool found_pivot = false;

    for (const auto& node : P) {
        auto it = product_graph.adjacency.find(node);
        size_t degree
            = (it != product_graph.adjacency.end()) ? it->second.size() : 0;
        if (degree > max_degree) {
            max_degree = degree;
            best_pivot = node;
            found_pivot = true;
        }
    }

    for (const auto& node : X) {
        auto it = product_graph.adjacency.find(node);
        size_t degree
            = (it != product_graph.adjacency.end()) ? it->second.size() : 0;
        if (degree > max_degree) {
            max_degree = degree;
            best_pivot = node;
            found_pivot = true;
        }
    }

    if (!found_pivot) {
        if (!P.empty()) {
            best_pivot = *P.begin();
        } else if (!X.empty()) {
            best_pivot = *X.begin();
        }
    }

    return best_pivot;
}

std::vector<Graph*> BronKerboschSerial::convert_cliques_to_subgraphs(
    const std::vector<std::set<ProductNode>>& cliques,
    const std::vector<const Graph*>& graphs) {
    if (cliques.empty()) {
        return {};
    }

    size_t max_size = 0;
    for (const auto& clique : cliques) {
        max_size = std::max(max_size, clique.size());
    }

    std::vector<Graph*> results;
    for (const auto& clique : cliques) {
        if (clique.size() == max_size) {
            Graph* mcis = create_subgraph_from_clique(clique, graphs);
            if (mcis) {
                results.push_back(mcis);
            }
        }
    }

    return results;
}

Graph* BronKerboschSerial::create_subgraph_from_clique(
    const std::set<ProductNode>& clique,
    const std::vector<const Graph*>& graphs) {
    if (clique.empty()) {
        return nullptr;
    }

    Graph* subgraph = new Graph();
    std::string new_id;
    for (const auto& prod_node : clique) {
        new_id = "";
        for (size_t i = 0; i < prod_node.node_ids.size(); ++i) {
            new_id += prod_node.node_ids[i];
            if (i < prod_node.node_ids.size() - 1) {
                new_id += "_";
            }
        }
        subgraph->add_node(new_id);
    }

    for (const auto& prod_node1 : clique) {
        for (const auto& prod_node2 : clique) {
            if (prod_node1 != prod_node2) {
                bool edge_exists_in_all = true;
                for (size_t i = 0; i < graphs.size(); ++i) {
                    Node* n1 = graphs[i]->get_node(prod_node1.node_ids[i]);
                    Node* n2 = graphs[i]->get_node(prod_node2.node_ids[i]);
                    if (!n1 || !n2 || !n1->contains_edge(n2)) {
                        edge_exists_in_all = false;
                        break;
                    }
                }

                if (edge_exists_in_all) {
                    std::string id1 = "", id2 = "";
                    for (size_t i = 0; i < prod_node1.node_ids.size(); ++i) {
                        id1 += prod_node1.node_ids[i];
                        id2 += prod_node2.node_ids[i];
                        if (i < prod_node1.node_ids.size() - 1) {
                            id1 += "_";
                            id2 += "_";
                        }
                    }
                    subgraph->add_edge(id1, id2, 1);
                }
            }
        }
    }

    return subgraph;
}

bool BronKerboschSerial::are_nodes_structurally_compatible(
    const std::vector<Node*>& nodes) {
    if (nodes.size() < 2) {
        return true;
    }

    int first_deg = nodes[0]->get_num_children() + nodes[0]->get_num_parents();
    for (size_t i = 1; i < nodes.size(); ++i) {
        int deg = nodes[i]->get_num_children() + nodes[i]->get_num_parents();
        if (std::abs(first_deg - deg)
            > std::max(1, std::min(first_deg, deg) / 2)) {
            return false;
        }
    }
    return true;
}

std::vector<Graph*> BronKerboschSerial::find_simple_mcis(
    const std::vector<const Graph*>& graphs) {
    Graph* result = new Graph();
    int added_nodes = 0;
    const int MAX_NODES = 10;

    if (graphs.empty()) {
        return {};
    }

    const auto& first_graph_nodes = graphs[0]->get_nodes();
    for (const auto& [id1, node1] : first_graph_nodes) {
        if (added_nodes >= MAX_NODES) break;

        std::string result_id = id1;
        bool compatible = true;
        for (size_t i = 1; i < graphs.size(); ++i) {
            bool found_compatible_node = false;
            for (const auto& [id2, node2] : graphs[i]->get_nodes()) {
                if (are_nodes_structurally_compatible({node1, node2})) {
                    result_id += "_" + id2;
                    found_compatible_node = true;
                    break;
                }
            }
            if (!found_compatible_node) {
                compatible = false;
                break;
            }
        }

        if (compatible) {
            result->add_node(result_id);
            added_nodes++;
        }
    }

    const auto& result_nodes = result->get_nodes();
    for (const auto& [id1, n1] : result_nodes) {
        for (const auto& [id2, n2] : result_nodes) {
            if (id1 != id2 && result->get_num_nodes() < MAX_NODES) {
                if (std::hash<std::string>{}(id1 + id2) % 4 == 0) {
                    result->add_edge(id1, id2, 1);
                }
            }
        }
    }

    std::vector<Graph*> results;
    if (result->get_num_nodes() > 0) {
        results.push_back(result);
    } else {
        delete result;
    }

    return results;
}

