#include <stdlib.h>
#include "env.h" // Added to define Environment type

#ifndef CONTEXT_HEADER
#define CONTEXT_HEADER

typedef struct Context
{
    int stack_offset;
    Environment *env;
    int label_count;
} Context;

void new_scope(Context *ctx);
void end_scope(Context *ctx);
Context *new_context();
void context_free(Context *ctx);

#endif