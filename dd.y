%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../syntax.h"
#include "../stack.h"

int yyparse(void);
int yylex();
void yyerror(const char *str);
int yywrap();

extern FILE *yyin;

Stack *syntax_stack;
%}

%union {
    char *string;
    struct Syntax *syntax;
}

%token <string> IDENTIFIER NUMBER TYPE
%token FUN PRINT RETURN
%token LB RB LSB RSB OPEN_BRACE CLOSE_BRACE
%token IF ELSE
%token EQ GT_EQ LT_EQ
%token SEMICOLON COMMA ASN
%token PLUS MINUS MULT GRT LST AD ORR LN NT

%left ASN
%left LST GRT
%left PLUS MINUS
%left MULT
%left AD ORR EQ GT_EQ LT_EQ
%right LN NT
%nonassoc ELSE

%type <syntax> program function_stmt block_stmt statement expression
%type <syntax> parameter_list nonempty_parameter_list argument_list nonempty_argument_list
%type <syntax> array_assignment array_access

%%

program:
        function_stmt program
        {
            Syntax *top_level_syntax;
            if (stack_empty(syntax_stack)) {
                top_level_syntax = top_level_new();
            } else if (((Syntax *)stack_peek(syntax_stack))->type != TOP_LEVEL) {
                top_level_syntax = top_level_new();
            } else {
                top_level_syntax = stack_pop(syntax_stack);
            }
            list_push(top_level_syntax->top_level->declarations, stack_pop(syntax_stack));
            stack_push(syntax_stack, top_level_syntax);
        }
        | /* empty */
        {
            stack_push(syntax_stack, top_level_new());
        }
        ;

function_stmt:
        FUN IDENTIFIER LB parameter_list RB OPEN_BRACE block_stmt CLOSE_BRACE
        {
            Syntax *block = stack_pop(syntax_stack);
            Syntax *params = stack_pop(syntax_stack);
            stack_push(syntax_stack, function_new($2, params, block));
            free($2);
        }
        ;

parameter_list:
        nonempty_parameter_list
        |
        {
            stack_push(syntax_stack, function_arguments_new());
        }
        ;

nonempty_parameter_list:
        TYPE IDENTIFIER LSB NUMBER RSB COMMA parameter_list
        {
            Syntax *array_syntax = array_type_new(atoi($4));
            Syntax *param = define_var_new($2, array_syntax);
            Syntax *param_list = stack_pop(syntax_stack);
            list_push(param_list->function_arguments->arguments, param);
            stack_push(syntax_stack, param_list);
            free($2);
            free($4);
        }
        |
        TYPE IDENTIFIER COMMA parameter_list
        {
            Syntax *param = define_var_new($2, immediate_new(0));
            Syntax *param_list = stack_pop(syntax_stack);
            list_push(param_list->function_arguments->arguments, param);
            stack_push(syntax_stack, param_list);
            free($2);
        }
        |
        TYPE IDENTIFIER LSB NUMBER RSB
        {
            Syntax *array_syntax = array_type_new(atoi($4));
            Syntax *param = define_var_new($2, array_syntax);
            Syntax *param_list = function_arguments_new();
            list_push(param_list->function_arguments->arguments, param);
            stack_push(syntax_stack, param_list);
            free($2);
            free($4);
        }
        |
        TYPE IDENTIFIER
        {
            Syntax *param = define_var_new($2, immediate_new(0));
            Syntax *param_list = function_arguments_new();
            list_push(param_list->function_arguments->arguments, param);
            stack_push(syntax_stack, param_list);
            free($2);
        }
        ;

block_stmt:
        statement block_stmt
        {
            Syntax *block_syntax;
            if (stack_empty(syntax_stack)) {
                block_syntax = block_new(list_new());
            } else if (((Syntax *)stack_peek(syntax_stack))->type != BLOCK) {
                block_syntax = block_new(list_new());
            } else {
                block_syntax = stack_pop(syntax_stack);
            }
            list_push(block_syntax->block->statements, stack_pop(syntax_stack));
            stack_push(syntax_stack, block_syntax);
        }
        | /* empty */
        {
            stack_push(syntax_stack, block_new(list_new()));
        }
        ;

argument_list:
        nonempty_argument_list
        |
        {
            stack_push(syntax_stack, function_arguments_new());
        }
        ;

nonempty_argument_list:
        expression COMMA nonempty_argument_list
        {
            Syntax *args;
            if (((Syntax *)stack_peek(syntax_stack))->type != FUNCTION_ARGUMENTS) {
                args = function_arguments_new();
            } else {
                args = stack_pop(syntax_stack);
            }
            list_push(args->function_arguments->arguments, stack_pop(syntax_stack));
            stack_push(syntax_stack, args);
        }
        |
        expression
        {
            Syntax *args = function_arguments_new();
            list_push(args->function_arguments->arguments, stack_pop(syntax_stack));
            stack_push(syntax_stack, args);
        }
        ;

