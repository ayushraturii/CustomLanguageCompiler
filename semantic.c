#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "semantic.h"
#include "list.h"

Symbol *symbol_new(char *name, DataType type, int scope, int is_function, int array_size) {
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->type = type;
    symbol->scope = scope;
    symbol->is_function = is_function;
    symbol->parameters = is_function ? list_new() : NULL;
    symbol->array_size = array_size;
    return symbol;
}

void symbol_free(Symbol *symbol) {
    if (symbol->name) free(symbol->name);
    if (symbol->parameters) list_free(symbol->parameters);
    free(symbol);
}

SymbolTable *symbol_table_new() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->symbols = list_new();
    table->current_scope = 0;
    return table;
}

void symbol_table_free(SymbolTable *table) {
    for (int i = 0; i < list_length(table->symbols); i++) {
        symbol_free((Symbol *)list_get(table->symbols, i));
    }
    list_free(table->symbols);
    free(table);
}

void symbol_table_add(SymbolTable *table, char *name, DataType type, int is_function, int array_size) {
    Symbol *symbol = symbol_new(name, type, table->current_scope, is_function, array_size);
    list_append(table->symbols, symbol);
}

Symbol *symbol_table_lookup(SymbolTable *table, char *name, int scope) {
    for (int i = 0; i < list_length(table->symbols); i++) {
        Symbol *symbol = (Symbol *)list_get(table->symbols, i);
        if (strcmp(symbol->name, name) == 0 && symbol->scope <= scope) {
            return symbol;
        }
    }
    return NULL;
}

SemanticAnalyzer *semantic_analyzer_new() {
    SemanticAnalyzer *analyzer = malloc(sizeof(SemanticAnalyzer));
    analyzer->table = symbol_table_new();
    analyzer->errors = list_new();
    analyzer->in_function = 0;
    analyzer->current_function = NULL;
    return analyzer;
}

void semantic_analyzer_free(SemanticAnalyzer *analyzer) {
    symbol_table_free(analyzer->table);
    for (int i = 0; i < list_length(analyzer->errors); i++) {
        free(list_get(analyzer->errors, i));
    }
    list_free(analyzer->errors);
    if (analyzer->current_function) free(analyzer->current_function);
    free(analyzer);
}

void report_error(SemanticAnalyzer *analyzer, char *message, Syntax *syntax) {
    char *error = malloc(256);
    snprintf(error, 256, "Semantic Error: %s at %s", message, syntax_type_name(syntax));
    list_append(analyzer->errors, error);
}

DataType get_expression_type(SemanticAnalyzer *analyzer, Syntax *syntax);

