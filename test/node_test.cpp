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
#include "mcis/node.h"

#include <limits>
#include <memory>
#include <sstream>

#include "gtest/gtest.h"

class NodeTest : public ::testing::Test {
 protected:
    void SetUp() override {
        node_a = std::make_unique<Node>("A");
        node_b = std::make_unique<Node>("B");
        node_c = std::make_unique<Node>("C");
        node_d = std::make_unique<Node>("D");
    }

    void TearDown() override {
        node_a.reset();
        node_b.reset();
        node_c.reset();
        node_d.reset();
    }

    std::unique_ptr<Node> node_a;
    std::unique_ptr<Node> node_b;
    std::unique_ptr<Node> node_c;
    std::unique_ptr<Node> node_d;
};

// Test 1: Verifies node constructor sets correct initial state and properties
TEST_F(NodeTest, ConstructorAndInitialState) {
    Node node("TestNode");
    EXPECT_EQ(node.get_id(), "TestNode");
    EXPECT_EQ(node.get_num_parents(), 0);
    EXPECT_EQ(node.get_num_children(), 0);
    EXPECT_TRUE(node.is_source());
    EXPECT_TRUE(node.is_sink());
    EXPECT_TRUE(node.get_children().empty());
}

// Test 2: Ensures copy constructor creates independent node with same edges
TEST_F(NodeTest, CopyConstructor) {
    node_a->add_edge(node_b.get(), 10);
    node_a->add_edge(node_c.get(), 20);

    Node copied_node(*node_a);

    EXPECT_EQ(copied_node.get_id(), node_a->get_id());
    EXPECT_EQ(copied_node.get_num_parents(), node_a->get_num_parents());
    EXPECT_EQ(copied_node.get_num_children(), node_a->get_num_children());
    EXPECT_EQ(copied_node.get_children().size(), node_a->get_children().size());

    EXPECT_TRUE(copied_node.contains_edge(node_b.get()));
    EXPECT_TRUE(copied_node.contains_edge(node_c.get()));
}

// Test 3: Validates copy assignment operator handles existing state and
// self-assignment
TEST_F(NodeTest, CopyAssignment) {
    node_a->add_edge(node_b.get(), 15);
    Node assigned_node("Different");

    assigned_node = *node_a;

    EXPECT_EQ(assigned_node.get_id(), node_a->get_id());
    EXPECT_EQ(assigned_node.get_num_children(), node_a->get_num_children());
    EXPECT_TRUE(assigned_node.contains_edge(node_b.get()));

    EXPECT_EQ(assigned_node.get_id(), node_a->get_id());
}

// Test 4: Confirms move constructor transfers resources and leaves source in
// valid state
TEST_F(NodeTest, MoveConstructor) {
    node_a->add_edge(node_b.get(), 25);
    std::string original_id = node_a->get_id();
    int original_children = node_a->get_num_children();

    Node moved_node(std::move(*node_a));

    EXPECT_EQ(moved_node.get_id(), original_id);
    EXPECT_EQ(moved_node.get_num_children(), original_children);
    EXPECT_TRUE(moved_node.contains_edge(node_b.get()));

    EXPECT_EQ(node_a->get_num_parents(), 0);
    EXPECT_EQ(node_a->get_num_children(), 0);
}

// Test 5: Tests move assignment operator transfers state and handles
// self-assignment
TEST_F(NodeTest, MoveAssignment) {
    node_a->add_edge(node_b.get(), 30);
    Node assigned_node("Target");
    std::string original_id = node_a->get_id();

    assigned_node = std::move(*node_a);

    EXPECT_EQ(assigned_node.get_id(), original_id);
    EXPECT_TRUE(assigned_node.contains_edge(node_b.get()));

    Node self_assign("Self");
    Node& ref = self_assign;
    self_assign = std::move(ref);
    EXPECT_EQ(self_assign.get_id(), "Self");
}

// Test 6: Verifies successful edge addition updates parent/child counts
// correctly
TEST_F(NodeTest, AddEdgeSuccess) {
    EXPECT_TRUE(node_a->add_edge(node_b.get(), 5));
    EXPECT_EQ(node_a->get_num_children(), 1);
    EXPECT_EQ(node_b->get_num_parents(), 1);
    EXPECT_FALSE(node_a->is_sink());
    EXPECT_FALSE(node_b->is_source());

    EXPECT_TRUE(node_a->add_edge(node_b.get(), 5));
    EXPECT_EQ(node_a->get_num_children(), 1);

    EXPECT_TRUE(node_a->add_edge(node_c.get(), 10));
    EXPECT_TRUE(node_a->add_edge(node_d.get(), 15));
    EXPECT_EQ(node_a->get_num_children(), 3);
}

// Test 7: Tests edge addition rejection for conflicting weights and self-loops
TEST_F(NodeTest, AddEdgeFailure) {
    node_a->add_edge(node_b.get(), 5);
    EXPECT_FALSE(node_a->add_edge(node_b.get(), 10));

    EXPECT_FALSE(node_a->add_edge(node_a.get(), 5));
    EXPECT_EQ(node_a->get_num_children(), 1);
}

