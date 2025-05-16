%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"

extern int yylex();
extern FILE *yyin;
void yyerror(const char *s);
Node *ast_root = NULL;
%}

%union {
    int num;
    char *str;
    Node *node;
}

%token FUN VAR RETURN IF ELSE
%token <str> IDENTIFIER
%token <num> NUMBER
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON ASSIGN
%token NOT BIT_NOT MINUS PLUS TIMES AND OR EQ GRT LST GT_EQ LT_EQ

%type <node> program function block statement expression

%left OR
%left AND
%left EQ GRT LST GT_EQ LT_EQ
%left PLUS MINUS
%left TIMES

%%

program: function { ast_root = $1; }
;

function: FUN IDENTIFIER LPAREN RPAREN LBRACE block RBRACE
    { $$ = create_function_node($2, $6); }
;

block: statement block { $$ = create_block_node($1, $2); }
     | /* empty */ { $$ = NULL; }
;

statement: RETURN expression SEMICOLON { $$ = create_return_node($2); }
         | VAR IDENTIFIER ASSIGN expression SEMICOLON { $$ = create_var_node($2, $4); }
         | IDENTIFIER ASSIGN expression SEMICOLON { $$ = create_assign_node($1, $3); }
         | IF LPAREN expression RPAREN block ELSE block { $$ = create_if_node($3, $5, $7); }
         | IF LPAREN expression RPAREN block { $$ = create_if_node($3, $5, NULL); }
;

expression: NUMBER { $$ = create_number_node($1); }
          | IDENTIFIER { $$ = create_identifier_node($1); }
          | NOT expression { $$ = create_unary_node(NOT, $2); }
          | BIT_NOT expression { $$ = create_unary_node(BIT_NOT, $2); }
          | MINUS expression %prec UNARY { $$ = create_unary_node(MINUS, $2); }
          | expression PLUS expression { $$ = create_binary_node(PLUS, $1, $3); }
          | expression MINUS expression { $$ = create_binary_node(MINUS, $1, $3); }
          | expression TIMES expression { $$ = create_binary_node(TIMES, $1, $3); }
          | expression AND expression { $$ = create_binary_node(AND, $1, $3); }
          | expression OR expression { $$ = create_binary_node(OR, $1, $3); }
          | expression EQ expression { $$ = create_binary_node(EQ, $1, $3); }
          | expression GRT expression { $$ = create_binary_node(GRT, $1, $3); }
          | expression LST expression { $$ = create_binary_node(LST, $1, $3); }
          | expression GT_EQ expression { $$ = create_binary_node(GT_EQ, $1, $3); }
          | expression LT_EQ expression { $$ = create_binary_node(LT_EQ, $1, $3); }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}