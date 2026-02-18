//
// Created by crl on 14/07/2024.
//

#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <solvers/boa.h>

BOAStar::BOAStar(const AdjacencyMatrix& adj_matrix)
    : AbstractSolver(adj_matrix, {0, 0}) {
}

void BOAStar::operator()(const size_t& source, const size_t& target,
                         const Heuristic& heuristic, SolutionSet& solutions,
                         const unsigned int time_limit,
                         const std::string& logging_file) {
    init_search();
    // Vector to hold mininum cost of 2nd criteria per node
    std::vector<float> min_g2(adj_matrix.size() + 1,
                              static_cast<float>(MAX_COST));

    std::vector expanded(adj_matrix.size() + 1, 0);
    std::vector generated(adj_matrix.size() + 1, 0);

    // Init open heap
    std::priority_queue<NodePtr, std::vector<NodePtr>, CompareNodeByFValue> open;

    auto initialization_time = std::clock();

    auto node = std::make_shared<Node>(source, std::vector<float>(2, 0.0f),
                                       heuristic(source));
    open.push(node);

    while (!open.empty()) {
        // if ((std::clock() - start_time) / CLOCKS_PER_SEC > time_limit) {
        //     return;
        // }

        // Pop min from queue and process
        node = open.top();
        open.pop();
        num_generation += 1;
        generated[node->id] += 1;

        // Dominance check
        if ((node->f[1]) >= min_g2[target] ||
            (node->g[1] >= min_g2[node->id])) {
            continue;
        }

        min_g2[node->id] = node->g[1];
        num_expansion += 1;
        expanded[node->id] += 1;

        if (node->id == target) {
            solutions.push_back(node);
            continue;
        }

        // Check to which neighbors we should extend the paths
        const std::vector<Edge>& outgoing_edges = adj_matrix[node->id];

        for (const auto& outgoing_edge : outgoing_edges) {
            size_t next_id = outgoing_edge.target;
            std::vector<float> next_g = {
                node->g[0] + outgoing_edge.cost[0],
                node->g[1] + outgoing_edge.cost[1]
            };
            auto next_h = heuristic(next_id);

            // Dominance check
            if ((next_g[1] + next_h[1]) >= min_g2[target] ||
                (next_g[1] >= min_g2[next_id])) {
                continue;
            }

            auto next = std::make_shared<Node>(next_id, next_g, next_h, node);

            open.push(next);
        }
    }

    runtime = static_cast<float>(std::clock() - start_time);

    std::stringstream solution_ss;
    solution_ss << logging_file << "_" << source << "_solutions.txt";
    std::ofstream SolutionOutput(solution_ss.str());

    for (auto const& solution : solutions) {
        SolutionOutput << solution->g[0] << "," << solution->g[1] << std::endl;
    }

    SolutionOutput.close();


    std::stringstream stats_ss;
    stats_ss << logging_file << "_" << source << "_stats.txt";
    std::ofstream StatsOutput(stats_ss.str());

    auto initialization_runtime = initialization_time - start_time;
    // write one line of clock time, number of expansions, number of generations
    StatsOutput << initialization_runtime / CLOCKS_PER_SEC << "\t" << runtime / CLOCKS_PER_SEC << "\t" << num_expansion << "\t" << num_generation << std::endl;
    StatsOutput << num_generation << "\t" << num_expansion << std::endl;

    for (size_t s = 0; s < adj_matrix.size() + 1; ++s) {
        StatsOutput << s << "\t" << generated[s] << "\t" << expanded[s] << std::endl;
    }

    StatsOutput.close();

}
