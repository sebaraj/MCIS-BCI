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
#include "mcis/graph.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <unordered_set>

#include "gtest/gtest.h"
#include "mcis/node.h"

class GraphTest : public ::testing::Test {
 protected:
    void SetUp() override {
        graph = std::make_unique<Graph>();
        generate_diagrams
            = std::getenv("GENERATE_DIAGRAMS") != nullptr
              && std::string(std::getenv("GENERATE_DIAGRAMS")) == "1";
    }

    void TearDown() override { graph.reset(); }

    std::unique_ptr<Graph> graph;
    bool generate_diagrams = false;
};

// Test 1: Verifies default constructor creates empty graph with DAG property
TEST_F(GraphTest, DefaultConstructorAndInitialState) {
    EXPECT_EQ(graph->get_num_nodes(), 0);
    EXPECT_TRUE(graph->get_nodes().empty());
    EXPECT_TRUE(graph->is_dag());
}

// Test 2: Tests constructor that creates graph from vector of nodes
TEST_F(GraphTest, VectorConstructor) {
    std::vector<Node> nodes;
    nodes.emplace_back("A");
    nodes.emplace_back("B");
    nodes.emplace_back("C");

    Graph graph_from_vector(nodes);

    EXPECT_EQ(graph_from_vector.get_num_nodes(), 3);
    EXPECT_NE(graph_from_vector.get_node("A"), nullptr);
    EXPECT_NE(graph_from_vector.get_node("B"), nullptr);
    EXPECT_NE(graph_from_vector.get_node("C"), nullptr);
    EXPECT_EQ(graph_from_vector.get_node("D"), nullptr);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_vector_constructor");
}

// Test 3: Validates copy constructor behavior with empty graph
TEST_F(GraphTest, CopyConstructorEmpty) {
    Graph copied_graph(*graph);

    EXPECT_EQ(copied_graph.get_num_nodes(), 0);
    EXPECT_TRUE(copied_graph.get_nodes().empty());
    EXPECT_TRUE(*graph == copied_graph);
}

// Test 4: Ensures copy constructor creates independent copy of populated graph
TEST_F(GraphTest, CopyConstructorPopulated) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_node("C");
    graph->add_edge("A", "B", 10);
    graph->add_edge("B", "C", 20);

    Graph copied_graph(*graph);

    EXPECT_EQ(copied_graph.get_num_nodes(), 3);
    EXPECT_TRUE(*graph == copied_graph);

    graph->add_node("D");
    EXPECT_NE(copied_graph.get_num_nodes(), graph->get_num_nodes());
    EXPECT_FALSE(*graph == copied_graph);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_copy_constructor_original");
    if (generate_diagrams)
        copied_graph.generate_diagram_file("graph_copy_constructor_copy");
}

// Test 5: Tests copy assignment operator handles existing state and
// self-assignment
TEST_F(GraphTest, CopyAssignmentOperator) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_edge("A", "B", 15);

    Graph assigned_graph;
    assigned_graph.add_node("X");

    assigned_graph = *graph;

    EXPECT_EQ(assigned_graph.get_num_nodes(), 2);
    EXPECT_TRUE(*graph == assigned_graph);
    EXPECT_EQ(assigned_graph.get_node("X"), nullptr);

    EXPECT_EQ(assigned_graph.get_num_nodes(), 2);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_assign_constructor_original");
    if (generate_diagrams)
        assigned_graph.generate_diagram_file("graph_assign_constructor_copy");
}

// Test 6: Verifies move constructor transfers resources and empties source
TEST_F(GraphTest, MoveConstructor) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_edge("A", "B", 25);

    int original_node_count = graph->get_num_nodes();

    Graph moved_graph(std::move(*graph));

    EXPECT_EQ(moved_graph.get_num_nodes(), original_node_count);
    EXPECT_EQ(graph->get_num_nodes(), 0);
    EXPECT_NE(moved_graph.get_node("A"), nullptr);
    EXPECT_NE(moved_graph.get_node("B"), nullptr);
    EXPECT_EQ(graph->get_node("A"), nullptr);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_move_constructor_original");
    if (generate_diagrams)
        moved_graph.generate_diagram_file("graph_move_constructor_move");
}

