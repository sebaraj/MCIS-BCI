/**
 * @file graph.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */
#include <mcis/graph.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "./time.h"

Graph::Graph() = default;

Graph::Graph(const std::vector<Node>& node_list) {
    for (const auto& node : node_list) {
        add_node(node.get_id());
    }
}

Graph::Graph(const Graph& other) {
    for (const auto& pair : other.nodes) {
        nodes[pair.first] = new Node(*pair.second);
    }
}

Graph& Graph::operator=(const Graph& other) {
    if (this != &other) {
        for (auto& pair : nodes) {
            delete pair.second;
        }
        nodes.clear();
        for (const auto& pair : other.nodes) {
            nodes[pair.first] = new Node(*pair.second);
        }
    }
    return *this;
}

Graph::Graph(Graph&& other) noexcept : nodes(std::move(other.nodes)) {
    other.nodes.clear();
}

Graph& Graph::operator=(Graph&& other) noexcept {
    if (this != &other) {
        for (auto& pair : nodes) {
            delete pair.second;
        }
        nodes = std::move(other.nodes);
        other.nodes.clear();
    }
    return *this;
}

Graph::~Graph() {
    for (auto& pair : nodes) {
        delete pair.second;
    }
}

bool Graph::is_dag() {
    if (dag_cache_valid) {
        return dag_cache_result;
    }

    // Kahn's algorithm for cycle detection
    std::unordered_map<std::string, int> in_degree;
    in_degree.reserve(nodes.size());

    for (const auto& pair : nodes) {
        in_degree[pair.first] = pair.second->get_num_parents();
    }

    std::vector<std::string> zero_in_degree;
    zero_in_degree.reserve(nodes.size() / 4);

    for (const auto& pair : in_degree) {
        if (pair.second == 0) {
            zero_in_degree.push_back(pair.first);
        }
    }

    int visited_count = 0;
    const int total_nodes = static_cast<int>(nodes.size());

    while (!zero_in_degree.empty()) {
        std::string node_id = zero_in_degree.back();
        zero_in_degree.pop_back();
        visited_count++;

        // early termination
        if (visited_count == total_nodes) {
            dag_cache_result = true;
            dag_cache_valid = true;
            return true;
        }

        Node* node = nodes[node_id];
        for (const auto& child_pair : node->get_children()) {
            Node* child = child_pair.first;
            in_degree[child->get_id()]--;
            if (in_degree[child->get_id()] == 0) {
                zero_in_degree.push_back(child->get_id());
            }
        }
    }

    dag_cache_result = (visited_count == total_nodes);
    dag_cache_valid = true;
    return dag_cache_result;
}

void Graph::print_graph() const {
    for (const auto& pair : nodes) {
        const Node* node = pair.second;
        std::cout << node->get_id() << ": [";
        bool first = true;
        for (const auto& child_pair : node->get_children()) {
            if (!first) {
                std::cout << ", ";
            }
            std::cout << child_pair.first->get_id() << "(" << child_pair.second
                      << ")";
            first = false;
        }
        std::cout << "]\n";
    }
}

std::optional<mcis::GraphError> Graph::add_node(const std::string& id) {
    if (nodes.count(id)) {
        return mcis::GraphError::NODE_ALREADY_EXISTS;
    }
    nodes[id] = new Node(id);
    invalidate_caches();
    return std::nullopt;
}

std::optional<mcis::GraphError> Graph::add_node_set(
    const std::vector<std::string>& ids) {
    bool any_added = false;
    for (const std::string& id : ids) {
        if (nodes.count(id)) {
            return mcis::GraphError::NODE_ALREADY_EXISTS;
        }
        nodes[id] = new Node(id);
        any_added = true;
    }

    if (any_added) {
        invalidate_caches();
    }
    return std::nullopt;
}

