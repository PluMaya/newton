#include "doctest.h"
#include "data_structures/node.h"

#include <queue>
#include <vector>

// ── Construction ─────────────────────────────────────────────────────────────

TEST_CASE("Node: f = g + h at construction") {
    Node n(1, {0.0f, 0.0f}, {3.0f, 4.0f});
    CHECK(n.f[0] == doctest::Approx(3.0f));
    CHECK(n.f[1] == doctest::Approx(4.0f));
}

TEST_CASE("Node: f = g + h with nonzero g") {
    Node n(2, {1.0f, 2.0f}, {3.0f, 4.0f});
    CHECK(n.f[0] == doctest::Approx(4.0f));
    CHECK(n.f[1] == doctest::Approx(6.0f));
}

TEST_CASE("Node: id and h_idx stored correctly") {
    Node n(42, {0.0f, 0.0f}, {0.0f, 0.0f}, nullptr,
           std::vector<float>(2, 0.0f), /*h_idx=*/3);
    CHECK(n.id == 42);
    CHECK(n.h_idx == 3);
}

TEST_CASE("Node: second constructor uses explicit f values") {
    // Constructor: (id, g, h, f, c, parent, h_idx)
    std::vector<float> g = {1.0f, 2.0f};
    std::vector<float> h = {3.0f, 4.0f};
    std::vector<float> f = {10.0f, 20.0f};  // override computed f
    std::vector<float> c(2, 0.0f);
    Node n(5, g, h, f, c);
    CHECK(n.f[0] == doctest::Approx(10.0f));
    CHECK(n.f[1] == doctest::Approx(20.0f));
}

// ── Comparator ───────────────────────────────────────────────────────────────

TEST_CASE("CompareNodeByFValue: smaller f[0] gets higher priority") {
    auto a = std::make_shared<Node>(1, std::vector<float>{1.0f, 0.0f},
                                       std::vector<float>{0.0f, 0.0f});
    auto b = std::make_shared<Node>(2, std::vector<float>{2.0f, 0.0f},
                                       std::vector<float>{0.0f, 0.0f});
    CompareNodeByFValue cmp;
    // cmp returns true when a has *lower* priority (max-heap semantics).
    // a->f=(1,0) < b->f=(2,0) lexicographically → a->f > b->f is false → a has higher priority.
    CHECK(cmp(a, b) == false);
    CHECK(cmp(b, a) == true);
}

TEST_CASE("CompareNodeByFValue: tie on f[0] broken by f[1]") {
    auto a = std::make_shared<Node>(1, std::vector<float>{1.0f, 1.0f},
                                       std::vector<float>{0.0f, 0.0f});
    auto b = std::make_shared<Node>(2, std::vector<float>{1.0f, 2.0f},
                                       std::vector<float>{0.0f, 0.0f});
    CompareNodeByFValue cmp;
    CHECK(cmp(a, b) == false);  // (1,1) < (1,2) → a has higher priority
    CHECK(cmp(b, a) == true);
}

TEST_CASE("CompareNodeByFValue: priority_queue pops minimum f first") {
    std::priority_queue<NodePtr, std::vector<NodePtr>, CompareNodeByFValue> pq;
    pq.push(std::make_shared<Node>(0, std::vector<float>{3.0f, 0.0f},
                                      std::vector<float>{0.0f, 0.0f}));
    pq.push(std::make_shared<Node>(1, std::vector<float>{1.0f, 0.0f},
                                      std::vector<float>{0.0f, 0.0f}));
    pq.push(std::make_shared<Node>(2, std::vector<float>{2.0f, 0.0f},
                                      std::vector<float>{0.0f, 0.0f}));
    CHECK(pq.top()->f[0] == doctest::Approx(1.0f));
    pq.pop();
    CHECK(pq.top()->f[0] == doctest::Approx(2.0f));
    pq.pop();
    CHECK(pq.top()->f[0] == doctest::Approx(3.0f));
}
