/**
 * @file kpt.h
 * @author Bryan SebaRaj <bryan.sebaraj@yale.edu>
 * @version 1.0
 * @section DESCRIPTION
 *
 * Copyright (c) 2025 Bryan SebaRaj
 *
 * This software is licensed under the MIT License.
 */

#ifndef SRC_ALGORITHMS_KPT_H_
#define SRC_ALGORITHMS_KPT_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "mcis/graph.h"
#include "mcis/mcis_finder.h"

class KPT : public MCISFinder {
 public:
    struct Hyperedge {
        std::vector<std::string> node_ids;

        bool operator<(const Hyperedge& other) const {
            return node_ids < other.node_ids;
        }
        bool operator==(const Hyperedge& other) const {
            return node_ids == other.node_ids;
        }
    };

    std::expected<std::vector<Graph*>, mcis::AlgorithmError> find(
        const std::vector<const Graph*>& graphs,
        std::optional<std::string> tag) override;

 private:
    using EdgeSet = std::set<Hyperedge>;
    using WeightMap = std::map<Hyperedge, double>;

    EdgeSet kPCM_Match(EdgeSet F, WeightMap w,
                       const std::vector<const Graph*>& graphs);
    bool are_conflicting(const Hyperedge& p1, const Hyperedge& p2,
                         const std::vector<const Graph*>& graphs);
    bool is_reachable(const Graph* g, const std::string& start_node_id,
                      const std::string& end_node_id);
};

#endif  // SRC_ALGORITHMS_KPT_H_
