/**
 * @file bron_kerbosch_serial.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef SRC_ALGORITHMS_BRON_KERBOSCH_SERIAL_H_
#define SRC_ALGORITHMS_BRON_KERBOSCH_SERIAL_H_

#include <vector>

#include "mcis/graph.h"
#include "mcis/mcis_finder.h"

/**
 * @class BronKerboschSerial
 *
 * Implements the Bron-Kerbosch algorithm for finding maximal cliques in an
 * undirected graph. This is a serial implementation without optimizations.
 */
class BronKerboschSerial : public MCISFinder {
 public:
    std::vector<Graph*> find(const Graph& g1, const Graph& g2) override;
};

#endif  // SRC_ALGORITHMS_BRON_KERBOSCH_SERIAL_H_
