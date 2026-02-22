#include "doctest.h"
#include "multivalued_heuristic/post_process.h"

// Helper: build a MultiValuedHeuristic with a single node's h-list.
// All other nodes are left empty (size 1, node 0 only).
static MultiValuedHeuristic make_mvh(std::vector<std::vector<float>> h_list) {
    MultiValuedHeuristic mvh(1);
    mvh[0] = std::move(h_list);
    return mvh;
}

// ── Empty / trivial inputs ────────────────────────────────────────────────────

TEST_CASE("PostProcess: empty h-list for a node passes through unchanged") {
    MultiValuedHeuristic mvh(1);  // node 0 has no h-values
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {0.0, 0.0});
    CHECK(result[0].empty());
}

TEST_CASE("PostProcess: single h-value returned as-is (no staircase)") {
    // With only one point there is no staircase to build.
    auto mvh = make_mvh({{1.0f, 4.0f}});
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {0.0, 0.0});
    REQUIRE(result[0].size() == 1);
    CHECK(result[0][0][0] == doctest::Approx(1.0f));
    CHECK(result[0][0][1] == doctest::Approx(4.0f));
}

// ── Staircase generation with eps=0 ──────────────────────────────────────────

TEST_CASE("PostProcess: two Pareto points produce one staircase edge") {
    // Input: [(1,4),(2,3)] — neither dominates the other.
    // eps=0: no merging. out=[(1,4),(2,3)].
    // stair = [{1, 3}]  (out[0][0], out[1][1])
    auto mvh = make_mvh({{1.0f, 4.0f}, {2.0f, 3.0f}});
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {0.0, 0.0});
    REQUIRE(result[0].size() == 1);
    CHECK(result[0][0][0] == doctest::Approx(1.0f));
    CHECK(result[0][0][1] == doctest::Approx(3.0f));
}

TEST_CASE("PostProcess: three Pareto points produce two staircase edges") {
    // Input: [(1,4),(2,3),(3,1)].
    // eps=0: no merging. out=[(1,4),(2,3),(3,1)].
    // stair = [(1,3),(2,1)]
    auto mvh = make_mvh({{1.0f, 4.0f}, {2.0f, 3.0f}, {3.0f, 1.0f}});
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {0.0, 0.0});
    REQUIRE(result[0].size() == 2);
    CHECK(result[0][0][0] == doctest::Approx(1.0f));
    CHECK(result[0][0][1] == doctest::Approx(3.0f));
    CHECK(result[0][1][0] == doctest::Approx(2.0f));
    CHECK(result[0][1][1] == doctest::Approx(1.0f));
}

// ── Epsilon merging ───────────────────────────────────────────────────────────

TEST_CASE("PostProcess: eps=(0.5,0.5) merges two close points") {
    // Input: [(1,4),(2,3)].
    // e0=1.5, e1=1.5. Check: 1<=1.5*2=3 (T), 4<=1.5*3=4.5 (T) → merge.
    // After merge: out=[(1,3)] — size 1, no staircase.
    auto mvh = make_mvh({{1.0f, 4.0f}, {2.0f, 3.0f}});
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {0.5, 0.5});
    REQUIRE(result[0].size() == 1);
    CHECK(result[0][0][0] == doctest::Approx(1.0f));
    CHECK(result[0][0][1] == doctest::Approx(3.0f));  // last[1] updated to cur[1]
}

TEST_CASE("PostProcess: eps=(0.5,0.5) merges first two, keeps third") {
    // Input: [(1,4),(2,3),(3,1)].
    // merge (1,4)+(2,3) → (1,3). Then (3,1): 1<=1.5*3=4.5 (T), 3<=1.5*1=1.5 (F) → push.
    // out=[(1,3),(3,1)]. stair=[(1,1)].
    auto mvh = make_mvh({{1.0f, 4.0f}, {2.0f, 3.0f}, {3.0f, 1.0f}});
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {0.5, 0.5});
    REQUIRE(result[0].size() == 1);
    CHECK(result[0][0][0] == doctest::Approx(1.0f));
    CHECK(result[0][0][1] == doctest::Approx(1.0f));
}

TEST_CASE("PostProcess: eps=(1,1) merges all three points") {
    // e0=2, e1=2.
    // (1,4)+(2,3): 1<=4 (T), 4<=6 (T) → merge → (1,3).
    // (1,3)+(3,1): 1<=6 (T), 3<=2 (F) → push.
    // out=[(1,3),(3,1)]. stair=[(1,1)].
    auto mvh = make_mvh({{1.0f, 4.0f}, {2.0f, 3.0f}, {3.0f, 1.0f}});
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {1.0, 1.0});
    REQUIRE(result[0].size() == 1);
    CHECK(result[0][0][0] == doctest::Approx(1.0f));
    CHECK(result[0][0][1] == doctest::Approx(1.0f));
}

// ── Multiple nodes ────────────────────────────────────────────────────────────

TEST_CASE("PostProcess: each node processed independently") {
    // Node 0: [(1,4),(2,3)] → stair=[(1,3)]
    // Node 1: [(2,5),(4,2)] → stair=[(2,2)]
    MultiValuedHeuristic mvh(2);
    mvh[0] = {{1.0f, 4.0f}, {2.0f, 3.0f}};
    mvh[1] = {{2.0f, 5.0f}, {4.0f, 2.0f}};
    PostProcess pp;
    auto result = pp(mvh, /*target=*/0, {0.0, 0.0});

    REQUIRE(result[0].size() == 1);
    CHECK(result[0][0][0] == doctest::Approx(1.0f));
    CHECK(result[0][0][1] == doctest::Approx(3.0f));

    REQUIRE(result[1].size() == 1);
    CHECK(result[1][0][0] == doctest::Approx(2.0f));
    CHECK(result[1][0][1] == doctest::Approx(2.0f));
}
