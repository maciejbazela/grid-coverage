#define GRAPH_IMPL
#include "../src/lib/graph.h"
#include "common.h"


TEST(graph, new_small) {
    // Arrange
    // Act
    graph* g = graph_new(3, 4);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_EQ(g->rows, (size_t) 3);
    EXPECT_EQ(g->cols, (size_t) 4);
    graph_free(g);
}

TEST(graph, new_empty_cells) {
    // Arrange
    // Act
    graph* g = graph_new(3, 3);
    // Assert
    EXPECT_TRUE(g != NULL);
    for (size_t r = 0; r < 3; r++)
        for (size_t c = 0; c < 3; c++)
            EXPECT_FALSE(graph_at(g, r, c));
    graph_free(g);
}

TEST(graph, set_cell) {
    // Arrange
    graph* g = graph_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(graph_set(g, 1, 2));
    EXPECT_TRUE(graph_at(g, 1, 2));
    graph_free(g);
}

TEST(graph, set_multiple_cells) {
    // Arrange
    graph* g = graph_new(4, 4);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(graph_set(g, 0, 0));
    EXPECT_TRUE(graph_set(g, 1, 1));
    EXPECT_TRUE(graph_set(g, 2, 2));
    EXPECT_TRUE(graph_at(g, 0, 0));
    EXPECT_TRUE(graph_at(g, 1, 1));
    EXPECT_TRUE(graph_at(g, 2, 2));
    EXPECT_FALSE(graph_at(g, 0, 1));
    EXPECT_FALSE(graph_at(g, 3, 3));
    graph_free(g);
}

TEST(graph, set_out_of_bounds_row) {
    // Arrange
    graph* g = graph_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_FALSE(graph_set(g, 3, 0));
    EXPECT_FALSE(graph_at(g, 3, 0));
    graph_free(g);
}

TEST(graph, set_out_of_bounds_col) {
    // Arrange
    graph* g = graph_new(3, 3);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_FALSE(graph_set(g, 0, 3));
    EXPECT_FALSE(graph_at(g, 0, 3));
    graph_free(g);
}

TEST(graph, get_unset_cell) {
    // Arrange
    graph* g = graph_new(2, 2);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_FALSE(graph_at(g, 0, 0));
    EXPECT_FALSE(graph_at(g, 1, 1));
    graph_free(g);
}

TEST(graph, overwrite_cell) {
    // Arrange
    graph* g = graph_new(2, 2);
    EXPECT_TRUE(g != NULL);
    // Act
    // Assert
    EXPECT_TRUE(graph_set(g, 0, 0));
    EXPECT_TRUE(graph_at(g, 0, 0));
    EXPECT_TRUE(graph_set(g, 0, 0));
    EXPECT_TRUE(graph_at(g, 0, 0));
    graph_free(g);
}

TEST(graph, zero_dimensions) {
    // Arrange
    // Act
    graph* g = graph_new(0, 0);
    // Assert
    EXPECT_TRUE(g == NULL);
}

TEST(graph, large_graph) {
    // Arrange
    // Act
    graph* g = graph_new(100, 100);
    // Assert
    EXPECT_TRUE(g != NULL);
    EXPECT_TRUE(graph_set(g, 99, 99));
    EXPECT_TRUE(graph_at(g, 99, 99));
    EXPECT_FALSE(graph_at(g, 98, 99));
    graph_free(g);
}

int main(void) {
    RUN_TEST(graph_new_small);
    RUN_TEST(graph_new_empty_cells);
    RUN_TEST(graph_set_cell);
    RUN_TEST(graph_set_multiple_cells);
    RUN_TEST(graph_set_out_of_bounds_row);
    RUN_TEST(graph_set_out_of_bounds_col);
    RUN_TEST(graph_get_unset_cell);
    RUN_TEST(graph_overwrite_cell);
    RUN_TEST(graph_zero_dimensions);
    RUN_TEST(graph_large_graph);
    return test_summary();
}

