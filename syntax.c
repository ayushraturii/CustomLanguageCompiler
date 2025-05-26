#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "syntax.h"
#include "list.h"

Syntax *immediate_new(int value)
{
    Immediate *immediate = malloc(sizeof(Immediate));
    immediate->value = value;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = IMMEDIATE;
    syntax->immediate = immediate;

    return syntax;
}

Syntax *variable_new(char *var_name)
{
    Variable *variable = malloc(sizeof(Variable));
    variable->var_name = var_name;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = VARIABLE;
    syntax->variable = variable;

    return syntax;
}

Syntax *negation_new(Syntax *expression)
{
    UnaryExpression *unary_syntax = malloc(sizeof(UnaryExpression));
    unary_syntax->unary_type = NEGATION;
    unary_syntax->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = UNARY_OPERATOR;
    syntax->unary_expression = unary_syntax;

    return syntax;
}

Syntax *bitwise_negation_new(Syntax *expression)
{
    UnaryExpression *unary_syntax = malloc(sizeof(UnaryExpression));
    unary_syntax->unary_type = BITWISE_NEGATION;
    unary_syntax->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = UNARY_OPERATOR;
    syntax->unary_expression = unary_syntax;

    return syntax;
}

Syntax *logical_negation_new(Syntax *expression)
{
    UnaryExpression *unary_syntax = malloc(sizeof(UnaryExpression));
    unary_syntax->unary_type = LOGICAL_NEGATION;
    unary_syntax->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = UNARY_OPERATOR;
    syntax->unary_expression = unary_syntax;

    return syntax;
}

Syntax *addition_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = ADDITION;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *subtraction_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = SUBTRACTION;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *multiplication_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = MULTIPLICATION;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *greater_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = GREATER;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *less_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = LESS;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *and_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = AND;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *or_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = OR;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *equals_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = EQUALS;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *greater_equals_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = GREATER_EQUALS;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *less_equals_new(Syntax *left, Syntax *right)
{
    BinaryExpression *binary_syntax = malloc(sizeof(BinaryExpression));
    binary_syntax->binary_type = LESS_EQUALS;
    binary_syntax->left = left;
    binary_syntax->right = right;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BINARY_OPERATOR;
    syntax->binary_expression = binary_syntax;

    return syntax;
}

Syntax *if_new(Syntax *condition, Syntax *then_stmts, Syntax *else_stmts)
{
    IfStatement *if_statement = malloc(sizeof(IfStatement));
    if_statement->condition = condition;
    if_statement->then_stmts = then_stmts;
    if_statement->else_stmts = else_stmts ? else_stmts : NULL;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = IF_STATEMENT;
    syntax->if_statement = if_statement;

    return syntax;
}

Syntax *function_call_new(char *function_name, Syntax *func_args)
{
    FunctionCall *function_call = malloc(sizeof(FunctionCall));
    function_call->function_name = function_name;
    function_call->function_arguments = func_args;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = FUNCTION_CALL;
    syntax->function_call = function_call;

    return syntax;
}

Syntax *function_arguments_new()
{
    FunctionArguments *func_args = malloc(sizeof(FunctionArguments));
    func_args->arguments = list_new();

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = FUNCTION_ARGUMENTS;
    syntax->function_arguments = func_args;

    return syntax;
}

Syntax *assignment_new(char *var_name, Syntax *expression)
{
    Assignment *assignment = malloc(sizeof(Assignment));
    assignment->var_name = var_name;
    assignment->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = ASSIGNMENT;
    syntax->assignment = assignment;

    return syntax;
}

Syntax *return_statement_new(Syntax *expression)
{
    ReturnStatement *return_statement = malloc(sizeof(ReturnStatement));
    return_statement->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = RETURN_STATEMENT;
    syntax->return_statement = return_statement;

    return syntax;
}

Syntax *print_statement_new(Syntax *expression)
{
    PrintStatement *print_statement = malloc(sizeof(PrintStatement));
    print_statement->expression = expression;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = PRINT_STATEMENT;
    syntax->print_statement = print_statement;

    return syntax;
}

