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
#include <vector>

#include "mcis/errors.h"
#include "mcis/graph.h"
#include "mcis/mcis_finder.h"

/**
 * @enum AlgorithmType
 * @brief Enumeration of available MCIS algorithms.
 */
enum class AlgorithmType { BRON_KERBOSCH_SERIAL };

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
     * @brief Runs the specified MCIS algorithm on two input graphs.
     * @param g1 The first input graph.
     * @param g2 The second input graph.
     * @param type The type of algorithm to run (from AlgorithmType enum).
     * @return A vector of pointers to Graph objects representing the found MCIS
     * results, or an error.
     */
    std::expected<std::vector<Graph*>, mcis::AlgorithmError> run(
        const Graph& g1, const Graph& g2, AlgorithmType type);

    /**
     * @brief Runs a user-specified MCIS algorithm on two input graphs.
     * @tparam T Type of the MCIS algorithm, must derive from MCISFinder.
     * @param g1 The first input graph.
     * @param g2 The second input graph.
     * @param algorithm Pointer to the user-specified algorithm instance.
     * @return A vector of pointers to Graph objects representing the found MCIS
     * results, or an error.
     */
    template <typename T>
        requires std::is_base_of_v<MCISFinder, T>
    std::expected<std::vector<Graph*>, mcis::AlgorithmError> run(
        const Graph& g1, const Graph& g2, T* algorithm);

    /**
     * @brief Runs multiple specified MCIS algorithms on two input graphs.
     * @param g1 The first input graph.
     * @param g2 The second input graph.
     * @param types A vector of algorithm types to run (from AlgorithmType
     * enum).
     * @return A vector of vectors, where each inner vector contains pointers to
     * Graph objects representing the found MCIS results for each algorithm, or
     * an error.
     */
    std::expected<std::vector<std::vector<Graph*>>, mcis::AlgorithmError>
    run_many(const Graph& g1, const Graph& g2,
             std::vector<AlgorithmType> types);
};

#endif  // INCLUDE_MCIS_MCIS_ALGORITHM_H_
