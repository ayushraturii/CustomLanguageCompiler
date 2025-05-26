#include <stdlib.h>
#include <err.h>
#include "gc.h"
#include "list.h"
#include "syntax.h"

GC *gc_new(List *symbol_table, Syntax *ast, int gc_mode) {
    if (!symbol_table || !ast || (gc_mode != 1 && gc_mode != 2)) {
        errx(1, "Invalid arguments in gc_new");
    }
    GC *gc = malloc(sizeof(GC));
    if (!gc) {
        errx(1, "Memory allocation failed for GC");
    }
    gc->symbol_table = symbol_table;
    gc->ast = ast;
    gc->gc_mode = gc_mode;
    return gc;
}

void gc_free(GC *gc) {
    if (!gc) return;
    free(gc);
}

static void mark_syntax(Syntax *syntax, List *symbol_table) {
    if (!syntax || !symbol_table) return;

    switch (syntax->type) {
        case VARIABLE: {
            Symbol *sym = symbol_exists(symbol_table, syntax->variable->var_name);
            if (sym) {
                symbol_mark(sym, 1);
                if (sym->var_type == TYPE_ARRAY) {
                    mark_syntax(sym->value, symbol_table);
                }
            }
            break;
        }
        case ARRAY: {
            for (size_t i = 0; i < syntax->array->elements->size; i++) {
                mark_syntax(syntax->array->elements->items[i], symbol_table);
            }
            break;
        }
        case ARRAY_ACCESS: {
            Symbol *sym = symbol_exists(symbol_table, syntax->array_access->var_name);
            if (sym) {
                symbol_mark(sym, 1);
            }
            mark_syntax(syntax->array_access->index, symbol_table);
            break;
        }
        case UNARY_OPERATOR:
            mark_syntax(syntax->unary_expression->expression, symbol_table);
            break;
        case BINARY_OPERATOR:
            mark_syntax(syntax->binary_expression->left, symbol_table);
            mark_syntax(syntax->binary_expression->right, symbol_table);
            break;
        case IF_STATEMENT:
            mark_syntax(syntax->if_statement->condition, symbol_table);
            mark_syntax(syntax->if_statement->then_stmts, symbol_table);
            if (syntax->if_statement->else_stmts) {
                mark_syntax(syntax->if_statement->else_stmts, symbol_table);
            }
            break;
        case RETURN_STATEMENT:
            mark_syntax(syntax->return_statement->expression, symbol_table);
            break;
        case PRINT_STATEMENT:
            mark_syntax(syntax->print_statement->expression, symbol_table);
            break;
        case FREE_STATEMENT:
            break;
        case ASSIGNMENT:
            mark_syntax(syntax->assignment->expression, symbol_table);
            break;
        case DEFINE_VAR: {
            Symbol *sym = symbol_exists(symbol_table, syntax->define_var_statement->var_name);
            if (sym) {
                symbol_mark(sym, 1);
            }
            mark_syntax(syntax->define_var_statement->init_value, symbol_table);
            break;
        }
        case BLOCK:
            for (size_t i = 0; i < syntax->block->statements->size; i++) {
                mark_syntax(syntax->block->statements->items[i], symbol_table);
            }
            break;
        case FUNCTION:
            mark_syntax(syntax->function->root_block, symbol_table);
            break;
        case TOP_LEVEL:
            for (size_t i = 0; i < syntax->top_level->declarations->size; i++) {
                mark_syntax(syntax->top_level->declarations->items[i], symbol_table);
            }
            break;
        case IMMEDIATE:
            break;
        default:
            warnx("Unknown syntax type in mark: %s", syntax_type_name(syntax));
            break;
    }
}

void gc_mark(GC *gc) {
    if (!gc || !gc->symbol_table) return;
    for (size_t i = 0; i < gc->symbol_table->size; i++) {
        Symbol *sym = gc->symbol_table->items[i];
        if (sym) {
            symbol_mark(sym, 0);
        }
    }
    mark_syntax(gc->ast, gc->symbol_table);
}

void gc_sweep(GC *gc) {
    if (!gc || gc->gc_mode != 1) return;
    for (size_t i = 0; i < gc->symbol_table->size; i++) {
        Symbol *sym = gc->symbol_table->items[i];
        if (sym && !sym->marked) {
            free(sym->name);
            syntax_free(sym->value);
            free(sym);
            gc->symbol_table->items[i] = NULL;
        }
    }
    list_compact(gc->symbol_table);
}

void gc_run(GC *gc) {
    if (!gc || gc->gc_mode != 1) return;
    gc_mark(gc);
    gc_sweep(gc);
}

void gc_free_variable(GC *gc, const char *var_name) {
    if (!gc || !var_name) {
        warnx("Invalid arguments in gc_free_variable");
        return;
    }
    if (gc->gc_mode != 2) {
        warnx("Error: free statement used in auto GC mode");
        return;
    }
    Symbol *sym = symbol_exists(gc->symbol_table, var_name);
    if (!sym) {
        warnx("Error: cannot free undeclared variable '%s'", var_name);
        return;
    }
    sym->ref_count--;
    if (sym->ref_count <= 0) {
        free(sym->name);
        syntax_free(sym->value);
        free(sym);
        for (size_t i = 0; i < gc->symbol_table->size; i++) {
            if (gc->symbol_table->items[i] == sym) {
                gc->symbol_table->items[i] = NULL;
                break;
            }
        }
        list_compact(gc->symbol_table);
    }
}

int gc_is_manual(GC *gc) {
    if (!gc) return 0;
    return gc->gc_mode == 2;
}