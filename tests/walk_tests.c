#define CELL_IMPL
#include "../src/lib/cell.h"

#define GRID_IMPL
#include "../src/lib/grid.h"

#define PATH_IMPL
#include "../src/lib/path.h"

#define QUEUE_IMPL
#include "../src/lib/queue.h"

#define STACK_IMPL
#include "../src/lib/stack.h"

#define PREPROC_IMPL
#include "../src/lib/preprocessing.h"

#define WALK_IMPL
#include "../src/lib/walk.h"

#include "common.h"

static grid* make_cross(void) {
    grid* g = grid_new(3, 3);
    grid_set(g, 0, 0); grid_set(g, 0, 2);
    grid_set(g, 2, 0); grid_set(g, 2, 2);
    return g;
}

TEST(walk, pick_start_1x3_strip) {
    // Arrange
    grid* g = grid_new(1, 3);
    grid_context* ctx = grid_preprocess(g);
    // Act
    cell start = walk_pick_first_good_start(g, ctx);
    // Assert
    EXPECT_EQ(start.idx, 0u);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk, pick_start_all_blocked) {
    // Arrange
    grid* g = grid_new(3, 3);
    for (size_t r = 0; r < 3; ++r)
        for (size_t c = 0; c < 3; ++c)
            grid_set(g, r, c);
    grid_context* ctx = grid_preprocess(g);
    // Act
    cell start = walk_pick_first_good_start(g, ctx);
    // Assert
    EXPECT_TRUE(cell_eq(start, CELL_NONE));
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, single_free_cell) {
    // Arrange
    grid* g = grid_new(1, 1);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 5, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(p->size, 1);
    EXPECT_SIZE_EQ(path_unique_cells(p), 1);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, strip_1x3_all) {
    // Arrange
    grid* g = grid_new(1, 3);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 10, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 3);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, strip_1x4_limited_steps) {
    // Arrange
    grid* g = grid_new(1, 4);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 2, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 3);
    EXPECT_SIZE_EQ(p->size, 3);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, open_2x2) {
    // Arrange
    grid* g = grid_new(2, 2);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 10, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 4);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, cross_center_is_cut) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 20, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 5);
    EXPECT_TRUE(p->size > path_unique_cells(p));
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, cross_backtrack_visible) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 5, start, ctx);
    // Assert
    EXPECT_TRUE(p->size > path_unique_cells(p));
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, max_steps_zero) {
    // Arrange
    grid* g = grid_new(3, 3);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 0, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(p->size, 1);
    EXPECT_SIZE_EQ(path_unique_cells(p), 1);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_deep, open_3x3_large_steps) {
    // Arrange
    grid* g = grid_new(3, 3);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_deep(g, 100, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 9);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_random, reproducibility) {
    // Arrange
    grid* g = grid_new(3, 3);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* a = walk_solve_random(g, 10, start, ctx, 42);
    path* b = walk_solve_random(g, 10, start, ctx, 42);
    // Assert
    EXPECT_SIZE_EQ(a->size, b->size);
    for (size_t i = 0; i < a->size; ++i)
        EXPECT_TRUE(cell_eq(path_at(a, i), path_at(b, i)));
    path_free(a);
    path_free(b);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_warnsdorff, strip_1x4_all_unique) {
    // Arrange
    grid* g = grid_new(1, 4);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_warnsdorff(g, 10, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 4);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_warnsdorff, cross_center_is_cut) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_warnsdorff(g, 20, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 5);
    EXPECT_TRUE(p->size > path_unique_cells(p));
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_warnsdorff, cross_backtrack_visible) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_warnsdorff(g, 5, start, ctx);
    // Assert
    EXPECT_TRUE(p->size > path_unique_cells(p));
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_heuristic, strip_1x4_all_unique) {
    // Arrange
    grid* g = grid_new(1, 4);
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_heuristic(g, 10, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 4);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_heuristic, cross_center_is_cut) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_heuristic(g, 20, start, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 5);
    EXPECT_TRUE(p->size > path_unique_cells(p));
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_heuristic, cross_backtrack_visible) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    cell start = walk_pick_first_good_start(g, ctx);
    // Act
    path* p = walk_solve_heuristic(g, 5, start, ctx);
    // Assert
    EXPECT_TRUE(p->size > path_unique_cells(p));
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_multistart, all_blocked_returns_null) {
    // Arrange
    grid* g = grid_new(3, 3);
    for (size_t r = 0; r < 3; ++r)
        for (size_t c = 0; c < 3; ++c)
            grid_set(g, r, c);
    grid_context* ctx = grid_preprocess(g);
    // Act
    path* p = walk_solve_multistart(g, 10, walk_solve_deep, ctx);
    // Assert
    EXPECT_TRUE(p == NULL);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_multistart, cross_shape) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    // Act
    path* p = walk_solve_multistart(g, 20, walk_solve_deep, ctx);
    // Assert
    EXPECT_TRUE(p != NULL);
    EXPECT_SIZE_EQ(path_unique_cells(p), 5);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_multistart, single_free) {
    // Arrange
    grid* g = grid_new(1, 1);
    grid_context* ctx = grid_preprocess(g);
    // Act
    path* p = walk_solve_multistart(g, 5, walk_solve_deep, ctx);
    // Assert
    EXPECT_TRUE(p != NULL);
    EXPECT_SIZE_EQ(path_unique_cells(p), 1);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_all_free, all_blocked_returns_null) {
    // Arrange
    grid* g = grid_new(3, 3);
    for (size_t r = 0; r < 3; ++r)
        for (size_t c = 0; c < 3; ++c)
            grid_set(g, r, c);
    grid_context* ctx = grid_preprocess(g);
    // Act
    path* p = walk_solve_all_free(g, 10, walk_solve_deep, ctx);
    // Assert
    EXPECT_TRUE(p == NULL);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_all_free, strip_1x3_all) {
    // Arrange
    grid* g = grid_new(1, 3);
    grid_context* ctx = grid_preprocess(g);
    // Act
    path* p = walk_solve_all_free(g, 10, walk_solve_deep, ctx);
    // Assert
    EXPECT_TRUE(p != NULL);
    EXPECT_SIZE_EQ(path_unique_cells(p), 3);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_all_free, cross_shape) {
    // Arrange
    grid* g = make_cross();
    grid_context* ctx = grid_preprocess(g);
    // Act
    path* p = walk_solve_all_free(g, 20, walk_solve_deep, ctx);
    // Assert
    EXPECT_TRUE(p != NULL);
    EXPECT_SIZE_EQ(path_unique_cells(p), 5);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

TEST(walk_all_free, limited_steps) {
    // Arrange
    grid* g = grid_new(1, 4);
    grid_context* ctx = grid_preprocess(g);
    // Act
    path* p = walk_solve_all_free(g, 2, walk_solve_deep, ctx);
    // Assert
    EXPECT_SIZE_EQ(path_unique_cells(p), 3);
    path_free(p);
    grid_context_free(ctx);
    grid_free(g);
}

int main(void) {
    RUN_TEST(walk, pick_start_1x3_strip);
    RUN_TEST(walk, pick_start_all_blocked);
    // These are mostly smoke tests, but some examples from preprocessing were reused
    RUN_TEST(walk_deep, single_free_cell);
    RUN_TEST(walk_deep, strip_1x3_all);
    RUN_TEST(walk_deep, strip_1x4_limited_steps);
    RUN_TEST(walk_deep, open_2x2);
    RUN_TEST(walk_deep, cross_center_is_cut);
    RUN_TEST(walk_deep, cross_backtrack_visible);
    RUN_TEST(walk_deep, max_steps_zero);
    RUN_TEST(walk_deep, open_3x3_large_steps);
    RUN_TEST(walk_random, reproducibility);
    RUN_TEST(walk_warnsdorff, strip_1x4_all_unique);
    RUN_TEST(walk_warnsdorff, cross_center_is_cut);
    RUN_TEST(walk_warnsdorff, cross_backtrack_visible);
    RUN_TEST(walk_heuristic, strip_1x4_all_unique);
    RUN_TEST(walk_heuristic, cross_center_is_cut);
    RUN_TEST(walk_heuristic, cross_backtrack_visible);
    RUN_TEST(walk_multistart, all_blocked_returns_null);
    RUN_TEST(walk_multistart, cross_shape);
    RUN_TEST(walk_multistart, single_free);
    RUN_TEST(walk_all_free, all_blocked_returns_null);
    RUN_TEST(walk_all_free, strip_1x3_all);
    RUN_TEST(walk_all_free, cross_shape);
    RUN_TEST(walk_all_free, limited_steps);
    return test_summary();
}
