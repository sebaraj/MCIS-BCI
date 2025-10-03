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
