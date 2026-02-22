#include "doctest.h"
#include "data_structures/apex_path_pair.h"

// Helper: make a Node at the given id with g=(g0,g1) and zero heuristic.
static NodePtr make_node(size_t id, float g0, float g1) {
    return std::make_shared<Node>(id,
                                  std::vector<float>{g0, g1},
                                  std::vector<float>{0.0f, 0.0f});
}

// Helper: make a leaf ApexPathPair (apex == path_node).
static ApexPathPairPtr make_ap(size_t id, float g0, float g1) {
    auto node = make_node(id, g0, g1);
    return std::make_shared<ApexPathPair>(node, node, std::vector<float>{0.0f, 0.0f});
}

// ── is_bounded ────────────────────────────────────────────────────────────────

TEST_CASE("is_bounded: apex->f dominates node->f within eps=0") {
    EPS eps = {0.0, 0.0};
    // apex->f = g + h = (2,3). node->f = (2,3).
    auto apex = make_node(0, 2.0f, 3.0f);
    auto node = make_node(1, 2.0f, 3.0f);
    CHECK(is_bounded(apex, node, eps) == true);
}

TEST_CASE("is_bounded: node->f exceeds apex->f on first objective") {
    EPS eps = {0.0, 0.0};
    auto apex = make_node(0, 2.0f, 3.0f);  // f=(2,3)
    auto node = make_node(1, 3.0f, 3.0f);  // f=(3,3): 3 > 1.0*2
    CHECK(is_bounded(apex, node, eps) == false);
}

TEST_CASE("is_bounded: node->f exceeds apex->f on second objective") {
    EPS eps = {0.0, 0.0};
    auto apex = make_node(0, 2.0f, 3.0f);  // f=(2,3)
    auto node = make_node(1, 2.0f, 4.0f);  // f=(2,4): 4 > 1.0*3
    CHECK(is_bounded(apex, node, eps) == false);
}

TEST_CASE("is_bounded: node->f within eps=0.5 slack") {
    EPS eps = {0.5, 0.5};
    auto apex = make_node(0, 2.0f, 3.0f);  // f=(2,3). bounds: (3,4.5)
    auto node = make_node(1, 3.0f, 4.0f);  // f=(3,4): 3<=3 (T), 4<=4.5 (T)
    CHECK(is_bounded(apex, node, eps) == true);
}

TEST_CASE("is_bounded: node->f exceeds eps=0.5 slack on second objective") {
    EPS eps = {0.5, 0.5};
    auto apex = make_node(0, 2.0f, 3.0f);  // f=(2,3). bound f[1]: 4.5
    auto node = make_node(1, 3.0f, 5.0f);  // f=(3,5): 5 > 4.5
    CHECK(is_bounded(apex, node, eps) == false);
}

// ── ApexPathPair construction ─────────────────────────────────────────────────

TEST_CASE("ApexPathPair: constructed with correct id and is_active=true") {
    auto ap = make_ap(7, 1.0f, 2.0f);
    CHECK(ap->id == 7);
    CHECK(ap->is_active == true);
    CHECK(ap->apex->g[0] == doctest::Approx(1.0f));
    CHECK(ap->apex->g[1] == doctest::Approx(2.0f));
}

TEST_CASE("ApexPathPair: constructed from edge propagates costs") {
    // Parent ap at node 0 with g=(1,2).
    auto parent_ap = make_ap(0, 1.0f, 2.0f);
    Edge e(0, 3, {2.0f, 1.0f});
    auto child_ap = std::make_shared<ApexPathPair>(parent_ap, e,
                                                    std::vector<float>{0.0f, 0.0f});
    CHECK(child_ap->id == 3);
    CHECK(child_ap->apex->g[0] == doctest::Approx(3.0f));  // 1+2
    CHECK(child_ap->apex->g[1] == doctest::Approx(3.0f));  // 2+1
}

// ── update_nodes_by_merge_if_bounded ─────────────────────────────────────────

TEST_CASE("update_nodes_by_merge: different ids → false immediately") {
    EPS eps = {0.0, 0.0};
    auto ap1 = make_ap(1, 1.0f, 2.0f);
    auto ap2 = make_ap(2, 1.0f, 2.0f);
    CHECK(ap1->update_nodes_by_merge_if_bounded(ap2, eps) == false);
}

TEST_CASE("update_nodes_by_merge: same id, path_node outside eps → false") {
    EPS eps = {0.0, 0.0};
    // ap1: id=5, apex->f=(1,2), path_node->f=(1,2).
    // ap2: id=5, apex->f=(2,1), path_node->f=(2,1).
    // f_value = (min(1,2), min(2,1)) = (1,1).
    // For ap1: path_node->f[1]=2 > 1.0*1=1 → false.
    auto ap1 = make_ap(5, 1.0f, 2.0f);
    auto ap2 = make_ap(5, 2.0f, 1.0f);
    CHECK(ap1->update_nodes_by_merge_if_bounded(ap2, eps) == false);
}

TEST_CASE("update_nodes_by_merge: same id, path_node within eps=1.0 → merge updates apex") {
    EPS eps = {1.0, 1.0};
    // ap1: apex->f=(1,2), path_node->f=(1,2).
    // ap2: apex->f=(2,1), path_node->f=(2,1).
    // f_value = (1,1). With eps=1: bound = (2,2).
    // For ap1: 1<=2 (T), 2<=2 (T) → merge.
    auto ap1 = make_ap(5, 1.0f, 2.0f);
    auto ap2 = make_ap(5, 2.0f, 1.0f);
    bool merged = ap1->update_nodes_by_merge_if_bounded(ap2, eps);
    CHECK(merged == true);
    // After merge: apex->g and apex->f updated to component-wise min.
    CHECK(ap1->apex->g[0] == doctest::Approx(1.0f));  // min(1,2)
    CHECK(ap1->apex->g[1] == doctest::Approx(1.0f));  // min(2,1)
}