Syntax *define_var_new(char *var_name, Syntax *init_value) {
    DefineVarStatement *define_var_statement = malloc(sizeof(DefineVarStatement));
    define_var_statement->var_name = strdup(var_name);
    define_var_statement->init_value = init_value;
    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = DEFINE_VAR;
    syntax->define_var_statement = define_var_statement;
    return syntax;
}
Syntax *block_new(List *statements)
{
    Block *block = malloc(sizeof(Block));
    block->statements = statements;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = BLOCK;
    syntax->block = block;

    return syntax;
}

Syntax *function_new(char *name, Syntax *parameters, Syntax *root_block)
{
    Function *function = malloc(sizeof(Function));
    function->name = name;
    function->parameters = parameters;
    function->root_block = root_block;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = FUNCTION;
    syntax->function = function;

    return syntax;
}

Syntax *top_level_new()
{
    TopLevel *top_level = malloc(sizeof(TopLevel));
    top_level->declarations = list_new();

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = TOP_LEVEL;
    syntax->top_level = top_level;

    return syntax;
}

Syntax *array_type_new(int size)
{
    Immediate *immediate = malloc(sizeof(Immediate));
    immediate->value = size;

    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = ARRAY_TYPE;
    syntax->immediate = immediate;

    return syntax;
}

Syntax *array_expression_new(char *array_name, Syntax *index) {
    ArrayAccess *array_access = malloc(sizeof(ArrayAccess));
    array_access->array_name = strdup(array_name);
    array_access->index = index;
    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = ARRAY_TYPE; // Fix type below
    syntax->array_access = array_access;
    return syntax;
}

Syntax *array_assignment_new(char *array_name, Syntax *index, Syntax *value) {
    ArrayAssignment *array_assignment = malloc(sizeof(ArrayAssignment));
    array_assignment->array_name = strdup(array_name);
    array_assignment->index = index;
    array_assignment->value = value;
    Syntax *syntax = malloc(sizeof(Syntax));
    syntax->type = ARRAY_ASSIGNMENT;
    syntax->array_assignment = array_assignment;
    return syntax;
}

void syntax_list_free(List *syntaxes)
{
    if (syntaxes == NULL) return;
    for (int i = 0; i < list_length(syntaxes); i++) {
        syntax_free(list_get(syntaxes, i));
    }
    list_free(syntaxes);
}

void syntax_free(Syntax *syntax)
{
    if (!syntax) return;

    switch (syntax->type) {
        case IMMEDIATE:
            free(syntax->immediate);
            break;

        case VARIABLE:
            free(syntax->variable->var_name);
            free(syntax->variable);
            break;

        case UNARY_OPERATOR:
            syntax_free(syntax->unary_expression->expression);
            free(syntax->unary_expression);
            break;

        case BINARY_OPERATOR:
            syntax_free(syntax->binary_expression->left);
            syntax_free(syntax->binary_expression->right);
            free(syntax->binary_expression);
            break;

        case FUNCTION_CALL:
            syntax_free(syntax->function_call->function_arguments);
            free(syntax->function_call->function_name);
            free(syntax->function_call);
            break;

        case FUNCTION_ARGUMENTS:
            syntax_list_free(syntax->function_arguments->arguments);
            free(syntax->function_arguments);
            break;

        case IF_STATEMENT:
            syntax_free(syntax->if_statement->condition);
            syntax_free(syntax->if_statement->then_stmts);
            if (syntax->if_statement->else_stmts) {
                syntax_free(syntax->if_statement->else_stmts);
            }
            free(syntax->if_statement); // Fixed: syntax_if_statement -> syntax->if_statement
            break;

        case RETURN_STATEMENT:
            if (syntax->return_statement->expression) {
                syntax_free(syntax->return_statement->expression);
            }
            free(syntax->return_statement);
            break;

        case PRINT_STATEMENT:
            syntax_free(syntax->print_statement->expression);
            free(syntax->print_statement);
            break;

        case DEFINE_VAR:
            free(syntax->define_var_statement->var_name);
            syntax_free(syntax->define_var_statement->init_value); // Fixed: define_var->expression->init_value -> define_var_statement->init_value
            free(syntax->define_var_statement);
            break;

        case BLOCK:
            syntax_list_free(syntax->block->statements);
            free(syntax->block);
            break;

        case FUNCTION:
            free(syntax->function->name);
            if (syntax->function->parameters) {
                syntax_free(syntax->function->parameters);
            }
            syntax_free(syntax->function->root_block);
            free(syntax->function);
            break;

        case ASSIGNMENT:
            free(syntax->assignment->var_name);
            syntax_free(syntax->assignment->expression);
            free(syntax->assignment);
            break;

        case TOP_LEVEL:
            syntax_list_free(syntax->top_level->declarations);
            free(syntax->top_level);
            break;

        case ARRAY_TYPE:
            if (syntax->immediate) {
                free(syntax->immediate);
            } else if (syntax->array_access) {
                free(syntax->array_access->array_name);
                syntax_free(syntax->array_access->index);
                free(syntax->array_access);
            }
            break;

        case ARRAY_ASSIGNMENT:
            free(syntax->array_assignment->array_name);
            syntax_free(syntax->array_assignment->index);
            syntax_free(syntax->array_assignment->value);
            free(syntax->array_assignment);
            break;

        default:
            warnx("Could not free syntax tree with type: %s", syntax_type_name(syntax));
            break;
    }

    free(syntax);
}

