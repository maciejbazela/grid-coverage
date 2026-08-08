#pragma once

#ifdef DEBUG

#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#define DEBUG_SCOPE if (1)

#else

#define DEBUG_PRINT(...) ((void)0)
#define DEBUG_SCOPE if (0)

#endif // DEBUG