// Test 7: Tests move assignment operator resource transfer and cleanup
TEST_F(GraphTest, MoveAssignmentOperator) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_edge("A", "B", 30);

    Graph assigned_graph;
    assigned_graph.add_node("Y");

    assigned_graph = std::move(*graph);

    EXPECT_EQ(assigned_graph.get_num_nodes(), 2);
    EXPECT_EQ(graph->get_num_nodes(), 0);
    EXPECT_NE(assigned_graph.get_node("A"), nullptr);
    EXPECT_EQ(assigned_graph.get_node("Y"), nullptr);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_move_assign_constructor_original");
    if (generate_diagrams)
        assigned_graph.generate_diagram_file(
            "graph_move_assign_constructor_moved");
}

// Test 8: Validates successful addition of single nodes including edge cases
TEST_F(GraphTest, AddSingleNodeSuccess) {
    EXPECT_TRUE(graph->add_node("Node1"));
    EXPECT_EQ(graph->get_num_nodes(), 1);
    EXPECT_NE(graph->get_node("Node1"), nullptr);

    EXPECT_TRUE(graph->add_node("Node2"));
    EXPECT_EQ(graph->get_num_nodes(), 2);

    EXPECT_TRUE(graph->add_node(""));
    EXPECT_NE(graph->get_node(""), nullptr);

    EXPECT_TRUE(graph->add_node("Node@#$%"));
    EXPECT_NE(graph->get_node("Node@#$%"), nullptr);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_add_single_node_success");
}

// Test 9: Tests node addition rejection for duplicate IDs
TEST_F(GraphTest, AddSingleNodeFailure) {
    EXPECT_TRUE(graph->add_node("Duplicate"));
    EXPECT_FALSE(graph->add_node("Duplicate"));
    EXPECT_EQ(graph->get_num_nodes(), 1);
}

// Test 10: Tests batch node addition with success and partial failure scenarios
TEST_F(GraphTest, AddMultipleNodes) {
    std::vector<std::string> node_ids = {"A", "B", "C", "D", "E"};

    EXPECT_TRUE(graph->add_node_set(node_ids));
    EXPECT_EQ(graph->get_num_nodes(), 5);

    for (const auto& id : node_ids) {
        EXPECT_NE(graph->get_node(id), nullptr);
    }

    std::vector<std::string> mixed_ids = {"A", "F", "B", "G"};
    EXPECT_FALSE(graph->add_node_set(mixed_ids));
    EXPECT_EQ(graph->get_num_nodes(), 7);
    EXPECT_NE(graph->get_node("F"), nullptr);
    EXPECT_NE(graph->get_node("G"), nullptr);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_add_multiple_nodes");
}

// Test 11: Validates node removal and automatic edge cleanup
TEST_F(GraphTest, RemoveNodeSuccess) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_node("C");
    graph->add_edge("A", "B", 10);
    graph->add_edge("B", "C", 20);

    EXPECT_TRUE(graph->remove_node("B"));
    EXPECT_EQ(graph->get_num_nodes(), 2);
    EXPECT_EQ(graph->get_node("B"), nullptr);

    Node* nodeA = graph->get_node("A");
    Node* nodeC = graph->get_node("C");
    EXPECT_EQ(nodeA->get_num_children(), 0);
    EXPECT_EQ(nodeC->get_num_parents(), 0);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_remove_node_success");
}

