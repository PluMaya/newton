//
// Created by crl on 14/02/2026.
//

#include "multivalued_heuristic/closure.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>


class ClosureNode;
using ClosureNodePtr = std::shared_ptr<ClosureNode>;

class ClosureNode {
public:
    const size_t id;
    const std::vector<float> h;

    ClosureNode(size_t id, const std::vector<float>& h) : id(id), h(h) {}

};

struct CompareClosureNodeByHValue {
    bool operator()(const ClosureNodePtr& a, const ClosureNodePtr& b) const {
        return a->h > b->h;  // TODO: make sure this is lexicographical
    }
};


Closure::Closure(const AdjacencyMatrix& adj_matrix) : adj_matrix(adj_matrix) {}

MultiValuedHeuristic Closure::operator()(const MultiValuedHeuristic& heuristic, size_t target,
    float e1, float e2) {
    start_time = std::clock();
    MultiValuedHeuristic mvh_results(adj_matrix.size() + 1);
    min_g2.resize(adj_matrix.size() + 1, static_cast<float>(MAX_COST));
    for (size_t i = 0; i < adj_matrix.size() + 1; ++i) {
        mvh_results[i] = {};
    }

    std::vector<ClosureNodePtr> heuristic_values = {};

    for (size_t i = 0; i < heuristic.size(); i++) {
        for (size_t j = 0; j < heuristic[i].size(); j++) {
            auto closure_node = std::make_shared<ClosureNode>(i, heuristic[i][j]);
            heuristic_values.push_back(closure_node);
        }
    }
    std::vector<float> prev_value = {0, 0};
    std::priority_queue open(CompareClosureNodeByHValue(), heuristic_values);
    while(!open.empty()) {
        auto node = open.top();
        open.pop();
        if ((node->h[0] < prev_value[0]) || (node->h[0] == prev_value[0] && node->h[1] < prev_value[1])) {
            std::cout << "Warning: heuristic values are not sorted in non-decreasing order. Current node: " << node->id << " h value: (" << node->h[0] << ", " << node->h[1] << "). Previous h value: (" << prev_value[0] << ", " << prev_value[1] << ")." << std::endl;
        }
        prev_value = node->h;
        num_generation += 1;
        if (min_g2[node->id] <= node->h[1]) {
            continue;
        }
        mvh_results[node->id].push_back(node->h);
        num_expansion += 1;
        min_g2[node->id] = node->h[1];
        for (const auto& outgoing_edge : adj_matrix[node->id]) {
            std::vector<float> new_h = {node->h[0] + outgoing_edge.cost[0],
                                        node->h[1] + outgoing_edge.cost[1]};
            if (min_g2[outgoing_edge.target] <= new_h[1]) {
                continue;
            }
            auto closure_node = std::make_shared<ClosureNode>(outgoing_edge.target, new_h);
            open.push(closure_node);

        }
    }

    std::string e1string = std::to_string(e1);
    std::string e2string = std::to_string(e2);
    std::vector<std::string> dceomp_temp;
    e1string = split(dceomp_temp, e1string, boost::is_any_of("."))[1].substr(0, 3);
    e2string = split(dceomp_temp, e2string, boost::is_any_of("."))[1].substr(0, 3);

    std::stringstream ss;
    ss << target << "_" << e1string << "_" << e2string << "_closure_mvh.txt";

    std::ofstream PlotOutput(ss.str());

    for (int i = 0; i < adj_matrix.size() + 1; i++) {
        for (const auto& solution : mvh_results[i]) {
            PlotOutput << i << "\t" << solution[0] << "\t" << solution[1] << std::endl;
        }
    }

    std::cout << "finised writing results to file " << ss.str() << std::endl;

    PlotOutput.close();

    return mvh_results;
}
