/**
 * @file
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

#include <iostream>
#include <string>
#include <vector>

Graph Graph::create_mvm_graph_from_mat_vec(
    const std::vector<std::vector<std::string>>& mat,
    const std::vector<std::string>& vec, bool from_dimensions) {
    Graph graph;
    int m = static_cast<int>(mat.size());
    int n = static_cast<int>(vec.size());
    if (m <= 0 || n <= 0 || mat[0].size() != static_cast<size_t>(n)) {
        return graph;
    }

    graph.reserve_nodes((m * n) + n + (m * n) + (m * (n - 1)));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            graph.add_node(mat[i][j]);
        }
    }
    for (int j = 0; j < n; ++j) {
        graph.add_node(vec[j]);
    }

    for (int i = 1; i <= (m * n); ++i) {
        std::string product_node = "v^2_" + std::to_string(i);
        graph.add_node(product_node);
    }

    for (int set = 3; set <= n + 1; ++set) {
        for (int i = 1; i <= m; ++i) {
            std::string acc_node
                = "v^" + std::to_string(set) + "_" + std::to_string(i);
            graph.add_node(acc_node);
        }
    }

    // add edges from 4.1.1
    // edges from the vector elements
    int incr = 0;
    for (int j = 1; j < (m * n + n); j = j + m + 1) {
        std::string from_node = vec[incr];
        ++incr;
        int k = (j - 1) / (m + 1);
        for (int i = 0; i < m; ++i) {
            std::string to_node = "v^2_" + std::to_string(j - k + i);
            std::cout << "Adding edge from " << from_node << " to " << to_node
                      << std::endl;
            graph.add_edge(from_node, to_node, 0);
        }
    }

    // edges from the matrix elements
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < m; ++i) {
            std::string from_node = mat[i][j];
            std::string to_node = "v^2_" + std::to_string(i + ((m * j) + 1));
            graph.add_edge(from_node, to_node, 0);
        }
    }

    // edges from 4.1.2
    for (int i = 2; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            std::string from_node
                = "v^" + std::to_string(i) + "_" + std::to_string(j);
            std::string to_node
                = "v^" + std::to_string(i + 1) + "_" + std::to_string(j);
            graph.add_edge(from_node, to_node, 0);
        }
    }

    // edges from 4.1.3

    for (int j = m + 1; j <= m * n; ++j) {
        std::string from_node;
        std::string to_node;
        if (j % m == 0) {
            from_node = "v^2_" + std::to_string(j);
            to_node = "v^" + std::to_string(2 + ((j - 1) / m)) + "_"
                      + std::to_string(m);
        } else {
            from_node = "v^2_" + std::to_string(j);
            to_node = "v^" + std::to_string(2 + ((j - 1) / m)) + "_"
                      + std::to_string(j % m);
        }
        graph.add_edge(from_node, to_node, 0);
    }

    return graph;
}

Graph Graph::create_mvm_graph_from_dimensions(int m, int n) {
    if (m <= 0 || n <= 0) {
        return Graph();
    }
    std::vector<std::vector<std::string>> mat(m, std::vector<std::string>(n));
    std::vector<std::string> vec(n);

    const bool use_parallel = (m * n >= MVM_PARALLEL_THRESHOLD);

    if (use_parallel) {
#pragma omp parallel for
        for (int i = 0; i < n; ++i) {
            int z = ((m + 1) * i) + 1;
            vec[i] = "v^1_" + std::to_string(z);
            for (int j = 0; j < m; ++j) {
                mat[j][i] = "v^1_" + std::to_string(z + j + 1);
            }
        }
    } else {
        for (int i = 0; i < n; ++i) {
            int z = ((m + 1) * i) + 1;
            vec[i] = "v^1_" + std::to_string(z);
            for (int j = 0; j < m; ++j) {
                mat[j][i] = "v^1_" + std::to_string(z + j + 1);
            }
        }
    }

    return create_mvm_graph_from_mat_vec(mat, vec, true);
}
