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

#include <iostream>
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
    const Graph& g1, const Graph& g2, AlgorithmType type) {
    switch (type) {
        case AlgorithmType::BRON_KERBOSCH_SERIAL:
            return algorithms[static_cast<int>(type)]->find(g1, g2);
            break;
        default:
            return std::unexpected(mcis::AlgorithmError::INVALID_ALGORITHM);
    }

    return std::unexpected(mcis::AlgorithmError::INVALID_ALGORITHM);
}

template <typename T>
    requires std::is_base_of_v<MCISFinder, T>
std::expected<std::vector<Graph*>, mcis::AlgorithmError> MCISAlgorithm::run(
    const Graph& g1, const Graph& g2, T* algorithm) {
    return algorithm->find(g1, g2);
}

std::expected<std::vector<std::vector<Graph*>>, mcis::AlgorithmError>
MCISAlgorithm::run_many(const Graph& g1, const Graph& g2,
                        std::vector<AlgorithmType> types) {
    std::vector<std::vector<Graph*>> results;
    for (const auto& type : types) {
        switch (type) {
            case AlgorithmType::BRON_KERBOSCH_SERIAL: {
                auto result = algorithms[static_cast<int>(type)]->find(g1, g2);
                if (result) {
                    results.push_back(*result);
                } else {
                    return std::unexpected(result.error());
                }
                break;
            }
            default:
                return std::unexpected(mcis::AlgorithmError::INVALID_ALGORITHM);
        }
    }
    return results;
}
