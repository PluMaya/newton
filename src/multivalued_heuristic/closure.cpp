//
// Created by crl on 14/02/2026.
//

#include "multivalued_heuristic/closure.h"

#include <array>
#include <fstream>
#include <iostream>


// Plain value struct — no heap allocation per node, no atomic refcounting.
struct ClosureNode {
    size_t id;
    std::array<float, 2> h;
};

struct CompareClosureNode {
    bool operator()(const ClosureNode& a, const ClosureNode& b) const {
        return a.h > b.h;  // lexicographic min-heap
    }
};


Closure::Closure(const AdjacencyMatrix& adj_matrix) : adj_matrix(adj_matrix) {}

MultiValuedHeuristic Closure::operator()(const MultiValuedHeuristic& heuristic,
    const std::string& output_file) {
    start_time = std::clock();
    MultiValuedHeuristic mvh_results(adj_matrix.size() + 1);
    min_g2.assign(adj_matrix.size() + 1, static_cast<float>(MAX_COST));

    // Pre-count total entries so we can reserve and heapify in O(N)
    // instead of doing N individual O(log N) pushes.
    size_t total = 0;
    for (const auto& h_node : heuristic) total += h_node.size();

    std::vector<ClosureNode> heuristic_values;
    heuristic_values.reserve(total);
    for (size_t i = 0; i < heuristic.size(); i++) {
        for (size_t j = 0; j < heuristic[i].size(); j++) {
            heuristic_values.push_back({i, {heuristic[i][j][0], heuristic[i][j][1]}});
        }
    }

    // O(N) heap construction via the container constructor.
    std::priority_queue<ClosureNode, std::vector<ClosureNode>, CompareClosureNode> open(
        CompareClosureNode{}, std::move(heuristic_values));

    std::array<float, 2> prev_value = {0.0f, 0.0f};

    while (!open.empty()) {
        const ClosureNode node = open.top();
        open.pop();
        if ((node.h[0] < prev_value[0]) ||
            (node.h[0] == prev_value[0] && node.h[1] < prev_value[1])) {
            std::cout << "Warning: heuristic values are not sorted in non-decreasing order. "
                      << "Current node: " << node.id
                      << " h value: (" << node.h[0] << ", " << node.h[1] << "). "
                      << "Previous h value: (" << prev_value[0] << ", " << prev_value[1] << ")."
                      << std::endl;
        }
        prev_value = node.h;
        num_generation += 1;
        if (min_g2[node.id] <= node.h[1]) {
            continue;
        }
        mvh_results[node.id].push_back({node.h[0], node.h[1]});
        num_expansion += 1;
        min_g2[node.id] = node.h[1];
        for (const auto& outgoing_edge : adj_matrix[node.id]) {
            const std::array<float, 2> new_h = {
                node.h[0] + outgoing_edge.cost[0],
                node.h[1] + outgoing_edge.cost[1]
            };
            if (min_g2[outgoing_edge.target] <= new_h[1]) {
                continue;
            }
            open.push({outgoing_edge.target, new_h});
        }
    }

    std::ofstream PlotOutput(output_file);

    for (size_t i = 0; i < adj_matrix.size() + 1; i++) {
        for (const auto& solution : mvh_results[i]) {
            PlotOutput << i << "\t" << solution[0] << "\t" << solution[1] << "\n";
        }
    }

    std::cout << "finished writing results to file " << output_file << std::endl;

    PlotOutput.close();

    return mvh_results;
}
