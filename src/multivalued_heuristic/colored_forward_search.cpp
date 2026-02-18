#include "multivalued_heuristic/colored_forward_search.h"
#include "solvers/apex.h"

#include <cassert>
#include <fstream>
#include <optional>
#include <sstream>

ColoredForwardSearch::ColoredForwardSearch(const AdjacencyMatrix& adj_matrix, const EPS& eps)
    : AbstractSolver(adj_matrix, eps) {
}


void ColoredForwardSearch::operator()(
    const size_t& source, const size_t& target, const MultiValuedHeuristic& heuristic, SolutionSet& solutions,
    const std::string& logging_file) {
    init_search();
    min_g2.assign(adj_matrix.size() + 1, {});
    std::vector<std::vector<float>> generated;
    generated.assign(adj_matrix.size() + 1, {});
    std::vector<std::vector<float>> expanded;
    expanded.assign(adj_matrix.size() + 1, {});

    for (size_t s = 0; s < adj_matrix.size() + 1; ++s) {
        const size_t K = heuristic[s].size(); // number of heuristics for this statex
        min_g2[s].resize(K);
        generated[s].resize(K);
        expanded[s].resize(K);
        for (size_t i = 0; i < K; ++i) {
            min_g2[s][i] = MAX_COST;
            generated[s][i] = 0;
            expanded[s][i] = 0;
        }
    }

    std::priority_queue<NodePtr, std::vector<NodePtr>, CompareNodeByFValue> open;

    auto initialization_time = std::clock();

    for (int i = 0; i < heuristic[source].size(); i++) {
        NodePtr source_node = std::make_unique<Node>(
            source,
            std::vector<float>(2, 0),
            heuristic[source][i],
            heuristic[source][i],
            std::vector<float>(2, 0),
            nullptr,
            i);
        open.push(source_node);
    }

    while (!open.empty()) {
        auto node = open.top();
        open.pop();
        num_generation += 1;
        generated[node->id][node->h_idx] += 1;

        if (min_g2[node->id][node->h_idx] <= node->g[1]) {
            // local dominance check
            continue;
        }

        if (min_g2[target][0] <= node->f[1]) {
            // global dominance check
            continue;
        }

        num_expansion += 1;
        min_g2[node->id][node->h_idx] = node->g[1];
        expanded[node->id][node->h_idx] += 1;

        if (node->id == target) {
            solutions.push_back(node);
            continue;
        }

        const std::vector<Edge>& outgoing_edges = adj_matrix[node->id];
        for (const auto& outgoing_edge : outgoing_edges) {
            std::vector new_g(node->g);
            for (int i = 0; i < new_g.size(); i++) {
                new_g[i] += outgoing_edge.cost[i];
            }

            for (int i = 0; i < heuristic[outgoing_edge.target].size(); i++) {
                const auto hvalue = heuristic[outgoing_edge.target][i];

                if ((node->h[0] <= hvalue[0] + outgoing_edge.cost[0]) && (node->h[1] <= hvalue[1] + outgoing_edge.cost[
                    1])) {
                    NodePtr successor_node = std::make_shared<Node>(
                        outgoing_edge.target, new_g, hvalue, node, outgoing_edge.cost, i);
                    if (min_g2[successor_node->id][i] <= successor_node->g[1]) {
                        // local dominance check
                        continue;
                    }
                    if (min_g2[target][0] <= successor_node->f[1]) {
                        // global dominance check
                        continue;
                    }
                    open.push(successor_node);
                }
            }
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
        for (size_t i = 0; i < heuristic[s].size(); i++) {
            StatsOutput << s << "\t" << i << "\t" << heuristic[s][i][0] <<',' << heuristic[s][i][1] << "\t" << generated[s][i] << "\t" << expanded[s][i] << std::endl;
        }
     }

    StatsOutput.close();
}
