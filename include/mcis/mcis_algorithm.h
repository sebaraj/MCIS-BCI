/**
 * @file mcis_algorithm.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef INCLUDE_MCIS_MCIS_ALGORITHM_H_
#define INCLUDE_MCIS_MCIS_ALGORITHM_H_

#include <expected>
#include <string>
#include <vector>

#include "mcis/errors.h"
#include "mcis/graph.h"
#include "mcis/mcis_finder.h"

/**
 * @enum AlgorithmType
 * @brief Enumeration of available MCIS algorithms.
 */
enum class AlgorithmType { BRON_KERBOSCH_SERIAL, KPT };

/**
 * @class MCISAlgorithm
 * @brief Manages and runs different MCIS algorithms on pairs of graphs.
 * This class allows users to select and execute various algorithms to find the
 * Maximum Common Induced Subgraph (MCIS) between two graphs.
 */
class MCISAlgorithm {
 private:
    /**
     * @brief List of available MCIS algorithms.
     */
    std::vector<MCISFinder*> algorithms;

 public:
    /**
     * @brief Constructs the MCISAlgorithm manager and initializes available
     * algorithms.
     */
    MCISAlgorithm();

    /**
     * @brief Destructor to clean up allocated algorithms.
     */
    ~MCISAlgorithm();

    /**
     * @brief Runs the specified MCIS algorithm on a set of input graphs.
     * @param graphs A vector of pointers to the input graphs.
     * @param type The type of algorithm to run (from AlgorithmType enum).
     * @param tag An optional tag to filter nodes by.
     * @return A vector of pointers to Graph objects representing the found MCIS
     * results, or an error.
     */
    std::expected<std::vector<Graph*>, mcis::AlgorithmError> run(
        const std::vector<const Graph*>& graphs, AlgorithmType type,
        std::optional<std::string> tag = std::nullopt);

    /**
     * @brief Runs a user-specified MCIS algorithm on a set of input graphs.
     * @tparam T Type of the MCIS algorithm, must derive from MCISFinder.
     * @param graphs A vector of pointers to the input graphs.
     * @param algorithm Pointer to the user-specified algorithm instance.
     * @param tag An optional tag to filter nodes by.
     * @return A vector of pointers to Graph objects representing the found MCIS
     * results, or an error.
     */
    template <typename T>
        requires std::is_base_of_v<MCISFinder, T>
    std::expected<std::vector<Graph*>, mcis::AlgorithmError> run(
        const std::vector<const Graph*>& graphs, T* algorithm,
        std::optional<std::string> tag = std::nullopt);

    /**
     * @brief Runs multiple specified MCIS algorithms on a set of input graphs.
     * @param graphs A vector of pointers to the input graphs.
     * @param types A vector of algorithm types to run (from AlgorithmType
     * enum).
     * @param tag An optional tag to filter nodes by.
     * @return A vector of vectors, where each inner vector contains pointers to
     * Graph objects representing the found MCIS results for each algorithm, or
     * an error.
     */
    std::expected<std::vector<std::vector<Graph*>>, mcis::AlgorithmError>
    run_many(const std::vector<const Graph*>& graphs,
             std::vector<AlgorithmType> types,
             std::optional<std::string> tag = std::nullopt);
};

#endif  // INCLUDE_MCIS_MCIS_ALGORITHM_H_
