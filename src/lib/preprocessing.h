#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "grid.h"
#include "path.h"
#include "queue.h"
#include "stack.h"

#ifndef PREPROC_DEF
#define PREPROC_DEF static inline
#endif

#define NO_NEIGHBOUR SIZE_MAX

typedef struct {
    size_t   neighbour_north;
    size_t   neighbour_south;
    size_t   neighbour_east;
    size_t   neighbour_west;
    uint32_t component_id;    // 0 - unassigned, 1+ - component number
    uint32_t cut_distance;    // 0 for cut vertices, UINT32_MAX for non-cut-vertex components
    uint8_t  neighbour_count; // 0..4 free neighbours
    bool     is_cut_vertex;
    bool     is_good_start;   // set by grid_good_start for feasible starting cells
} cell_info;

typedef struct {
    size_t    cell_count;
    size_t    cols;
    size_t    component_count;
    size_t*   component_sizes;
    cell_info cells[];
} grid_context;

PREPROC_DEF grid_context* grid_context_new(size_t cell_count, size_t cols);
PREPROC_DEF void grid_context_free(grid_context* ctx);
PREPROC_DEF bool grid_context_alloc_component_sizes(grid_context* ctx);
PREPROC_DEF grid_context* grid_context_copy(const grid_context* src);

// Time complexity: O(V + E) = O(cell_count), space complexity: O(V) = O(cell_count)
PREPROC_DEF void grid_cut_vertices(const grid* g, grid_context* ctx);
// Time complexity: O(V + E) = O(cell_count), space complexity: O(V) = O(cell_count)
PREPROC_DEF void grid_cut_distances(const grid* g, grid_context* ctx);
// Time complexity: O(V + E) = O(cell_count), space complexity: O(V) = O(cell_count)
PREPROC_DEF void grid_components(const grid* g, grid_context* ctx);
// Time complexity: O(V) = O(cell_count), space complexity: O(1)
PREPROC_DEF void grid_calculate_neighbours(const grid* g, grid_context* ctx);
// Time complexity: O(V) = O(cell_count), space complexity: O(1)
PREPROC_DEF void grid_good_start(const grid* g, grid_context* ctx);

// Combine all preprocessing steps
PREPROC_DEF grid_context* grid_preprocess(const grid* g);

#ifdef PREPROC_IMPL

PREPROC_DEF grid_context* grid_context_new(size_t cell_count, size_t cols) {
    if (cell_count == 0 || cols == 0) return NULL;
    if (cell_count > (SIZE_MAX - sizeof(grid_context)) / sizeof(cell_info)) return NULL;

    grid_context* ctx = calloc(1, sizeof(grid_context) + cell_count * sizeof(cell_info));
    if (ctx == NULL) return NULL;
    ctx->cell_count      = cell_count;
    ctx->cols            = cols;
    ctx->component_count = 0;
    ctx->component_sizes = NULL;

    return ctx;
}

PREPROC_DEF void grid_context_free(grid_context* ctx) {
    if (ctx == NULL) return;
    if (ctx->component_sizes != NULL) {
        free(ctx->component_sizes);
    }
    free(ctx);
}

PREPROC_DEF bool grid_context_alloc_component_sizes(grid_context* ctx) {
    if (ctx == NULL) return false;
    if (ctx->component_count == 0) return false;
    if (ctx->component_sizes != NULL) return false;

    ctx->component_sizes = calloc(ctx->component_count + 1, sizeof(size_t));
    if (ctx->component_sizes == NULL) return false;
    return true;
}

PREPROC_DEF grid_context* grid_context_copy(const grid_context* src) {
    grid_context* dst = grid_context_new(src->cell_count, src->cols);

    dst->cell_count = src->cell_count;
    dst->cols = src->cols;
    dst->component_count = src->component_count;
    if (src->component_sizes != NULL) {
        size_t to_alloc = (src->component_count + 1) * sizeof(size_t);
        dst->component_sizes = calloc(1, to_alloc);
        memcpy(dst->component_sizes, src->component_sizes, to_alloc);
    }
    memcpy(dst->cells, src->cells, src->cell_count * sizeof(cell_info));

    return dst;
}

