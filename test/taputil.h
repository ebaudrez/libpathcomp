#ifndef TAPUTIL_INCLUDED
#define TAPUTIL_INCLUDED

#include "config.h"
#include "list.h"

extern int cmp_bag_at_loc(const char *, int, list_t *, list_t *, const char *, ...);
extern int path_exists_ok_at_loc(const char *, int, const char *, const char *, ...);
extern int path_not_exists_ok_at_loc(const char *, int, const char *, const char *, ...);
extern int dir_exists_ok_at_loc(const char *, int, const char *, const char *, ...);
#define cmp_bag(...) cmp_bag_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)
#define path_exists_ok(...) path_exists_ok_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)
#define path_not_exists_ok(...) path_not_exists_ok_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)
#define dir_exists_ok(...) dir_exists_ok_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)

#endif /* TAPUTIL_INCLUDED */