void analyze_syntax(SemanticAnalyzer *analyzer, Syntax *syntax) {
    if (!syntax) return;

    switch (syntax->type) {
        case TOP_LEVEL: {
            List *declarations = syntax->top_level->declarations;
            // First pass: Register all function declarations
            for (int i = 0; i < list_length(declarations); i++) {
                Syntax *decl = list_get(declarations, i);
                if (decl->type == FUNCTION) {
                    char *name = decl->function->name;
                    if (symbol_table_lookup(analyzer->table, name, analyzer->table->current_scope)) {
                        report_error(analyzer, "Function already declared", decl);
                    } else {
                        symbol_table_add(analyzer->table, name, TYPE_VOID, 1, 0);
                        Symbol *func_symbol = symbol_table_lookup(analyzer->table, name, analyzer->table->current_scope);
                        if (decl->function->parameters && decl->function->parameters->type == FUNCTION_ARGUMENTS) {
                            List *params = decl->function->parameters->function_arguments->arguments;
                            for (int j = 0; j < list_length(params); j++) {
                                Syntax *param = list_get(params, j);
                                if (param->type == DEFINE_VAR) {
                                    char *param_name = param->define_var_statement->var_name;
                                    list_append(func_symbol->parameters, strdup(param_name));
                                }
                            }
                        }
                    }
                }
            }
            // Second pass: Analyze bodies of all declarations
            for (int i = 0; i < list_length(declarations); i++) {
                analyze_syntax(analyzer, list_get(declarations, i));
            }
            break;
        }
        case ARRAY_ASSIGNMENT: {
            char *array_name = syntax->array_assignment->array_name;
            Symbol *symbol = symbol_table_lookup(analyzer->table, array_name, analyzer->table->current_scope);
            if (!symbol || symbol->type != TYPE_ARRAY) {
                report_error(analyzer, "Assignment to non-array variable", syntax);
            } else {
                DataType index_type = get_expression_type(analyzer, syntax->array_assignment->index);
                if (index_type != TYPE_INT) {
                    report_error(analyzer, "Array index must be an integer", syntax);
                }
                DataType value_type = get_expression_type(analyzer, syntax->array_assignment->value);
                if (value_type != TYPE_INT) {
                    report_error(analyzer, "Array element must be an integer", syntax);
                }
                analyze_syntax(analyzer, syntax->array_assignment->index);
                analyze_syntax(analyzer, syntax->array_assignment->value);
            }
            break;
        }
        case FUNCTION: {
            char *name = syntax->function->name;
            analyzer->in_function = 1;
            analyzer->current_function = strdup(name);
            analyzer->table->current_scope++;
            // Analyze parameters
            if (syntax->function->parameters && syntax->function->parameters->type == FUNCTION_ARGUMENTS) {
                List *params = syntax->function->parameters->function_arguments->arguments;
                for (int i = 0; i < list_length(params); i++) {
                    Syntax *param = list_get(params, i);
                    if (param->type == DEFINE_VAR) {
                        char *param_name = param->define_var_statement->var_name;
                        if (symbol_table_lookup(analyzer->table, param_name, analyzer->table->current_scope)) {
                            report_error(analyzer, "Parameter already declared", param);
                        } else {
                            symbol_table_add(analyzer->table, param_name, TYPE_INT, 0, 0);
                        }
                    }
                }
            }
            analyze_syntax(analyzer, syntax->function->root_block);
            analyzer->table->current_scope--;
            analyzer->in_function = 0;
            free(analyzer->current_function);
            analyzer->current_function = NULL;
            break;
        }
        case BLOCK: {
            analyzer->table->current_scope++;
            List *statements = syntax->block->statements;
            for (int i = 0; i < list_length(statements); i++) {
                analyze_syntax(analyzer, list_get(statements, i));
            }
            // Clean up symbols in the current scope
            for (int i = list_length(analyzer->table->symbols) - 1; i >= 0; i--) {
                Symbol *symbol = list_get(analyzer->table->symbols, i);
                if (symbol && symbol->scope >= analyzer->table->current_scope) {
                    symbol_free(symbol);
                    list_set(analyzer->table->symbols, i, NULL);
                    list_pop(analyzer->table->symbols);
                }
            }
            analyzer->table->current_scope--;
            break;
        }
        case DEFINE_VAR: {
            char *var_name = syntax->define_var_statement->var_name;
            if (symbol_table_lookup(analyzer->table, var_name, analyzer->table->current_scope)) {
                report_error(analyzer, "Variable already declared in this scope", syntax);
            } else {
                DataType init_type = get_expression_type(analyzer, syntax->define_var_statement->init_value);
                if (init_type == TYPE_ARRAY) {
                    if (syntax->define_var_statement->init_value->type != ARRAY_TYPE ||
                        !syntax->define_var_statement->init_value->immediate) {
                        report_error(analyzer, "Invalid array declaration", syntax);
                    } else {
                        int array_size = syntax->define_var_statement->init_value->immediate->value;
                        if (array_size <= 0) {
                            report_error(analyzer, "Array size must be positive", syntax);
                        } else {
                            symbol_table_add(analyzer->table, var_name, TYPE_ARRAY, 0, array_size);
                        }
                    }
                } else if (init_type == TYPE_VOID) {
                    report_error(analyzer, "Variable initialized with void type", syntax);
                } else {
                    symbol_table_add(analyzer->table, var_name, init_type, 0, 0);
                }
                analyze_syntax(analyzer, syntax->define_var_statement->init_value);
            }
            break;
        }
        case ASSIGNMENT: {
            char *var_name = syntax->assignment->var_name;
            Symbol *symbol = symbol_table_lookup(analyzer->table, var_name, analyzer->table->current_scope);
            if (!symbol || symbol->is_function) {
                report_error(analyzer, "Assignment to undeclared variable or function", syntax);
            } else {
                DataType expr_type = get_expression_type(analyzer, syntax->assignment->expression);
                if (expr_type != symbol->type && !(symbol->type == TYPE_ARRAY && expr_type == TYPE_INT)) {
                    report_error(analyzer, "Type mismatch in assignment", syntax);
                }
                analyze_syntax(analyzer, syntax->assignment->expression);
            }
            break;
        }
        case VARIABLE: {
            char *var_name = syntax->variable->var_name;
            if (!symbol_table_lookup(analyzer->table, var_name, analyzer->table->current_scope)) {
                report_error(analyzer, "Use of undeclared variable", syntax);
            }
            break;
        }
        case FUNCTION_CALL: {
            char *func_name = syntax->function_call->function_name;
            Symbol *symbol = symbol_table_lookup(analyzer->table, func_name, analyzer->table->current_scope);
            if (!symbol || !symbol->is_function) {
                report_error(analyzer, "Call to undeclared function", syntax);
            }
            analyze_syntax(analyzer, syntax->function_call->function_arguments);
            break;
        }
        case FUNCTION_ARGUMENTS: {
            List *args = syntax->function_arguments->arguments;
            for (int i = 0; i < list_length(args); i++) {
                analyze_syntax(analyzer, list_get(args, i));
            }
            break;
        }
        case IF_STATEMENT: {
            DataType cond_type = get_expression_type(analyzer, syntax->if_statement->condition);
            if (cond_type != TYPE_BOOL) {
                report_error(analyzer, "If condition must be boolean", syntax);
            }
            analyze_syntax(analyzer, syntax->if_statement->condition);
            analyze_syntax(analyzer, syntax->if_statement->then_stmts);
            if (syntax->if_statement->else_stmts) {
                analyze_syntax(analyzer, syntax->if_statement->else_stmts);
            }
            break;
        }
        case RETURN_STATEMENT: {
            if (!analyzer->in_function) {
                report_error(analyzer, "Return statement outside function", syntax);
            } else {
                analyze_syntax(analyzer, syntax->return_statement->expression);
            }
            break;
        }
        case PRINT_STATEMENT: {
            DataType expr_type = get_expression_type(analyzer, syntax->print_statement->expression);
            if (expr_type != TYPE_INT) {
                report_error(analyzer, "Print statement requires integer expression", syntax);
            }
            analyze_syntax(analyzer, syntax->print_statement->expression);
            break;
        }
        case ARRAY_TYPE: {
            if (syntax->array_access) { // Fixed: array_expression -> array_access
                char *array_name = syntax->array_access->array_name;
                Symbol *symbol = symbol_table_lookup(analyzer->table, array_name, analyzer->table->current_scope);
                if (!symbol || symbol->type != TYPE_ARRAY) {
                    report_error(analyzer, "Use of undeclared array", syntax);
                } else {
                    DataType index_type = get_expression_type(analyzer, syntax->array_access->index);
                    if (index_type != TYPE_INT) {
                        report_error(analyzer, "Array index must be an integer", syntax);
                    }
                    analyze_syntax(analyzer, syntax->array_access->index);
                }
            }
            break;
        }
        case BINARY_OPERATOR: {
            analyze_syntax(analyzer, syntax->binary_expression->left);
            analyze_syntax(analyzer, syntax->binary_expression->right);
            break;
        }
        case UNARY_OPERATOR: {
            analyze_syntax(analyzer, syntax->unary_expression->expression);
            break;
        }
        case IMMEDIATE: {
            break;
        }
        default:
            warnx("Unknown syntax type in semantic analysis: %s", syntax_type_name(syntax));
            break;
    }
}

