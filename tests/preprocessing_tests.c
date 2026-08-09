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

#include "common.h"

TEST(grid_context, new_small) {
    // Arrange
    // Act
    grid_context* ctx = grid_context_new(16, 4);
    // Assert
    EXPECT_TRUE(ctx != NULL);
    EXPECT_SIZE_EQ(ctx->cell_count, 16);
    EXPECT_SIZE_EQ(ctx->cols, 4);
    EXPECT_SIZE_EQ(ctx->component_count, 0);
    EXPECT_TRUE(ctx->component_sizes == NULL);
    grid_context_free(ctx);
}

TEST(grid_context, new_zero_dimensions) {
    // Arrange
    // Act
    grid_context* a = grid_context_new(0, 4);
    grid_context* b = grid_context_new(16, 0);
    // Assert
    EXPECT_TRUE(a == NULL);
    EXPECT_TRUE(b == NULL);
}

TEST(grid_context, free_null) {
    // Arrange
    // Act
    grid_context_free(NULL);
    // Assert
}

TEST(grid_context, cells_zeroed_after_new) {
    // Arrange
    // Act
    grid_context* ctx = grid_context_new(8, 4);
    // Assert
    EXPECT_TRUE(ctx != NULL);
    for (size_t i = 0; i < ctx->cell_count; ++i) {
        EXPECT_EQ(ctx->cells[i].component_id, 0u);
        EXPECT_EQ(ctx->cells[i].cut_distance, 0u);
        EXPECT_EQ(ctx->cells[i].neighbour_count, 0);
        EXPECT_FALSE(ctx->cells[i].is_cut_vertex);
        EXPECT_FALSE(ctx->cells[i].is_good_start);
    }
    grid_context_free(ctx);
}

TEST(grid_context, alloc_component_sizes_null_context) {
    // Arrange
    // Act
    bool ok = grid_context_alloc_component_sizes(NULL);
    // Assert
    EXPECT_FALSE(ok);
}

TEST(grid_context, alloc_component_sizes_zero_components) {
    // Arrange
    grid_context* ctx = grid_context_new(16, 4);
    EXPECT_TRUE(ctx != NULL);
    // Act
    bool ok = grid_context_alloc_component_sizes(ctx);
    // Assert
    EXPECT_FALSE(ok);
    grid_context_free(ctx);
}

TEST(grid_context, alloc_component_sizes_success) {
    // Arrange
    grid_context* ctx = grid_context_new(16, 4);
    EXPECT_TRUE(ctx != NULL);
    ctx->component_count = 3;
    // Act
    bool ok = grid_context_alloc_component_sizes(ctx);
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_TRUE(ctx->component_sizes != NULL);
    for (size_t i = 0; i <= 3; ++i)
        EXPECT_SIZE_EQ(ctx->component_sizes[i], 0);
    grid_context_free(ctx);
}

TEST(grid_context, alloc_component_sizes_twice_fails) {
    // Arrange
    grid_context* ctx = grid_context_new(16, 4);
    EXPECT_TRUE(ctx != NULL);
    ctx->component_count = 2;
    EXPECT_TRUE(grid_context_alloc_component_sizes(ctx));
    // Act
    bool ok = grid_context_alloc_component_sizes(ctx);
    // Assert
    EXPECT_FALSE(ok);
    grid_context_free(ctx);
}

TEST(grid_context, copy_fields_and_cells) {
    // Arrange
    grid_context* src = grid_context_new(16, 4);
    EXPECT_TRUE(src != NULL);
    src->component_count = 2;
    EXPECT_TRUE(grid_context_alloc_component_sizes(src));
    src->component_sizes[1] = 5;
    src->component_sizes[2] = 3;
    src->cells[3].is_cut_vertex = true;
    src->cells[7].component_id  = 1;
    // Act
    grid_context* dst = grid_context_copy(src);
    // Assert
    EXPECT_TRUE(dst != NULL);
    EXPECT_SIZE_EQ(dst->cell_count, 16);
    EXPECT_SIZE_EQ(dst->cols, 4);
    EXPECT_SIZE_EQ(dst->component_count, 2);
    EXPECT_SIZE_EQ(dst->component_sizes[1], 5);
    EXPECT_SIZE_EQ(dst->component_sizes[2], 3);
    EXPECT_TRUE(dst->cells[3].is_cut_vertex);
    EXPECT_EQ(dst->cells[7].component_id, 1u);
    grid_context_free(src);
    grid_context_free(dst);
}

