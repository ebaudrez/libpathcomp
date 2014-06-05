#ifndef TAPUTIL_INCLUDED
#define TAPUTIL_INCLUDED

#include "config.h"
#include "list.h"

extern int cmp_bag_at_loc(const char *, int, list_t *, list_t *, const char *, ...);
#define cmp_bag(...) cmp_bag_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)

#endif /* TAPUTIL_INCLUDED */
