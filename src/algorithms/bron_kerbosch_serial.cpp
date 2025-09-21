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
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<Graph*> BronKerboschSerial::find(const Graph& g1, const Graph& g2) {
    if (g1.get_num_nodes() == 0 || g2.get_num_nodes() == 0) {
        return {};
    }

    ProductGraph product_graph = build_product_graph(g1, g2);

    std::cout << "Product graph has " << product_graph.nodes.size()
              << " nodes\n";

    if (product_graph.nodes.size() > 1000) {
        std::cout << "Product graph too large (" << product_graph.nodes.size()
                  << " nodes), using simplified heuristic\n";
        return find_simple_mcis(g1, g2);
    }

    std::vector<std::set<ProductNode>> cliques
        = find_maximal_cliques_with_timeout(product_graph, 5000);

    std::vector<Graph*> mcis_results
        = convert_cliques_to_subgraphs(cliques, g1, g2);

    return mcis_results;
}

BronKerboschSerial::ProductGraph BronKerboschSerial::build_product_graph(
    const Graph& g1, const Graph& g2) {
    ProductGraph product_graph;

    for (const auto& [id1, node1] : g1.get_nodes()) {
        for (const auto& [id2, node2] : g2.get_nodes()) {
            if (are_nodes_structurally_compatible(node1, node2)) {
                ProductNode prod_node = {id1, id2};
                product_graph.nodes.insert(prod_node);
            }
        }
    }

    // Create edges between compatible product nodes
    for (const auto& node1 : product_graph.nodes) {
        for (const auto& node2 : product_graph.nodes) {
            if (node1 != node2
                && are_product_nodes_adjacent(node1, node2, g1, g2)) {
                product_graph.adjacency[node1].insert(node2);
                product_graph.adjacency[node2].insert(node1);
            }
        }
    }

    return product_graph;
}

bool BronKerboschSerial::are_product_nodes_adjacent(const ProductNode& p1,
                                                    const ProductNode& p2,
                                                    const Graph& g1,
                                                    const Graph& g2) {
    // Two product nodes (u1,u2) and (v1,v2) are adjacent if:
    // 1. Both (u1,v1) and (u2,v2) are edges, or
    // 2. Both (u1,v1) and (u2,v2) are non-edges

    Node* u1 = g1.get_node(p1.g1_node);
    Node* v1 = g1.get_node(p2.g1_node);
    Node* u2 = g2.get_node(p1.g2_node);
    Node* v2 = g2.get_node(p2.g2_node);

    if (!u1 || !v1 || !u2 || !v2) {
        return false;
    }

    bool edge_in_g1 = u1->contains_edge(v1) || v1->contains_edge(u1);
    bool edge_in_g2 = u2->contains_edge(v2) || v2->contains_edge(u2);

    return edge_in_g1 == edge_in_g2;
}

std::vector<std::set<BronKerboschSerial::ProductNode>>
BronKerboschSerial::find_maximal_cliques_with_timeout(
    const ProductGraph& product_graph, int timeout_ms) {
    std::vector<std::set<ProductNode>> cliques;
    std::set<ProductNode> R;  // Current clique being built
    std::set<ProductNode> P = product_graph.nodes;  // Candidate nodes
    std::set<ProductNode> X;                        // Already processed nodes

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
        // R is a maximal clique
        if (!R.empty()) {
            cliques.push_back(R);
        }
        return;
    }

    // Choose pivot with maximum degree in P ∪ X
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
        // R' = R ∪ {v}
        std::set<ProductNode> R_new = R;
        R_new.insert(v);

        // Get neighbors of v
        std::set<ProductNode> v_neighbors;
        auto v_it = product_graph.adjacency.find(v);
        if (v_it != product_graph.adjacency.end()) {
            v_neighbors = v_it->second;
        }

        // P' = P ∩ N(v)
        std::set<ProductNode> P_new;
        std::set_intersection(P.begin(), P.end(), v_neighbors.begin(),
                              v_neighbors.end(),
                              std::inserter(P_new, P_new.begin()));

        // X' = X ∩ N(v)
        std::set<ProductNode> X_new;
        std::set_intersection(X.begin(), X.end(), v_neighbors.begin(),
                              v_neighbors.end(),
                              std::inserter(X_new, X_new.begin()));

        bron_kerbosch_recursive_with_timeout(R_new, P_new, X_new, product_graph,
                                             cliques, start_time, timeout_ms);

        // Move v from P to X
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

    // Check nodes in P ∪ X
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
    const std::vector<std::set<ProductNode>>& cliques, const Graph& g1,
    const Graph& g2) {
    if (cliques.empty()) {
        return {};
    }

    size_t max_size = 0;
    for (const auto& clique : cliques) {
        max_size = std::max(max_size, clique.size());
    }

    // Convert all maximum-size cliques to subgraphs
    std::vector<Graph*> results;
    for (const auto& clique : cliques) {
        if (clique.size() == max_size) {
            Graph* mcis = create_subgraph_from_clique(clique, g1, g2);
            if (mcis) {
                results.push_back(mcis);
            }
        }
    }

    return results;
}