#define NO_PARENT SIZE_MAX

typedef struct {
    size_t   parent;
    uint32_t disc;      // discovery time, 0 = unvisited
    uint32_t low;
    uint8_t  num_children;
} cut_vertex_dfs_state;

PREPROC_DEF void grid_cut_vertices(const grid* g, grid_context* ctx) {
    cut_vertex_dfs_state* state = calloc(g->cell_count, sizeof(cut_vertex_dfs_state));
    stack*                s   = stack_new(g->cell_count);
    uint32_t              timer = 0;

    for (size_t i = 0; i < g->cell_count; ++i) state[i].parent = SIZE_MAX;

    for (size_t u = 0; u < g->cell_count; ++u) {
        if (grid_at_flat(g, u) != CELL_FREE) continue;
        if (state[u].disc != 0) continue; // Already visited

        DEBUG_PRINT("[cut_vertices] start root is %zu\n", u);

        stack_push(s, u);
        state[u].parent = SIZE_MAX;

        while (!stack_empty(s)) {
            size_t v = stack_top(s);

            if (state[v].disc == 0) {

                state[v].disc = state[v].low = ++timer;
                DEBUG_PRINT("[cut_vertices] discover time of %zu is %u\n", v, state[v].disc);
            }

            bool child_undiscovered = false;

            for (size_t ni = 0; ni < DIRECTION_COUNT && !child_undiscovered; ++ni) {
                size_t n_idx = cell_neighbour_idx(v, (direction)ni, g->cols, g->cell_count);
                if (n_idx == CELL_NONE_IDX) continue; // Out of bounds

                size_t w = n_idx;
                if (grid_at_flat(g, w) != CELL_FREE) continue;

                if (state[w].disc == 0) { // Undiscovered child. Descend into it.
                    DEBUG_PRINT("[cut_vertices] discovering %zu -> %zu\n", v, w);
                    ++state[v].num_children;
                    state[w].parent = v;
                    stack_push(s, w);
                    child_undiscovered = true;
                }
                else if (w != state[v].parent) { // Already discovered, back edge to v's ancestors
                    if (state[w].disc < state[v].low) {
                        DEBUG_PRINT(
                            "[cut_vertices] back-edge %zu -> %zu (updating low %zu's %u -> %u)\n",
                            v, w, v, state[v].low, state[w].disc
                        );
                        state[v].low = state[w].disc;
                    }
                }
            }

            if (!child_undiscovered) {
                // No more children to check
                stack_pop(s);
                size_t p = state[v].parent;
                DEBUG_PRINT(
                    "[cut_vertices] no more children to discover for %zu (low=%u, parent=%zu, children=%u)\n",
                    v, state[v].low, p, state[v].num_children
                );
                if (p != SIZE_MAX) {
                    // Not a DFS root, update parent's low
                    if (state[v].low < state[p].low) {
                        DEBUG_PRINT("[cut_vertices] propagate low %zu -> %zu (%u -> %u)\n",
                            v, p, state[p].low, state[v].low
                        );
                        state[p].low = state[v].low;
                    }
                    // We have to pass through p, so p is a cut vertex
                    if (state[p].parent != SIZE_MAX && state[v].low >= state[p].disc) {
                        DEBUG_PRINT(
                            "[cut_vertices] cut vertex %zu detected (child %zu low %u >= %zu disc %u)\n",
                            p, v, state[v].low, p, state[p].disc
                        );
                        ctx->cells[p].is_cut_vertex = true;
                    }
                }
                else if (state[v].num_children > 1) { // DFS root is a cut vertex if it has multiple children
                    DEBUG_PRINT(
                        "[cut_vertices] %zu root is cut vertex (%u children)\n",
                        v, state[v].num_children
                    );
                    ctx->cells[v].is_cut_vertex = true;
                }
            }
        }
    }
    stack_free(s);
    free(state);
}

