/**
 * @file graph.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef INCLUDE_MCIS_GRAPH_H_
#define INCLUDE_MCIS_GRAPH_H_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "mcis/errors.h"
#include "mcis/node.h"

constexpr int MVM_PARALLEL_THRESHOLD = 100;

enum class HaarWaveletGraph { BOTH, PRUNED_AVERAGE, PRUNED_COEFFICIENT };

/**
 * @class Graph
 * @brief Represents a directed graph using an adjacency list.
 * This class provides methods to manage nodes and edges, and to check if the
 * graph is a DAG.
 */
class Graph {
 private:
    /**
     * @brief Map of node IDs to Node pointers representing the graph's nodes.
     */
    std::unordered_map<std::string, Node*> nodes;

    /**
     * @brief Cache for frequently accessed data to improve performance.
     */
    mutable bool dag_cache_valid = false;
    mutable bool dag_cache_result = false;
    mutable int version = 0;

    /**
     * @brief Invalidates all caches when the graph is modified.
     */
    void invalidate_caches() const;

    /**
     * @brief Indicates if the graph is weighted (edges have weights).
     */
    bool is_weighted = false;

 public:
    /**
     * @brief Default constructor that initializes an empty graph.
     */
    Graph();

    /**
     * @brief Constructs a graph from a list of nodes.
     * @param node_list Vector of Node objects to initialize the graph with.
     */
    explicit Graph(const std::vector<Node>& node_list);

    /**
     * @brief Copy constructor.
     * @param other Graph to copy from.
     */
    Graph(const Graph& other);

    /**
     * @brief Copy assignment operator.
     * @param other Graph to copy from.
     * @return Reference to the assigned Graph.
     */
    Graph& operator=(const Graph& other);

    /**
     * @brief Move constructor.
     * @param other Graph to move from.
     */
    Graph(Graph&& other) noexcept;

    /**
     * @brief Move assignment operator.
     * @param other Graph to move from.
     * @return Reference to the assigned Graph.
     */
    Graph& operator=(Graph&& other) noexcept;

    /**
     * @brief Destructor that cleans up dynamically allocated nodes.
     */
    ~Graph();

    /**
     * @brief Checks if the graph is a Directed Acyclic Graph (DAG).
     * @return True if the graph is a DAG, false otherwise.
     */
    bool is_dag();

    /**
     * @brief Prints the graph as node:[adjacency list].
     */
    void print_graph() const;

    /**
     * @brief Adds a node with the given ID to the graph.
     * @param id Unique identifier for the new node.
     * @return An optional error if the node already exists.
     */
    std::optional<mcis::GraphError> add_node(const std::string& id);

    /**
     * @brief Adds multiple nodes with the given IDs to the graph.
     * @param ids Vector of unique identifiers for the new nodes.
     * @return An optional error if any node already exists.
     */
    std::optional<mcis::GraphError> add_node_set(
        const std::vector<std::string>& ids);

    /**
     * @brief Removes the node with the given ID from the graph.
     * @param id Unique identifier of the node to remove.
     * @return An optional error if the node does not exist.
     */
    std::optional<mcis::GraphError> remove_node(const std::string& id);

    /**
     * @brief Adds a directed edge from one node to another with a specified
     * weight.
     * @param from_id ID of the source node.
     * @param to_id ID of the destination node.
     * @param weight Weight of the edge.
     * @return An optional error if either node does not exist or the edge
     * already exists.
     */
    std::optional<mcis::GraphError> add_edge(const std::string& from_id,
                                             const std::string& to_id,
                                             int weight);

    /**
     * @brief Adds directed edges from one node to multiple other nodes with a
     * specified weight.
     * @param from_id ID of the source node.
     * @param to_ids Vector of IDs of the children nodes.
     * @param weights Vector of weights of the edges, defaults to 0 if vector is
     * empty.
     * @return An optional error if the source node does not exist or any edge
     * already exists.
     */
    std::optional<mcis::GraphError> add_edge_set(
        const std::string& from_id, const std::vector<std::string>& to_ids,
        const std::vector<int>& weights = {});

    /**
     * @brief Removes the directed edge from one node to another.
     * @param from_id ID of the source node.
     * @param to_id ID of the destination node.
     * @return An optional error if either node does not exist or the edge does
     * not exist.
     */
    std::optional<mcis::GraphError> remove_edge(const std::string& from_id,
                                                const std::string& to_id);

