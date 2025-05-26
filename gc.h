#ifndef GC_HEADER
#define GC_HEADER

#include "list.h"
#include "syntax.h"

typedef struct {
    List *symbol_table;
    Syntax *ast;
    int gc_mode; /* 1 = auto, 2 = manual */
} GC;

GC *gc_new(List *symbol_table, Syntax *ast, int gc_mode);
void gc_free(GC *gc);
void gc_mark(GC *gc);
void gc_sweep(GC *gc);
void gc_run(GC *gc);
void gc_free_variable(GC *gc, const char *var_name);
int gc_is_manual(GC *gc);

#endif