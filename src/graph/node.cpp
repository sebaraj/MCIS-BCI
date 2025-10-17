/**
 * @file node.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */
#include <mcis/node.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

Node::Node(const std::string& id)
    : id(std::move(id)), num_parents(0), num_children(0), tag("") {}

Node::Node(const Node& other)
    : id(other.id),
      num_parents(other.num_parents),
      num_children(other.num_children),
      children(other.children),
      parents(other.parents),
      tag(other.tag) {}

Node& Node::operator=(const Node& other) {
    if (this != &other) {
        id = other.id;
        num_parents = other.num_parents;
        num_children = other.num_children;
        children = other.children;
        parents = other.parents;
        tag = other.tag;
    }
    return *this;
}

Node::Node(Node&& other) noexcept
    : id(std::move(other.id)),
      num_parents(other.num_parents),
      num_children(other.num_children),
      children(std::move(other.children)),
      parents(std::move(other.parents)),
      tag(std::move(other.tag)) {
    other.num_parents = 0;
    other.num_children = 0;
}

Node& Node::operator=(Node&& other) noexcept {
    if (this != &other) {
        id = std::move(other.id);
        num_parents = other.num_parents;
        num_children = other.num_children;
        children = std::move(other.children);
        parents = std::move(other.parents);
        tag = std::move(other.tag);
        other.num_parents = 0;
        other.num_children = 0;
    }
    return *this;
}

Node::~Node() {
    children.clear();
    parents.clear();
}

std::string Node::get_id() const { return id; }

std::string Node::get_tag() const { return tag; }

void Node::set_tag(const std::string& new_tag) { tag = new_tag; }

int Node::get_num_parents() const { return num_parents; }

int Node::get_num_children() const { return num_children; }

std::optional<mcis::NodeError> Node::add_edge(Node* neighbor, int weight) {
    if (children.count(neighbor)) {
        return (children[neighbor] == weight)
                   ? std::nullopt
                   : std::optional(mcis::NodeError::EDGE_ALREADY_EXISTS);
    }
    if (id == neighbor->id) {
        return mcis::NodeError::SELF_LOOP;
    }
    children[neighbor] = weight;
    num_children++;
    neighbor->add_parent(this, weight);
    return std::nullopt;
}

std::optional<mcis::NodeError> Node::remove_edge(Node* neighbor) {
    if (!children.count(neighbor)) {
        return mcis::NodeError::EDGE_DOES_NOT_EXIST;
    }
    children.erase(neighbor);
    num_children--;
    neighbor->remove_parent(this);
    return std::nullopt;
}

std::optional<mcis::NodeError> Node::add_parent(Node* parent, int weight) {
    if (parents.count(parent)) {
        return (parents[parent] == weight)
                   ? std::nullopt
                   : std::optional(mcis::NodeError::EDGE_ALREADY_EXISTS);
    }
    if (id == parent->id) {
        return mcis::NodeError::SELF_LOOP;
    }
    parents[parent] = weight;
    num_parents++;
    return std::nullopt;
}

std::optional<mcis::NodeError> Node::remove_parent(Node* parent) {
    if (!parents.count(parent)) {
        return mcis::NodeError::EDGE_DOES_NOT_EXIST;
    }
    parents.erase(parent);
    num_parents--;
    return std::nullopt;
}

bool Node::check_parent(const std::string& parent_id) const {
    for (const auto& [parent, weight] : parents) {
        if (parent->get_id() == parent_id) {
            return true;
        }
    }
    return false;
}

std::optional<mcis::NodeError> Node::change_edge_weight(Node* neighbor,
                                                        int new_weight) {
    if (!children.count(neighbor)) {
        return mcis::NodeError::EDGE_DOES_NOT_EXIST;
    }
    children[neighbor] = new_weight;
    return std::nullopt;
}

bool Node::contains_edge(Node* neighbor) const {
    return children.find(neighbor) != children.end();
}

bool Node::is_source() const { return num_parents == 0; }

bool Node::is_sink() const { return num_children == 0; }

bool Node::operator==(const Node& other) const {
    if (id != other.id || num_parents != other.num_parents
        || num_children != other.num_children) {
        return false;
    }

    if (children.size() != other.children.size()) {
        return false;
    }

    std::unordered_map<std::string, int> other_children_by_id;
    for (const auto& [child_node, weight] : other.children) {
        other_children_by_id[child_node->get_id()] = weight;
    }

    for (const auto& [child_node, weight] : children) {
        auto it = other_children_by_id.find(child_node->get_id());
        if (it == other_children_by_id.end() || it->second != weight) {
            return false;
        }
    }

    return true;
}

bool Node::same_id(const Node& other) const { return id == other.id; }

const std::unordered_map<Node*, int>& Node::get_children() const {
    return children;
}

const std::unordered_map<Node*, int>& Node::get_parents() const {
    return parents;
}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    os << node.id << " -> { ";
    std::vector<Node*> keys(node.children.size());
    int idx = 0;
    for (const auto& [key, _] : node.children) {
        keys[idx++] = key;
    }
    std::sort(keys.begin(), keys.end(),
              [](Node* a, Node* b) { return a->id < b->id; });
    for (const auto key : keys) {
        os << std::quoted(key->id) << "(" << node.children.at(key) << ") ";
    }
    os << "}\n";
    return os;
}

void Node::print_full() const {
    std::cout << "Node ID: " << id << "\n";
    std::cout << "Number of Parents: " << num_parents << "\n";
    std::cout << "Number of Children: " << num_children << "\n";
    std::cout << "Children:\n";
    std::vector<Node*> keys(children.size());
    int idx = 0;
    for (const auto& [key, _] : children) {
        keys[idx++] = key;
    }
    std::sort(keys.begin(), keys.end(),
              [](Node* a, Node* b) { return a->get_id() < b->get_id(); });
    for (const auto key : keys) {
        std::cout << "  Child ID: " << key->get_id()
                  << ", Weight: " << children.at(key) << "\n";
    }
}