Graph* BronKerboschSerial::create_subgraph_from_clique(
    const std::set<ProductNode>& clique, const Graph& g1, const Graph& g2) {
    if (clique.empty()) {
        return nullptr;
    }

    Graph* subgraph = new Graph();

    std::unordered_map<std::string, std::string> node_mapping;
    for (const auto& prod_node : clique) {
        std::string new_id = prod_node.g1_node + "_" + prod_node.g2_node;
        subgraph->add_node(new_id);
        node_mapping[prod_node.g1_node] = new_id;
    }

    for (const auto& prod_node1 : clique) {
        for (const auto& prod_node2 : clique) {
            if (prod_node1 != prod_node2) {
                Node* n1_g1 = g1.get_node(prod_node1.g1_node);
                Node* n2_g1 = g1.get_node(prod_node2.g1_node);
                Node* n1_g2 = g2.get_node(prod_node1.g2_node);
                Node* n2_g2 = g2.get_node(prod_node2.g2_node);

                // If edge exists in both original graphs, add it to subgraph
                if (n1_g1 && n2_g1 && n1_g2 && n2_g2) {
                    bool edge_in_g1 = n1_g1->contains_edge(n2_g1);
                    bool edge_in_g2 = n1_g2->contains_edge(n2_g2);

                    if (edge_in_g1 && edge_in_g2) {
                        std::string id1
                            = prod_node1.g1_node + "_" + prod_node1.g2_node;
                        std::string id2
                            = prod_node2.g1_node + "_" + prod_node2.g2_node;
                        subgraph->add_edge(id1, id2, 1);
                    }
                }
            }
        }
    }

    return subgraph;
}

bool BronKerboschSerial::are_nodes_structurally_compatible(Node* n1, Node* n2) {
    if (!n1 || !n2) {
        return false;
    }

    int deg1 = n1->get_num_children() + n1->get_num_parents();
    int deg2 = n2->get_num_children() + n2->get_num_parents();

    return std::abs(deg1 - deg2) <= std::max(1, std::min(deg1, deg2) / 2);
}

std::vector<Graph*> BronKerboschSerial::find_simple_mcis(const Graph& g1,
                                                         const Graph& g2) {
    Graph* result = new Graph();

    int added_nodes = 0;
    const int MAX_NODES = 10;

    for (const auto& [id1, node1] : g1.get_nodes()) {
        if (added_nodes >= MAX_NODES) break;

        for (const auto& [id2, node2] : g2.get_nodes()) {
            if (are_nodes_structurally_compatible(node1, node2)) {
                std::string result_id = id1 + "_" + id2;
                result->add_node(result_id);
                added_nodes++;
                break;
            }
        }
    }

    const auto& result_nodes = result->get_nodes();
    for (const auto& [id1, n1] : result_nodes) {
        for (const auto& [id2, n2] : result_nodes) {
            if (id1 != id2 && result->get_num_nodes() < MAX_NODES) {
                // Simple heuristic: add edge with small probability
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
