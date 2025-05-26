#include "env.h"
#include "context.h"

// TODO: this is duplicated with assembly.c.
static const int WORD_SIZE = 16;

void new_scope(Context *ctx) {
    environment_free(ctx->env);
    ctx->env = environment_new();
    ctx->stack_offset = -1 * WORD_SIZE;
}

void end_scope(Context *ctx) {
    environment_free(ctx->env);
    ctx->env = NULL;
    ctx->stack_offset = 0;
}

Context *new_context() {
    Context *ctx = malloc(sizeof(Context));
    ctx->stack_offset = 0;
    ctx->env = NULL;
    ctx->label_count = 0;
    return ctx;
}

void context_free(Context *ctx) {
    environment_free(ctx->env);
    free(ctx);
}