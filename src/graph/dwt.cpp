/**
 * @file dwt.cpp
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
#include <iostream>
#include <string>
#include <vector>

#include "mcis/errors.h"

const double SQRT2 = sqrt(2);

std::expected<std::vector<Graph>, mcis::GraphError>
Graph::create_haar_wavelet_transform_graph_from_dimensions(
    int n, int d, int k, HaarWaveletGraph type) {
    if (n <= 0 || d <= 0 || k <= 0 || n % (k * (1 << d)) != 0) {
        return std::unexpected(mcis::GraphError::INVALID_PARAMETERS);
    }

    Graph pruned_avg_graph;
    Graph pruned_coeff_graph;

    if (type == HaarWaveletGraph::PRUNED_AVERAGE || type == HaarWaveletGraph::BOTH) {
        for (int i = 0; i < k * (1 << d); ++i) {
            pruned_avg_graph.add_node("s_" + std::to_string(i));
        }
        for (int i = d; i > 0; --i) {
            for (int j = 0; j < k * (1 << (i - 1)); ++j) {
                std::string avg_node_name =
                    "a^" + std::to_string(d - i) + "_" + std::to_string(j);
                pruned_avg_graph.add_node(avg_node_name);
                if (i == d) {
                    pruned_avg_graph.add_edge("s_" + std::to_string(2 * j),
                                              avg_node_name, 0);
                    pruned_avg_graph.add_edge("s_" + std::to_string(2 * j + 1),
                                              avg_node_name, 0);
                } else {
                    pruned_avg_graph.add_edge(
                        "a^" + std::to_string(d - i - 1) + "_" +
                            std::to_string(2 * j),
                        avg_node_name, 0);
                    pruned_avg_graph.add_edge(
                        "a^" + std::to_string(d - i - 1) + "_" +
                            std::to_string(2 * j + 1),
                        avg_node_name, 0);
                }
            }
        }
    }

    if (type == HaarWaveletGraph::PRUNED_COEFFICIENT
        || type == HaarWaveletGraph::BOTH) {
        for (int i = 0; i < k * (1 << d); ++i) {
            pruned_coeff_graph.add_node("s_" + std::to_string(i));
        }
        for (int i = d; i > 0; --i) {
            for (int j = 0; j < k * (1 << (i - 1)); ++j) {
                std::string coeff_node_name =
                    "d^" + std::to_string(d - i) + "_" + std::to_string(j);
                pruned_coeff_graph.add_node(coeff_node_name);
                if (i == d) {
                    pruned_coeff_graph.add_edge("s_" + std::to_string(2 * j),
                                                coeff_node_name, 0);
                    pruned_coeff_graph.add_edge("s_" + std::to_string(2 * j + 1),
                                                coeff_node_name, 0);
                } else {
                    pruned_coeff_graph.add_edge(
                        "a^" + std::to_string(d - i - 1) + "_" +
                            std::to_string(2 * j),
                        coeff_node_name, 0);
                    pruned_coeff_graph.add_edge(
                        "a^" + std::to_string(d - i - 1) + "_" +
                            std::to_string(2 * j + 1),
                        coeff_node_name, 0);
                }
            }
        }
    }

    if (type == HaarWaveletGraph::PRUNED_AVERAGE) {
        return std::vector<Graph>{pruned_avg_graph};
    }
    if (type == HaarWaveletGraph::PRUNED_COEFFICIENT) {
        return std::vector<Graph>{pruned_coeff_graph};
    }
    return std::vector<Graph>{pruned_avg_graph, pruned_coeff_graph};
}

std::ostream& operator<<(std::ostream& os,
                         const std::vector<std::vector<double>>& v) {
    os << "[\n";
    for (const auto& row : v) {
        os << "  [";
        for (size_t i = 0; i < row.size(); ++i) {
            os << row[i];
            if (i < row.size() - 1) {
                os << ", ";
            }
        }
        os << "],\n";
    }
    os << "]\n";
    return os;
}

std::expected<std::vector<Graph>, mcis::GraphError>
Graph::create_haar_wavelet_transform_graph_from_signal(
    const std::vector<double>& signal, HaarWaveletGraph type) {
    int n = static_cast<int>(signal.size());
    if (n <= 0 || (n & (n - 1)) != 0) {
        return std::unexpected(mcis::GraphError::INVALID_PARAMETERS);
    }
    int d = static_cast<int>(log2(n));

    Graph pruned_avg_graph;
    Graph pruned_coeff_graph;

    std::vector<std::vector<double>> averages(d);
    std::vector<std::vector<double>> coefficients(d);

    for (int i = 0; i < d; ++i) {
        int size = n / (1 << (i + 1));
        averages[i].resize(size);
        coefficients[i].resize(size);
    }

    std::vector<double> current_level_signal = signal;
    for (int i = 0; i < d; ++i) {
        int current_size = n / (1 << i);
        int next_size = n / (1 << (i + 1));
        std::vector<double> next_level_averages(next_size);

        for (int j = 0; j < next_size; ++j) {
            averages[i][j] = (current_level_signal[2 * j] +
                              current_level_signal[2 * j + 1]) /
                             SQRT2;
            coefficients[i][j] = (current_level_signal[2 * j] -
                                  current_level_signal[2 * j + 1]) /
                                 SQRT2;
            next_level_averages[j] = averages[i][j];
        }
        current_level_signal = next_level_averages;
    }

    auto build_graph = [&](Graph& graph, bool is_coeff_graph) {
        for (int i = 0; i < n; ++i) {
            graph.add_node("s_" + std::to_string(i));
            graph.set_node_tag("s_" + std::to_string(i),
                               std::to_string(signal[i]));
        }

        for (int i = d; i > 0; --i) {
            for (int j = 0; j < (n / (1 << i)); ++j) {
                std::string avg_node_name =
                    "a^" + std::to_string(d - i) + "_" + std::to_string(j);
                graph.add_node(avg_node_name);
                graph.set_node_tag(
                    avg_node_name,
                    std::to_string(averages[d - i][j]));

                if (i == d) {
                    graph.add_edge("s_" + std::to_string(2 * j), avg_node_name,
                                   0);
                    graph.add_edge("s_" + std::to_string(2 * j + 1),
                                   avg_node_name, 0);
                } else {
                    graph.add_edge("a^" + std::to_string(d - i - 1) + "_" +
                                       std::to_string(2 * j),
                                   avg_node_name, 0);
                    graph.add_edge("a^" + std::to_string(d - i - 1) + "_" +
                                       std::to_string(2 * j + 1),
                                   avg_node_name, 0);
                }
            }
        }

        if (is_coeff_graph) {
            for (int i = d; i > 0; --i) {
                for (int j = 0; j < (n / (1 << i)); ++j) {
                    std::string coeff_node_name =
                        "d^" + std::to_string(d - i) + "_" + std::to_string(j);
                    graph.add_node(coeff_node_name);
                    graph.set_node_tag(
                        coeff_node_name,
                        std::to_string(coefficients[d - i][j]));

                    if (i == d) {
                        graph.add_edge("s_" + std::to_string(2 * j),
                                       coeff_node_name, 0);
                        graph.add_edge("s_" + std::to_string(2 * j + 1),
                                       coeff_node_name, 0);
                    } else {
                        graph.add_edge("a^" + std::to_string(d - i - 1) + "_" +
                                           std::to_string(2 * j),
                                       coeff_node_name, 0);
                        graph.add_edge("a^" + std::to_string(d - i - 1) + "_" +
                                           std::to_string(2 * j + 1),
                                       coeff_node_name, 0);
                    }
                }
            }
        }
    };

    if (type == HaarWaveletGraph::PRUNED_AVERAGE
        || type == HaarWaveletGraph::BOTH) {
        build_graph(pruned_avg_graph, false);
    }
    if (type == HaarWaveletGraph::PRUNED_COEFFICIENT
        || type == HaarWaveletGraph::BOTH) {
        build_graph(pruned_coeff_graph, true);
    }

    if (type == HaarWaveletGraph::PRUNED_AVERAGE) {
        return std::vector<Graph>{pruned_avg_graph};
    }
    if (type == HaarWaveletGraph::PRUNED_COEFFICIENT) {
        return std::vector<Graph>{pruned_coeff_graph};
    }
    return std::vector<Graph>{pruned_avg_graph, pruned_coeff_graph};
}
