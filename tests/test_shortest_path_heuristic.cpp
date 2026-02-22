#include "doctest.h"
#include "data_structures/adjacency_matrix.h"
#include "solvers/shortest_path_heuristic_computer.h"

// Forward graph: 0→1 (1,5), 1→2 (1,1), 0→2 (3,1).
// To build the backward heuristic from target=2, we run Dijkstra
// from node 2 on the INVERSE graph.
//
// Inverse edges: 1→0 (1,5), 2→1 (1,1), 2→0 (3,1).
//
// Dijkstra from node 2 on inverse graph:
//   Objective 0:
//     dist[2]=0, dist[1]=1 (2→1), dist[0]=min(3, 1+1)=2 (via 2→1→0)
//   Objective 1:
//     dist[2]=0, dist[1]=1 (2→1), dist[0]=min(1, 1+5)=1 (via 2→0 directly)
//
// Expected ideal-point heuristic values:
//   h(0) = (2, 1)
//   h(1) = (1, 1)
//   h(2) = (0, 0)

static AdjacencyMatrix make_inverse_graph() {
    std::vector<Edge> inverse_edges = {
        Edge(1, 0, {1.0f, 5.0f}),
        Edge(2, 1, {1.0f, 1.0f}),
        Edge(2, 0, {3.0f, 1.0f})
    };
    return AdjacencyMatrix(2, inverse_edges);
}

TEST_CASE("compute_ideal_point_heuristic: h(target) = (0,0)") {
    auto inv = make_inverse_graph();
    auto h = ShortestPathHeuristicComputer::compute_ideal_point_heuristic(
        /*source (= target node 2)=*/2, inv, "/tmp/test_iph.txt");

    auto h2 = h(2);
    CHECK(h2[0] == doctest::Approx(0.0f));
    CHECK(h2[1] == doctest::Approx(0.0f));
}

TEST_CASE("compute_ideal_point_heuristic: h(intermediate node) correct") {
    auto inv = make_inverse_graph();
    auto h = ShortestPathHeuristicComputer::compute_ideal_point_heuristic(2, inv, "/tmp/test_iph.txt");

    // Node 1: shortest on obj0 = 2→1 = 1. Shortest on obj1 = 2→1 = 1.
    auto h1 = h(1);
    CHECK(h1[0] == doctest::Approx(1.0f));
    CHECK(h1[1] == doctest::Approx(1.0f));
}

TEST_CASE("compute_ideal_point_heuristic: h(source node) correct") {
    auto inv = make_inverse_graph();
    auto h = ShortestPathHeuristicComputer::compute_ideal_point_heuristic(2, inv, "/tmp/test_iph.txt");

    // Node 0: shortest on obj0 = 2→1→0 = 2. Shortest on obj1 = 2→0 = 1.
    auto h0 = h(0);
    CHECK(h0[0] == doctest::Approx(2.0f));
    CHECK(h0[1] == doctest::Approx(1.0f));
}

TEST_CASE("compute_ideal_point_heuristic: returns vector of size num_of_objectives") {
    auto inv = make_inverse_graph();
    auto h = ShortestPathHeuristicComputer::compute_ideal_point_heuristic(2, inv, "/tmp/test_iph.txt");

    CHECK(h(0).size() == 2);
    CHECK(h(1).size() == 2);
    CHECK(h(2).size() == 2);
}

TEST_CASE("compute_ideal_point_heuristic: heuristic is admissible on test graph") {
    // An admissible heuristic must never overestimate the true cost.
    // True costs from each node to target=2:
    //   From 0: min obj0 path = 0→1→2 = 2. Min obj1 path = 0→2 = 1.
    //   From 1: only 1→2 available = (1,1).
    //   From 2: at goal = (0,0).
    auto inv = make_inverse_graph();
    auto h = ShortestPathHeuristicComputer::compute_ideal_point_heuristic(2, inv, "/tmp/test_iph.txt");

    auto h0 = h(0);
    CHECK(h0[0] <= doctest::Approx(2.0f));  // obj0 true min = 2
    CHECK(h0[1] <= doctest::Approx(1.0f));  // obj1 true min = 1

    auto h1 = h(1);
    CHECK(h1[0] <= doctest::Approx(1.0f));
    CHECK(h1[1] <= doctest::Approx(1.0f));
}