PREPROC_DEF void grid_cut_distances(const grid* g, grid_context* ctx) {
    queue* q = queue_new(g->cell_count);
    // Multi-source BFS, start from all cut vertices
    for (size_t u = 0; u < g->cell_count; ++u) {
        if (!ctx->cells[u].is_cut_vertex) continue;
        ctx->cells[u].cut_distance = 0;

        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            size_t n_idx = cell_neighbour_idx(u, d, g->cols, g->cell_count);
            if (n_idx == CELL_NONE_IDX) continue;
            size_t w = n_idx;
            if (grid_at_flat(g, w) != CELL_FREE) continue;
            if (ctx->cells[w].is_cut_vertex)     continue;
            if (ctx->cells[w].cut_distance != 0) continue; // Already visited

            ctx->cells[w].cut_distance = 1;
            queue_push(q, w);
        }
    }
    // Calculate distances from cut vertices
    while (!queue_empty(q)) {
        size_t   v      = queue_pop(q);
        uint32_t v_dist = ctx->cells[v].cut_distance;

        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            size_t n_idx = cell_neighbour_idx(v, d, g->cols, g->cell_count);
            if (n_idx == CELL_NONE_IDX) continue;
            size_t w = n_idx;
            if (grid_at_flat(g, w) != CELL_FREE) continue;
            if (ctx->cells[w].is_cut_vertex)     continue;
            if (ctx->cells[w].cut_distance != 0) continue;

            ctx->cells[w].cut_distance = v_dist + 1;
            queue_push(q, w);
        }
    }
    // Cells never reached by any BFS belong to components with no adjacent cut vertex
    for (size_t u = 0; u < g->cell_count; ++u) {
        if (grid_at_flat(g, u) != CELL_FREE) continue;
        if (ctx->cells[u].is_cut_vertex)     continue;
        if (ctx->cells[u].cut_distance == 0)
            ctx->cells[u].cut_distance = UINT32_MAX;
    }

    queue_free(q);
}

PREPROC_DEF void grid_components(const grid* g, grid_context* ctx) {
    size_t comp_id = 1;
    stack* s = stack_new(g->cell_count);
    for (size_t u = 0; u < g->cell_count; ++u) {
        if (grid_at_flat(g, u) != CELL_FREE) continue;
        if (ctx->cells[u].component_id != 0) continue;

        DEBUG_PRINT("[components] start DFS comp %zu from %zu\n", comp_id, u);

        stack_push(s, u);
        ctx->cells[u].component_id = comp_id;

        while (!stack_empty(s)) {
            size_t v = stack_pop(s);

            for (direction d = 0; d < DIRECTION_COUNT; ++d) {
                size_t n_idx = cell_neighbour_idx(v, d, g->cols, g->cell_count);
                if (n_idx == CELL_NONE_IDX) continue;
                size_t w = n_idx;
                if (grid_at_flat(g, w) != CELL_FREE) continue;
                if (ctx->cells[w].component_id != 0) continue;

                ctx->cells[w].component_id = comp_id;
                stack_push(s, w);
            }
        }
        ++comp_id;
    }
    stack_free(s);

    DEBUG_PRINT("[components] total components: %zu\n", comp_id - 1);

    ctx->component_count = comp_id - 1;
    if (grid_context_alloc_component_sizes(ctx)) {
        for (size_t u = 0; u < g->cell_count; ++u) {
            if (grid_at_flat(g, u) != CELL_FREE) continue;
            uint32_t cid = ctx->cells[u].component_id;
            if (cid > 0) ++ctx->component_sizes[cid];
        }
        DEBUG_SCOPE {
            for (size_t i = 1; i <= ctx->component_count; ++i)
                DEBUG_PRINT("[components] comp %zu size: %zu\n", i, ctx->component_sizes[i]);
        }
    }

}