// Test 12: Tests node removal rejection for non-existent nodes
TEST_F(GraphTest, RemoveNodeFailure) {
    EXPECT_FALSE(graph->remove_node("NonExistent"));

    graph->add_node("A");
    EXPECT_TRUE(graph->remove_node("A"));
    EXPECT_FALSE(graph->remove_node("A"));
    if (generate_diagrams)
        graph->generate_diagram_file("graph_remove_node_failure");
}

// Test 13: Verifies successful edge addition and parent/child count updates
TEST_F(GraphTest, AddEdgeSuccess) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_node("C");

    EXPECT_TRUE(graph->add_edge("A", "B", 5));
    EXPECT_TRUE(graph->add_edge("B", "C", 10));
    EXPECT_TRUE(graph->add_edge("A", "C", 15));

    Node* nodeA = graph->get_node("A");
    Node* nodeB = graph->get_node("B");
    Node* nodeC = graph->get_node("C");

    EXPECT_TRUE(nodeA->contains_edge(nodeB));
    EXPECT_TRUE(nodeA->contains_edge(nodeC));
    EXPECT_TRUE(nodeB->contains_edge(nodeC));
    EXPECT_EQ(nodeB->get_num_parents(), 1);
    EXPECT_EQ(nodeC->get_num_parents(), 2);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_add_edge_success");
}

// Test 14: Tests edge addition rejection for non-existent nodes and conflicts
TEST_F(GraphTest, AddEdgeFailure) {
    graph->add_node("A");
    graph->add_node("B");

    EXPECT_FALSE(graph->add_edge("X", "A", 5));

    EXPECT_FALSE(graph->add_edge("A", "Y", 5));

    EXPECT_FALSE(graph->add_edge("X", "Y", 5));

    EXPECT_TRUE(graph->add_edge("A", "B", 10));

    EXPECT_FALSE(graph->add_edge("A", "B", 20));
    if (generate_diagrams)
        graph->generate_diagram_file("graph_add_edge_failure");
}

// Test 15: Tests batch edge addition with explicit and default weights
TEST_F(GraphTest, AddMultipleEdges) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_node("C");
    graph->add_node("D");

    std::vector<std::string> targets = {"B", "C", "D"};
    std::vector<int> weights = {10, 20, 30};

    EXPECT_TRUE(graph->add_edge_set("A", targets, weights));

    Node* nodeA = graph->get_node("A");
    EXPECT_EQ(nodeA->get_num_children(), 3);

    graph->add_node("E");
    graph->add_node("F");
    std::vector<std::string> targets2 = {"E", "F"};

    EXPECT_TRUE(graph->add_edge_set("B", targets2));

    Node* nodeB = graph->get_node("B");
    const auto& children = nodeB->get_children();
    EXPECT_EQ(children.at(graph->get_node("E")), 0);
    EXPECT_EQ(children.at(graph->get_node("F")), 0);
    if (generate_diagrams)
        graph->generate_diagram_file("graph_add_multiple_edges");
}

// Test 16: Validates edge removal and parent/child count updates
TEST_F(GraphTest, RemoveEdge) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_node("C");
    graph->add_edge("A", "B", 10);
    graph->add_edge("A", "C", 20);

    EXPECT_TRUE(graph->remove_edge("A", "B"));

    Node* nodeA = graph->get_node("A");
    Node* nodeB = graph->get_node("B");

    EXPECT_FALSE(nodeA->contains_edge(nodeB));
    EXPECT_EQ(nodeA->get_num_children(), 1);
    EXPECT_EQ(nodeB->get_num_parents(), 0);

    EXPECT_FALSE(graph->remove_edge("B", "C"));

    EXPECT_FALSE(graph->remove_edge("X", "Y"));
    if (generate_diagrams) graph->generate_diagram_file("graph_remove_edge");
}