char *syntax_type_name(Syntax *syntax)
{
    switch (syntax->type) {
        case IMMEDIATE: return "IMMEDIATE";
        case VARIABLE: return "VARIABLE";
        case UNARY_OPERATOR:
            switch (syntax->unary_expression->unary_type) {
                case NEGATION: return "UNARY NEGATION";
                case BITWISE_NEGATION: return "UNARY BITWISE_NEGATION";
                case LOGICAL_NEGATION: return "UNARY LOGICAL_NEGATION";
            }
            break;
        case BINARY_OPERATOR:
            switch (syntax->binary_expression->binary_type) {
                case ADDITION: return "ADDITION";
                case SUBTRACTION: return "SUBTRACTION";
                case MULTIPLICATION: return "MULTIPLICATION";
                case GREATER: return "GREATER THAN";
                case LESS: return "LESS THAN";
                case AND: return "AND";
                case OR: return "OR";
                case EQUALS: return "EQUALS";
                case GREATER_EQUALS: return "GREATER OR EQUAL";
                case LESS_EQUALS: return "LESS OR EQUAL";
            }
            break;
        case FUNCTION_CALL: return "FUNCTION CALL";
        case FUNCTION_ARGUMENTS: return "FUNCTION ARGUMENTS";
        case IF_STATEMENT: return "IF";
        case RETURN_STATEMENT: return "RETURN";
        case PRINT_STATEMENT: return "PRINT";
        case DEFINE_VAR: return "DEFINE VARIABLE";
        case BLOCK: return "BLOCK";
        case FUNCTION: return "FUNCTION";
        case ASSIGNMENT: return "ASSIGNMENT";
        case TOP_LEVEL: return "TOP LEVEL";
        case ARRAY_TYPE:
            return syntax->immediate ? "ARRAY DECLARATION" : "ARRAY ACCESS";
        case ARRAY_ASSIGNMENT: return "ARRAY ASSIGNMENT";
    }
    return "??? UNKNOWN SYNTAX";
}

