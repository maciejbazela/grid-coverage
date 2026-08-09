#pragma once

#include "grid.h"
#include "path.h"
#include "preprocessing.h"

#ifndef WALK_DEF
#define WALK_DEF static inline
#endif

typedef path* (*traverse_fn)(const grid*, size_t max_steps, cell start, const grid_context*);

WALK_DEF cell  walk_pick_first_good_start(const grid* g, const grid_context* ctx);
WALK_DEF path* walk_solve_deep(const grid* g, size_t max_steps, cell start, const grid_context* ctx);
WALK_DEF path* walk_solve_random(const grid* g, size_t max_steps, cell start, const grid_context* ctx, unsigned seed);
WALK_DEF path* walk_solve_warnsdorff(const grid* g, size_t max_steps, cell start, const grid_context* ctx);
WALK_DEF path* walk_solve_heuristic(const grid* g, size_t max_steps, cell start, const grid_context* ctx);
WALK_DEF path* walk_solve_multistart(const grid* g, size_t max_steps, traverse_fn inner, const grid_context* ctx);
WALK_DEF path* walk_solve_all_free(const grid* g, size_t max_steps, traverse_fn inner, const grid_context* ctx);

#ifdef WALK_IMPL

WALK_DEF cell walk_pick_first_good_start(const grid* g, const grid_context* ctx) {
    for (size_t i = 0; i < g->cell_count; ++i)
        if (ctx->cells[i].is_good_start)
            return (cell){ .idx = i };
    for (size_t i = 0; i < g->cell_count; ++i)
        if (grid_at_flat(g, i) == CELL_FREE)
            return (cell){ .idx = i };
    return CELL_NONE;
}

WALK_DEF path* walk_solve_deep(const grid* g, size_t max_steps, cell start, const grid_context* ctx) {
    (void) ctx;

    bool* visited = calloc(g->cell_count, sizeof(bool));
    path* p = path_new(max_steps + 1);
    stack* frontier = stack_new(max_steps + 1);

    path_push(p, start);
    stack_push(frontier, start.idx);
    visited[start.idx] = true;

    while (p->size <= max_steps) {
        cell v = path_top(p);
        bool moved = false;
        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            cell w = cell_neighbour(v, d, g->cols, g->cell_count);
            if (!cell_eq(w, CELL_NONE) && grid_at_flat(g, w.idx) == CELL_FREE && !visited[w.idx]) {
                path_push(p, w);
                visited[w.idx] = true;
                stack_push(frontier, w.idx);
                moved = true;
                break;
            }
        }
        if (moved) continue;
        if (stack_empty(frontier)) break; // Started at dead end
        stack_pop(frontier);
        if (stack_empty(frontier)) break; // Nothing to backtrack to
        size_t backtrack_to = stack_top(frontier);
        path_push_idx(p, backtrack_to);
    }

    free(visited);
    stack_free(frontier);
    return p;
}

WALK_DEF path* walk_solve_random(const grid* g, size_t max_steps, cell start, const grid_context* ctx, unsigned seed) {
    (void) ctx;

    srand(seed);
    unsigned* rng = malloc(max_steps * sizeof(unsigned));
    for (size_t i = 0; i < max_steps; ++i)
        rng[i] = (unsigned) rand();
    bool* visited = calloc(g->cell_count, sizeof(bool));
    path* p = path_new(max_steps + 1);

    path_push(p, start);
    visited[start.idx] = true;

    size_t rng_i = 0;
    for (size_t step = 0; step < max_steps; ++step) {
        cell v = path_top(p);
        size_t unvisited[4], visited_neighbours[4];
        size_t unv_count = 0, v_count = 0;

        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            cell w = cell_neighbour(v, d, g->cols, g->cell_count);
            if (cell_eq(w, CELL_NONE) || grid_at_flat(g, w.idx) != CELL_FREE) continue;
            if (!visited[w.idx])
                unvisited[unv_count++] = w.idx;
            else
                visited_neighbours[v_count++] = w.idx;
        }

        size_t chosen;
        if (unv_count > 0) {
            chosen = unvisited[rng[rng_i++] % unv_count];
            visited[chosen] = true;
        }
        else if (v_count > 0) {
            chosen = visited_neighbours[rng[rng_i++] % v_count];
        }
        else {
            break;
        }
        path_push(p, (cell){ .idx = chosen });
    }

    free(rng);
    free(visited);
    return p;
}

