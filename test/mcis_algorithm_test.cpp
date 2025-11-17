/**
 * @file mcis_algorithm_test.cpp
 * @author Gemini
 * @version 1.0
 * @section DESCRIPTION
 *
 * This file contains unit tests for finding MCIS between various graph types,
 * focusing on generating diagrams rather than correctness assertions.
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#include "mcis/mcis_algorithm.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "mcis/graph.h"

class MCISDiscoveryTest : public ::testing::Test {
 protected:
    void SetUp() override {
        mcis_algorithm = std::make_unique<MCISAlgorithm>();
    }

    void TearDown() override { mcis_algorithm.reset(); }

    std::unique_ptr<MCISAlgorithm> mcis_algorithm;

    void run_and_diagram(std::vector<Graph>& graphs,
                         const std::string& test_name,
                         const std::optional<std::string>& tag = std::nullopt) {
        std::vector<const Graph*> graph_ptrs;
        for (size_t i = 0; i < graphs.size(); ++i) {
            graph_ptrs.push_back(&graphs[i]);
            graphs[i].generate_diagram_file(test_name + "_input_"
                                            + std::to_string(i));
        }

        auto result = mcis_algorithm->run(
            graph_ptrs, AlgorithmType::BRON_KERBOSCH_SERIAL, tag);

        if (result.has_value()) {
            std::vector<Graph*> results = *result;
            std::cout << "Found " << results.size() << " MCIS(s) for "
                      << test_name << std::endl;
            for (size_t i = 0; i < results.size(); ++i) {
                results[i]->generate_diagram_file(test_name + "_mcis_"
                                                  + std::to_string(i));
                delete results[i];
            }
        } else {
            std::cerr
                << "MCIS algorithm failed or returned no result for test: "
                << test_name
                << " with error: " << static_cast<int>(result.error())
                << std::endl;
        }
    }
};

TEST_F(MCISDiscoveryTest, MVMsOfDifferentSizes) {
    auto mvm1_opt = Graph::create_mvm_graph_from_dimensions(2, 2);
    auto mvm2_opt = Graph::create_mvm_graph_from_dimensions(2, 3);
    auto mvm3_opt = Graph::create_mvm_graph_from_dimensions(3, 3);
    ASSERT_TRUE(mvm1_opt.has_value());
    ASSERT_TRUE(mvm2_opt.has_value());
    ASSERT_TRUE(mvm3_opt.has_value());

    std::vector<Graph> graphs
        = {std::move(*mvm1_opt), std::move(*mvm2_opt), std::move(*mvm3_opt)};
    run_and_diagram(graphs, "MVMsOfDifferentSizes");
}

TEST_F(MCISDiscoveryTest, DWTsOfDifferentSizes) {
    Graph g;
    auto dwt1_vec_opt
        = g.create_haar_wavelet_transform_graph_from_dimensions(8, 3);
    auto dwt2_vec_opt
        = g.create_haar_wavelet_transform_graph_from_dimensions(16, 4);
    ASSERT_TRUE(dwt1_vec_opt.has_value() && !dwt1_vec_opt->empty());
    ASSERT_TRUE(dwt2_vec_opt.has_value() && !dwt2_vec_opt->empty());

    std::vector<Graph> graphs;
    graphs.push_back(std::move(dwt1_vec_opt->front()));
    graphs.push_back(std::move(dwt2_vec_opt->front()));
    run_and_diagram(graphs, "DWTsOfDifferentSizes");
}

TEST_F(MCISDiscoveryTest, FFTsOfDifferentSizes) {
    auto fft1_opt = Graph::create_fft_graph_from_dimensions(4);
    auto fft2_opt = Graph::create_fft_graph_from_dimensions(4);
    auto fft3_opt = Graph::create_fft_graph_from_dimensions(4);
    ASSERT_TRUE(fft1_opt.has_value());
    ASSERT_TRUE(fft2_opt.has_value());
    ASSERT_TRUE(fft3_opt.has_value());

    std::vector<Graph> graphs
        = {std::move(*fft1_opt), std::move(*fft2_opt), std::move(*fft3_opt)};
    run_and_diagram(graphs, "FFTsOfDifferentSizes");
}

TEST_F(MCISDiscoveryTest, MediumMVMAndDWT) {
    Graph g;
    auto mvm_opt = Graph::create_mvm_graph_from_dimensions(4, 4);
    auto dwt_vec_opt
        = g.create_haar_wavelet_transform_graph_from_dimensions(16, 4);
    ASSERT_TRUE(mvm_opt.has_value());
    ASSERT_TRUE(dwt_vec_opt.has_value() && !dwt_vec_opt->empty());

    std::vector<Graph> graphs;
    graphs.push_back(std::move(*mvm_opt));
    graphs.push_back(std::move(dwt_vec_opt->front()));
    run_and_diagram(graphs, "MediumMVMAndDWT");
}

TEST_F(MCISDiscoveryTest, MediumMVMAndFFT) {
    auto mvm_opt = Graph::create_mvm_graph_from_dimensions(4, 4);
    auto fft_opt = Graph::create_fft_graph_from_dimensions(16);
    ASSERT_TRUE(mvm_opt.has_value());
    ASSERT_TRUE(fft_opt.has_value());

    std::vector<Graph> graphs = {std::move(*mvm_opt), std::move(*fft_opt)};
    run_and_diagram(graphs, "MediumMVMAndFFT");
}

TEST_F(MCISDiscoveryTest, MediumDWTAndFFT) {
    Graph g;
    auto dwt_vec_opt
        = g.create_haar_wavelet_transform_graph_from_dimensions(2, 1);
    auto fft_opt = Graph::create_fft_graph_from_dimensions(2);
    ASSERT_TRUE(dwt_vec_opt.has_value() && !dwt_vec_opt->empty());
    ASSERT_TRUE(fft_opt.has_value());

    std::vector<Graph> graphs;
    graphs.push_back(std::move(dwt_vec_opt->front()));
    graphs.push_back(std::move(*fft_opt));
    run_and_diagram(graphs, "MediumDWTAndFFT");
}

TEST_F(MCISDiscoveryTest, MediumDWTAndMVMAndFFT) {
    Graph g;
    auto dwt_vec_opt
        = g.create_haar_wavelet_transform_graph_from_dimensions(2, 1);
    auto mvm_opt = Graph::create_mvm_graph_from_dimensions(2, 2);
    auto fft_opt = Graph::create_fft_graph_from_dimensions(2);
    ASSERT_TRUE(dwt_vec_opt.has_value() && !dwt_vec_opt->empty());
    ASSERT_TRUE(mvm_opt.has_value());
    ASSERT_TRUE(fft_opt.has_value());

    std::vector<Graph> graphs;
    graphs.push_back(std::move(dwt_vec_opt->front()));
    graphs.push_back(std::move(*mvm_opt));
    graphs.push_back(std::move(*fft_opt));
    run_and_diagram(graphs, "MediumDWTAndMVMAndFFT");
}

TEST_F(MCISDiscoveryTest, LargeGraphsMVMAndFFT) {
    auto mvm_opt = Graph::create_mvm_graph_from_dimensions(3, 3);  // 99 nodes
    auto fft_opt = Graph::create_fft_graph_from_dimensions(4);     // 96 nodes
    ASSERT_TRUE(mvm_opt.has_value());
    ASSERT_TRUE(fft_opt.has_value());

    std::vector<Graph> graphs = {std::move(*mvm_opt), std::move(*fft_opt)};
    run_and_diagram(graphs, "LargeGraphsMVMAndFFT");
}

TEST_F(MCISDiscoveryTest, LargeGraphsDWTAndMVM) {
    auto dwt_vec_opt
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(2, 1);
    auto mvm_opt = Graph::create_mvm_graph_from_dimensions(2, 2);  // 120 nodes
    ASSERT_TRUE(dwt_vec_opt.has_value() && !dwt_vec_opt->empty());
    ASSERT_TRUE(mvm_opt.has_value());

    std::vector<Graph> graphs;
    graphs.push_back(std::move(dwt_vec_opt->front()));
    graphs.push_back(std::move(*mvm_opt));
    run_and_diagram(graphs, "LargeGraphsDWTAndMVM");
}

// --- Tagged Tests ---

void tag_graph_nodes(Graph& g, const std::string& tag_prefix) {
    int i = 0;
    for (auto const& [id, node] : g.get_nodes()) {
        if (node->get_tag().empty()) {
            g.set_node_tag(id, tag_prefix + std::to_string(i % 3));
        }
        i++;
    }
}

TEST_F(MCISDiscoveryTest, TaggedMVMs) {
    auto mvm1_opt = Graph::create_mvm_graph_from_dimensions(3, 3);
    auto mvm2_opt = Graph::create_mvm_graph_from_dimensions(3, 4);
    ASSERT_TRUE(mvm1_opt.has_value());
    ASSERT_TRUE(mvm2_opt.has_value());

    Graph mvm1 = std::move(*mvm1_opt);
    Graph mvm2 = std::move(*mvm2_opt);
    tag_graph_nodes(mvm1, "g1_");
    tag_graph_nodes(mvm2, "g2_");

    std::vector<Graph> graphs = {std::move(mvm1), std::move(mvm2)};
    run_and_diagram(graphs, "TaggedMVMs_group0", "g1_0");
    run_and_diagram(graphs, "TaggedMVMs_group1", "g1_1");
}

TEST_F(MCISDiscoveryTest, TaggedDWTAndFFT) {
    auto dwt_vec_opt
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(2, 1);
    auto fft_opt = Graph::create_fft_graph_from_dimensions(2);
    ASSERT_TRUE(dwt_vec_opt.has_value() && !dwt_vec_opt->empty());
    ASSERT_TRUE(fft_opt.has_value());

    Graph dwt = std::move(dwt_vec_opt->front());
    Graph fft = std::move(*fft_opt);
    tag_graph_nodes(dwt, "dwt_");
    tag_graph_nodes(fft, "fft_");

    std::vector<Graph> graphs = {std::move(dwt), std::move(fft)};
    run_and_diagram(graphs, "TaggedDWTAndFFT_group0", "dwt_0");
    run_and_diagram(graphs, "TaggedDWTAndFFT_group2", "dwt_2");
}

TEST_F(MCISDiscoveryTest, TaggedMVM_DWT_FFT) {
    auto mvm_opt = Graph::create_mvm_graph_from_dimensions(2, 2);
    auto dwt_vec_opt
        = Graph::create_haar_wavelet_transform_graph_from_dimensions(2, 1);
    auto fft_opt = Graph::create_fft_graph_from_dimensions(2);
    ASSERT_TRUE(mvm_opt.has_value());
    ASSERT_TRUE(dwt_vec_opt.has_value() && !dwt_vec_opt->empty());
    ASSERT_TRUE(fft_opt.has_value());

    Graph mvm = std::move(*mvm_opt);
    Graph dwt = std::move(dwt_vec_opt->front());
    Graph fft = std::move(*fft_opt);
    tag_graph_nodes(mvm, "mvm_");
    tag_graph_nodes(dwt, "dwt_");
    tag_graph_nodes(fft, "fft_");

    std::vector<Graph> graphs
        = {std::move(mvm), std::move(dwt), std::move(fft)};
    run_and_diagram(graphs, "TaggedAll_group1", "mvm_1");
}