statement:
        RETURN expression SEMICOLON
        {
            Syntax *expr = stack_pop(syntax_stack);
            stack_push(syntax_stack, return_statement_new(expr));
        }
        |
        PRINT expression SEMICOLON
        {
            Syntax *expr = stack_pop(syntax_stack);
            stack_push(syntax_stack, print_statement_new(expr));
        }
        |
        IF LB expression RB OPEN_BRACE block_stmt CLOSE_BRACE
        {
            Syntax *then_block = stack_pop(syntax_stack);
            Syntax *condition = stack_pop(syntax_stack);
            stack_push(syntax_stack, if_new(condition, then_block, NULL));
        }
        |
        IF LB expression RB OPEN_BRACE block_stmt CLOSE_BRACE ELSE OPEN_BRACE block_stmt CLOSE_BRACE
        {
            Syntax *else_block = stack_pop(syntax_stack);
            Syntax *then_block = stack_pop(syntax_stack);
            Syntax *condition = stack_pop(syntax_stack);
            stack_push(syntax_stack, if_new(condition, then_block, else_block));
        }
        |
        TYPE IDENTIFIER ASN expression SEMICOLON
        {
            Syntax *init_value = stack_pop(syntax_stack);
            stack_push(syntax_stack, define_var_new($2, init_value));
            free($2);
        }
        |
        TYPE IDENTIFIER LSB NUMBER RSB SEMICOLON
        {
            Syntax *array_syntax = array_type_new(atoi($4));
            stack_push(syntax_stack, define_var_new($2, array_syntax));
            free($2);
            free($4);
        }
        |
        TYPE IDENTIFIER SEMICOLON
        {
            stack_push(syntax_stack, define_var_new($2, immediate_new(0)));
            free($2);
        }
        |
        array_assignment SEMICOLON
        {
            // Array assignment statement
        }
        |
        expression SEMICOLON
        {
            // Expression statement
        }
        ;

array_assignment:
        IDENTIFIER LSB expression RSB ASN expression
        {
            Syntax *value = stack_pop(syntax_stack);
            Syntax *index = stack_pop(syntax_stack);
            stack_push(syntax_stack, array_assignment_new($1, index, value));
            free($1);
        }
        ;

expression:
        NUMBER
        {
            stack_push(syntax_stack, immediate_new(atoi($1)));
            free($1);
        }
        |
        IDENTIFIER
        {
            stack_push(syntax_stack, variable_new($1));
            free($1);
        }
        |
        IDENTIFIER ASN expression
        {
            Syntax *expr = stack_pop(syntax_stack);
            stack_push(syntax_stack, assignment_new($1, expr));
            free($1);
        }
        |
        array_access
        {
            // Array access expression
        }
        |
        LB expression RB
        {
            // Parenthesized expression
        }
        |
        MINUS expression %prec LN
        {
            Syntax *expr = stack_pop(syntax_stack);
            stack_push(syntax_stack, negation_new(expr));
        }
        |
        NT expression %prec LN
        {
            Syntax *expr = stack_pop(syntax_stack);
            stack_push(syntax_stack, bitwise_negation_new(expr));
        }
        |
        LN expression %prec LN
        {
            Syntax *expr = stack_pop(syntax_stack);
            stack_push(syntax_stack, logical_negation_new(expr));
        }
        |
        expression PLUS expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, addition_new(left, right));
        }
        |
        expression MINUS expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, subtraction_new(left, right));
        }
        |
        expression MULT expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, multiplication_new(left, right));
        }
        |
        expression GRT expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, greater_new(left, right));
        }
        |
        expression LST expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, less_new(left, right));
        }
        |
        expression AD expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, and_new(left, right));
        }
        |
        expression ORR expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, or_new(left, right));
        }
        |
        expression EQ expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, equals_new(left, right));
        }
        |
        expression GT_EQ expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, greater_equals_new(left, right));
        }
        |
        expression LT_EQ expression
        {
            Syntax *right = stack_pop(syntax_stack);
            Syntax *left = stack_pop(syntax_stack);
            stack_push(syntax_stack, less_equals_new(left, right));
        }
        |
        IDENTIFIER LB argument_list RB
        {
            Syntax *args = stack_pop(syntax_stack);
            stack_push(syntax_stack, function_call_new($1, args));
            free($1);
        }
        ;

array_access:
        IDENTIFIER LSB expression RSB
        {
            Syntax *index = stack_pop(syntax_stack);
            stack_push(syntax_stack, array_expression_new($1, index));
            free($1);
        }
        ;

%%