static size_t walk_count_unvisited_neighbours(const grid_context* ctx, const bool* visited, size_t cell_idx) {
    size_t count = 0;
    if (ctx->cells[cell_idx].neighbour_north != NO_NEIGHBOUR && !visited[ctx->cells[cell_idx].neighbour_north]) ++count;
    if (ctx->cells[cell_idx].neighbour_south != NO_NEIGHBOUR && !visited[ctx->cells[cell_idx].neighbour_south]) ++count;
    if (ctx->cells[cell_idx].neighbour_east  != NO_NEIGHBOUR && !visited[ctx->cells[cell_idx].neighbour_east])  ++count;
    if (ctx->cells[cell_idx].neighbour_west  != NO_NEIGHBOUR && !visited[ctx->cells[cell_idx].neighbour_west])  ++count;
    return count;
}

WALK_DEF path* walk_solve_warnsdorff(const grid* g, size_t max_steps, cell start, const grid_context* ctx) {
    bool* visited = calloc(g->cell_count, sizeof(bool));
    path* p = path_new(max_steps + 1);
    stack* frontier = stack_new(max_steps + 1);

    path_push(p, start);
    stack_push(frontier, start.idx);
    visited[start.idx] = true;

    while (p->size <= max_steps) {
        cell v = path_top(p);

        size_t best_unvisited_neighbours = SIZE_MAX;
        uint32_t best_cut_distance = 0;
        size_t best_comp_size = 0;
        cell best_cell = CELL_NONE;

        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            cell w = cell_neighbour(v, d, g->cols, g->cell_count);
            if (cell_eq(w, CELL_NONE)) continue;
            if (grid_at_flat(g, w.idx) != CELL_FREE) continue;
            if (visited[w.idx]) continue;

            size_t unvisited_neighbours = walk_count_unvisited_neighbours(ctx, visited, w.idx);
            uint32_t cut_dist = ctx->cells[w.idx].cut_distance;
            size_t comp_size = 0;
            uint32_t comp_id = ctx->cells[w.idx].component_id;
            if (comp_id > 0 && ctx->component_sizes != NULL)
                comp_size = ctx->component_sizes[comp_id];

            bool better = false;
            if (unvisited_neighbours < best_unvisited_neighbours)
                better = true;
            else if (unvisited_neighbours == best_unvisited_neighbours && comp_size > best_comp_size)
                better = true;
            else if (unvisited_neighbours == best_unvisited_neighbours && comp_size == best_comp_size && cut_dist > best_cut_distance)
                better = true;

            if (better) {
                best_unvisited_neighbours = unvisited_neighbours;
                best_cut_distance = cut_dist;
                best_comp_size = comp_size;
                best_cell = w;
            }
        }

        if (!cell_eq(best_cell, CELL_NONE)) {
            path_push(p, best_cell);
            visited[best_cell.idx] = true;
            stack_push(frontier, best_cell.idx);
        }
        else {
            if (stack_empty(frontier)) break; // Started at dead end
            stack_pop(frontier);
            if (stack_empty(frontier)) break; // Nothing to backtrack to
            size_t backtrack_to = stack_top(frontier);
            path_push_idx(p, backtrack_to);
        }
    }

    free(visited);
    stack_free(frontier);
    return p;
}

