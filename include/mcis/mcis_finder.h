/**
 * @file mcis_finder.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef INCLUDE_MCIS_MCIS_FINDER_H_
#define INCLUDE_MCIS_MCIS_FINDER_H_

#include <expected>
#include <string>
#include <vector>

#include "mcis/errors.h"
#include "mcis/graph.h"

/**
 * @class MCISFinder
 *
 * Abstract base class for finding the Maximum Common Induced Subgraph (MCIS)
 * between two graphs. Derived classes must implement the find method.
 */
class MCISFinder {
 public:
    /**
     * Pure virtual function to find the MCIS between a set of graphs.
     * @param graphs A vector of pointers to the graphs.
     * @param tag An optional tag to filter nodes by.
     * @return A vector of pointers to Graph objects representing the MCIS
     * found, or an error if the graphs are empty.
     */
    virtual std::expected<std::vector<Graph*>, mcis::AlgorithmError> find(
        const std::vector<const Graph*>& graphs, std::optional<std::string> tag)
        = 0;

    /**
     * Virtual destructor.
     */
    virtual ~MCISFinder() {}
};

#endif  // INCLUDE_MCIS_MCIS_FINDER_H_