    /**
     * @brief Changes the weight of the edge from one node to another.
     * @param from_id ID of the source node.
     * @param to_id ID of the destination node.
     * @param new_weight New weight for the edge.
     * @return An optional error if either node does not exist or the edge does
     * not exist.
     */
    std::optional<mcis::GraphError> change_edge_weight(
        const std::string& from_id, const std::string& to_id, int new_weight);

    /**
     * @brief Changes the tag of the node with the given ID.
     * @param id Unique identifier of the node.
     * @param new_tag New tag for the node.
     * @return An optional error if the node does not exist.
     */
    std::optional<mcis::GraphError> set_node_tag(const std::string& id,
                                                 int new_tag);

    /**
     * @brief Retrieves the node identified by the given ID
     * @param id Unique identifier of the node to retrieve.
     * @return Pointer to the Node if found, nullptr otherwise.
     */
    Node* get_node(const std::string& id) const;

    /**
     * @brief Retrieves the number of nodes in the graph.
     * @return The number of nodes.
     */
    [[nodiscard]]
    int get_num_nodes() const;

    /**
     * @brief Retrieves the map of nodes in the graph.
     * @return Constant reference to the unordered map of node IDs to Node
     * pointers.
     */
    [[nodiscard]]
    const std::unordered_map<std::string, Node*>& get_nodes() const;

    /**
     * @brief Equality operator to compare two graphs.
     * @return True if the graphs are equal, false otherwise.
     */
    bool operator==(const Graph& other) const;

    /**
     * @brief Stream insertion operator to print the graph.
     * @param os Output stream.
     * @param graph Graph to print.
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Graph& graph);

    /**
     * @brief Generates a DOT file representing the graph for visualization.
     * @param filename Name of the output graph name
     */
    void generate_diagram_file(const std::string& graph_name) const;

    /**
     * @brief Bulk operations for better performance with large datasets.
     */

    /**
     * @brief Removes multiple nodes efficiently in a single operation.
     * @param node_ids Vector of node IDs to remove.
     * @return Number of nodes successfully removed.
     */
    int remove_nodes_bulk(const std::vector<std::string>& node_ids);

    /**
     * @brief Reserves memory for expected number of nodes to reduce
     * allocations.
     * @param expected_size Expected number of nodes.
     */
    void reserve_nodes(size_t expected_size);

    /**
     * @brief Gets the current version/revision of the graph for change
     * tracking.
     * @return Current version number.
     */
    [[nodiscard]]
    int get_version() const {
        return version;
    }

    /**
     * @brief Static factory method for MVM dataflow CDAG creation from actual
     * matrix and vector
     * @param mat 2D vector representing the matrix
     * @param vec 1D vector representing the vector
     * @return Graph representing the MVM dataflow CDAG
     */
    [[nodiscard]]
    static Graph create_mvm_graph_from_mat_vec(
        const std::vector<std::vector<std::string>>& mat,
        const std::vector<std::string>& vec);

    /**
     * @brief Static factory method for MVM dataflow CDAG creation from matrix
     * dimensions
     * @param m Number of rows in the matrix
     * @param n Number of columns in the matrix
     * @return Graph representing the MVM dataflow CDAG
     */
    [[nodiscard]]
    static Graph create_mvm_graph_from_dimensions(int m, int n);

    /**
     * @brief Static factory method for Haar wavelet transform CDAG creation
     * from dimensions
     * @param n, s.t. n is an element of {k * 2^d | k is an integer >= 1, d is
     * an integer >= 0}
     * @param d level of DWT graph (integer >= 1)
     * @return Graph representing the Haar wavelet transform CDAG
     */
    [[nodiscard]]
    static Graph create_haar_wavelet_transform_graph_from_dimensions(int n,
                                                                     int d,
                                                                     int k = 1);

    /**
     * @brief Static factory method for Haar wavelet transform CDAG creation
     * from signal
     * @param signal Input signal as a vector of doubles
     * @return Graph representing the Haar wavelet transform CDAG
     */
    [[nodiscard]]
    static std::vector<Graph> create_haar_wavelet_transform_graph_from_signal(
        const std::vector<double>& signal, HaarWaveletGraph type
                                           = HaarWaveletGraph::BOTH);
};

#endif  // INCLUDE_MCIS_GRAPH_H_
