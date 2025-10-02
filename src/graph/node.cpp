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
    : id(std::move(id)), num_parents(0), num_children(0), tag(0) {}

Node::Node(const Node& other)
    : id(other.id),
      num_parents(other.num_parents),
      num_children(other.num_children),
      children(other.children),
      tag(other.tag) {}

Node& Node::operator=(const Node& other) {
    if (this != &other) {
        id = other.id;
        num_parents = other.num_parents;
        num_children = other.num_children;
        children = other.children;
        tag = other.tag;
    }
    return *this;
}

Node::Node(Node&& other) noexcept
    : id(std::move(other.id)),
      num_parents(other.num_parents),
      num_children(other.num_children),
      children(std::move(other.children)),
      tag(other.tag) {
    other.num_parents = 0;
    other.num_children = 0;
    other.tag = 0;
}

Node& Node::operator=(Node&& other) noexcept {
    if (this != &other) {
        id = std::move(other.id);
        num_parents = other.num_parents;
        num_children = other.num_children;
        children = std::move(other.children);
        tag = other.tag;
        other.num_parents = 0;
        other.num_children = 0;
        other.tag = 0;
    }
    return *this;
}

Node::~Node() { children.clear(); }

std::string Node::get_id() const { return id; }

int Node::get_tag() const { return tag; }

void Node::set_tag(int new_tag) { tag = new_tag; }

int Node::get_num_parents() const { return num_parents; }

int Node::get_num_children() const { return num_children; }

bool Node::add_edge(Node* neighbor, int weight) {
    if (children.find(neighbor) != children.end()) {
        return (children[neighbor] == weight) ? true : false;
    }
    if (id == neighbor->id) {
        return false;
    }
    children[neighbor] = weight;
    num_children++;
    neighbor->num_parents++;
    return true;
}

bool Node::remove_edge(Node* neighbor) {
    if (children.find(neighbor) == children.end()) {
        return false;
    }
    children.erase(neighbor);
    num_children--;
    neighbor->num_parents--;
    return true;
}

bool Node::change_edge_weight(Node* neighbor, int new_weight) {
    if (children.find(neighbor) == children.end()) {
        return false;
    }
    children[neighbor] = new_weight;
    return true;
}

bool Node::contains_edge(Node* neighbor) const {
    return children.find(neighbor) != children.end();
}

bool Node::is_source() const { return num_parents == 0; }

bool Node::is_sink() const { return num_children == 0; }

bool Node::operator==(const Node& other) const {
    bool same = num_parents == other.num_parents && id == other.id
                && num_children == other.num_children;

    for (const auto& [child, weight] : children) {
        auto it = other.children.find(child);
        if (it == other.children.end() || it->second != weight) {
            return false;
        }
    }
    return same;
}

bool Node::same_id(const Node& other) const { return id == other.id; }

const std::unordered_map<Node*, int>& Node::get_children() const {
    return children;
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
