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
    if (n <= 0 || d <= 0 || ((n / k) & ((n / k) - 1)) != 0) {
        return std::unexpected(mcis::GraphError::INVALID_PARAMETERS);
    }
    Graph pruned_avg_graph;
    Graph pruned_coeff_graph;
#pragma omp parallel sections
    {
#pragma omp section
        {
            if (type == HaarWaveletGraph::PRUNED_AVERAGE
                || type == HaarWaveletGraph::BOTH) {
                std::string avg_node_name, parent_1, parent_2;
                for (int i = 0; i < d; ++i) {
                    for (int j = 0; j < n / (1 << i); ++j) {
                        avg_node_name = "a^" + std::to_string(i + 1) + "_"
                                        + std::to_string(j + 1);
                        if (i == 0) {
                            pruned_avg_graph.add_node(avg_node_name);

                        } else {
                            pruned_avg_graph.add_node(avg_node_name);
                            parent_1 = "a^" + std::to_string(i - 1) + "_"
                                       + std::to_string(2 * j + 1);
                            parent_2 = "a^" + std::to_string(i - 1) + "_"
                                       + std::to_string(2 * j + 2);
                            pruned_avg_graph.add_edge(parent_1, avg_node_name,
                                                      0);
                            pruned_avg_graph.add_edge(parent_2, avg_node_name,
                                                      0);
                        }
                    }
                }
            }
        }
#pragma omp section
        {
            if (type == HaarWaveletGraph::PRUNED_COEFFICIENT
                || type == HaarWaveletGraph::BOTH) {
                std::string coeff_node_name, parent_1, parent_2;
                for (int i = 0; i < d; ++i) {
                    for (int j = 0; j < n / (1 << i); ++j) {
                        coeff_node_name = "a^" + std::to_string(i + 1) + "_"
                                          + std::to_string(j + 1);
                        if (i == 0) {
                            pruned_coeff_graph.add_node(coeff_node_name);

                        } else {
                            pruned_coeff_graph.add_node(coeff_node_name);
                            parent_1 = "a^" + std::to_string(i - 1) + "_"
                                       + std::to_string(2 * j + 1);
                            parent_2 = "a^" + std::to_string(i - 1) + "_"
                                       + std::to_string(2 * j + 2);
                            pruned_coeff_graph.add_edge(parent_1,
                                                        coeff_node_name, 0);
                            pruned_coeff_graph.add_edge(parent_2,
                                                        coeff_node_name, 0);
                        }
                    }
                }
            }
        }
    }
    if (type == HaarWaveletGraph::PRUNED_AVERAGE) {
        return std::vector<Graph>{pruned_avg_graph};
    } else if (type == HaarWaveletGraph::PRUNED_COEFFICIENT) {
        return std::vector<Graph>{pruned_coeff_graph};
    } else {
        return std::vector<Graph>{pruned_avg_graph, pruned_coeff_graph};
    }
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

