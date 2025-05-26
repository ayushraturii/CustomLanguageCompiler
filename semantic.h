#include "syntax.h"
#include "list.h"

#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

typedef enum {
    TYPE_INT,    // For integers (e.g., NUMBER)
    TYPE_BOOL,   // For boolean results (e.g., comparisons)
    TYPE_VOID,   // For functions with no return value
    TYPE_ARRAY   // For arrays
} DataType;

typedef struct Symbol {
    char *name;
    DataType type;
    int scope;  // Scope level (0 for global, increments for nested blocks)
    int is_function; // 1 if function, 0 if variable
    List *parameters; // For functions, stores parameter names (if any)
    int array_size;  // Size of array (for TYPE_ARRAY, otherwise 0)
} Symbol;

typedef struct SymbolTable {
    List *symbols; // List of Symbol structs
    int current_scope;
} SymbolTable;

typedef struct SemanticAnalyzer {
    SymbolTable *table;
    List *errors; // List of error messages
    int in_function; // Track if inside a function for return statements
    char *current_function; // Name of current function being analyzed
} SemanticAnalyzer;

SemanticAnalyzer *semantic_analyzer_new();
void semantic_analyzer_free(SemanticAnalyzer *analyzer);
void analyze_semantics(SemanticAnalyzer *analyzer, Syntax *syntax);
List *get_semantic_errors(SemanticAnalyzer *analyzer);

#endif