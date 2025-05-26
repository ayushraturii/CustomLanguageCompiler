#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <err.h>
#include "syntax.h"
#include "env.h"
#include "context.h"

static int is_M1 = 0;
static const int WORD_SIZE = 16;
const int MAX_MNEMONIC_LENGTH = 7;

void check_target_architecture() {
#ifdef TARGET_ARCH_M1
    is_M1=1;
#endif
}

void emit_header(FILE *out, char *name) { fprintf(out, "%s\n", name); }

void emit_instr(FILE *out, char *instr, char *operands) {
    fprintf(out, "    %s", instr);

    int argument_offset = MAX_MNEMONIC_LENGTH - strlen(instr) + 4;
    while (argument_offset > 0) {
        fprintf(out, " ");
        argument_offset--;
    }

    fprintf(out, "%s;\n", operands);
}

void emit_instr_format(FILE *out, char *instr, char *operands_format, ...) {
    fprintf(out, "    %s", instr);

    int argument_offset = MAX_MNEMONIC_LENGTH - strlen(instr) + 4;
    while (argument_offset > 0) {
        fprintf(out, " ");
        argument_offset--;
    }

    va_list argptr;
    va_start(argptr, operands_format);
    vfprintf(out, operands_format, argptr);
    va_end(argptr);

    fprintf(out, "\n");
}

char *fresh_local_label(char *prefix, Context *ctx) {
    size_t buffer_size = strlen(prefix) + 16; // Increased size for safety
    char *buffer = malloc(buffer_size);
    snprintf(buffer, buffer_size, ".%s_%d", prefix, ctx->label_count);
    ctx->label_count++;
    return buffer;
}

void emit_label(FILE *out, char *label) { fprintf(out, "%s:\n", label); }

void emit_function_prologue(FILE *out) {
    if(is_M1){
        fprintf(out, ".align 4");
    } else {
        fprintf(out, ".align 2");
    }
    fprintf(out, "\n\n");
}

void emit_function_declaration(FILE *out, char *name) {
    fprintf(out, ".global _%s\n", name);
    emit_function_prologue(out);
    fprintf(out, "_%s:\n", name);
}

void emit_return(FILE *out) {
    emit_instr(out, "ret", "");
}

void emit_print(FILE *out) {
    emit_instr_format(out, "adrp", "x0, .Lformat@PAGE");
    emit_instr_format(out, "add", "x0, x0, .Lformat@PAGEOFF");
    emit_instr(out, "mov", "x1, x0"); // Expression value already in x0
    emit_instr(out, "bl", "_printf");
}

void emit_function_epilogue(FILE *out) {
    fprintf(out, "\n");
}

void write_header(FILE *out) {
    fprintf(out, ".data\n");
    fprintf(out, ".Lformat: .asciz \"%%d\\n\"\n");
    fprintf(out, ".text\n");
}

void write_footer(FILE *out) {
    fprintf(out, "\n");

    if (is_M1) {
        emit_instr(out, "mov", "x16, #1");
    } else {
        emit_instr(out, "mov", "x8, #93");
    }

    emit_instr(out, "svc", "#0xFFFF");
}

