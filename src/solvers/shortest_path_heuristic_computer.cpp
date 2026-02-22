//
// Created by crl on 17/07/2024.
//

#include "solvers/shortest_path_heuristic_computer.h"

#include <fstream>
#include <limits>

#include "data_structures/node.h"

#include <iostream>
#include <queue>

struct PairComparator {
    bool operator()(const std::pair<float, size_t>& lhs,
                    const std::pair<float, size_t>& rhs) const {
        if (lhs.first != rhs.first) {
            return lhs.first >
                rhs.first; // Compare by the first element (float), smallest first
        }
        return lhs.second >
            rhs.second; // If first elements are equal, compare by the second
        // element (size_t), smallest first
    }
};

static std::vector<float>
compute_single_cost(const size_t& source,
                    const AdjacencyMatrix& adjacency_matrix,
                    const int cost_idx) {
    size_t V = adjacency_matrix.size() + 1;
    std::vector<float> dist(V, std::numeric_limits<float>::max());
    std::priority_queue<std::pair<float, size_t>,
                        std::vector<std::pair<float, size_t>>, PairComparator>
        pq;

    dist[source] = 0;
    pq.emplace(0, source);

    while (!pq.empty()) {
        size_t u = pq.top().second;
        pq.pop();

        for (const auto& edge : adjacency_matrix[u]) {  // const& avoids copying Edge+cost vector
            float move_cost = dist[edge.source] + edge.cost[cost_idx];
            if (move_cost < std::numeric_limits<float>::max() && move_cost < dist[edge.target]) {
                dist[edge.target] = move_cost;
                pq.emplace(dist[edge.target], edge.target);
            }
        }
    }
    return dist;
}


static std::vector<std::vector<float>>
compute_single_cost_upper_bound(const size_t& source,
                                const AdjacencyMatrix& adjacency_matrix,
                                const int cost_idx) {
    size_t V = adjacency_matrix.size() + 1;
    std::vector<float> dist(V, std::numeric_limits<float>::max());
    std::vector<std::vector<float>> all_distances(V, std::vector<float>(adjacency_matrix.num_of_objectives, 0));
    std::priority_queue<std::pair<float, size_t>,
                        std::vector<std::pair<float, size_t>>, PairComparator>
        pq;

    // Scratch buffer — allocated once and reused each iteration.
    std::vector<float> all_objectives_move_cost(adjacency_matrix.num_of_objectives, 0);

    dist[source] = 0;
    pq.emplace(0, source);

    while (!pq.empty()) {
        size_t u = pq.top().second;
        pq.pop();

        for (const auto& edge : adjacency_matrix[u]) {  // const& avoids copying Edge+cost vector
            float move_cost = dist[edge.source] + edge.cost[cost_idx];
            if (move_cost < std::numeric_limits<float>::max() && move_cost < dist[edge.target]) {
                for (int i = 0; i < adjacency_matrix.num_of_objectives; i++) {
                    all_objectives_move_cost[i] = all_distances[edge.source][i] + edge.cost[i];
                }
                dist[edge.target] = move_cost;
                all_distances[edge.target] = all_objectives_move_cost;
                pq.emplace(dist[edge.target], edge.target);
            }
        }
    }
    return all_distances;
}

Heuristic ShortestPathHeuristicComputer::compute_ideal_point_heuristic(
    const size_t& source, const AdjacencyMatrix& adjacency_matrix,
    const std::string& output_file) {
    std::vector<std::vector<float>> heuristic_values_for_objectives(
        adjacency_matrix.num_of_objectives);

    for (int j = 0; j < adjacency_matrix.num_of_objectives; j++) {
        heuristic_values_for_objectives[j] =
            compute_single_cost(source, adjacency_matrix, j);
    }
    std::ofstream PlotOutput(output_file);

    for (size_t i = 0; i < adjacency_matrix.size() + 1; i++) {
        PlotOutput << i << '\t' << heuristic_values_for_objectives[0][i] << "\t" << heuristic_values_for_objectives[1][i] << "\n";
    }

    PlotOutput.close();

    // Pre-transpose from [objective][node] to [node][objective] so each lookup
    // is a single contiguous row-copy rather than striding across separate arrays.
    const size_t V = adjacency_matrix.size() + 1;
    const int num_obj = adjacency_matrix.num_of_objectives;
    std::vector<std::vector<float>> transposed(V, std::vector<float>(num_obj));
    for (size_t n = 0; n < V; ++n)
        for (int obj = 0; obj < num_obj; ++obj)
            transposed[n][obj] = heuristic_values_for_objectives[obj][n];

    return [t = std::move(transposed)](size_t index) -> std::vector<float> {
        return t[index];
    };
}

UpperBoundHeuristic ShortestPathHeuristicComputer::compute_ideal_point_heuristic_with_bound(const size_t& target,
                                                             const AdjacencyMatrix& adjacency_matrix) {
    std::vector<std::vector<std::vector<float>>> heuristic_values_for_objectives(
        adjacency_matrix.num_of_objectives);

    for (int j = 0; j < adjacency_matrix.num_of_objectives; j++) {
        heuristic_values_for_objectives[j] =
            compute_single_cost_upper_bound(target, adjacency_matrix, j);
    }

    return [heuristic_values_for_objectives](size_t index) -> std::vector<std::vector<float>> {
        std::vector<std::vector<float>> result;
        result.reserve(heuristic_values_for_objectives.size());
        for (const auto& heuristic_values_for_objective :
             heuristic_values_for_objectives) {
            result.push_back(heuristic_values_for_objective[index]);
        }
        return result;
    };
}
