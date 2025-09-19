/**
 * @file
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 */

#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>
#include <unordered_map>

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
     * @brief Map of child nodes to the weights of the directed edges connecting to them.
     */
    std::unordered_map<Node*, int> children;

public:
    /**
     * @brief Constructs a Node with a given ID and optional parent/child counts.
     * @param id Unique identifier for the node.
     */
    Node(const std::string& id);

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
     * @return True if the edge was added successfully, false if it already exists.
     */
    bool add_edge(Node* neighbor, int weight);

    /**
     * @brief Removes the directed edge to a neighbor node.
     * @param neighbor Pointer to the neighbor node.
     * @return True if the edge was removed successfully, false if it does not exist.
     */
    bool remove_edge(Node* neighbor);

    /**
     * @brief Changes the weight of the edge to a neighbor node.
     * @param neighbor Pointer to the neighbor node.
     * @param new_weight New weight for the edge.
     * @return True if the weight was changed successfully, false if the edge does not exist.
     */
    bool change_edge_weight(Node* neighbor, int new_weight);

    /**
     * @brief Checks if there is a directed edge to a neighbor node.
     * @param neighbor Pointer to the neighbor node.
     * @return True if the edge exists, false otherwise.
     */
    [[nodiscard]]
    bool contains_edge(Node* neighbor) const;

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
     * @brief Equality operator to compare two nodes based on their ID, parent count, child count,
     * and edges.
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
     * @brief Provides access to the children map for external use (e.g., in Graph class).
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
     * @brief prints node with all elements (id, number of parents, number of children, and children
     * ids with weights)
     * @return void, prints to std::cout
     */
    void print_full() const;
};

#endif  // NODE_H
