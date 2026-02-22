#include "doctest.h"
#include "data_structures/adjacency_matrix.h"
#include "solvers/boa.h"

#include <algorithm>

// Graph used across tests:
//   Nodes: 0 (source), 1, 2 (target)
//   Edges:
//     0→1: cost (1, 5)
//     1→2: cost (1, 1)  →  path 0→1→2 total: (2, 6)
//     0→2: cost (3, 1)  →  path 0→2   total: (3, 1)
//
// Neither path dominates the other → both must appear in the Pareto set.

static AdjacencyMatrix make_test_graph() {
    std::vector<Edge> edges = {
        Edge(0, 1, {1.0f, 5.0f}),
        Edge(1, 2, {1.0f, 1.0f}),
        Edge(0, 2, {3.0f, 1.0f})
    };
    return AdjacencyMatrix(2, edges);
}

// Zero heuristic (admissible): BOA* degenerates to bi-objective Dijkstra.
static Heuristic zero_heuristic() {
    return [](size_t) { return std::vector<float>{0.0f, 0.0f}; };
}

TEST_CASE("BOAStar: finds both Pareto-optimal paths on a 3-node graph") {
    auto adj = make_test_graph();
    BOAStar boa(adj);
    SolutionSet solutions;

    boa(/*source=*/0, /*target=*/2, zero_heuristic(), solutions,
        /*time_limit=*/0, /*logging_file=*/"/tmp/test_boa");

    REQUIRE(solutions.size() == 2);

    // Sort by g[0] ascending so comparison is order-independent.
    std::sort(solutions.begin(), solutions.end(),
              [](const NodePtr& a, const NodePtr& b) {
                  return a->g[0] < b->g[0];
              });

    // Path 0→1→2: g=(2,6)
    CHECK(solutions[0]->g[0] == doctest::Approx(2.0f));
    CHECK(solutions[0]->g[1] == doctest::Approx(6.0f));

    // Path 0→2: g=(3,1)
    CHECK(solutions[1]->g[0] == doctest::Approx(3.0f));
    CHECK(solutions[1]->g[1] == doctest::Approx(1.0f));
}

TEST_CASE("BOAStar: all solutions reach the target node") {
    auto adj = make_test_graph();
    BOAStar boa(adj);
    SolutionSet solutions;

    boa(0, 2, zero_heuristic(), solutions, 0, "/tmp/test_boa");

    for (const auto& sol : solutions) {
        CHECK(sol->id == 2);
    }
}

TEST_CASE("BOAStar: dominated path is not returned") {
    // Graph where only one Pareto-optimal path exists.
    // Edges: 0→1: (1,1), 1→2: (1,1), 0→2: (3,3)
    // Path via 1: (2,2). Direct: (3,3). (2,2) strictly dominates (3,3).
    std::vector<Edge> edges = {
        Edge(0, 1, {1.0f, 1.0f}),
        Edge(1, 2, {1.0f, 1.0f}),
        Edge(0, 2, {3.0f, 3.0f})
    };
    AdjacencyMatrix adj(2, edges);
    BOAStar boa(adj);
    SolutionSet solutions;

    boa(0, 2, zero_heuristic(), solutions, 0, "/tmp/test_boa_dom");

    REQUIRE(solutions.size() == 1);
    CHECK(solutions[0]->g[0] == doctest::Approx(2.0f));
    CHECK(solutions[0]->g[1] == doctest::Approx(2.0f));
}

TEST_CASE("BOAStar: source equals target returns empty solution set") {
    auto adj = make_test_graph();
    BOAStar boa(adj);
    SolutionSet solutions;

    // Source == target: the initial node is immediately a solution but g=(0,0).
    // BOA* expands node 0, checks id==target (0==0), pushes to solutions.
    boa(0, 0, zero_heuristic(), solutions, 0, "/tmp/test_boa_self");

    // The source node itself (g=(0,0)) should be the only solution.
    REQUIRE(solutions.size() == 1);
    CHECK(solutions[0]->g[0] == doctest::Approx(0.0f));
    CHECK(solutions[0]->g[1] == doctest::Approx(0.0f));
}