WALK_DEF path* walk_solve_heuristic(const grid* g, size_t max_steps, cell start, const grid_context* ctx) {
    bool* visited = calloc(g->cell_count, sizeof(bool));
    path* p = path_new(max_steps + 1);
    stack* frontier = stack_new(max_steps + 1);

    path_push(p, start);
    stack_push(frontier, start.idx);
    visited[start.idx] = true;

    while (p->size <= max_steps) {
        cell v = path_top(p);

        int64_t best_score = INT64_MAX;
        cell best_cell = CELL_NONE;

        for (direction d = 0; d < DIRECTION_COUNT; ++d) {
            cell w = cell_neighbour(v, d, g->cols, g->cell_count);
            if (cell_eq(w, CELL_NONE)) continue;
            if (grid_at_flat(g, w.idx) != CELL_FREE) continue;
            if (visited[w.idx]) continue;

            size_t unvisited_neighbours = walk_count_unvisited_neighbours(ctx, visited, w.idx);
            uint32_t cut_dist = ctx->cells[w.idx].cut_distance;
            bool is_cut = ctx->cells[w.idx].is_cut_vertex;
            size_t comp_size = 0;
            uint32_t comp_id = ctx->cells[w.idx].component_id;
            if (comp_id > 0 && ctx->component_sizes != NULL)
                comp_size = ctx->component_sizes[comp_id];

            int64_t score = (int64_t)unvisited_neighbours * 1000
                          - (int64_t)comp_size * 10
                          - (int64_t)cut_dist * 1
                          + (is_cut ? 10000 : 0);

            if (score < best_score) {
                best_score = score;
                best_cell = w;
            }
        }

        if (!cell_eq(best_cell, CELL_NONE)) {
            path_push(p, best_cell);
            visited[best_cell.idx] = true;
            stack_push(frontier, best_cell.idx);
        }
        else {
            if (stack_empty(frontier)) break; // Started at dead end
            stack_pop(frontier);
            if (stack_empty(frontier)) break; // Nothing to backtrack to
            size_t backtrack_to = stack_top(frontier);
            path_push_idx(p, backtrack_to);
        }
    }

    free(visited);
    stack_free(frontier);
    return p;
}

WALK_DEF path* walk_solve_multistart(const grid* g, size_t max_steps, traverse_fn inner, const grid_context* ctx) {
    path* best_path = NULL;
    size_t best_unique = 0;

    DEBUG_SCOPE {
        size_t good_starts = 0;
        for (size_t i = 0; i < g->cell_count; ++i)
            if (ctx->cells[i].is_good_start) ++good_starts;
        DEBUG_PRINT("[multistart] %zu good start cells, max_steps=%zu\n", good_starts, max_steps);
    }

    for (size_t i = 0; i < g->cell_count; ++i) {
        if (!ctx->cells[i].is_good_start) continue;

        DEBUG_PRINT("[multistart] trying start (%zu, %zu)\n", i / g->cols, i % g->cols);

        path* candidate = inner(g, max_steps, (cell){ .idx = i }, ctx);
        if (candidate == NULL) continue;

        size_t u = path_unique_cells(candidate);
        DEBUG_PRINT("[multistart]   unique=%zu\n", u);
        if (u > best_unique) {
            DEBUG_PRINT("[multistart]   new best! %zu -> %zu\n", best_unique, u);
            if (best_path != NULL) path_free(best_path);
            best_path = candidate;
            best_unique = u;
            if (u == max_steps + 1) // Theoretical optimuum
                return best_path;
        }
        else
            path_free(candidate);
    }

    DEBUG_PRINT("[multistart] done, best=%zu\n", best_unique);
    return best_path;
}

WALK_DEF path* walk_solve_all_free(const grid* g, size_t max_steps, traverse_fn inner, const grid_context* ctx) {
    (void) ctx;

    path* best_path = NULL;
    size_t best_unique = 0;

    DEBUG_SCOPE {
        size_t free_cells = 0;
        for (size_t i = 0; i < g->cell_count; ++i)
            if (grid_at_flat(g, i) == CELL_FREE) ++free_cells;
        DEBUG_PRINT("[all_free] %zu free cells, max_steps=%zu\n", free_cells, max_steps);
    }

    for (size_t i = 0; i < g->cell_count; ++i) {
        if (grid_at_flat(g, i) != CELL_FREE) continue;

        DEBUG_PRINT("[all_free] trying start (%zu, %zu)\n", i / g->cols, i % g->cols);

        path* candidate = inner(g, max_steps, (cell){ .idx = i }, ctx);
        if (candidate == NULL) continue;

        size_t u = path_unique_cells(candidate);
        DEBUG_PRINT("[all_free]   unique=%zu\n", u);
        if (u > best_unique) {
            DEBUG_PRINT("[all_free]   new best! %zu -> %zu\n", best_unique, u);
            if (best_path != NULL) path_free(best_path);
            best_path = candidate;
            best_unique = u;
            if (u == max_steps + 1) // Theoretical optimum
                return best_path;
        }
        else
            path_free(candidate);
    }

    DEBUG_PRINT("[all_free] done, best=%zu\n", best_unique);
    return best_path;
}

#endif