// Test 8: Validates edge removal updates counts and handles non-existent edges
TEST_F(NodeTest, RemoveEdge) {
    node_a->add_edge(node_b.get(), 5);
    node_a->add_edge(node_c.get(), 10);

    EXPECT_TRUE(node_a->remove_edge(node_b.get()));
    EXPECT_EQ(node_a->get_num_children(), 1);
    EXPECT_EQ(node_b->get_num_parents(), 0);
    EXPECT_FALSE(node_a->contains_edge(node_b.get()));

    EXPECT_FALSE(node_a->remove_edge(node_d.get()));
    EXPECT_EQ(node_a->get_num_children(), 1);

    EXPECT_TRUE(node_a->remove_edge(node_c.get()));
    EXPECT_EQ(node_a->get_num_children(), 0);
    EXPECT_TRUE(node_a->is_sink());
}

// Test 9: Tests edge weight modification for existing and non-existent edges
TEST_F(NodeTest, ChangeEdgeWeight) {
    node_a->add_edge(node_b.get(), 5);

    EXPECT_TRUE(node_a->change_edge_weight(node_b.get(), 15));
    auto& children = node_a->get_children();
    EXPECT_EQ(children.at(node_b.get()), 15);

    EXPECT_FALSE(node_a->change_edge_weight(node_c.get(), 20));

    EXPECT_TRUE(node_a->change_edge_weight(node_b.get(), -5));
    EXPECT_EQ(children.at(node_b.get()), -5);
}

// Test 10: Validates edge existence queries and children map access
TEST_F(NodeTest, EdgeQueries) {
    node_a->add_edge(node_b.get(), 5);
    node_a->add_edge(node_c.get(), 10);

    EXPECT_TRUE(node_a->contains_edge(node_b.get()));
    EXPECT_TRUE(node_a->contains_edge(node_c.get()));
    EXPECT_FALSE(node_a->contains_edge(node_d.get()));

    const auto& children = node_a->get_children();
    EXPECT_EQ(children.size(), 2u);
    EXPECT_EQ(children.at(node_b.get()), 5);
    EXPECT_EQ(children.at(node_c.get()), 10);
}

// Test 11: Confirms correct identification of source and sink nodes in graph
// topology
TEST_F(NodeTest, SourceAndSinkIdentification) {
    EXPECT_TRUE(node_a->is_source() && node_a->is_sink());

    node_a->add_edge(node_b.get(), 5);
    EXPECT_TRUE(node_a->is_source());
    EXPECT_FALSE(node_a->is_sink());
    EXPECT_FALSE(node_b->is_source());
    EXPECT_TRUE(node_b->is_sink());

    node_b->add_edge(node_c.get(), 10);
    EXPECT_FALSE(node_b->is_source());
    EXPECT_FALSE(node_b->is_sink());
    EXPECT_TRUE(node_a->is_source());
    EXPECT_TRUE(node_c->is_sink());
}

// Test 12: Tests node equality based on ID, edges, and weights
TEST_F(NodeTest, EqualityOperator) {
    Node node1("Equal");
    Node node2("Equal");
    Node node3("Different");

    EXPECT_TRUE(node1 == node2);
    EXPECT_FALSE(node1 == node3);

    Node helper("Helper");
    node1.add_edge(&helper, 5);
    EXPECT_FALSE(node1 == node2);

    node2.add_edge(&helper, 5);
    EXPECT_TRUE(node1 == node2);

    Node node4("Equal");
    node4.add_edge(&helper, 10);
    EXPECT_FALSE(node1 == node4);
}

// Test 13: Validates ID-only comparison ignoring node state
TEST_F(NodeTest, SameIdComparison) {
    Node node1("SameID");
    Node node2("SameID");
    Node node3("DifferentID");

    EXPECT_TRUE(node1.same_id(node2));
    EXPECT_FALSE(node1.same_id(node3));

    Node helper("Helper");
    node1.add_edge(&helper, 5);
    EXPECT_TRUE(node1.same_id(node2));
}

// Test 14: Verifies string representation format for nodes with and without
// edges
TEST_F(NodeTest, StreamOutputOperator) {
    std::ostringstream oss;

    oss << *node_a;
    std::string output = oss.str();
    EXPECT_TRUE(output.find("A -> { }") != std::string::npos);

    node_a->add_edge(node_b.get(), 5);
    node_a->add_edge(node_c.get(), 10);

    oss.str("");
    oss << *node_a;
    output = oss.str();

    EXPECT_TRUE(output.find("A ->") != std::string::npos);
    EXPECT_TRUE(output.find("\"B\"(5)") != std::string::npos);
    EXPECT_TRUE(output.find("\"C\"(10)") != std::string::npos);
}