std::optional<mcis::GraphError> Graph::remove_node(const std::string& id) {
    auto it = nodes.find(id);
    if (it == nodes.end()) {
        return mcis::GraphError::NODE_DOES_NOT_EXIST;
    }

    Node* node_to_remove = it->second;

    auto children_copy = node_to_remove->get_children();
    for (const auto& [child, weight] : children_copy) {
        node_to_remove->remove_edge(
            child);  // This properly updates parent counts
    }

    for (auto& pair : nodes) {
        Node* node = pair.second;
        if (node != node_to_remove) {
            const auto& children = node->get_children();
            if (children.find(node_to_remove) != children.end()) {
                node->remove_edge(node_to_remove);
            }
        }
    }

    delete node_to_remove;
    nodes.erase(it);
    invalidate_caches();
    return std::nullopt;
}

std::optional<mcis::GraphError> Graph::add_edge(const std::string& from_id,
                                                const std::string& to_id,
                                                int weight) {
    auto from_it = nodes.find(from_id);
    auto to_it = nodes.find(to_id);
    if (from_it == nodes.end() || to_it == nodes.end()) {
        return mcis::GraphError::NODE_DOES_NOT_EXIST;
    }
    is_weighted = is_weighted || (weight != 0);
    if (auto error = from_it->second->add_edge(to_it->second, weight)) {
        return mcis::GraphError::EDGE_ALREADY_EXISTS;
    }
    invalidate_caches();
    return std::nullopt;
}

std::optional<mcis::GraphError> Graph::add_edge_set(
    const std::string& from_id, const std::vector<std::string>& to_ids,
    const std::vector<int>& weights) {
    bool use_zero_weights = weights.empty() || weights.size() != to_ids.size();
    auto from_it = nodes.find(from_id);
    if (from_it == nodes.end()) {
        return mcis::GraphError::NODE_DOES_NOT_EXIST;
    }

    bool any_added = false;

    for (size_t i = 0; i < to_ids.size(); ++i) {
        int weight = use_zero_weights ? 0 : weights[i];
        auto to_it = nodes.find(to_ids[i]);
        if (to_it != nodes.end()) {
            if (auto error = from_it->second->add_edge(to_it->second, weight)) {
                return mcis::GraphError::EDGE_ALREADY_EXISTS;
            }
            any_added = true;
        } else {
            return mcis::GraphError::NODE_DOES_NOT_EXIST;
        }
    }

    if (any_added) {
        invalidate_caches();
    }
    return std::nullopt;
}

std::optional<mcis::GraphError> Graph::remove_edge(const std::string& from_id,
                                                   const std::string& to_id) {
    auto from_it = nodes.find(from_id);
    auto to_it = nodes.find(to_id);
    if (from_it == nodes.end() || to_it == nodes.end()) {
        return mcis::GraphError::NODE_DOES_NOT_EXIST;
    }
    if (auto error = from_it->second->remove_edge(to_it->second)) {
        return mcis::GraphError::EDGE_DOES_NOT_EXIST;
    }
    invalidate_caches();
    return std::nullopt;
}

std::optional<mcis::GraphError> Graph::change_edge_weight(
    const std::string& from_id, const std::string& to_id, int new_weight) {
    auto from_it = nodes.find(from_id);
    auto to_it = nodes.find(to_id);
    if (from_it == nodes.end() || to_it == nodes.end()) {
        return mcis::GraphError::NODE_DOES_NOT_EXIST;
    }
    if (auto error
        = from_it->second->change_edge_weight(to_it->second, new_weight)) {
        return mcis::GraphError::EDGE_DOES_NOT_EXIST;
    }
    return std::nullopt;
}

std::optional<mcis::GraphError> Graph::set_node_tag(const std::string& id,
                                                    int new_tag) {
    auto it = nodes.find(id);
    if (it == nodes.end()) {
        return mcis::GraphError::NODE_DOES_NOT_EXIST;
    }
    it->second->set_tag(new_tag);
    return std::nullopt;
}

Node* Graph::get_node(const std::string& id) const {
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        return it->second;
    }
    return nullptr;
}

int Graph::get_num_nodes() const { return static_cast<int>(nodes.size()); }

const std::unordered_map<std::string, Node*>& Graph::get_nodes() const {
    return nodes;
}

