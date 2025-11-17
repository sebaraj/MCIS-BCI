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
    auto dwt_graph_expected
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(8, 3);
    ASSERT_TRUE(dwt_graph_expected.has_value());
    std::vector<Graph> dwt_graphs = dwt_graph_expected.value();

    if (generate_diagrams) {
        int i = 0;
        for (const auto& g : dwt_graphs) {
            g.generate_diagram_file("dwt_" + std::to_string(i++));
        }
    }
}

// Test 2: Test invalid parameters
TEST_F(DWTTest, DWTGraphInvalidParameters) {
    auto dwt_graph_expected
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(7, 3);
    ASSERT_FALSE(dwt_graph_expected.has_value());
    ASSERT_EQ(dwt_graph_expected.error(), mcis::GraphError::INVALID_PARAMETERS);
}

// Test 3: Create DWT graph with n=16, d=4
TEST_F(DWTTest, DWTGraphCreation_n16_d4) {
    auto dwt_graph_expected
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(16, 4);
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

// Test 4: Test from_signal constructor for correctness
TEST_F(DWTTest, DWTGraphFromSignalCorrectness) {
    std::vector<double> signal = {9.0, 7.0, 5.0, 3.0};
    auto dwt_graphs_expected
        = Graph::create_haar_wavelet_transform_graph_from_signal(signal);
    ASSERT_TRUE(dwt_graphs_expected.has_value());
    auto dwt_graphs = dwt_graphs_expected.value();
    ASSERT_EQ(dwt_graphs.size(), static_cast<size_t>(2));

    Graph avg_graph = dwt_graphs[0];
    Graph coeff_graph = dwt_graphs[1];

    // Check for final average value: ( ( (9+7)/sqrt(2) + (5+3)/sqrt(2) ) /
    // sqrt(2) ) = 12
    Node* final_avg_node = avg_graph.get_node("a^1_0");
    ASSERT_NE(final_avg_node, nullptr);
    EXPECT_NEAR(std::stod(final_avg_node->get_tag()), 12.0, 1e-9);

    // Check for final coefficient value: ( ( (9+7)/sqrt(2) - (5+3)/sqrt(2) ) /
    // sqrt(2) ) = 4
    Node* final_coeff_node = coeff_graph.get_node("d^1_0");
    ASSERT_NE(final_coeff_node, nullptr);
    EXPECT_NEAR(std::stod(final_coeff_node->get_tag()), 4.0, 1e-9);

    if (generate_diagrams) {
        avg_graph.generate_diagram_file("dwt_from_signal_avg_correctness");
        coeff_graph.generate_diagram_file("dwt_from_signal_coeff_correctness");
    }
}

// Test 5: Test PRUNED_AVERAGE graph type
TEST_F(DWTTest, DWTGraphPrunedAverage) {
    auto dwt_graph_expected
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(
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
    auto dwt_graph_expected
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(
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

// Test 7: Verify the structure of the coefficient graph
TEST_F(DWTTest, DWTGraphStructureCorrectness) {
    std::vector<double> signal = {9.0, 7.0, 5.0, 3.0};
    auto dwt_graphs_expected
        = Graph::create_haar_wavelet_transform_graph_from_signal(
            signal, HaarWaveletGraph::PRUNED_COEFFICIENT);
    ASSERT_TRUE(dwt_graphs_expected.has_value());
    auto dwt_graphs = dwt_graphs_expected.value();
    ASSERT_EQ(dwt_graphs.size(), 1);

    Graph coeff_graph = dwt_graphs[0];

    Node* d_node = coeff_graph.get_node("d^0_0");
    ASSERT_NE(d_node, nullptr) << "No coefficient node found in the graph.";

    const auto& parents = d_node->get_parents();
    EXPECT_EQ(parents.size(), 2) << "Coefficient node should have two parents.";

    bool s0_parent_found = false;
    bool s1_parent_found = false;
    for (const auto& parent : parents) {
        if (parent.first->get_id() == "s_0") {
            s0_parent_found = true;
        }
        if (parent.first->get_id() == "s_1") {
            s1_parent_found = true;
        }
    }
    EXPECT_TRUE(s0_parent_found) << "s_0 parent not found for d^0_0";
    EXPECT_TRUE(s1_parent_found) << "s_1 parent not found for d^0_0";
}