// Test 15: Stress tests node operations with complex multi-node graph
// structures
TEST_F(NodeTest, ComplexGraphScenarios) {
    std::vector<std::unique_ptr<Node>> nodes;
    for (int i = 0; i < 10; ++i) {
        nodes.push_back(std::make_unique<Node>("Node" + std::to_string(i)));
    }

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (i != j) {
                nodes[i]->add_edge(nodes[j].get(), i * 10 + j);
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(nodes[i]->get_num_children(), 4);
        EXPECT_EQ(nodes[i]->get_num_parents(), 4);
        EXPECT_FALSE(nodes[i]->is_source());
        EXPECT_FALSE(nodes[i]->is_sink());
    }

    auto start_node = nodes[5].get();
    for (int i = 6; i < 10; ++i) {
        EXPECT_TRUE(start_node->add_edge(nodes[i].get(), i));
    }
    EXPECT_EQ(start_node->get_num_children(), 4);

    for (int i = 6; i < 10; ++i) {
        EXPECT_TRUE(start_node->remove_edge(nodes[i].get()));
    }
    EXPECT_EQ(start_node->get_num_children(), 0);
    EXPECT_TRUE(start_node->is_sink());
}

// Test 16: Tests safe handling of temporary node pointers and edge cleanup
TEST_F(NodeTest, MemoryManagementAndDanglingPointers) {
    Node* temp_node = new Node("TempNode");
    node_a->add_edge(temp_node, 42);

    EXPECT_TRUE(node_a->contains_edge(temp_node));
    EXPECT_EQ(node_a->get_num_children(), 1);
    EXPECT_EQ(temp_node->get_num_parents(), 1);

    const auto& children = node_a->get_children();
    EXPECT_EQ(children.at(temp_node), 42);

    EXPECT_TRUE(node_a->remove_edge(temp_node));
    EXPECT_EQ(node_a->get_num_children(), 0);
    EXPECT_EQ(temp_node->get_num_parents(), 0);

    delete temp_node;

    EXPECT_TRUE(node_a->is_sink());
    EXPECT_TRUE(node_a->get_children().empty());
}

// Test 17: Tests edge operations with extreme weight values and boundary
// conditions
TEST_F(NodeTest, EdgeWeightExtremesAndBoundaryConditions) {
    const int max_weight = std::numeric_limits<int>::max();
    const int min_weight = std::numeric_limits<int>::min();
    const int zero_weight = 0;

    EXPECT_TRUE(node_a->add_edge(node_b.get(), max_weight));
    EXPECT_TRUE(node_a->add_edge(node_c.get(), min_weight));
    EXPECT_TRUE(node_a->add_edge(node_d.get(), zero_weight));

    const auto& children = node_a->get_children();
    EXPECT_EQ(children.at(node_b.get()), max_weight);
    EXPECT_EQ(children.at(node_c.get()), min_weight);
    EXPECT_EQ(children.at(node_d.get()), zero_weight);

    EXPECT_TRUE(node_a->change_edge_weight(node_b.get(), min_weight));
    EXPECT_TRUE(node_a->change_edge_weight(node_c.get(), max_weight));
    EXPECT_TRUE(node_a->change_edge_weight(node_d.get(), -1));

    EXPECT_EQ(children.at(node_b.get()), min_weight);
    EXPECT_EQ(children.at(node_c.get()), max_weight);
    EXPECT_EQ(children.at(node_d.get()), -1);

    EXPECT_EQ(node_a->get_num_children(), 3);
    EXPECT_EQ(node_b->get_num_parents(), 1);
    EXPECT_EQ(node_c->get_num_parents(), 1);
    EXPECT_EQ(node_d->get_num_parents(), 1);
}

// Test 18: Comprehensive testing of equality operations with complex edge
// structures
TEST_F(NodeTest, ComprehensiveEqualityAndComparisonEdgeCases) {
    Node node1("Complex");
    Node node2("Complex");
    Node helper1("Helper1");
    Node helper2("Helper2");
    Node helper3("Helper3");

    node1.add_edge(&helper1, 10);
    node1.add_edge(&helper2, -5);
    node1.add_edge(&helper3, 0);

    node2.add_edge(&helper1, 10);
    node2.add_edge(&helper2, -5);
    node2.add_edge(&helper3, 0);

    EXPECT_TRUE(node1 == node2);

    EXPECT_TRUE(node1.change_edge_weight(&helper1, 15));
    EXPECT_FALSE(node1 == node2);

    EXPECT_TRUE(node1.change_edge_weight(&helper1, 10));
    EXPECT_TRUE(node1 == node2);

    Node helper4("Helper4");
    node1.add_edge(&helper4, 100);
    EXPECT_FALSE(node1 == node2);

    Node different_id("Different");
    different_id.add_edge(&helper1, 10);
    different_id.add_edge(&helper2, -5);
    different_id.add_edge(&helper3, 0);

    EXPECT_TRUE(node1.same_id(node2));
    EXPECT_FALSE(node1.same_id(different_id));

    Node empty1("Empty");
    Node empty2("Empty");
    helper1.add_edge(&empty1, 1);

    EXPECT_TRUE(empty1.same_id(empty2));
    EXPECT_FALSE(empty1 == empty2);
}