// Test 17: Tests edge weight modification for existing and non-existent edges
TEST_F(GraphTest, ChangeEdgeWeight) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_edge("A", "B", 10);

    EXPECT_TRUE(graph->change_edge_weight("A", "B", 50));

    Node* nodeA = graph->get_node("A");
    Node* nodeB = graph->get_node("B");
    const auto& children = nodeA->get_children();

    EXPECT_EQ(children.at(nodeB), 50);

    EXPECT_FALSE(graph->change_edge_weight("B", "A", 100));

    EXPECT_FALSE(graph->change_edge_weight("X", "Y", 100));
    if (generate_diagrams)
        graph->generate_diagram_file("graph_change_edge_weight");
}

// Test 18: Confirms DAG detection for various valid acyclic graph structures
TEST_F(GraphTest, DAGDetectionValid) {
    EXPECT_TRUE(graph->is_dag());

    graph->add_node("A");
    EXPECT_TRUE(graph->is_dag());

    graph->add_node("B");
    graph->add_node("C");
    graph->add_edge("A", "B", 1);
    graph->add_edge("B", "C", 1);
    EXPECT_TRUE(graph->is_dag());

    graph->add_node("D");
    graph->add_node("E");
    graph->add_edge("A", "D", 1);
    graph->add_edge("A", "E", 1);
    EXPECT_TRUE(graph->is_dag());
    if (generate_diagrams)
        graph->generate_diagram_file("graph_dag_detection_valid");
}

// Test 19: Tests cycle detection and DAG validation after cycle removal
TEST_F(GraphTest, DAGDetectionCycles) {
    graph->add_node("A");
    graph->add_node("B");
    graph->add_node("C");

    graph->add_edge("A", "B", 1);
    graph->add_edge("B", "C", 1);
    graph->add_edge("C", "A", 1);

    EXPECT_FALSE(graph->is_dag());

    graph->remove_edge("C", "A");
    EXPECT_TRUE(graph->is_dag());

    EXPECT_FALSE(graph->add_edge("B", "B", 1));
    EXPECT_TRUE(graph->is_dag());
    if (generate_diagrams)
        graph->generate_diagram_file("graph_dag_detection_cycles");
}

// Test 20: Stress tests graph operations with large hierarchical structure
TEST_F(GraphTest, ComplexGraphOperationsAndStressTesting) {
    const int num_nodes = 100;
    std::vector<std::string> node_ids;

    for (int i = 0; i < num_nodes; ++i) {
        std::string id = "Node" + std::to_string(i);
        node_ids.push_back(id);
        EXPECT_TRUE(graph->add_node(id));
    }

    EXPECT_EQ(graph->get_num_nodes(), num_nodes);

    for (int i = 0; i < num_nodes - 1; ++i) {
        for (int j = i + 1; j < std::min(i + 5, num_nodes); ++j) {
            EXPECT_TRUE(graph->add_edge("Node" + std::to_string(i),
                                        "Node" + std::to_string(j), i + j));
        }
    }

    EXPECT_TRUE(graph->is_dag());

    std::vector<std::string> new_nodes = {"Extra1", "Extra2", "Extra3"};
    EXPECT_TRUE(graph->add_node_set(new_nodes));
    EXPECT_EQ(graph->get_num_nodes(), num_nodes + 3);

    std::vector<std::string> targets = {"Extra2", "Extra3"};
    std::vector<int> weights = {100, 200};
    EXPECT_TRUE(graph->add_edge_set("Extra1", targets, weights));

    EXPECT_TRUE(graph->remove_node("Node50"));
    EXPECT_EQ(graph->get_num_nodes(), num_nodes + 2);
    EXPECT_EQ(graph->get_node("Node50"), nullptr);

    for (const auto& [id, node] : graph->get_nodes()) {
        EXPECT_FALSE(node->contains_edge(nullptr));
        for (const auto& [child, weight] : node->get_children()) {
            EXPECT_NE(child, nullptr);
            EXPECT_NE(graph->get_node(child->get_id()), nullptr);
        }
    }
    if (generate_diagrams)
        graph->generate_diagram_file("graph_complex_operations");
}
