#include "list.h"

#ifndef SYNTAX_HEADER
#define SYNTAX_HEADER

typedef enum
{
    IMMEDIATE,
    VARIABLE,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
    BLOCK,
    IF_STATEMENT,
    RETURN_STATEMENT,
    DEFINE_VAR,
    FUNCTION,
    FUNCTION_CALL,
    FUNCTION_ARGUMENTS,
    ASSIGNMENT,
    TOP_LEVEL,
    PRINT_STATEMENT,
    ARRAY_TYPE,
    ARRAY_ACCESS, // New type
    ARRAY_ASSIGNMENT
} SyntaxType;

typedef enum
{
    NEGATION,
    BITWISE_NEGATION,
    LOGICAL_NEGATION
} UnaryExpressionType;

typedef enum
{
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    GREATER,
    LESS,
    AND,
    OR,
    EQUALS,
    GREATER_EQUALS,
    LESS_EQUALS
} BinaryExpressionType;

struct Syntax;
typedef struct Syntax Syntax;

typedef struct Immediate
{
    int value;
} Immediate;

typedef struct Variable
{
    char *var_name;
} Variable;

typedef struct UnaryExpression
{
    UnaryExpressionType unary_type;
    Syntax *expression;
} UnaryExpression;

typedef struct BinaryExpression
{
    BinaryExpressionType binary_type;
    Syntax *left;
    Syntax *right;
} BinaryExpression;

typedef struct FunctionArguments
{
    List *arguments;
} FunctionArguments;

typedef struct FunctionCall
{
    char *function_name;
    Syntax *function_arguments;
} FunctionCall;

typedef struct Assignment
{
    char *var_name;
    Syntax *expression;
} Assignment;

typedef struct DefineVarStatement
{
    char *var_name;
    Syntax *init_value;
} DefineVarStatement;

typedef struct PrintStatement
{
    Syntax *expression;
} PrintStatement;

typedef struct ArrayAccess // Renamed from ArrayExpression
{
    char *array_name;
    Syntax *index;
} ArrayAccess;

typedef struct ArrayAssignment // Added
{
    char *array_name;
    Syntax *index;
    Syntax *value;
} ArrayAssignment;

typedef struct IfStatement {
    Syntax *condition;
    Syntax *then_stmts;
    Syntax *else_stmts;
} IfStatement;

typedef struct ReturnStatement
{
    Syntax *expression;
} ReturnStatement;

typedef struct Block
{
    List *statements;
} Block;

typedef struct Function
{
    char *name;
    Syntax *parameters; // Changed to Syntax*
    Syntax *root_block;
} Function;

typedef struct TopLevel
{
    List *declarations;
} TopLevel;

struct Syntax
{
    SyntaxType type;
    union
    {
        Immediate *immediate;
        Variable *variable;
        UnaryExpression *unary_expression;
        BinaryExpression *binary_expression;
        Assignment *assignment;
        IfStatement *if_statement;
        ReturnStatement *return_statement;
        DefineVarStatement *define_var_statement;
        FunctionArguments *function_arguments;
        FunctionCall *function_call;
        Block *block;
        Function *function;
        TopLevel *top_level;
        PrintStatement *print_statement;
        ArrayAccess *array_access; // Updated
        ArrayAssignment *array_assignment; // Added
    };
};

Syntax *immediate_new(int value);
Syntax *variable_new(char *var_name);
Syntax *negation_new(Syntax *expression);
Syntax *bitwise_negation_new(Syntax *expression);
Syntax *logical_negation_new(Syntax *expression);
Syntax *addition_new(Syntax *left, Syntax *right);
Syntax *subtraction_new(Syntax *left, Syntax *right);
Syntax *multiplication_new(Syntax *left, Syntax *right);
Syntax *greater_new(Syntax *left, Syntax *right);
Syntax *less_new(Syntax *left, Syntax *right);
Syntax *and_new(Syntax *left, Syntax *right);
Syntax *or_new(Syntax *left, Syntax *right);
Syntax *equals_new(Syntax *left, Syntax *right);
Syntax *greater_equals_new(Syntax *left, Syntax *right);
Syntax *less_equals_new(Syntax *left, Syntax *right);
Syntax *function_call_new(char *function_name, Syntax *func_args);
Syntax *function_arguments_new();
Syntax *assignment_new(char *var_name, Syntax *expression);
Syntax *if_new(Syntax *condition, Syntax *then_stmts, Syntax *else_stmts);
Syntax *return_statement_new(Syntax *expression);
Syntax *print_statement_new(Syntax *expression);
Syntax *block_new(List *statements);
Syntax *define_var_new(char *var_name, Syntax *init_value);
Syntax *function_new(char *name, Syntax *parameters, Syntax *root_block);
Syntax *top_level_new();
Syntax *array_type_new(int size);
Syntax *array_expression_new(char *array_name, Syntax *index);
Syntax *array_assignment_new(char *array_name, Syntax *index, Syntax *value); // Added

void syntax_free(Syntax *syntax);
char *syntax_type_name(Syntax *syntax);
void print_syntax(Syntax *syntax);

#endif