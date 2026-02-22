#include "doctest.h"
#include "data_structures/adjacency_matrix.h"

// ── Construction ─────────────────────────────────────────────────────────────

TEST_CASE("AdjacencyMatrix: single edge stored correctly") {
    std::vector<Edge> edges = {Edge(0, 1, {1.0f, 2.0f})};
    AdjacencyMatrix adj(1, edges);

    CHECK(adj.size() == 1);
    REQUIRE(adj[0].size() == 1);
    CHECK(adj[0][0].source == 0);
    CHECK(adj[0][0].target == 1);
    CHECK(adj[0][0].cost[0] == doctest::Approx(1.0f));
    CHECK(adj[0][0].cost[1] == doctest::Approx(2.0f));
}

TEST_CASE("AdjacencyMatrix: nodes with no outgoing edges are empty") {
    std::vector<Edge> edges = {Edge(0, 2, {1.0f, 1.0f})};
    AdjacencyMatrix adj(2, edges);

    CHECK(adj[1].size() == 0);  // node 1 has no outgoing edges
    CHECK(adj[2].size() == 0);  // node 2 is a sink
}

TEST_CASE("AdjacencyMatrix: multiple edges from same source") {
    std::vector<Edge> edges = {
        Edge(0, 1, {1.0f, 3.0f}),
        Edge(0, 2, {4.0f, 1.0f})
    };
    AdjacencyMatrix adj(2, edges);

    REQUIRE(adj[0].size() == 2);
    CHECK(adj[1].size() == 0);
}

TEST_CASE("AdjacencyMatrix: num_of_objectives inferred from first edge") {
    std::vector<Edge> edges = {Edge(0, 1, {1.0f, 2.0f})};
    AdjacencyMatrix adj(1, edges);
    CHECK(adj.num_of_objectives == 2);
}

// ── Inverse construction ──────────────────────────────────────────────────────

TEST_CASE("AdjacencyMatrix: inverse=true adds reverse edges") {
    std::vector<Edge> edges = {Edge(0, 1, {1.0f, 2.0f})};
    AdjacencyMatrix adj(1, edges, /*inverse=*/true);

    REQUIRE(adj[0].size() == 1);  // forward 0→1
    REQUIRE(adj[1].size() == 1);  // reverse 1→0
    CHECK(adj[1][0].target == 0);
    CHECK(adj[1][0].cost[0] == doctest::Approx(1.0f));
    CHECK(adj[1][0].cost[1] == doctest::Approx(2.0f));
}

TEST_CASE("AdjacencyMatrix: inverse=false does not add reverse edges") {
    std::vector<Edge> edges = {Edge(0, 1, {1.0f, 2.0f})};
    AdjacencyMatrix adj(1, edges, /*inverse=*/false);

    CHECK(adj[1].size() == 0);
}

// ── add() ─────────────────────────────────────────────────────────────────────

TEST_CASE("AdjacencyMatrix: add() appends an edge") {
    std::vector<Edge> edges = {Edge(0, 1, {1.0f, 1.0f})};
    AdjacencyMatrix adj(2, edges);

    adj.add(Edge(0, 2, {3.0f, 3.0f}));
    REQUIRE(adj[0].size() == 2);
    CHECK(adj[0][1].target == 2);
}
