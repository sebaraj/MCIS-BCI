/**
 * @file mcis_algorithm.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#include "mcis/mcis_algorithm.h"

#include <string>
#include <vector>

#include "./bron_kerbosch_serial.h"

MCISAlgorithm::MCISAlgorithm() {
    algorithms.push_back(new BronKerboschSerial());
}

MCISAlgorithm::~MCISAlgorithm() {
    for (auto algorithm : algorithms) {
        delete algorithm;
    }
}

std::expected<std::vector<Graph*>, mcis::AlgorithmError> MCISAlgorithm::run(
    const std::vector<const Graph*>& graphs, AlgorithmType type,
    std::optional<std::string> tag) {
    if (tag) {
        std::vector<Graph> subgraphs;
        subgraphs.reserve(graphs.size());
        std::vector<const Graph*> subgraph_ptrs;
        subgraph_ptrs.reserve(graphs.size());
        for (const auto& graph : graphs) {
            subgraphs.push_back(graph->get_subgraph_with_tag(*tag));
            subgraph_ptrs.push_back(&subgraphs.back());
        }
        return algorithms[static_cast<int>(type)]->find(subgraph_ptrs, tag);
    } else {
        return algorithms[static_cast<int>(type)]->find(graphs, tag);
    }
}

template <typename T>
    requires std::is_base_of_v<MCISFinder, T>
std::expected<std::vector<Graph*>, mcis::AlgorithmError> MCISAlgorithm::run(
    const std::vector<const Graph*>& graphs, T* algorithm,
    std::optional<std::string> tag) {
    if (tag) {
        std::vector<Graph> subgraphs;
        subgraphs.reserve(graphs.size());
        std::vector<const Graph*> subgraph_ptrs;
        subgraph_ptrs.reserve(graphs.size());
        for (const auto& graph : graphs) {
            subgraphs.push_back(graph->get_subgraph_with_tag(*tag));
            subgraph_ptrs.push_back(&subgraphs.back());
        }
        return algorithm->find(subgraph_ptrs, tag);
    } else {
        return algorithm->find(graphs, tag);
    }
}

std::expected<std::vector<std::vector<Graph*>>, mcis::AlgorithmError>
MCISAlgorithm::run_many(const std::vector<const Graph*>& graphs,
                        std::vector<AlgorithmType> types,
                        std::optional<std::string> tag) {
    std::vector<std::vector<Graph*>> results;
    for (const auto& type : types) {
        auto result = run(graphs, type, tag);
        if (result) {
            results.push_back(*result);
        } else {
            return std::unexpected(result.error());
        }
    }
    return results;
}
