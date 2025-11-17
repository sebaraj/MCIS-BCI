/**
 * @file fft.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */
#include <mcis/graph.h>
#include <omp.h>

#include <cmath>
#include <expected>
#include <string>

#include "mcis/errors.h"

// Cooley-Tukey FFT algorithm, specifically the decimation-in-time
// (DIT) variant, which is a ecursive "divide and conquer" algorithm
// that breaks down a Discrete Fourier Transform (DFT) of size N into smaller
// DFTs. For the graph representation, N must be a power of 2.

std::expected<Graph, mcis::GraphError> Graph::create_fft_graph_from_dimensions(
    int n) {
    if (n <= 0 || (n & (n - 1)) != 0) {
        return std::unexpected(mcis::GraphError::INVALID_PARAMETERS);
    }

    Graph graph;
    int stages = static_cast<int>(log2(n));
    graph.reserve_nodes(n + n * stages + n);

    for (int i = 0; i < n; ++i) {
        graph.add_node("x_" + std::to_string(i));
    }

    for (int stage = 1; stage <= stages; ++stage) {
        for (int i = 0; i < n; ++i) {
            std::string node_name
                = "s" + std::to_string(stage) + "_" + std::to_string(i);
            graph.add_node(node_name);
            graph.set_node_tag(node_name, "+/-*");
        }
    }

    for (int stage = 1; stage <= stages; ++stage) {
        int butterflies_in_stage = 1 << (stage - 1);
        int butterfly_size = n / butterflies_in_stage;
        int half_size = butterfly_size / 2;

        for (int b = 0; b < butterflies_in_stage; ++b) {
            for (int j = 0; j < half_size; ++j) {
                int top_idx = b * butterfly_size + j;
                int bottom_idx = top_idx + half_size;

                std::string top_node_in
                    = (stage == 1) ? "x_" + std::to_string(top_idx)
                                   : "s" + std::to_string(stage - 1) + "_"
                                         + std::to_string(top_idx);
                std::string bottom_node_in
                    = (stage == 1) ? "x_" + std::to_string(bottom_idx)
                                   : "s" + std::to_string(stage - 1) + "_"
                                         + std::to_string(bottom_idx);

                std::string top_node_out = "s" + std::to_string(stage) + "_"
                                           + std::to_string(top_idx);
                std::string bottom_node_out = "s" + std::to_string(stage) + "_"
                                              + std::to_string(bottom_idx);

                graph.add_edge(top_node_in, top_node_out, 0);
                graph.add_edge(bottom_node_in, top_node_out, 0);
                graph.add_edge(top_node_in, bottom_node_out, 0);
                graph.add_edge(bottom_node_in, bottom_node_out, 0);
            }
        }
    }

    auto reverse_bits = [&](int val) {
        int reversed_val = 0;
        for (int i = 0; i < stages; ++i) {
            if ((val >> i) & 1) {
                reversed_val |= 1 << (stages - 1 - i);
            }
        }
        return reversed_val;
    };

    for (int i = 0; i < n; ++i) {
        std::string output_node = "X_" + std::to_string(i);
        graph.add_node(output_node);
        int reversed_idx = reverse_bits(i);
        std::string last_stage_node
            = "s" + std::to_string(stages) + "_" + std::to_string(reversed_idx);
        graph.add_edge(last_stage_node, output_node, 0);
    }

    return graph;
}
