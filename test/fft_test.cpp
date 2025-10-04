/**
 * @file fft_test.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <cstdlib>
#include <string>

#include "mcis/graph.h"

class FFTGraphTest : public ::testing::Test {
 protected:
    void SetUp() override {
        generate_diagrams
            = std::getenv("GENERATE_DIAGRAMS") != nullptr
              && std::string(std::getenv("GENERATE_DIAGRAMS")) == "1";
    }

    void TearDown() override {}

    bool generate_diagrams = false;
};

TEST_F(FFTGraphTest, InvalidParameters) {
    auto graph_or_error = Graph::create_fft_graph_from_dimensions(0);
    EXPECT_FALSE(graph_or_error.has_value());

    graph_or_error = Graph::create_fft_graph_from_dimensions(7);
    EXPECT_FALSE(graph_or_error.has_value());
}

TEST_F(FFTGraphTest, N2_GraphStructure) {
    auto graph_or_error = Graph::create_fft_graph_from_dimensions(2);
    ASSERT_TRUE(graph_or_error.has_value());
    auto graph = graph_or_error.value();

    EXPECT_EQ(graph.get_num_nodes(), 6);

    EXPECT_NE(graph.get_node("x_0"), nullptr);
    EXPECT_NE(graph.get_node("x_1"), nullptr);
    EXPECT_NE(graph.get_node("s1_0"), nullptr);
    EXPECT_NE(graph.get_node("s1_1"), nullptr);
    EXPECT_NE(graph.get_node("X_0"), nullptr);
    EXPECT_NE(graph.get_node("X_1"), nullptr);

    // Check edges for butterfly
    EXPECT_TRUE(graph.get_node("s1_0")->check_parent("x_0"));
    EXPECT_TRUE(graph.get_node("s1_0")->check_parent("x_1"));
    EXPECT_TRUE(graph.get_node("s1_1")->check_parent("x_0"));
    EXPECT_TRUE(graph.get_node("s1_1")->check_parent("x_1"));

    EXPECT_TRUE(graph.get_node("X_0")->check_parent("s1_0"));
    EXPECT_TRUE(graph.get_node("X_1")->check_parent("s1_1"));

    if (generate_diagrams) {
        graph.generate_diagram_file("fft_n2");
    }
}

TEST_F(FFTGraphTest, N4_GraphStructure) {
    auto graph_or_error = Graph::create_fft_graph_from_dimensions(4);
    ASSERT_TRUE(graph_or_error.has_value());
    auto graph = graph_or_error.value();

    EXPECT_EQ(graph.get_num_nodes(), 16);

    EXPECT_NE(graph.get_node("x_0"), nullptr);
    EXPECT_NE(graph.get_node("x_3"), nullptr);
    EXPECT_NE(graph.get_node("s1_0"), nullptr);
    EXPECT_NE(graph.get_node("s1_3"), nullptr);
    EXPECT_NE(graph.get_node("s2_0"), nullptr);
    EXPECT_NE(graph.get_node("s2_3"), nullptr);
    EXPECT_NE(graph.get_node("X_0"), nullptr);
    EXPECT_NE(graph.get_node("X_3"), nullptr);

    EXPECT_TRUE(graph.get_node("s1_0")->check_parent("x_0"));
    EXPECT_TRUE(graph.get_node("s1_2")->check_parent("x_2"));
    EXPECT_TRUE(graph.get_node("s2_0")->check_parent("s1_0"));
    EXPECT_TRUE(graph.get_node("s2_1")->check_parent("s1_1"));
    EXPECT_TRUE(graph.get_node("X_0")->check_parent("s2_0"));

    if (generate_diagrams) {
        graph.generate_diagram_file("fft_n4");
    }
}

TEST_F(FFTGraphTest, N8_GraphStructure) {
    auto graph_or_error = Graph::create_fft_graph_from_dimensions(8);
    ASSERT_TRUE(graph_or_error.has_value());
    auto graph = graph_or_error.value();

    int n = 8;
    int stages = log2(n);
    int expected_nodes = n + n * stages + n;
    EXPECT_EQ(graph.get_num_nodes(), expected_nodes);

    EXPECT_NE(graph.get_node("x_0"), nullptr);
    EXPECT_NE(graph.get_node("x_7"), nullptr);
    EXPECT_NE(graph.get_node("s1_0"), nullptr);
    EXPECT_NE(graph.get_node("s2_0"), nullptr);
    EXPECT_NE(graph.get_node("s3_0"), nullptr);
    EXPECT_NE(graph.get_node("X_0"), nullptr);
    EXPECT_NE(graph.get_node("X_7"), nullptr);

    EXPECT_TRUE(graph.get_node("s1_0")->check_parent("x_0"));
    EXPECT_TRUE(graph.get_node("s1_0")->check_parent("x_4"));
    EXPECT_TRUE(graph.get_node("s2_0")->check_parent("s1_0"));
    EXPECT_TRUE(graph.get_node("s2_0")->check_parent("s1_2"));
    EXPECT_TRUE(graph.get_node("s3_0")->check_parent("s2_0"));
    EXPECT_TRUE(graph.get_node("s3_0")->check_parent("s2_1"));
    EXPECT_TRUE(graph.get_node("X_0")->check_parent("s3_0"));

    if (generate_diagrams) {
        graph.generate_diagram_file("fft_n8");
    }
}
