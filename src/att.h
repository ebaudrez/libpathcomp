#ifndef ATT_INCLUDED
#define ATT_INCLUDED

typedef struct att_t att_t;

extern att_t      *att_new(const char *, const char *);
extern void        att_replace_value(att_t *, const char *);
extern void        att_add_value(att_t *, const char *);
extern void        att_free(att_t *);
extern int         att_name_equal_to(att_t *, char *);
extern const char *att_eval(att_t *, void *, const char *);
extern void        att_reset(att_t *);
extern int         att_next(att_t *);

#endif /* ATT_INCLUDED */
