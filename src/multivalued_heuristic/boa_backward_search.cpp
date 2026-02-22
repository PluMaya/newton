//
// Created by crl on 21/02/2026.
//


#include "multivalued_heuristic/boa_backward_search.h"

#include <fstream>
#include <iostream>
#include <queue>

#include "data_structures/node.h"


BOABackwardSearch::BOABackwardSearch(const AdjacencyMatrix& adj_matrix):
    adj_matrix(adj_matrix) {
}


MultiValuedHeuristic BOABackwardSearch::operator()(const size_t& source, const std::string& output_file) {
    std::vector<float> min_g2(adj_matrix.size() + 1,
                              static_cast<float>(MAX_COST));

    std::vector expanded(adj_matrix.size() + 1, 0);
    std::vector generated(adj_matrix.size() + 1, 0);

    std::priority_queue<NodePtr, std::vector<NodePtr>, CompareNodeByFValue> open;
    std::vector<std::vector<std::vector<float>>> mvh_results(adj_matrix.size() + 1, std::vector<std::vector<float>>{});

    auto initialization_time = std::clock();

    auto node = std::make_shared<Node>(source, std::vector<float>(2, 0.0f),
                                       std::vector<float>{0,0});
    open.push(node);

    while (!open.empty()) {
        // Pop min from queue and process
        node = open.top();
        open.pop();
        num_generation += 1;
        generated[node->id] += 1;

        // Dominance check
        if (node->g[1] >= min_g2[node->id]) {
            continue;
        }

        min_g2[node->id] = node->g[1];
        num_expansion += 1;
        expanded[node->id] += 1;

        mvh_results[node->id].push_back(node->g);

        // Check to which neighbors we should extend the paths
        const std::vector<Edge>& outgoing_edges = adj_matrix[node->id];

        for (const auto& outgoing_edge : outgoing_edges) {
            size_t next_id = outgoing_edge.target;
            std::vector<float> next_g = {
                node->g[0] + outgoing_edge.cost[0],
                node->g[1] + outgoing_edge.cost[1]
            };

            // Dominance check
            if (next_g[1] >= min_g2[next_id]) {
                continue;
            }

            auto next = std::make_shared<Node>(next_id, next_g, std::vector<float>(2, 0.0f));

            open.push(next);
        }
    }

    runtime = static_cast<float>(std::clock() - start_time);

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
