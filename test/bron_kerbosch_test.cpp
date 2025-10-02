/**
 * @file bron_kerbosch_test.cpp
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "mcis/graph.h"
#include "mcis/mcis_algorithm.h"

class BronKerboschTest : public ::testing::Test {
 protected:
    void SetUp() override {
        generate_diagrams
            = std::getenv("GENERATE_DIAGRAMS") != nullptr
              && std::string(std::getenv("GENERATE_DIAGRAMS")) == "1";
        mcis_algorithm = std::make_unique<MCISAlgorithm>();
    }

    void TearDown() override { mcis_algorithm.reset(); }

    std::unique_ptr<MCISAlgorithm> mcis_algorithm;
    bool generate_diagrams = false;

    void print_mcis_results(const std::vector<Graph*>& results,
                            const std::string& test_name) {
        std::cout << "\n=== " << test_name << " MCIS Results ===\n";
        std::cout << "Found " << results.size() << " MCIS(s)\n";

        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "\nMCIS " << (i + 1) << " has "
                      << results[i]->get_num_nodes() << " nodes:\n";
            results[i]->print_graph();

            if (generate_diagrams) {
                std::string diagram_name
                    = test_name + "_mcis_" + std::to_string(i + 1);
                results[i]->generate_diagram_file(diagram_name);
            }
        }
        std::cout << "=============================\n\n";
    }

    Graph create_simple_triangle() {
        Graph g;
        g.add_node("A");
        g.add_node("B");
        g.add_node("C");
        g.add_edge("A", "B", 1);
        g.add_edge("B", "C", 1);
        g.add_edge("A", "C", 1);
        return g;
    }

    Graph create_simple_path() {
        Graph g;
        g.add_node("X");
        g.add_node("Y");
        g.add_node("Z");
        g.add_edge("X", "Y", 1);
        g.add_edge("Y", "Z", 1);
        return g;
    }

    Graph create_square() {
        Graph g;
        g.add_node("P");
        g.add_node("Q");
        g.add_node("R");
        g.add_node("S");
        g.add_edge("P", "Q", 1);
        g.add_edge("Q", "R", 1);
        g.add_edge("R", "S", 1);
        g.add_edge("S", "P", 1);
        return g;
    }

    Graph create_star_graph(int n) {
        Graph g;
        g.add_node("center");
        for (int i = 1; i <= n; ++i) {
            std::string leaf = "leaf" + std::to_string(i);
            g.add_node(leaf);
            g.add_edge("center", leaf, 1);
        }
        return g;
    }
};

// Test 1: Basic functionality with identical graphs
TEST_F(BronKerboschTest, IdenticalTriangleGraphs) {
    Graph g1 = create_simple_triangle();
    Graph g2 = create_simple_triangle();

    if (generate_diagrams) {
        g1.generate_diagram_file("identical_triangle_g1");
        g2.generate_diagram_file("identical_triangle_g2");
    }

    auto result
        = mcis_algorithm->run(g1, g2, AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    EXPECT_FALSE(results.empty())
        << "MCIS should find at least one result for identical graphs";

    if (!results.empty()) {
        EXPECT_EQ(results[0]->get_num_nodes(), 3)
            << "MCIS of identical triangles should have 3 nodes";
        print_mcis_results(results, "IdenticalTriangles");
    }

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 2: Different graph structures
TEST_F(BronKerboschTest, TriangleVsPath) {
    Graph triangle = create_simple_triangle();
    Graph path = create_simple_path();

    if (generate_diagrams) {
        triangle.generate_diagram_file("triangle_vs_path_triangle");
        path.generate_diagram_file("triangle_vs_path_path");
    }

    auto result = mcis_algorithm->run(triangle, path,
                                      AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    print_mcis_results(results, "TriangleVsPath");

    EXPECT_FALSE(results.empty()) << "Should find some common structure";

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 3: Empty graphs
TEST_F(BronKerboschTest, EmptyGraphs) {
    Graph empty1, empty2;

    auto result = mcis_algorithm->run(empty1, empty2,
                                      AlgorithmType::BRON_KERBOSCH_SERIAL);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), mcis::AlgorithmError::EMPTY_GRAPH);
}

// Test 4: Single node graphs
TEST_F(BronKerboschTest, SingleNodeGraphs) {
    Graph g1, g2;
    g1.add_node("A");
    g2.add_node("B");

    if (generate_diagrams) {
        g1.generate_diagram_file("single_node_g1");
        g2.generate_diagram_file("single_node_g2");
    }

    auto result
        = mcis_algorithm->run(g1, g2, AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    print_mcis_results(results, "SingleNodeGraphs");

    EXPECT_FALSE(results.empty())
        << "Should find single node as common structure";

    if (!results.empty()) {
        EXPECT_EQ(results[0]->get_num_nodes(), 1)
            << "MCIS should have exactly 1 node";
    }

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 5: Complex MVM graphs
TEST_F(BronKerboschTest, MVMGraphComparison) {
    Graph mvm_2x2 = Graph::create_mvm_graph_from_dimensions(2, 2);
    Graph mvm_3x2 = Graph::create_mvm_graph_from_dimensions(3, 2);

    if (generate_diagrams) {
        mvm_2x2.generate_diagram_file("mvm_comparison_2x2");
        mvm_3x2.generate_diagram_file("mvm_comparison_3x2");
    }

    std::cout << "MVM(2,2) has " << mvm_2x2.get_num_nodes() << " nodes\n";
    std::cout << "MVM(3,2) has " << mvm_3x2.get_num_nodes() << " nodes\n";

    auto result = mcis_algorithm->run(mvm_2x2, mvm_3x2,
                                      AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    print_mcis_results(results, "MVMComparison");

    EXPECT_FALSE(results.empty()) << "MVM graphs should have common structure";

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 6: Square vs Triangle
TEST_F(BronKerboschTest, SquareVsTriangle) {
    Graph square = create_square();
    Graph triangle = create_simple_triangle();

    if (generate_diagrams) {
        square.generate_diagram_file("square_vs_triangle_square");
        triangle.generate_diagram_file("square_vs_triangle_triangle");
    }

    auto result = mcis_algorithm->run(square, triangle,
                                      AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    print_mcis_results(results, "SquareVsTriangle");

    EXPECT_FALSE(results.empty())
        << "Should find common structure between square and triangle";

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 7: Star graphs of different sizes
TEST_F(BronKerboschTest, StarGraphComparison) {
    Graph star3 = create_star_graph(3);
    Graph star5 = create_star_graph(5);

    if (generate_diagrams) {
        star3.generate_diagram_file("star_comparison_star3");
        star5.generate_diagram_file("star_comparison_star5");
    }

    auto result = mcis_algorithm->run(star3, star5,
                                      AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    print_mcis_results(results, "StarComparison");

    EXPECT_FALSE(results.empty()) << "Star graphs should have common structure";

    if (!results.empty()) {
        // Should find at least the star with 3 leaves as common structure
        EXPECT_GE(results[0]->get_num_nodes(), 4)
            << "MCIS should have at least 4 nodes (center + 3 leaves)";
    }

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 8: Disconnected components
TEST_F(BronKerboschTest, DisconnectedComponents) {
    Graph g1, g2;

    // Graph 1: Two disconnected triangles
    g1.add_node("A1");
    g1.add_node("B1");
    g1.add_node("C1");
    g1.add_edge("A1", "B1", 1);
    g1.add_edge("B1", "C1", 1);
    g1.add_edge("A1", "C1", 1);

    g1.add_node("A2");
    g1.add_node("B2");
    g1.add_node("C2");
    g1.add_edge("A2", "B2", 1);
    g1.add_edge("B2", "C2", 1);
    g1.add_edge("A2", "C2", 1);

    // Graph 2: One triangle and some isolated nodes
    g2.add_node("X");
    g2.add_node("Y");
    g2.add_node("Z");
    g2.add_edge("X", "Y", 1);
    g2.add_edge("Y", "Z", 1);
    g2.add_edge("X", "Z", 1);

    g2.add_node("P");
    g2.add_node("Q");

    if (generate_diagrams) {
        g1.generate_diagram_file("disconnected_g1");
        g2.generate_diagram_file("disconnected_g2");
    }

    auto result
        = mcis_algorithm->run(g1, g2, AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    print_mcis_results(results, "DisconnectedComponents");

    EXPECT_FALSE(results.empty())
        << "Should find common structure in disconnected graphs";

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 9: Performance test with larger MVM graphs
TEST_F(BronKerboschTest, LargerMVMPerformance) {
    Graph mvm_4x3 = Graph::create_mvm_graph_from_dimensions(4, 3);
    Graph mvm_3x4 = Graph::create_mvm_graph_from_dimensions(3, 4);

    std::cout << "Testing performance with MVM(4,3) vs MVM(3,4)\n";
    std::cout << "MVM(4,3) has " << mvm_4x3.get_num_nodes() << " nodes\n";
    std::cout << "MVM(3,4) has " << mvm_3x4.get_num_nodes() << " nodes\n";

    if (generate_diagrams) {
        mvm_4x3.generate_diagram_file("performance_mvm_4x3");
        mvm_3x4.generate_diagram_file("performance_mvm_3x4");
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    auto result = mcis_algorithm->run(mvm_4x3, mvm_3x4,
                                      AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);

    std::cout << "Algorithm completed in " << duration.count()
              << " milliseconds\n";

    print_mcis_results(results, "LargerMVMPerformance");

    EXPECT_FALSE(results.empty()) << "Should find MCIS for larger MVM graphs";

    for (auto* graph : results) {
        delete graph;
    }
}

// Test 10: Custom named graphs matching specific patterns
TEST_F(BronKerboschTest, CustomNamedGraphs) {
    Graph g1, g2;

    // Create two similar but not identical graph patterns
    // Graph 1: A diamond pattern
    g1.add_node("TOP");
    g1.add_node("LEFT");
    g1.add_node("RIGHT");
    g1.add_node("BOTTOM");
    g1.add_edge("TOP", "LEFT", 1);
    g1.add_edge("TOP", "RIGHT", 1);
    g1.add_edge("LEFT", "BOTTOM", 1);
    g1.add_edge("RIGHT", "BOTTOM", 1);

    // Graph 2: A diamond with extra cross edge
    g2.add_node("NORTH");
    g2.add_node("WEST");
    g2.add_node("EAST");
    g2.add_node("SOUTH");
    g2.add_node("CENTER");
    g2.add_edge("NORTH", "WEST", 1);
    g2.add_edge("NORTH", "EAST", 1);
    g2.add_edge("WEST", "SOUTH", 1);
    g2.add_edge("EAST", "SOUTH", 1);
    g2.add_edge("WEST", "EAST", 1);
    g2.add_edge("CENTER", "NORTH", 1);

    if (generate_diagrams) {
        g1.generate_diagram_file("custom_diamond");
        g2.generate_diagram_file("custom_diamond_extended");
    }

    auto result
        = mcis_algorithm->run(g1, g2, AlgorithmType::BRON_KERBOSCH_SERIAL);
    ASSERT_TRUE(result.has_value());
    std::vector<Graph*> results = *result;

    print_mcis_results(results, "CustomNamedGraphs");

    EXPECT_FALSE(results.empty()) << "Should find common diamond structure";

    for (auto* graph : results) {
        delete graph;
    }
}
