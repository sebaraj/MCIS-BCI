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

    // Check for final average value: ( ( (9+7)/sqrt(2) + (5+3)/sqrt(2) ) / sqrt(2) ) = 12
    bool final_avg_found = false;
    for (const auto& [id, node] : avg_graph.get_nodes()) {
        if (id.find("(12.0") != std::string::npos) {
            final_avg_found = true;
            break;
        }
    }
    EXPECT_TRUE(final_avg_found) << "Final average node with value 12.0 not found.";

    // Check for final coefficient value: ( ( (9+7)/sqrt(2) - (5+3)/sqrt(2) ) / sqrt(2) ) = 4
    bool final_coeff_found = false;
    for (const auto& [id, node] : coeff_graph.get_nodes()) {
        if (id.find("d^2_1 (4.0") != std::string::npos) {
            final_coeff_found = true;
            break;
        }
    }
    EXPECT_TRUE(final_coeff_found) << "Final coefficient node with value 4.0 not found.";

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
            g.generate_diagram_file("dwt_pruned_average_" + std::to_string(i++));
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
            g.generate_diagram_file("dwt_pruned_coefficient_" + std::to_string(i++));
        }
    }
}

// Test 7: Verify the structure of the coefficient graph
TEST_F(DWTTest, DWTGraphStructureCorrectness) {
    std::vector<double> signal = {9.0, 7.0, 5.0, 3.0};
    auto dwt_graphs_expected = Graph::create_haar_wavelet_transform_graph_from_signal(
        signal, HaarWaveletGraph::PRUNED_COEFFICIENT);
    ASSERT_TRUE(dwt_graphs_expected.has_value());
    auto dwt_graphs = dwt_graphs_expected.value();
    ASSERT_EQ(dwt_graphs.size(), 1);

    Graph coeff_graph = dwt_graphs[0];

    Node* d_node = nullptr;
    for (const auto& [id, node] : coeff_graph.get_nodes()) {
        if (id.rfind("d^", 0) == 0) { // if node name starts with d^
            d_node = node;
            break;
        }
    }

    ASSERT_NE(d_node, nullptr) << "No coefficient node found in the graph.";

    const auto& parents = d_node->get_parents();
    EXPECT_FALSE(parents.empty()) << "Coefficient node should have parents.";

    bool parent_is_a_node = false;
    for (const auto& parent : parents) {
        if (parent.first->get_id().rfind("a^", 0) == 0) { // if parent name starts with a^
            parent_is_a_node = true;
            break;
        }
    }
    EXPECT_TRUE(parent_is_a_node) << "Coefficient node parent must be an average node.";
}