std::vector<Graph> Graph::create_haar_wavelet_transform_graph_from_signal(
    const std::vector<double>& signal, HaarWaveletGraph type) {
    int N = static_cast<int>(signal.size());
    if (N <= 0) {
        return {Graph()};
    }
    int d = static_cast<int>(log2(N));
    Graph pruned_avg_graph;
    Graph pruned_coeff_graph;

    std::vector<std::vector<double>> averages(d);
    std::vector<std::vector<double>> coefficients(d);
    for (int i = 0; i < d; ++i) {
        averages[i] = std::vector<double>((N / (1 << i)), 0.0);
        coefficients[i] = std::vector<double>((N / (1 << i)), 0.0);
    }
#pragma omp parallel sections
    {
#pragma omp section
        {
            if (type == HaarWaveletGraph::PRUNED_AVERAGE
                || type == HaarWaveletGraph::BOTH) {
                std::string avg_node_name, parent_1, parent_2;
                for (int i = 0; i < d; ++i) {
                    for (int j = 0; j < N / (1 << i); ++j) {
                        avg_node_name = "a^" + std::to_string(i + 1) + "_"
                                        + std::to_string(j + 1);
                        if (i == 0) {
                            averages[0][j]
                                = (signal[2 * j] + signal[(2 * j) + 1]) / SQRT2;
                            avg_node_name
                                += " ("
                                   + std::format("{:.5}",
                                                 std::to_string(averages[0][j]))
                                   + ")";
                            pruned_avg_graph.add_node(avg_node_name);

                        } else {
                            averages[i][j] = (averages[i - 1][2 * j]
                                              + averages[i - 1][(2 * j) + 1])
                                             / SQRT2;
                            avg_node_name
                                += " ("
                                   + std::format("{:.5}",
                                                 std::to_string(averages[i][j]))
                                   + ")";
                            pruned_avg_graph.add_node(avg_node_name);
                            parent_1
                                = "a^" + std::to_string(i - 1) + "_"
                                  + std::to_string(2 * j + 1) + " ("
                                  + std::format(
                                      "{:.5}",
                                      std::to_string(averages[i - 1][2 * j]))
                                  + ")";
                            parent_2 = "a^" + std::to_string(i - 1) + "_"
                                       + std::to_string(2 * j + 2) + " ("
                                       + std::format(
                                           "{:.5}",
                                           std::to_string(
                                               averages[i - 1][(2 * j) + 1]))
                                       + ")";
                            pruned_avg_graph.add_edge(parent_1, avg_node_name,
                                                      0);
                            pruned_avg_graph.add_edge(parent_2, avg_node_name,
                                                      0);
                        }
                    }
                }
            }
        }
#pragma omp section
        {
            if (type == HaarWaveletGraph::PRUNED_COEFFICIENT
                || type == HaarWaveletGraph::BOTH) {
                std::string coeff_node_name, parent_1, parent_2;
                for (int i = 0; i < d; ++i) {
                    for (int j = 0; j < N / (1 << i); ++j) {
                        coeff_node_name = "a^" + std::to_string(i + 1) + "_"
                                          + std::to_string(j + 1);
                        if (i == 0) {
                            coefficients[0][j]
                                = (signal[2 * j] - signal[(2 * j) + 1]) / SQRT2;
                            coeff_node_name
                                += " ("
                                   + std::format(
                                       "{:.5}",
                                       std::to_string(coefficients[0][j]))
                                   + ")";
                            pruned_coeff_graph.add_node(coeff_node_name);

                        } else {
                            coefficients[i][j]
                                = (coefficients[i - 1][2 * j]
                                   - coefficients[i - 1][(2 * j) + 1])
                                  / SQRT2;
                            coeff_node_name
                                += " ("
                                   + std::format(
                                       "{:.5}",
                                       std::to_string(coefficients[i][j]))
                                   + ")";
                            pruned_coeff_graph.add_node(coeff_node_name);
                            parent_1
                                = "a^" + std::to_string(i - 1) + "_"
                                  + std::to_string(2 * j + 1) + " ("
                                  + std::format("{:.5}",
                                                std::to_string(
                                                    coefficients[i - 1][2 * j]))
                                  + ")";
                            parent_2
                                = "a^" + std::to_string(i - 1) + "_"
                                  + std::to_string(2 * j + 2) + " ("
                                  + std::format(
                                      "{:.5}",
                                      std::to_string(
                                          coefficients[i - 1][(2 * j) + 1]))
                                  + ")";
                            pruned_coeff_graph.add_edge(parent_1,
                                                        coeff_node_name, 0);
                            pruned_coeff_graph.add_edge(parent_2,
                                                        coeff_node_name, 0);
                        }
                    }
                }
            }
        }
    }
    // for debugging
    std::cout << "Averages:\n" << averages;
    std::cout << "Coefficients:\n" << coefficients;

    return {pruned_avg_graph, pruned_coeff_graph};
}
