/**
 * @file bron_kerbosch_serial.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef SRC_ALGORITHMS_BRON_KERBOSCH_SERIAL_H_
#define SRC_ALGORITHMS_BRON_KERBOSCH_SERIAL_H_

#include <chrono>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "mcis/graph.h"
#include "mcis/mcis_finder.h"

/**
 * @class BronKerboschSerial
 *
 * Implements the Bron-Kerbosch algorithm for finding maximal cliques in an
 * undirected graph. This is a serial implementation without optimizations.
 * Uses the product graph approach for MCIS computation.
 */
class BronKerboschSerial : public MCISFinder {
 public:
    /**
     * @brief Represents a node in the product graph formed by N input graphs.
     * Each product node corresponds to a tuple of nodes, one from each input
     * graph.
     */
    struct ProductNode {
        std::vector<std::string> node_ids;

        bool operator<(const ProductNode& other) const {
            return node_ids < other.node_ids;
        }

        bool operator==(const ProductNode& other) const {
            return node_ids == other.node_ids;
        }

        bool operator!=(const ProductNode& other) const {
            return !(*this == other);
        }
    };

    /**
     * @brief Hash function for ProductNode to enable use in unordered
     * containers.
     */
    struct ProductNodeHash {
        std::size_t operator()(const ProductNode& pn) const {
            std::size_t seed = 0;
            for (const auto& id : pn.node_ids) {
                seed ^= std::hash<std::string>{}(id) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    /**
     * @brief Represents the product graph structure used for MCIS computation.
     */
    struct ProductGraph {
        std::set<ProductNode> nodes;
        std::unordered_map<ProductNode, std::set<ProductNode>, ProductNodeHash>
            adjacency;
    };

    /**
     * @brief Finds the Maximum Common Induced Subgraph (MCIS) between a set of
     * graphs using the Bron-Kerbosch algorithm.
     * @param graphs A vector of pointers to the input graphs.
     * @param tag An optional tag to filter nodes by.
     * @return A vector of pointers to Graph objects representing the found MCIS
     * results, or an error if the graphs are empty.
     */
    std::expected<std::vector<Graph*>, mcis::AlgorithmError> find(
        const std::vector<const Graph*>& graphs,
        std::optional<std::string> tag) override;

 private:
    /**
     * @brief Constructs the product graph from a set of input graphs.
     * @param graphs A vector of pointers to the input graphs.
     * @return The product graph structure.
     */
    ProductGraph build_product_graph(const std::vector<const Graph*>& graphs);

    /**
     * @brief Determines if two product nodes should be adjacent in the product
     * graph.
     * @param p1 First product node.
     * @param p2 Second product node.
     * @param graphs A vector of pointers to the input graphs.
     * @return True if the product nodes should be adjacent.
     */
    bool are_product_nodes_adjacent(const ProductNode& p1,
                                    const ProductNode& p2,
                                    const std::vector<const Graph*>& graphs);

    /**
     * @brief Finds all maximal cliques in the product graph using Bron-Kerbosch
     * with timeout.
     * @param product_graph The product graph to search.
     * @param timeout_ms Maximum time to spend searching in milliseconds.
     * @return Vector of cliques (each clique is a set of ProductNodes).
     */
    std::vector<std::set<ProductNode>> find_maximal_cliques_with_timeout(
        const ProductGraph& product_graph, int timeout_ms);

    /**
     * @brief Recursive implementation of the Bron-Kerbosch algorithm with
     * timeout.
     * @param R Current clique being built.
     * @param P Candidate nodes that can extend the clique.
     * @param X Already processed nodes.
     * @param product_graph The product graph being searched.
     * @param cliques Output vector to store found cliques.
     * @param start_time Start time for timeout calculation.
     * @param timeout_ms Timeout in milliseconds.
     */
    void bron_kerbosch_recursive_with_timeout(
        std::set<ProductNode> R, std::set<ProductNode> P,
        std::set<ProductNode> X, const ProductGraph& product_graph,
        std::vector<std::set<ProductNode>>& cliques,
        std::chrono::high_resolution_clock::time_point start_time,
        int timeout_ms);

    /**
     * @brief Chooses a pivot node to optimize the Bron-Kerbosch algorithm.
     * @param P Candidate nodes.
     * @param X Already processed nodes.
     * @param product_graph The product graph.
     * @return The chosen pivot node.
     */
    ProductNode choose_pivot(const std::set<ProductNode>& P,
                             const std::set<ProductNode>& X,
                             const ProductGraph& product_graph);

    /**
     * @brief Converts maximal cliques to actual induced subgraphs.
     * @param cliques The cliques found in the product graph.
     * @param graphs A vector of pointers to the input graphs.
     * @return Vector of MCIS graphs.
     */
    std::vector<Graph*> convert_cliques_to_subgraphs(
        const std::vector<std::set<ProductNode>>& cliques,
        const std::vector<const Graph*>& graphs);

    /**
     * @brief Creates a subgraph from a single clique.
     * @param clique The clique to convert.
     * @param graphs A vector of pointers to the input graphs.
     * @return Pointer to the created subgraph.
     */
    Graph* create_subgraph_from_clique(const std::set<ProductNode>& clique,
                                       const std::vector<const Graph*>& graphs);

    /**
     * @brief Checks if a set of nodes are structurally compatible.
     * @param nodes A vector of nodes to check.
     * @return True if nodes are compatible.
     */
    bool are_nodes_structurally_compatible(const std::vector<Node*>& nodes);

    /**
     * @brief Simple heuristic MCIS finder for large graphs.
     * @param graphs A vector of pointers to the input graphs.
     * @return Vector of simple MCIS results.
     */
    std::vector<Graph*> find_simple_mcis(const std::vector<const Graph*>& graphs);
};

#endif  // SRC_ALGORITHMS_BRON_KERBOSCH_SERIAL_H_
