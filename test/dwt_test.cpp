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
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "mcis/graph.h"

class DWTTest : public ::testing::Test {
 protected:
    void SetUp() override {
        generate_diagrams
            = std::getenv("GENERATE_DIAGRAMS") != nullptr
              && std::string(std::getenv("GENERATE_DIAGRAMS")) == "1";
    }

    void TearDown() override {}

    bool generate_diagrams = false;
};

// Test 1: Create DWT graph and generate diagram
TEST_F(DWTTest, DWTGraphCreation) {
    Graph graph;
    auto dwt_graph_expected
        = graph.create_haar_wavelet_transform_graph_from_dimensions(8, 3);
    ASSERT_TRUE(dwt_graph_expected.has_value());
    std::vector<Graph> dwt_graphs = dwt_graph_expected.value();

    std::cout << "DWT created with " << dwt_graphs.size() << " graphs\n";

    if (generate_diagrams) {
        int i = 0;
        for (const auto& g : dwt_graphs) {
            g.generate_diagram_file("dwt_" + std::to_string(i++));
        }
    }
}

// Test 2: Test invalid parameters
TEST_F(DWTTest, DWTGraphInvalidParameters) {
    Graph graph;
    auto dwt_graph_expected
        = graph.create_haar_wavelet_transform_graph_from_dimensions(7, 3);
    ASSERT_FALSE(dwt_graph_expected.has_value());
    ASSERT_EQ(dwt_graph_expected.error(), mcis::GraphError::INVALID_PARAMETERS);
}

// Test 3: Create DWT graph with n=16, d=4
TEST_F(DWTTest, DWTGraphCreation_n16_d4) {
    Graph graph;
    auto dwt_graph_expected
        = graph.create_haar_wavelet_transform_graph_from_dimensions(16, 4);
    ASSERT_TRUE(dwt_graph_expected.has_value());
    auto dwt_graphs = dwt_graph_expected.value();
    EXPECT_EQ(dwt_graphs.size(), static_cast<size_t>(2));

    if (generate_diagrams) {
        int i = 0;
        for (const auto& g : dwt_graphs) {
            g.generate_diagram_file("dwt_n16_d4_" + std::to_string(i++));
        }
    }
}

// Test 4: Test from_signal constructor
TEST_F(DWTTest, DWTGraphFromSignal) {
    std::vector<double> signal = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    auto dwt_graphs
        = Graph::create_haar_wavelet_transform_graph_from_signal(signal);
    EXPECT_EQ(dwt_graphs.size(), static_cast<size_t>(2));

    if (generate_diagrams) {
        int i = 0;
        for (const auto& g : dwt_graphs) {
            g.generate_diagram_file("dwt_from_signal_" + std::to_string(i++));
        }
    }
}

// Test 5: Test PRUNED_AVERAGE graph type
TEST_F(DWTTest, DWTGraphPrunedAverage) {
    Graph graph;
    auto dwt_graph_expected
        = graph.create_haar_wavelet_transform_graph_from_dimensions(
            8, 3, 1, HaarWaveletGraph::PRUNED_AVERAGE);
    ASSERT_TRUE(dwt_graph_expected.has_value());
    std::vector<Graph> dwt_graphs = dwt_graph_expected.value();
    EXPECT_EQ(dwt_graphs.size(), 1);

    if (generate_diagrams) {
        int i = 0;
        for (const auto& g : dwt_graphs) {
            g.generate_diagram_file("dwt_pruned_average_"
                                    + std::to_string(i++));
        }
    }
}

// Test 6: Test PRUNED_COEFFICIENT graph type
TEST_F(DWTTest, DWTGraphPrunedCoefficient) {
    Graph graph;
    auto dwt_graph_expected
        = graph.create_haar_wavelet_transform_graph_from_dimensions(
            8, 3, 1, HaarWaveletGraph::PRUNED_COEFFICIENT);
    ASSERT_TRUE(dwt_graph_expected.has_value());
    std::vector<Graph> dwt_graphs = dwt_graph_expected.value();
    EXPECT_EQ(dwt_graphs.size(), 1);

    if (generate_diagrams) {
        int i = 0;
        for (const auto& g : dwt_graphs) {
            g.generate_diagram_file("dwt_pruned_coefficient_"
                                    + std::to_string(i++));
        }
    }
}
