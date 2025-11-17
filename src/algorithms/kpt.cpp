/**
 * @file kpt.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#include "mcis/algorithms/kpt.h"

#include <algorithm>
#include <functional>
#include <map>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

std::expected<std::vector<Graph*>, mcis::AlgorithmError> KPT::find(
    const std::vector<const Graph*>& graphs, std::optional<std::string> tag) {
    if (graphs.empty()) {
        return std::unexpected(mcis::AlgorithmError::EMPTY_GRAPH);
    }
    for (const auto& graph : graphs) {
        if (graph->get_num_nodes() == 0) {
            return std::unexpected(mcis::AlgorithmError::EMPTY_GRAPH);
        }
    }

    EdgeSet F;
    std::vector<std::vector<Node*>> nodes_per_graph;
    for (const auto& graph : graphs) {
        std::vector<Node*> nodes;
        for (const auto& pair : graph->get_nodes()) {
            if (!tag || (tag && pair.second->get_tag() == *tag)) {
                nodes.push_back(pair.second);
            }
        }
        nodes_per_graph.push_back(nodes);
    }

    std::function<void(int, std::vector<std::string>)> generate_hyperedges
        = [&](int graph_idx, std::vector<std::string> current_combination) {
              if (graph_idx == graphs.size()) {
                  F.insert({current_combination});
                  return;
              }

              for (const auto& node : nodes_per_graph[graph_idx]) {
                  std::vector<std::string> next_combination
                      = current_combination;
                  next_combination.push_back(node->get_id());
                  generate_hyperedges(graph_idx + 1, next_combination);
              }
          };

    generate_hyperedges(0, {});

    WeightMap w;
    for (const auto& edge : F) {
        w[edge] = 1.0;
    }

    EdgeSet matching = kPCM_Match(F, w, graphs);

    Graph* result_graph = new Graph();
    for (const auto& hyperedge : matching) {
        std::string node_id = "";
        for (size_t i = 0; i < hyperedge.node_ids.size(); ++i) {
            node_id += hyperedge.node_ids[i]
                       + (i == hyperedge.node_ids.size() - 1 ? "" : "_");
        }
        result_graph->add_node(node_id);
    }

    std::vector<Graph*> results;
    results.push_back(result_graph);

    return results;
}

KPT::EdgeSet KPT::kPCM_Match(EdgeSet F, WeightMap w,
                             const std::vector<const Graph*>& graphs) {
    if (F.empty()) {
        return {};
    }

    // 1. Compute optimal fractional solution x
    // Placeholder: assign uniform fractional values
    std::map<Hyperedge, double> x;
    double total_weight = 0;
    for (const auto& edge : F) total_weight += w[edge];
    if (total_weight == 0) return {};

    for (const auto& edge : F) {
        x[edge] = w[edge] / total_weight;
    }

    // 3. Handle Zero-Value Edges
    EdgeSet F_nonzero;
    for (const auto& e : F) {
        if (x[e] > 1e-9) {  // Use a small epsilon for floating point comparison
            F_nonzero.insert(e);
        }
    }

    if (F_nonzero.size() < F.size()) {
        return kPCM_Match(F_nonzero, w, graphs);
    }

    // 4. Find Low-Conflict Edge
    // Placeholder for alpha
    double alpha = 2.0 * graphs.size();

    Hyperedge selected_edge;
    bool found_low_conflict_edge = false;

    for (const auto& e : F) {
        double conflict_sum = 0;
        for (const auto& q : F) {
            if (are_conflicting(e, q, graphs)) {
                conflict_sum += x[q];
            }
        }
        if (conflict_sum <= alpha) {
            selected_edge = e;
            found_low_conflict_edge = true;
            break;
        }
    }

    if (!found_low_conflict_edge) {
        // Fallback: if no such edge is found (should not happen based on
        // theory) To prevent infinite recursion, we can just pick one edge.
        if (!F.empty()) {
            selected_edge = *F.begin();
        } else {
            return {};
        }
    }

    // 5. Perform Local Ratio Step
    WeightMap w_hat;
    double w_e = w[selected_edge];
    for (const auto& f : F) {
        if (are_conflicting(selected_edge, f, graphs)) {
            w_hat[f] = std::min(w[f], w_e);
        } else {
            w_hat[f] = 0.0;
        }
    }

    WeightMap w_new;
    for (const auto& f : F) {
        w_new[f] = w[f] - w_hat[f];
    }

    EdgeSet M_prime = kPCM_Match(F, w_new, graphs);

    // 6. Update Matching
    bool conflicts_with_M_prime = false;
    for (const auto& m_edge : M_prime) {
        if (are_conflicting(selected_edge, m_edge, graphs)) {
            conflicts_with_M_prime = true;
            break;
        }
    }

    if (!conflicts_with_M_prime) {
        M_prime.insert(selected_edge);
        return M_prime;
    } else {
        return M_prime;
    }
}

bool KPT::are_conflicting(const Hyperedge& p1, const Hyperedge& p2,
                          const std::vector<const Graph*>& graphs) {
    if (p1 == p2) return true;
    for (size_t i = 0; i < graphs.size(); ++i) {
        if (is_reachable(graphs[i], p1.node_ids[i], p2.node_ids[i])
            || is_reachable(graphs[i], p2.node_ids[i], p1.node_ids[i])) {
            return true;
        }
    }
    return false;
}

bool KPT::is_reachable(const Graph* g, const std::string& start_node_id,
                       const std::string& end_node_id) {
    if (start_node_id == end_node_id) return true;

    std::queue<std::string> q;
    q.push(start_node_id);
    std::unordered_set<std::string> visited;
    visited.insert(start_node_id);

    while (!q.empty()) {
        std::string u_id = q.front();
        q.pop();

        Node* u_node = g->get_node(u_id);
        if (!u_node) continue;

        for (const auto& edge : u_node->get_children()) {
            Node* v_node = edge.first;
            std::string v_id = v_node->get_id();
            if (v_id == end_node_id) return true;
            if (visited.find(v_id) == visited.end()) {
                visited.insert(v_id);
                q.push(v_id);
            }
        }
    }
    return false;
}