bool Graph::operator==(const Graph& other) const {
    if (get_num_nodes() != other.get_num_nodes()) {
        return false;
    }

    for (const auto& pair : nodes) {
        const std::string& node_id = pair.first;
        Node* this_node = pair.second;
        Node* other_node = other.get_node(node_id);
        if (!other_node || *this_node != *other_node) {
            return false;
        }
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const Graph& graph) {
    const auto& nodes_map = graph.get_nodes();
    std::vector<Node*> node_list(nodes_map.size());
    int idx = 0;
    for (const auto& [_, node] : nodes_map) {
        node_list[idx++] = node;
    }
    std::sort(
        node_list.begin(), node_list.end(),
        [](const Node* a, const Node* b) { return a->get_id() < b->get_id(); });
    for (const auto node : node_list) {
        os << *node << "\n";
    }
    return os;
}

void Graph::generate_diagram_file(const std::string& graph_name) const {
    // https://www.graphviz.org/pdf/dotguide.pdf
    std::string filename = currentDateTime() + "_" + graph_name;
    std::string dotpath = "../../dot/" + filename + ".gv";
    std::string diagrampath = "../../diagrams/" + filename + ".png";
    std::ofstream outputFile(dotpath);

    outputFile << "digraph G {\n";
    if (!is_weighted) {
        for (const auto& [_, node] : nodes) {
            std::string node_id = node->get_id();
            size_t super_pos = node_id.find('^');
            size_t sub_pos = node_id.find('_');

            if (node_id.find('^') != std::string::npos
                || node_id.find('_') != std::string::npos) {
                outputFile << "    " << std::quoted(node->get_id())
                           << " [label=<v<SUB>" << node_id.substr(sub_pos + 1)
                           << "</SUB><SUP>"
                           << node_id.substr(super_pos + 1,
                                             sub_pos - super_pos - 1)
                           << "</SUP>>];\n";
            }
            for (const auto& [child, weight] : node->get_children()) {
                outputFile << "    " << std::quoted(node->get_id()) << " -> "
                           << std::quoted(child->get_id()) << ";\n";
            }
        }
    } else {
        for (const auto& [_, node] : nodes) {
            for (const auto& [child, weight] : node->get_children()) {
                outputFile << "    " << std::quoted(node->get_id()) << " -> "
                           << std::quoted(child->get_id()) << " [label=\""
                           << weight << "\"];\n";
            }
        }
    }
    outputFile << "}\n";
    outputFile.close();

    system(("dot -Tpng " + dotpath + " -o " + diagrampath).c_str());
}

int Graph::remove_nodes_bulk(const std::vector<std::string>& node_ids) {
    if (node_ids.empty()) {
        return 0;
    }

    std::vector<Node*> nodes_to_remove;
    nodes_to_remove.reserve(node_ids.size());

    for (const std::string& id : node_ids) {
        auto it = nodes.find(id);
        if (it != nodes.end()) {
            nodes_to_remove.push_back(it->second);
        }
    }

    if (nodes_to_remove.empty()) return 0;

    std::unordered_set<Node*> removal_set(nodes_to_remove.begin(),
                                          nodes_to_remove.end());

    for (Node* node_to_remove : nodes_to_remove) {
        auto children_copy = node_to_remove->get_children();
        for (const auto& [child, weight] : children_copy) {
            node_to_remove->remove_edge(child);
        }
    }

    for (auto& [id, node] : nodes) {
        if (removal_set.find(node) == removal_set.end()) {
            std::vector<Node*> edges_to_remove;
            for (const auto& [child, weight] : node->get_children()) {
                if (removal_set.find(child) != removal_set.end()) {
                    edges_to_remove.push_back(child);
                }
            }
            for (Node* child_to_remove : edges_to_remove) {
                node->remove_edge(child_to_remove);
            }
        }
    }

    for (Node* node_to_remove : nodes_to_remove) {
        nodes.erase(node_to_remove->get_id());
        delete node_to_remove;
    }

    invalidate_caches();
    return static_cast<int>(nodes_to_remove.size());
}

void Graph::reserve_nodes(size_t expected_size) {
    nodes.reserve(expected_size);
}

void Graph::invalidate_caches() const {
    dag_cache_valid = false;
    ++version;
}
