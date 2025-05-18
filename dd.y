%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../syntax.h"
#include "../stack.h"

#define YYSTYPE char*

int yyparse(void);
int yylex();

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
	return 1;
}

extern FILE *yyin;

Stack *syntax_stack;

%}

%token FUN
%token TYPE IDENTIFIER RETURN NUMBER
%token LB RB OPEN_BRACE CLOSE_BRACE
%token IF ELSE
%token EQ GT_EQ LT_EQ
%token SEMICOLON COMMA

/* Operator associativity, least precedence first.
 * See http://en.cppreference.com/w/c/language/operator_precedence
 */
%left ASN
%left LST
%left GRT
%left PLUS
%left MINUS
%left MULT
%left AD ORR EQ
%left GT_EQ 
%left LT_EQ
%nonassoc LN
%nonassoc NT
%nonassoc ELSE

%%

program:
        function program
        {
            Syntax *top_level_syntax;
            if (stack_empty(syntax_stack)) {
                top_level_syntax = top_level_new();
            } else if (((Syntax *)stack_peek(syntax_stack))->type != TOP_LEVEL) {
                top_level_syntax = top_level_new();
            } else {
                top_level_syntax = stack_pop(syntax_stack);
            }

            list_push(top_level_syntax->top_level->declarations,
                      stack_pop(syntax_stack));
            stack_push(syntax_stack, top_level_syntax);
        }
        |
        ;

function:
	FUN IDENTIFIER LB parameter_list RB OPEN_BRACE block CLOSE_BRACE
        {
            Syntax *current_syntax = stack_pop(syntax_stack);
            // TODO: assert current_syntax has type BLOCK.
            stack_push(syntax_stack, function_new((char*)$2, current_syntax));
        }
        ;

parameter_list:
        nonempty_parameter_list
        |
        ;

nonempty_parameter_list:
        TYPE IDENTIFIER COMMA parameter_list
        |
        TYPE IDENTIFIER
        ;

block:
        statement block
        {
            /* Append to the current block, or start a new block. */
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
        |
        ;

argument_list:
        nonempty_argument_list
        |
        {
            // Empty argument list.
            stack_push(syntax_stack, function_arguments_new());
        }
        ;

nonempty_argument_list:
        expression COMMA nonempty_argument_list
        {
            Syntax *arguments_syntax;
            if (stack_empty(syntax_stack)) {
                // This should be impossible, we shouldn't be able to
                // parse this on its own.
                assert(false);
            } else if (((Syntax *)stack_peek(syntax_stack))->type != FUNCTION_ARGUMENTS) {
                arguments_syntax = function_arguments_new();
            } else {
                arguments_syntax = stack_pop(syntax_stack);
            }

            list_push(arguments_syntax->function_arguments->arguments, stack_pop(syntax_stack));
            stack_push(syntax_stack, arguments_syntax);
        }
        |
        expression
        {
            // TODO: find a way to factor out the duplication with the above.
            if (stack_empty(syntax_stack)) {
                // This should be impossible, we shouldn't be able to
                // parse this on its own.
                assert(false);
            }

            Syntax *arguments_syntax = function_arguments_new();
            list_push(arguments_syntax->function_arguments->arguments, stack_pop(syntax_stack));

            stack_push(syntax_stack, arguments_syntax);
        }
        ;

statement:
        RETURN expression SEMICOLON
        {
            Syntax *current_syntax = stack_pop(syntax_stack);
            stack_push(syntax_stack, return_statement_new(current_syntax));
        }
        |
        IF LB expression RB OPEN_BRACE block CLOSE_BRACE
        {
            // TODO: else statements.
            Syntax *then_stmts = stack_pop(syntax_stack);
            Syntax *condition = stack_pop(syntax_stack);
            stack_push(syntax_stack, if_new(condition, then_stmts, NULL));
        }
        |
        IF LB expression RB OPEN_BRACE block CLOSE_BRACE ELSE OPEN_BRACE block CLOSE_BRACE
        {
            Syntax *else_stmts = stack_pop(syntax_stack);
            Syntax *then_stmts = stack_pop(syntax_stack);
            Syntax *condition = stack_pop(syntax_stack);
            stack_push(syntax_stack, if_new(condition, then_stmts, else_stmts));
        }
        |
        TYPE IDENTIFIER ASN expression SEMICOLON
        {
            Syntax *init_value = stack_pop(syntax_stack);
            stack_push(syntax_stack, define_var_new((char*)$2, init_value));
        }
        |
        expression SEMICOLON
        {
            // Nothing to do, we have the AST node already.
        }
        ;
expression:
	    NUMBER
        {
            stack_push(syntax_stack, immediate_new(atoi((char*)$1)));
            free($1);
        }
        |
	    IDENTIFIER
        {
            stack_push(syntax_stack, variable_new((char*)$1));
        }
        |
	    IDENTIFIER ASN expression
        {
            Syntax *expression = stack_pop(syntax_stack);
            stack_push(syntax_stack, assignment_new((char*)$1, expression));
        }
        | OPEN_BRACE expression CLOSE_BRACE          {}
        | LB expression RB                    {}
        |
        MINUS expression
        {
            Syntax *current_syntax = stack_pop(syntax_stack);
            stack_push(syntax_stack, negation_new(current_syntax));
        }
        |
        NT expression
        {
            Syntax *current_syntax = stack_pop(syntax_stack);
            stack_push(syntax_stack, bitwise_negation_new(current_syntax));
        }
        |
        LN expression
        {
            Syntax *current_syntax = stack_pop(syntax_stack);
            stack_push(syntax_stack, logical_negation_new(current_syntax));
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
            Syntax *arguments = stack_pop(syntax_stack);
            stack_push(syntax_stack, function_call_new((char*)$1, arguments));
        }
        ;
