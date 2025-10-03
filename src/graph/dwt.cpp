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

#include <iostream>
#include <vector>

const double SQRT2 = sqrt(2);

Graph Graph::create_haar_wavelet_transform_graph_from_dimensions(int n, int d,
                                                                 int k) {
    if (n <= 0 || d <= 0 || ((n / k) & ((n / k) - 1)) != 0) {
        return Graph();
    }
    // TODO:
    return Graph();
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

Graph Graph::create_haar_wavelet_transform_graph_from_signal(
    const std::vector<double>& signal) {
    int N = static_cast<int>(signal.size());
    if (N <= 0) {
        return Graph();
    }
    int d = static_cast<int>(log2(N));

    std::vector<std::vector<double>> averages(d);
    std::vector<std::vector<double>> coefficients(d);
    for (int i = 0; i < d; ++i) {
        averages[i] = std::vector<double>((N / (1 << i)), 0.0);
        coefficients[i] = std::vector<double>((N / (1 << i)), 0.0);
    }
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < N / (1 << i); ++j) {
            if (i == 0) {
                averages[0][j] = (signal[2 * j] + signal[(2 * j) + 1]) / SQRT2;
                coefficients[0][j]
                    = (signal[2 * j] - signal[(2 * j) + 1]) / SQRT2;
            } else {
                averages[i][j]
                    = (averages[i - 1][2 * j] + averages[i - 1][(2 * j) + 1])
                      / SQRT2;
                coefficients[i][j]
                    = (averages[i - 1][2 * j] - averages[i - 1][(2 * j) + 1])
                      / SQRT2;
            }
        }
    }
    // NOTE: for debugging
    std::cout << "Averages:\n" << averages;
    std::cout << "Coefficients:\n" << coefficients;

    // create nodes w/ averages and coefficients as labels

    // create edges between nodes based on DWT structure

    return Graph();
}
