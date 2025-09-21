/**
 * @file
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 */

#ifndef MCIS_ALGORITHM_H
#define MCIS_ALGORITHM_H

#include <vector>

#include "graph.h"
#include "mcis_finder.h"

/**
 * @enum AlgorithmType
 * @brief Enumeration of available MCIS algorithms.
 */
enum class AlgorithmType { BRON_KERBOSCH_SERIAL };

/**
 * @class MCISAlgorithm
 * @brief Manages and runs different MCIS algorithms on pairs of graphs.
 * This class allows users to select and execute various algorithms to find the Maximum Common
 * Induced Subgraph (MCIS) between two graphs.
 */
class MCISAlgorithm {
private:
    /**
     * @brief List of available MCIS algorithms.
     */
    std::vector<MCISFinder*> algorithms;

public:
    /**
     * @brief Constructs the MCISAlgorithm manager and initializes available algorithms.
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
     * @return A vector of pointers to Graph objects representing the found MCIS results.
     */
    std::vector<Graph*> run(const Graph& g1, const Graph& g2, AlgorithmType type);

    /**
     * @brief Runs a user-specified MCIS algorithm on two input graphs.
     * @tparam T Type of the MCIS algorithm, must derive from MCISFinder.
     * @param g1 The first input graph.
     * @param g2 The second input graph.
     * @param algorithm Pointer to the user-specified algorithm instance.
     * @return A vector of pointers to Graph objects representing the found MCIS results.
     */
    template <typename T>
        requires std::is_base_of_v<MCISFinder, T>
    std::vector<Graph*> run(const Graph& g1, const Graph& g2, T* algorithm) {
        return algorithm->find_mcis(g1, g2);
    }

    /**
     * @brief Runs multiple specified MCIS algorithms on two input graphs.
     * @param g1 The first input graph.
     * @param g2 The second input graph.
     * @param types A vector of algorithm types to run (from AlgorithmType enum).
     * @return A vector of vectors, where each inner vector contains pointers to Graph objects
     * representing the found MCIS results for each algorithm.
     */
    std::vector<std::vector<Graph*>> run_many(const Graph& g1, const Graph& g2,
                                              std::vector<AlgorithmType> types);
};

#endif  // MCIS_ALGORITHM_H
