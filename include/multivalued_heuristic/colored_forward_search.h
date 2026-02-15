//
// Created by crl on 14/02/2026.
//

#ifndef COLORED_FORWARD_SEARCH_H
#define COLORED_FORWARD_SEARCH_H


#include "data_structures/adjacency_matrix.h"
#include "data_structures/apex_path_pair.h"
#include "definitions.h"
#include "solvers/abstract_solver.h"
#include <iostream>


class ColoredForwardSearch : public AbstractSolver {
public:
  std::vector<std::vector<size_t>> min_g2;
  std::string get_solver_name() override { return "ColoredForwardSearch"; }

  void operator()(const size_t &source, const size_t &target,
                  const MultiValuedHeuristic &heuristic,
                  SolutionSet &solutions);

  void operator()(const size_t &source, const size_t &target,
                  const Heuristic &heuristic, SolutionSet &solutions,
                  unsigned time_limit) override {
    std::cout << "not a real func " << std::endl;
  };

  ColoredForwardSearch(const AdjacencyMatrix &adj_matrix, const EPS &eps);

};

#endif //COLORED_FORWARD_SEARCH_H