TEST(grid_context, copy_is_deep) {
    // Arrange
    grid_context* src = grid_context_new(4, 2);
    EXPECT_TRUE(src != NULL);
    src->component_count = 1;
    EXPECT_TRUE(grid_context_alloc_component_sizes(src));
    src->component_sizes[1] = 4;
    grid_context* dst = grid_context_copy(src);
    EXPECT_TRUE(dst != NULL);
    // Act
    src->component_sizes[1]     = 999;
    src->cells[0].is_cut_vertex = true;
    // Assert
    EXPECT_SIZE_EQ(dst->component_sizes[1], 4);
    EXPECT_FALSE(dst->cells[0].is_cut_vertex);
    grid_context_free(src);
    grid_context_free(dst);
}

// 3x3, fully blocked
// # # #
// # # #
// # # #
TEST(grid_cut_vertices, all_blocked) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    for (size_t r = 0; r < 3; ++r)
        for (size_t c = 0; c < 3; ++c)
            EXPECT_TRUE(grid_set(g, r, c));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    for (size_t i = 0; i < g->cell_count; ++i)
        EXPECT_FALSE(ctx->cells[i].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x1, single free cell
// .
TEST(grid_cut_vertices, single_free_cell) {
    // Arrange
    grid* g = grid_new(1, 1);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    EXPECT_FALSE(ctx->cells[0].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x3 open strip, middle cell is cut vertex
// ...
TEST(grid_cut_vertices, strip_1x3) {
    // Arrange
    grid* g = grid_new(1, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    EXPECT_FALSE(ctx->cells[0].is_cut_vertex);
    EXPECT_TRUE(ctx->cells[1].is_cut_vertex);
    EXPECT_FALSE(ctx->cells[2].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x4 open strip, both inner cells are cut vertices
// ... .
TEST(grid_cut_vertices, strip_1x4) {
    // Arrange
    grid* g = grid_new(1, 4);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    EXPECT_FALSE(ctx->cells[0].is_cut_vertex);
    EXPECT_TRUE(ctx->cells[1].is_cut_vertex);
    EXPECT_TRUE(ctx->cells[2].is_cut_vertex);
    EXPECT_FALSE(ctx->cells[3].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// Cross shape, center disconnects the 4 arms
// #.#
// ...
// #.#
TEST(grid_cut_vertices, cross_center_is_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    EXPECT_TRUE(ctx->cells[4].is_cut_vertex);
    EXPECT_FALSE(ctx->cells[1].is_cut_vertex);
    EXPECT_FALSE(ctx->cells[3].is_cut_vertex);
    EXPECT_FALSE(ctx->cells[5].is_cut_vertex);
    EXPECT_FALSE(ctx->cells[7].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// 2x3 open rectangle, biconnected, no cut vertices
// ...
// ...
TEST(grid_cut_vertices, solid_2x3_no_cut) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    for (size_t i = 0; i < g->cell_count; ++i)
        EXPECT_FALSE(ctx->cells[i].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// 3x3 open square, biconnected, no cut vertices
// ...
// ...
// ...
TEST(grid_cut_vertices, solid_3x3_no_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    for (size_t i = 0; i < g->cell_count; ++i)
        EXPECT_FALSE(ctx->cells[i].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// Top and bottom rows blocked, middle row is a 3-cell path
// # # #
// . . .
// # # #
TEST(grid_cut_vertices, burger) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 1)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 1)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    // Assert
    EXPECT_FALSE(ctx->cells[3].is_cut_vertex);
    EXPECT_TRUE(ctx->cells[4].is_cut_vertex);
    EXPECT_FALSE(ctx->cells[5].is_cut_vertex);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x1 free
// .
TEST(grid_components, single_free_cell) {
    // Arrange
    grid* g = grid_new(1, 1);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 1);
    EXPECT_EQ(ctx->cells[0].component_id, 1u);
    EXPECT_EQ(ctx->cells[0].cut_distance, UINT32_MAX);
    grid_free(g);
    grid_context_free(ctx);
}

// 3x3, fully blocked
// ###
// ###
// ###
TEST(grid_components, all_blocked) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    for (size_t r = 0; r < 3; ++r)
        for (size_t c = 0; c < 3; ++c)
            EXPECT_TRUE(grid_set(g, r, c));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 0);
    EXPECT_TRUE(ctx->component_sizes == NULL);
    for (size_t i = 0; i < g->cell_count; ++i)
        EXPECT_EQ(ctx->cells[i].component_id, 0u);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x3 open strip
// ...
TEST(grid_components, strip_1x3) {
    // Arrange
    grid* g = grid_new(1, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 3);
    EXPECT_EQ(ctx->cells[0].component_id, 1u);
    EXPECT_EQ(ctx->cells[0].cut_distance, 1u);
    EXPECT_EQ(ctx->cells[1].component_id, 1u);
    EXPECT_EQ(ctx->cells[1].cut_distance, 0u);
    EXPECT_EQ(ctx->cells[2].component_id, 1u);
    EXPECT_EQ(ctx->cells[2].cut_distance, 1u);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x4 open strip
// ....
TEST(grid_components, strip_1x4) {
    // Arrange
    grid* g = grid_new(1, 4);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 4);
    EXPECT_EQ(ctx->cells[0].component_id, 1u);
    EXPECT_EQ(ctx->cells[0].cut_distance, 1u);
    EXPECT_EQ(ctx->cells[1].component_id, 1u);
    EXPECT_EQ(ctx->cells[1].cut_distance, 0u);
    EXPECT_EQ(ctx->cells[2].component_id, 1u);
    EXPECT_EQ(ctx->cells[2].cut_distance, 0u);
    EXPECT_EQ(ctx->cells[3].component_id, 1u);
    EXPECT_EQ(ctx->cells[3].cut_distance, 1u);
    grid_free(g);
    grid_context_free(ctx);
}

// Cross shape
// #.#
// ...
// #.#
TEST(grid_components, cross_center_is_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 5);
    EXPECT_EQ(ctx->cells[4].component_id, 1u);
    EXPECT_EQ(ctx->cells[4].cut_distance, 0u);
    EXPECT_EQ(ctx->cells[1].component_id, 1u);
    EXPECT_EQ(ctx->cells[1].cut_distance, 1u);
    EXPECT_EQ(ctx->cells[3].component_id, 1u);
    EXPECT_EQ(ctx->cells[3].cut_distance, 1u);
    EXPECT_EQ(ctx->cells[5].component_id, 1u);
    EXPECT_EQ(ctx->cells[5].cut_distance, 1u);
    EXPECT_EQ(ctx->cells[7].component_id, 1u);
    EXPECT_EQ(ctx->cells[7].cut_distance, 1u);
    grid_free(g);
    grid_context_free(ctx);
}

// 2x3 open rectangle
// ...
// ...
TEST(grid_components, solid_2x3_no_cut) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 6);
    for (size_t i = 0; i < g->cell_count; ++i) {
        EXPECT_EQ(ctx->cells[i].component_id, 1u);
        EXPECT_EQ(ctx->cells[i].cut_distance, UINT32_MAX);
    }
    grid_free(g);
    grid_context_free(ctx);
}

// 3x3 open square
// ...
// ...
// ...
TEST(grid_components, solid_3x3_no_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 9);
    for (size_t i = 0; i < g->cell_count; ++i) {
        EXPECT_EQ(ctx->cells[i].component_id, 1u);
        EXPECT_EQ(ctx->cells[i].cut_distance, UINT32_MAX);
    }
    grid_free(g);
    grid_context_free(ctx);
}

// Top and bottom rows blocked
// ###
// ...
// ###
TEST(grid_components, burger) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 1)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 1)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 3);
    EXPECT_EQ(ctx->cells[3].component_id, 1u);
    EXPECT_EQ(ctx->cells[3].cut_distance, 1u);
    EXPECT_EQ(ctx->cells[4].component_id, 1u);
    EXPECT_EQ(ctx->cells[4].cut_distance, 0u);
    EXPECT_EQ(ctx->cells[5].component_id, 1u);
    EXPECT_EQ(ctx->cells[5].cut_distance, 1u);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x1 free
// .
TEST(grid_calculate_neighbours, single_free_cell) {
    // Arrange
    grid* g = grid_new(1, 1);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_EQ(ctx->cells[0].neighbour_count, 0);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x3 open strip
// ...
TEST(grid_calculate_neighbours, strip_1x3) {
    // Arrange
    grid* g = grid_new(1, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_EQ(ctx->cells[0].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[1].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[2].neighbour_count, 1);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x4 open strip
// ....
TEST(grid_calculate_neighbours, strip_1x4) {
    // Arrange
    grid* g = grid_new(1, 4);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_EQ(ctx->cells[0].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[1].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[2].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[3].neighbour_count, 1);
    grid_free(g);
    grid_context_free(ctx);
}

// Cross shape
// #.#
// ...
// #.#
TEST(grid_calculate_neighbours, cross_center_is_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_EQ(ctx->cells[4].neighbour_count, 4);
    EXPECT_EQ(ctx->cells[1].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[3].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[5].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[7].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[0].neighbour_count, 0);
    grid_free(g);
    grid_context_free(ctx);
}

// 2x3 open rectangle
// ...
// ...
TEST(grid_calculate_neighbours, solid_2x3_no_cut) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_EQ(ctx->cells[0].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[1].neighbour_count, 3);
    EXPECT_EQ(ctx->cells[2].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[3].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[4].neighbour_count, 3);
    EXPECT_EQ(ctx->cells[5].neighbour_count, 2);
    grid_free(g);
    grid_context_free(ctx);
}

// 3x3 open square
// ...
// ...
// ...
TEST(grid_calculate_neighbours, solid_3x3_no_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_EQ(ctx->cells[0].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[1].neighbour_count, 3);
    EXPECT_EQ(ctx->cells[2].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[3].neighbour_count, 3);
    EXPECT_EQ(ctx->cells[4].neighbour_count, 4);
    EXPECT_EQ(ctx->cells[5].neighbour_count, 3);
    EXPECT_EQ(ctx->cells[6].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[7].neighbour_count, 3);
    EXPECT_EQ(ctx->cells[8].neighbour_count, 2);
    grid_free(g);
    grid_context_free(ctx);
}

// Top and bottom rows blocked
// ###
// ...
// ###
TEST(grid_calculate_neighbours, burger) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 1)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 1)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_EQ(ctx->cells[3].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[4].neighbour_count, 2);
    EXPECT_EQ(ctx->cells[5].neighbour_count, 1);
    EXPECT_EQ(ctx->cells[0].neighbour_count, 0);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x1 free, all neighbours are SIZE_MAX
// .
TEST(grid_calculate_neighbours, single_cell_indices) {
    // Arrange
    grid* g = grid_new(1, 1);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->cells[0].neighbour_north, SIZE_MAX);
    EXPECT_SIZE_EQ(ctx->cells[0].neighbour_south, SIZE_MAX);
    EXPECT_SIZE_EQ(ctx->cells[0].neighbour_east,  SIZE_MAX);
    EXPECT_SIZE_EQ(ctx->cells[0].neighbour_west,  SIZE_MAX);
    EXPECT_EQ(ctx->cells[0].neighbour_count, 0);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x3 strip, check neighbour indices for end and middle cells
// ...
TEST(grid_calculate_neighbours, strip_1x3_indices) {
    // Arrange
    grid* g = grid_new(1, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_calculate_neighbours(g, ctx);
    // Assert
    EXPECT_SIZE_EQ(ctx->cells[0].neighbour_east,  1);
    EXPECT_SIZE_EQ(ctx->cells[0].neighbour_west,  SIZE_MAX);
    EXPECT_EQ(ctx->cells[0].neighbour_count, 1);

    EXPECT_SIZE_EQ(ctx->cells[1].neighbour_west,  0);
    EXPECT_SIZE_EQ(ctx->cells[1].neighbour_east,  2);
    EXPECT_EQ(ctx->cells[1].neighbour_count, 2);

    EXPECT_SIZE_EQ(ctx->cells[2].neighbour_west,  1);
    EXPECT_SIZE_EQ(ctx->cells[2].neighbour_east,  SIZE_MAX);
    EXPECT_EQ(ctx->cells[2].neighbour_count, 1);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x1 free
// .
TEST(grid_good_start, single_free_cell) {
    // Arrange
    grid* g = grid_new(1, 1);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);
    // Assert
    EXPECT_TRUE(ctx->cells[0].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x3 open strip
// ...
TEST(grid_good_start, strip_1x3) {
    // Arrange
    grid* g = grid_new(1, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);
    // Assert
    EXPECT_TRUE(ctx->cells[0].is_good_start);
    EXPECT_FALSE(ctx->cells[1].is_good_start);
    EXPECT_TRUE(ctx->cells[2].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x4 open strip
// ....
TEST(grid_good_start, strip_1x4) {
    // Arrange
    grid* g = grid_new(1, 4);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);
    // Assert
    EXPECT_TRUE(ctx->cells[0].is_good_start);
    EXPECT_FALSE(ctx->cells[1].is_good_start);
    EXPECT_FALSE(ctx->cells[2].is_good_start);
    EXPECT_TRUE(ctx->cells[3].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// Cross shape
// #.#
// ...
// #.#
TEST(grid_good_start, cross_center_is_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);
    // Assert
    EXPECT_FALSE(ctx->cells[4].is_good_start);
    EXPECT_TRUE(ctx->cells[1].is_good_start);
    EXPECT_TRUE(ctx->cells[3].is_good_start);
    EXPECT_TRUE(ctx->cells[5].is_good_start);
    EXPECT_TRUE(ctx->cells[7].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// 2x3 open rectangle
// ...
// ...
TEST(grid_good_start, solid_2x3_no_cut) {
    // Arrange
    grid* g = grid_new(2, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);
    // Assert
    EXPECT_TRUE(ctx->cells[0].is_good_start);
    EXPECT_FALSE(ctx->cells[1].is_good_start);
    EXPECT_TRUE(ctx->cells[2].is_good_start);
    EXPECT_TRUE(ctx->cells[3].is_good_start);
    EXPECT_FALSE(ctx->cells[4].is_good_start);
    EXPECT_TRUE(ctx->cells[5].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// 3x3 open square
// ...
// ...
// ...
TEST(grid_good_start, solid_3x3_no_cut) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);
    // Assert
    EXPECT_TRUE(ctx->cells[0].is_good_start);
    EXPECT_FALSE(ctx->cells[1].is_good_start);
    EXPECT_TRUE(ctx->cells[2].is_good_start);
    EXPECT_FALSE(ctx->cells[3].is_good_start);
    EXPECT_FALSE(ctx->cells[4].is_good_start);
    EXPECT_FALSE(ctx->cells[5].is_good_start);
    EXPECT_TRUE(ctx->cells[6].is_good_start);
    EXPECT_FALSE(ctx->cells[7].is_good_start);
    EXPECT_TRUE(ctx->cells[8].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// Top and bottom rows blocked
// ###
// ...
// ###
TEST(grid_good_start, burger) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(grid_set(g, 0, 0)); EXPECT_TRUE(grid_set(g, 0, 1)); EXPECT_TRUE(grid_set(g, 0, 2));
    EXPECT_TRUE(grid_set(g, 2, 0)); EXPECT_TRUE(grid_set(g, 2, 1)); EXPECT_TRUE(grid_set(g, 2, 2));
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    EXPECT_TRUE(ctx != NULL);
    // Act
    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);
    // Assert
    EXPECT_TRUE(ctx->cells[3].is_good_start);
    EXPECT_FALSE(ctx->cells[4].is_good_start);
    EXPECT_TRUE(ctx->cells[5].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// 3x3 open square, full pipeline smoke test
// ...
// ...
// ...
TEST(grid_preprocess, solid_3x3) {
    // Arrange
    grid* g = grid_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    grid_context* ctx = grid_preprocess(g);
    // Assert
    EXPECT_TRUE(ctx != NULL);
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 9);
    EXPECT_TRUE(ctx->cells[0].is_good_start);
    EXPECT_FALSE(ctx->cells[4].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// 1x3 strip, full pipeline smoke test
// ...
TEST(grid_preprocess, strip_1x3) {
    // Arrange
    grid* g = grid_new(1, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    grid_context* ctx = grid_preprocess(g);
    // Assert
    EXPECT_TRUE(ctx != NULL);
    EXPECT_FALSE(ctx->cells[0].is_cut_vertex);
    EXPECT_TRUE(ctx->cells[1].is_cut_vertex);
    EXPECT_SIZE_EQ(ctx->component_count, 1);
    EXPECT_SIZE_EQ(ctx->component_sizes[1], 3);
    EXPECT_TRUE(ctx->cells[0].is_good_start);
    EXPECT_TRUE(ctx->cells[2].is_good_start);
    grid_free(g);
    grid_context_free(ctx);
}

// NULL grid returns NULL
TEST(grid_preprocess, null_grid) {
    // Arrange
    // Act
    grid_context* ctx = grid_preprocess(NULL);
    // Assert
    EXPECT_TRUE(ctx == NULL);
}

int main(void) {
    RUN_TEST(grid_context, new_small);
    RUN_TEST(grid_context, new_zero_dimensions);
    RUN_TEST(grid_context, free_null);
    RUN_TEST(grid_context, cells_zeroed_after_new);
    RUN_TEST(grid_context, alloc_component_sizes_null_context);
    RUN_TEST(grid_context, alloc_component_sizes_zero_components);
    RUN_TEST(grid_context, alloc_component_sizes_success);
    RUN_TEST(grid_context, alloc_component_sizes_twice_fails);
    RUN_TEST(grid_context, copy_fields_and_cells);
    RUN_TEST(grid_context, copy_is_deep);
    RUN_TEST(grid_cut_vertices, all_blocked);
    RUN_TEST(grid_cut_vertices, single_free_cell);
    RUN_TEST(grid_cut_vertices, strip_1x3);
    RUN_TEST(grid_cut_vertices, strip_1x4);
    RUN_TEST(grid_cut_vertices, cross_center_is_cut);
    RUN_TEST(grid_cut_vertices, solid_2x3_no_cut);
    RUN_TEST(grid_cut_vertices, solid_3x3_no_cut);
    RUN_TEST(grid_cut_vertices, burger);
    RUN_TEST(grid_components, single_free_cell);
    RUN_TEST(grid_components, all_blocked);
    RUN_TEST(grid_components, strip_1x3);
    RUN_TEST(grid_components, strip_1x4);
    RUN_TEST(grid_components, cross_center_is_cut);
    RUN_TEST(grid_components, solid_2x3_no_cut);
    RUN_TEST(grid_components, solid_3x3_no_cut);
    RUN_TEST(grid_components, burger);
    RUN_TEST(grid_calculate_neighbours, single_free_cell);
    RUN_TEST(grid_calculate_neighbours, strip_1x3);
    RUN_TEST(grid_calculate_neighbours, strip_1x4);
    RUN_TEST(grid_calculate_neighbours, cross_center_is_cut);
    RUN_TEST(grid_calculate_neighbours, solid_2x3_no_cut);
    RUN_TEST(grid_calculate_neighbours, solid_3x3_no_cut);
    RUN_TEST(grid_calculate_neighbours, burger);
    RUN_TEST(grid_calculate_neighbours, single_cell_indices);
    RUN_TEST(grid_calculate_neighbours, strip_1x3_indices);
    RUN_TEST(grid_good_start, single_free_cell);
    RUN_TEST(grid_good_start, strip_1x3);
    RUN_TEST(grid_good_start, strip_1x4);
    RUN_TEST(grid_good_start, cross_center_is_cut);
    RUN_TEST(grid_good_start, solid_2x3_no_cut);
    RUN_TEST(grid_good_start, solid_3x3_no_cut);
    RUN_TEST(grid_good_start, burger);
    RUN_TEST(grid_preprocess, solid_3x3);
    RUN_TEST(grid_preprocess, strip_1x3);
    RUN_TEST(grid_preprocess, null_grid);
    return test_summary();
}
