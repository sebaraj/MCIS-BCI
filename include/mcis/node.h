/**
 * @file node.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef INCLUDE_MCIS_NODE_H_
#define INCLUDE_MCIS_NODE_H_

#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

#include "mcis/errors.h"

/**
 * @class Node
 * @brief Represents a node in a directed graph with edges to its children.
 * This class provides methods to manage edges and retrieve node information.
 */
class Node {
 private:
    /**
     * @brief Unique identifier for the node.
     */
    std::string id;

    /**
     * @brief Number of parent nodes (incoming edges).
     */
    int num_parents;

    /**
     * @brief Number of child nodes (outgoing edges).
     */
    int num_children;

    /**
     * @brief Map of child nodes to the weights of the directed edges connecting
     * to them.
     */
    std::unordered_map<Node*, int> children;

    /**
     * @brief Map of parent nodes to the weights of the directed edges
     * connecting from them.
     */
    std::unordered_map<Node*, int> parents;

    /**
     * @brief String tag for grouping nodes.
     */
    std::string tag;

 public:
    /**
     * @brief Constructs a Node with a given ID and optional parent/child
     * counts.
     * @param id Unique identifier for the node.
     */
    explicit Node(const std::string& id);

    /**
     * @brief Copy constructor.
     * @param other Node to copy from.
     */
    Node(const Node& other);

    /**
     * @brief Copy assignment operator.
     * @param other Node to copy from.
     * @return Reference to the assigned Node.
     */
    Node& operator=(const Node& other);

    /**
     * @brief Move constructor.
     * @param other Node to move from.
     */
    Node(Node&& other) noexcept;

    /**
     * @brief Move assignment operator.
     * @param other Node to move from.
     * @return Reference to the assigned Node.
     */
    Node& operator=(Node&& other) noexcept;

    /**
     * @brief Destructor.
     */
    ~Node();

    /**
     * @brief Retrieves the node's unique identifier.
     * @return The node's ID as a string.
     */
    [[nodiscard]]
    std::string get_id() const;

    /**
     * @brief Retrieves the node's tag.
     * @return The node's tag as a string.
     */
    [[nodiscard]]
    std::string get_tag() const;

    /**
     * @brief Sets the node's tag.
     * @param new_tag The new tag for the node.
     */
    void set_tag(const std::string& new_tag);

    /**
     * @brief Retrieves the number of parent nodes (incoming edges).
     * @return The number of parent nodes.
     */
    [[nodiscard]]
    int get_num_parents() const;

    /**
     * @brief Retrieves the number of child nodes (outgoing edges).
     * @return The number of child nodes.
     */
    [[nodiscard]]
    int get_num_children() const;

    /**
     * @brief Adds a directed edge to a neighbor node with a specified weight.
     * @param neighbor Pointer to the neighbor node.
     * @param weight Weight of the edge.
     * @return An optional error if the edge already exists or is a self-loop.
     */
    std::optional<mcis::NodeError> add_edge(Node* neighbor, int weight);

    /**
     * @brief Removes the directed edge to a neighbor node.
     * @param neighbor Pointer to the neighbor node.
     * @return An optional error if the edge does not exist.
     */
    std::optional<mcis::NodeError> remove_edge(Node* neighbor);

    /**
     * @brief Adds a directed edge from a parent node with a specified weight.
     * @param parent Pointer to the parent node.
     * @param weight Weight of the edge.
     * @return An optional error if the edge already exists or is a self-loop.
     */
    std::optional<mcis::NodeError> add_parent(Node* parent, int weight);

    /**
     * @brief Removes the directed edge from a parent node.
     * @param parent Pointer to the parent node.
     * @return An optional error if the edge does not exist.
     */
    std::optional<mcis::NodeError> remove_parent(Node* parent);

    /**
     * @brief Changes the weight of the edge to a neighbor node.
     * @param neighbor Pointer to the neighbor node.
     * @param new_weight New weight for the edge.
     * @return An optional error if the edge does not exist.
     */
    std::optional<mcis::NodeError> change_edge_weight(Node* neighbor,
                                                      int new_weight);

    /**
     * @brief Checks if there is a directed edge to a neighbor node.
     * @param neighbor Pointer to the neighbor node.
     * @return True if the edge exists, false otherwise.
     */
    [[nodiscard]]
    bool contains_edge(Node* neighbor) const;

    /**
     * @brief Checks if the given node is a parent of this node.
     * @param parent_id The ID of the potential parent node.
     * @return True if the given node is a parent, false otherwise.
     */
    [[nodiscard]]
    bool check_parent(const std::string& parent_id) const;

    /**
     * @brief Checks if the node is a source (no incoming edges).
     * @return True if the node is a source, false otherwise.
     */
    [[nodiscard]]
    bool is_source() const;

    /**
     * @brief Checks if the node is a sink (no outgoing edges).
     * @return True if the node is a sink, false otherwise.
     */
    [[nodiscard]]
    bool is_sink() const;

    /**
     * @brief Equality operator to compare two nodes based on their ID, parent
     * count, child count, and edges.
     * @return True if the nodes are equal, false otherwise.
     */
    bool operator==(const Node& other) const;

    /**
     * @brief Compares two nodes based on their unique identifiers.
     * @param other The node to compare with.
     * @return True if the nodes have the same ID, false otherwise.
     */
    [[nodiscard]]
    bool same_id(const Node& other) const;

    /**
     * @brief Provides access to the children map for external use (e.g., in
     * Graph class).
     * @return Reference to the children map.
     */
    [[nodiscard]]
    const std::unordered_map<Node*, int>& get_children() const;

    /**
     * @brief operator to print node and its children
     * @param os output stream
     * @param node node to print
     * @return output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const Node& node);

    /**
     * @brief prints node with all elements (id, number of parents, number of
     * children, and children ids with weights)
     * @return void, prints to std::cout
     */
    void print_full() const;
};

#endif  // INCLUDE_MCIS_NODE_H_
