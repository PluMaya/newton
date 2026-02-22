//
// Created by crl on 21/02/2026.
//

#ifndef BOA_BACKWARD_SEARCH_H
#define BOA_BACKWARD_SEARCH_H
#include "definitions.h"
#include "data_structures/adjacency_matrix.h"
#include <ctime>


class BOABackwardSearch {
public:
    const AdjacencyMatrix& adj_matrix;
    std::clock_t start_time = std::clock();
    std::clock_t runtime = std::clock();
    size_t num_expansion = 0;
    size_t num_generation = 0;

    MultiValuedHeuristic operator()(const size_t& source);
    virtual ~BOABackwardSearch() = default;

    explicit BOABackwardSearch(const AdjacencyMatrix& adj_matrix);
};

#endif //BOA_BACKWARD_SEARCH_H