PREPROC_DEF void grid_calculate_neighbours(const grid* g, grid_context* ctx) {
    for (size_t u = 0; u < g->cell_count; ++u) {
        if (grid_at_flat(g, u) != CELL_FREE) continue;

        ctx->cells[u].neighbour_north = NO_NEIGHBOUR;
        ctx->cells[u].neighbour_south = NO_NEIGHBOUR;
        ctx->cells[u].neighbour_east  = NO_NEIGHBOUR;
        ctx->cells[u].neighbour_west  = NO_NEIGHBOUR;

        uint8_t count = 0;

        size_t n_idx = cell_neighbour_idx(u, DIRECTION_NORTH, g->cols, g->cell_count);
        if (n_idx != CELL_NONE_IDX && grid_at_flat(g, n_idx) == CELL_FREE) {
            ctx->cells[u].neighbour_north = n_idx;
            ++count;
        }

        n_idx = cell_neighbour_idx(u, DIRECTION_SOUTH, g->cols, g->cell_count);
        if (n_idx != CELL_NONE_IDX && grid_at_flat(g, n_idx) == CELL_FREE) {
            ctx->cells[u].neighbour_south = n_idx;
            ++count;
        }

        n_idx = cell_neighbour_idx(u, DIRECTION_EAST, g->cols, g->cell_count);
        if (n_idx != CELL_NONE_IDX && grid_at_flat(g, n_idx) == CELL_FREE) {
            ctx->cells[u].neighbour_east = n_idx;
            ++count;
        }

        n_idx = cell_neighbour_idx(u, DIRECTION_WEST, g->cols, g->cell_count);
        if (n_idx != CELL_NONE_IDX && grid_at_flat(g, n_idx) == CELL_FREE) {
            ctx->cells[u].neighbour_west = n_idx;
            ++count;
        }

        ctx->cells[u].neighbour_count = count;
    }
}

PREPROC_DEF void grid_good_start(const grid* g, grid_context* ctx) {
    uint8_t  best_neighbours = UINT8_MAX;
    size_t   best_comp_size   = 0;
    uint32_t best_distance      = 0;

    for (size_t i = 0; i < g->cell_count; ++i) {
        if (grid_at_flat(g, i) != CELL_FREE) continue;

        uint8_t  neighbours  = ctx->cells[i].neighbour_count;
        size_t   comp_size = ctx->component_sizes[ctx->cells[i].component_id];
        uint32_t cut_distance = ctx->cells[i].cut_distance;

        if (comp_size > best_comp_size) {
            best_comp_size = comp_size;
            best_distance  = cut_distance;
            best_neighbours = neighbours;
        }
        else if (comp_size == best_comp_size) {
            if (cut_distance > best_distance) {
                best_distance  = cut_distance;
                best_neighbours = neighbours;
            }
            else if (cut_distance == best_distance && neighbours < best_neighbours) {
                best_neighbours = neighbours;
            }
        }
    }

    for (size_t i = 0; i < g->cell_count; ++i) {
        if (grid_at_flat(g, i) != CELL_FREE) continue;

        uint8_t  neighbours  = ctx->cells[i].neighbour_count;
        size_t   comp_size = ctx->component_sizes[ctx->cells[i].component_id];
        uint32_t cut_distance  = ctx->cells[i].cut_distance;

        if (neighbours == best_neighbours && comp_size == best_comp_size && cut_distance == best_distance) {
            ctx->cells[i].is_good_start = true;
            DEBUG_PRINT("[good_start] marking %zu as good start\n", i);
        }
    }
}

PREPROC_DEF grid_context* grid_preprocess(const grid* g) {
    if (g == NULL) return NULL;
    grid_context* ctx = grid_context_new(g->cell_count, g->cols);
    if (ctx == NULL) return NULL;

    grid_cut_vertices(g, ctx);
    grid_cut_distances(g, ctx);
    grid_components(g, ctx);
    grid_calculate_neighbours(g, ctx);
    grid_good_start(g, ctx);

    return ctx;
}

#endif // PREPROC_IMPL
