#include "multivalued_heuristic/colored_forward_search.h"
#include "solvers/apex.h"

#include <cassert>
#include <fstream>
#include <optional>

ColoredForwardSearch::ColoredForwardSearch(const AdjacencyMatrix &adj_matrix, const EPS &eps)
    : AbstractSolver(adj_matrix, eps) {}


void ColoredForwardSearch::operator()(
  const size_t &source, const size_t &target, const MultiValuedHeuristic &heuristic, SolutionSet &solutions) {
  init_search();
  min_g2.assign(adj_matrix.size() + 1, {});
  std::vector<std::map<std::pair<int,int>, int>> heuristics;

  for (size_t s = 0; s < adj_matrix.size() + 1; ++s) {
    const size_t K = heuristic[s].size();  // number of heuristics for this statex
    min_g2[s].resize(K);
    for (size_t i = 0; i < K; ++i) {min_g2[s][i] = MAX_COST;}
  }
  start_time = std::clock();

  std::priority_queue<NodePtr, std::vector<NodePtr>, CompareNodeByFValue> open;

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

    if (min_g2[node->id][node->h_idx] <= node->g[1]) {  // local dominance check
      continue;
    }

    if (min_g2[target][0] <= node->f[1]) {  // global dominance check
      continue;
    }

    num_expansion += 1;
    min_g2[node->id][node->h_idx] = node->g[1];

    if (node->id == target) {
      solutions.push_back(node);
      continue;
    }

    const std::vector<Edge> &outgoing_edges = adj_matrix[node->id];
    for (const auto &outgoing_edge : outgoing_edges) {
      std::vector new_g(node->g);
      for (int i = 0; i < new_g.size(); i++) {
        new_g[i] += outgoing_edge.cost[i];
      }

      for (int i = 0; i < heuristic[outgoing_edge.target].size(); i++) {
        const auto hvalue = heuristic[outgoing_edge.target][i];

        if ((node->h[0] <= hvalue[0] + outgoing_edge.cost[0]) && (node->h[1] <= hvalue[1] + outgoing_edge.cost[1])) {
          NodePtr successor_node = std::make_shared<Node>(
              outgoing_edge.target, new_g, hvalue, node, outgoing_edge.cost, i);
          if (min_g2[successor_node->id][i] <= successor_node->g[1]) {  // local dominance check
            continue;
          }
          if (min_g2[target][0] <= successor_node->f[1]) {  // global dominance check
            continue;
          }
          open.push(successor_node);
        }
      }
    }
  }


  std::ofstream PlotOutput("coloredfs.txt");

  for (auto const &solution : solutions) {
    PlotOutput << solution->g[0] << "," << solution->g[1] << std::endl;
  }

  PlotOutput.close();

  runtime = static_cast<float>(std::clock() - start_time);
}