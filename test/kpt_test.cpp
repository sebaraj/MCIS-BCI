/**
 * @file kpt_test.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "mcis/graph.h"
#include "mcis/mcis_algorithm.h"

class KPTTest : public ::testing::Test {
 protected:
    void SetUp() override {
        mcis_algorithm = std::make_unique<MCISAlgorithm>();
    }

    void TearDown() override { mcis_algorithm.reset(); }

    std::unique_ptr<MCISAlgorithm> mcis_algorithm;
};

// Test with two simple graphs where a matching is possible
TEST_F(KPTTest, SimpleMatching) {
    Graph g1;
    g1.add_node("A1");
    g1.add_node("B1");
    g1.add_edge("A1", "B1", 1);

    Graph g2;
    g2.add_node("A2");
    g2.add_node("B2");
    g2.add_edge("A2", "B2", 1);

    std::vector<const Graph*> graphs = {&g1, &g2};
    auto result = mcis_algorithm->run(graphs, AlgorithmType::KPT);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;
    EXPECT_FALSE(results.empty());
    if (!results.empty()) {
        EXPECT_GT(results[0]->get_num_nodes(), 0);
    }
    for (auto* graph : results) {
        delete graph;
    }
}

// Test with conflicting nodes
TEST_F(KPTTest, ConflictingNodes) {
    Graph g1;
    g1.add_node("A1");
    g1.add_node("B1");
    g1.add_edge("A1", "B1",
                1);  // A1 -> B1, so (A1, X) and (B1, Y) hyperedges conflict

    Graph g2;
    g2.add_node("A2");
    g2.add_node("B2");

    std::vector<const Graph*> graphs = {&g1, &g2};
    auto result = mcis_algorithm->run(graphs, AlgorithmType::KPT);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;
    EXPECT_FALSE(results.empty());
    if (!results.empty()) {
        // The matching should be smaller than the total number of possible
        // hyperedges Total possible hyperedges is 4: (A1,A2), (A1,B2), (B1,A2),
        // (B1,B2) A valid matching cannot contain conflicting hyperedges. E.g.,
        // if (A1,A2) is in matching, (B1,A2) and (B1,B2) cannot be.
        EXPECT_LE(results[0]->get_num_nodes(), 2);
    }
    for (auto* graph : results) {
        delete graph;
    }
}

// Test with empty graphs
TEST_F(KPTTest, EmptyGraphs) {
    Graph empty1, empty2;
    std::vector<const Graph*> graphs = {&empty1, &empty2};
    auto result = mcis_algorithm->run(graphs, AlgorithmType::KPT);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), mcis::AlgorithmError::EMPTY_GRAPH);
}
