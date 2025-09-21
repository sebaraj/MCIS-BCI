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

#include <vector>

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
     * Pure virtual function to find the MCIS between two graphs.
     * @param g1 The first graph.
     * @param g2 The second graph.
     * @return A vector of pointers to Graph objects representing the MCIS
     * found.
     */
    virtual std::vector<Graph*> find(const Graph& g1, const Graph& g2) = 0;

    /**
     * Virtual destructor.
     */
    virtual ~MCISFinder() {}
};

#endif  // INCLUDE_MCIS_MCIS_FINDER_H_