void print_syntax_indented(Syntax *syntax, int indent)
{
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }

    char *syntax_type_string = syntax_type_name(syntax);

    switch (syntax->type) {
        case IMMEDIATE:
            printf("%s %d\n", syntax_type_string, syntax->immediate->value);
            break;
        case VARIABLE:
            printf("%s '%s'\n", syntax_type_string, syntax->variable->var_name);
            break;
        case UNARY_OPERATOR:
            printf("%s\n", syntax_type_string);
            print_syntax_indented(syntax->unary_expression->expression, indent + 4);
            break;
        case BINARY_OPERATOR:
            printf("%s LEFT\n", syntax_type_string);
            print_syntax_indented(syntax->binary_expression->left, indent + 4);
            for (int i = 0; i < indent; i++) {
                printf(" ");
            }
            printf("%s RIGHT\n", syntax_type_string);
            print_syntax_indented(syntax->binary_expression->right, indent + 4);
            break;
        case FUNCTION_CALL:
            printf("%s '%s'\n", syntax_type_string, syntax->function_call->function_name);
            print_syntax_indented(syntax->function_call->function_arguments, indent);
            break;
        case FUNCTION_ARGUMENTS:
            printf("%s\n", syntax_type_string);
            for (int i = 0; i < list_length(syntax->function_arguments->arguments); i++) {
                print_syntax_indented(list_get(syntax->function_arguments->arguments, i), indent + 4);
            }
            break;
        case IF_STATEMENT:
            printf("%s CONDITION\n", syntax_type_string);
            print_syntax_indented(syntax->if_statement->condition, indent + 4);
            for (int i = 0; i < indent; i++) {
                printf(" ");
            }
            printf("%s THEN\n", syntax_type_string);
            print_syntax_indented(syntax->if_statement->then_stmts, indent + 4);
            if (syntax->if_statement->else_stmts) {
                for (int i = 0; i < indent; i++) {
                    printf(" ");
                }
                printf("%s ELSE\n", syntax_type_string);
                print_syntax_indented(syntax->if_statement->else_stmts, indent + 4);
            }
            break;
        case RETURN_STATEMENT:
            printf("%s\n", syntax_type_string);
            print_syntax_indented(syntax->return_statement->expression, indent + 4);
            break;
        case PRINT_STATEMENT:
            printf("%s\n", syntax_type_string);
            print_syntax_indented(syntax->print_statement->expression, indent + 4);
            break;
        case DEFINE_VAR:
            printf("%s '%s'\n", syntax_type_string, syntax->define_var_statement->var_name);
            for (int i = 0; i < indent; i++) {
                printf(" ");
            }
            printf("'%s' INITIAL VALUE\n", syntax->define_var_statement->var_name);
            print_syntax_indented(syntax->define_var_statement->init_value, indent + 4);
            break;
        case BLOCK:
            printf("%s\n", syntax_type_string);
            for (int i = 0; i < list_length(syntax->block->statements); i++) {
                print_syntax_indented(list_get(syntax->block->statements, i), indent + 4);
            }
            break;
        case FUNCTION:
            printf("%s '%s'\n", syntax_type_string, syntax->function->name);
            if (syntax->function->parameters) {
                for (int i = 0; i < indent; i++) {
                    printf(" ");
                }
                printf("PARAMETERS\n");
                print_syntax_indented(syntax->function->parameters, indent + 4);
            }
            print_syntax_indented(syntax->function->root_block, indent + 4);
            break;
        case ASSIGNMENT:
            printf("%s '%s'\n", syntax_type_string, syntax->assignment->var_name);
            print_syntax_indented(syntax->assignment->expression, indent + 4);
            break;
        case TOP_LEVEL:
            printf("%s\n", syntax_type_string);
            for (int i = 0; i < list_length(syntax->top_level->declarations); i++) {
                print_syntax_indented(list_get(syntax->top_level->declarations, i), indent + 4);
            }
            break;
        case ARRAY_TYPE:
            if (syntax->immediate) {
                printf("%s SIZE %d\n", syntax_type_string, syntax->immediate->value);
            } else {
                printf("%s '%s'\n", syntax_type_string, syntax->array_access->array_name);
                print_syntax_indented(syntax->array_access->index, indent + 4);
            }
            break;
        case ARRAY_ASSIGNMENT:
            printf("%s '%s'\n", syntax_type_string, syntax->array_assignment->array_name);
            for (int i = 0; i < indent; i++) {
                printf(" ");
            }
            printf("INDEX\n");
            print_syntax_indented(syntax->array_assignment->index, indent + 4);
            for (int i = 0; i < indent; i++) {
                printf(" ");
            }
            printf("VALUE\n");
            print_syntax_indented(syntax->array_assignment->value, indent + 4);
            break;
        default:
            printf("??? UNKNOWN SYNTAX TYPE\n");
            break;
    }
}

void print_syntax(Syntax *syntax)
{
    print_syntax_indented(syntax, 0);
}

