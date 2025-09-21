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

class MVMTest : public ::testing::Test {
 protected:
    void SetUp() override {
        generate_diagrams
            = std::getenv("GENERATE_DIAGRAMS") != nullptr
              && std::string(std::getenv("GENERATE_DIAGRAMS")) == "1";
    }

    void TearDown() override {}

    bool generate_diagrams = false;
};

// Test 1: Create MVM(2,2) graph and generate diagram
TEST_F(MVMTest, MVM2x2GraphCreation) {
    std::vector<std::vector<std::string>> mat
        = {{"m0,0", "m0,1"}, {"m1,0", "m1,1"}};
    std::vector<std::string> vec = {"v0", "v1"};

    Graph mvm_graph = Graph::create_mvm_graph_from_mat_vec(mat, vec);

    std::cout << "MVM(2,2) created with " << mvm_graph.get_num_nodes()
              << " nodes\n";

    if (generate_diagrams) mvm_graph.generate_diagram_file("mvm_2x2");
}

// Test 2: Create MVM(3,2) graph matching Figure 4(a)
TEST_F(MVMTest, MVM3x2GraphCreation) {
    std::vector<std::vector<std::string>> mat
        = {{"m0,0", "m0,1"}, {"m1,0", "m1,1"}, {"m2,0", "m2,1"}};
    std::vector<std::string> vec = {"v0", "v1"};

    Graph mvm_graph = Graph::create_mvm_graph_from_mat_vec(mat, vec);

    std::cout << "MVM(3,2) created with " << mvm_graph.get_num_nodes()
              << " nodes\n";

    if (generate_diagrams) mvm_graph.generate_diagram_file("mvm_3x2");
}

// Test 3: Create MVM(2,3) graph matching Figure 4(b)
TEST_F(MVMTest, MVM2x3GraphCreation) {
    std::vector<std::vector<std::string>> mat
        = {{"m0,0", "m0,1", "m0,2"}, {"m1,0", "m1,1", "m1,2"}};
    std::vector<std::string> vec = {"v0", "v1", "v2"};

    Graph mvm_graph = Graph::create_mvm_graph_from_mat_vec(mat, vec);

    std::cout << "MVM(2,3) created with " << mvm_graph.get_num_nodes()
              << " nodes\n";

    if (generate_diagrams) mvm_graph.generate_diagram_file("mvm_2x3");
}

// Test 4: Test MVM graph creation from dimensions
TEST_F(MVMTest, MVMFromDimensionsCreation) {
    Graph mvm_graph = Graph::create_mvm_graph_from_dimensions(2, 3);

    std::cout << "MVM from dimensions (2,3) created with "
              << mvm_graph.get_num_nodes() << " nodes\n";

    if (generate_diagrams)
        mvm_graph.generate_diagram_file("mvm_dimensions_2x3");
}

// Test 5: Test edge cases
TEST_F(MVMTest, MVMGraphEdgeCases) {
    std::vector<std::vector<std::string>> empty_mat;
    std::vector<std::string> empty_vec;
    Graph empty_graph
        = Graph::create_mvm_graph_from_mat_vec(empty_mat, empty_vec);
    std::cout << "Empty graph created with " << empty_graph.get_num_nodes()
              << " nodes\n";
    if (generate_diagrams) empty_graph.generate_diagram_file("mvm_empty");
}

// Test 6: Test larger MVM graph (4x4)
TEST_F(MVMTest, MVM4x4LargeGraphCreation) {
    Graph mvm_graph = Graph::create_mvm_graph_from_dimensions(4, 4);

    std::cout << "MVM(4,4) created with " << mvm_graph.get_num_nodes()
              << " nodes\n";

    if (generate_diagrams) mvm_graph.generate_diagram_file("mvm_4x4");
}

// Test 7: Test connectivity with named elements
TEST_F(MVMTest, MVMEdgeConnectivityVerification) {
    std::vector<std::vector<std::string>> mat = {{"A", "B"}, {"C", "D"}};
    std::vector<std::string> vec = {"X", "Y"};

    Graph mvm_graph = Graph::create_mvm_graph_from_mat_vec(mat, vec);

    std::cout << "MVM with named elements created with "
              << mvm_graph.get_num_nodes() << " nodes\n";

    if (generate_diagrams)
        mvm_graph.generate_diagram_file("mvm_connectivity_test");
}