DataType get_expression_type(SemanticAnalyzer *analyzer, Syntax *syntax) {
    if (!syntax) return TYPE_VOID;

    switch (syntax->type) {
        case IMMEDIATE:
            return TYPE_INT;
        case VARIABLE: {
            Symbol *symbol = symbol_table_lookup(analyzer->table, syntax->variable->var_name, analyzer->table->current_scope);
            return symbol ? symbol->type : TYPE_VOID;
        }
        case ARRAY_TYPE: {
            if (syntax->array_access) { // Fixed: array_expression -> array_access
                Symbol *symbol = symbol_table_lookup(analyzer->table, syntax->array_access->array_name, analyzer->table->current_scope);
                if (symbol && symbol->type == TYPE_ARRAY) {
                    return TYPE_INT; // Array elements are integers
                }
                return TYPE_VOID;
            }
            return TYPE_ARRAY;
        }
        case BINARY_OPERATOR: {
            BinaryExpression *bin = syntax->binary_expression;
            DataType left_type = get_expression_type(analyzer, bin->left);
            DataType right_type = get_expression_type(analyzer, bin->right);
            if (left_type == TYPE_VOID || right_type == TYPE_VOID) {
                report_error(analyzer, "Invalid operand types in binary operation", syntax);
                return TYPE_VOID;
            }
            if (bin->binary_type == GREATER || bin->binary_type == LESS ||
                bin->binary_type == EQUALS || bin->binary_type == GREATER_EQUALS ||
                bin->binary_type == LESS_EQUALS) {
                return TYPE_BOOL;
            }
            if (left_type != TYPE_INT || right_type != TYPE_INT) {
                report_error(analyzer, "Binary operation requires integer operands", syntax);
                return TYPE_VOID;
            }
            return TYPE_INT;
        }
        case UNARY_OPERATOR: {
            UnaryExpression *unary = syntax->unary_expression;
            DataType expr_type = get_expression_type(analyzer, unary->expression);
            if (expr_type == TYPE_VOID) {
                report_error(analyzer, "Invalid operand type in unary operation", syntax);
                return TYPE_VOID;
            }
            if (unary->unary_type == LOGICAL_NEGATION) {
                if (expr_type != TYPE_BOOL) {
                    report_error(analyzer, "Logical negation requires boolean operand", syntax);
                    return TYPE_VOID;
                }
                return TYPE_BOOL;
            }
            if (expr_type != TYPE_INT) {
                report_error(analyzer, "Unary operation requires integer operand", syntax);
                return TYPE_VOID;
            }
            return TYPE_INT;
        }
        case FUNCTION_CALL: {
            Symbol *symbol = symbol_table_lookup(analyzer->table, syntax->function_call->function_name, analyzer->table->current_scope);
            return symbol ? symbol->type : TYPE_VOID;
        }
        default:
            return TYPE_VOID;
    }
}

void analyze_semantics(SemanticAnalyzer *analyzer, Syntax *syntax) {
    analyze_syntax(analyzer, syntax);
}

List *get_semantic_errors(SemanticAnalyzer *analyzer) {
    return analyzer->errors;
}