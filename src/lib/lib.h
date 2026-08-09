#pragma once

// Include all lib files with implementations using one header
// Mostly for convenience

#include "debug.h"

#define CELL_IMPL
#include "cell.h"

#define PARSE_IMPL
#include "parse.h"

#define GRID_IMPL
#include "grid.h"

#define PATH_IMPL
#include "path.h"

#define QUEUE_IMPL
#include "queue.h"

#define STACK_IMPL
#include "stack.h"

#define PREPROC_IMPL
#include "preprocessing.h"

#define WALK_IMPL
#include "walk.h"
