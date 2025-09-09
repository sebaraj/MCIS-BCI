/**
 * @file
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 */

#ifndef MCIS_FINDER_H
#define MCIS_FINDER_H

#include <vector>

#include "mcs/graph.h"

/**
 * @class MCISFinder
 *
 * Abstract base class for finding the Maximum Common Induced Subgraph (MCIS) between two graphs.
 * Derived classes must implement the find method.
 */
class MCISFinder {
public:
    virtual std::vector<Graph*> find(const Graph& g1, const Graph& g2) = 0;
    virtual ~MCISFinder() {};
};

#endif  // MCIS_FINDER_H