void write_syntax(FILE *out, Syntax *syntax, Context *ctx) {
    if (!syntax) return;

    if (syntax->type == UNARY_OPERATOR) {
        UnaryExpression *unary_syntax = syntax->unary_expression;

        write_syntax(out, unary_syntax->expression, ctx);

        if (unary_syntax->unary_type == NEGATION) {
            emit_instr(out, "neg", "x0, x0");
        } else if (unary_syntax->unary_type == BITWISE_NEGATION) {
            emit_instr(out, "mvn", "x0, x0");
        } else if (unary_syntax->unary_type == LOGICAL_NEGATION) {
            emit_instr(out, "cmp", "x0, #0");
            emit_instr(out, "cset", "x0, eq");
        }
    } else if (syntax->type == IMMEDIATE) {
        emit_instr_format(out, "mov", "x0, #%d", syntax->immediate->value);
    } else if (syntax->type == VARIABLE) {
        int offset = environment_get_offset(ctx->env, syntax->variable->var_name);
        emit_instr_format(out, "ldr", "x0, [sp, #%d]", offset);
    } else if (syntax->type == ASSIGNMENT) {
        write_syntax(out, syntax->assignment->expression, ctx);
        int offset = environment_get_offset(ctx->env, syntax->assignment->var_name);
        emit_instr_format(out, "str", "x0, [sp, #%d]", offset);
    } else if (syntax->type == BINARY_OPERATOR) {
        BinaryExpression *binary_syntax = syntax->binary_expression;
        int stack_offset = ctx->stack_offset;
        ctx->stack_offset -= WORD_SIZE;

        write_syntax(out, binary_syntax->left, ctx);
        emit_instr_format(out, "str", "x0, [sp, #%d]", stack_offset);

        write_syntax(out, binary_syntax->right, ctx);

        if (binary_syntax->binary_type == MULTIPLICATION) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "mul", "x0, x0, x1");
        } else if (binary_syntax->binary_type == ADDITION) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "add", "x0, x0, x1");
        } else if (binary_syntax->binary_type == SUBTRACTION) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "sub", "x0, x1, x0");
        } else if (binary_syntax->binary_type == GREATER) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "cmp", "x1, x0");
            emit_instr(out, "cset", "x0, gt");
        } else if (binary_syntax->binary_type == LESS) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "cmp", "x1, x0");
            emit_instr(out, "cset", "x0, lt");
        } else if (binary_syntax->binary_type == AND) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "and", "x0, x0, x1");
        } else if (binary_syntax->binary_type == OR) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "orr", "x0, x0, x1");
        } else if (binary_syntax->binary_type == EQUALS) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "cmp", "x0, x1");
            emit_instr(out, "cset", "x0, eq");
        } else if (binary_syntax->binary_type == GREATER_EQUALS) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "cmp", "x1, x0");
            emit_instr(out, "cset", "x0, ge");
        } else if (binary_syntax->binary_type == LESS_EQUALS) {
            emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
            emit_instr(out, "cmp", "x1, x0");
            emit_instr(out, "cset", "x0, le");
        }
        ctx->stack_offset += WORD_SIZE;
    } else if (syntax->type == RETURN_STATEMENT) {
        write_syntax(out, syntax->return_statement->expression, ctx);
        emit_return(out);
    } else if (syntax->type == PRINT_STATEMENT) {
        write_syntax(out, syntax->print_statement->expression, ctx);
        emit_print(out);
    } else if (syntax->type == IF_STATEMENT) {
        IfStatement *if_statement = syntax->if_statement;
        write_syntax(out, if_statement->condition, ctx);

        char *label_end = fresh_local_label("if_end", ctx);
        char *label_else = fresh_local_label("if_else", ctx);

        emit_instr(out, "cmp", "x0, #0");
        emit_instr_format(out, "beq", "%s", label_else);

        write_syntax(out, if_statement->then_stmts, ctx);
        emit_instr_format(out, "b", "%s", label_end);

        emit_label(out, label_else);
        if (if_statement->else_stmts != NULL) {
            write_syntax(out, if_statement->else_stmts, ctx);
        }
        emit_label(out, label_end);

        free(label_end);
        free(label_else);
    } else if (syntax->type == FUNCTION_CALL) {
        emit_instr_format(out, "bl", "_%s", syntax->function_call->function_name);
    } else if (syntax->type == DEFINE_VAR) {
        DefineVarStatement *define_var_statement = syntax->define_var_statement;
        int stack_offset = ctx->stack_offset;

        if (define_var_statement->init_value->type == ARRAY_TYPE && define_var_statement->init_value->immediate) {
            // Array declaration: var arr[10]
            int array_size = define_var_statement->init_value->immediate->value;
            ctx->stack_offset -= WORD_SIZE * array_size;
            environment_set_offset(ctx->env, define_var_statement->var_name, ctx->stack_offset);
            // Zero-initialize array
            for (int i = 0; i < array_size; i++) {
                emit_instr_format(out, "mov", "x0, #0");
                emit_instr_format(out, "str", "x0, [sp, #%d]", ctx->stack_offset + i * WORD_SIZE);
            }
        } else {
            // Regular variable
            ctx->stack_offset -= WORD_SIZE;
            environment_set_offset(ctx->env, define_var_statement->var_name, stack_offset);
            write_syntax(out, define_var_statement->init_value, ctx);
            emit_instr_format(out, "str", "x0, [sp, #%d]", stack_offset);
        }
    } else if (syntax->type == ARRAY_TYPE && syntax->array_access) { // Fixed: array_expression -> array_access
        // Array indexing: arr[5]
        ArrayAccess *array_access = syntax->array_access;
        int stack_offset = ctx->stack_offset;
        ctx->stack_offset -= WORD_SIZE;

        // Compute index
        write_syntax(out, array_access->index, ctx);
        emit_instr_format(out, "mov", "x1, #%d", WORD_SIZE);
        emit_instr(out, "mul", "x0, x0, x1"); // index * WORD_SIZE
        // Get base address
        int base_offset = environment_get_offset(ctx->env, array_access->array_name);
        emit_instr_format(out, "add", "x0, x0, %d", base_offset); // offset + base
        emit_instr(out, "add", "x0, sp, x0"); // sp + offset
        emit_instr(out, "ldr", "x0, [x0]");   // Load value at address

        ctx->stack_offset += WORD_SIZE;
    } else if (syntax->type == ARRAY_ASSIGNMENT) {
        ArrayAssignment *array_assign = syntax->array_assignment;
        int stack_offset = ctx->stack_offset;
        ctx->stack_offset -= WORD_SIZE;

        // Compute value
        write_syntax(out, array_assign->value, ctx);
        emit_instr_format(out, "str", "x0, [sp, #%d]", stack_offset);

        // Compute index
        write_syntax(out, array_assign->index, ctx);
        emit_instr_format(out, "mov", "x1, #%d", WORD_SIZE);
        emit_instr(out, "mul", "x0, x0, x1"); // index * WORD_SIZE
        // Get base address
        int base_offset = environment_get_offset(ctx->env, array_assign->array_name);
        emit_instr_format(out, "add", "x0, x0, %d", base_offset); // offset + base
        emit_instr(out, "add", "x0, sp, x0"); // sp + offset
        // Store value
        emit_instr_format(out, "ldr", "x1, [sp, #%d]", stack_offset);
        emit_instr(out, "str", "x1, [x0]"); // Store value at address

        ctx->stack_offset += WORD_SIZE;
    } else if (syntax->type == BLOCK) {
        List *statements = syntax->block->statements;
        for (int i = 0; i < list_length(statements); i++) {
            write_syntax(out, list_get(statements, i), ctx);
        }
    } else if (syntax->type == FUNCTION) {
        new_scope(ctx);
        emit_function_declaration(out, syntax->function->name);
        // Process parameters
        if (syntax->function->parameters && syntax->function->parameters->type == FUNCTION_ARGUMENTS) { // Fixed: FUNCTION_LIST -> FUNCTION_ARGUMENTS
            List *params = syntax->function->parameters->function_arguments->arguments;
            for (int i = 0; i < list_length(params); i++) {
                Syntax *param = list_get(params, i);
                if (param->type == DEFINE_VAR) {
                    int offset = ctx->stack_offset;
                    ctx->stack_offset -= WORD_SIZE;
                    environment_set_offset(ctx->env, param->define_var_statement->var_name, offset);
                    emit_instr_format(out, "str", "x%d, [sp, #%d]", i, offset);
                }
            }
        }
        write_syntax(out, syntax->function->root_block, ctx);
        emit_function_epilogue(out);
        end_scope(ctx);
    } else if (syntax->type == TOP_LEVEL) {
        List *declarations = syntax->top_level->declarations;
        for (int i = 0; i < list_length(declarations); i++) {
            write_syntax(out, list_get(declarations, i), ctx);
        }
    } else {
        warnx("Unknown syntax type in codegen: %s", syntax_type_name(syntax));
    }
}

void write_assembly(Syntax *syntax, char *file_name) {
    FILE *out = fopen(file_name, "w");
    if (!out) {
        err(1, "Could not open output file %s", file_name);
    }

    check_target_architecture();
    Context *ctx = new_context();

    write_header(out);
    write_syntax(out, syntax, ctx);
    write_footer(out);

    context_free(ctx);
    fclose(out);
}