#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <err.h>

#include "stack.h"
#include "syntax.h"
#include "build/y.tab.h"
#include "semantic.h"
#include "assembly.h"

void print_help()
{
    printf("dd\n");
    printf("To compile a file:\n");
    printf("    $ dd foo.dd\n");
    printf("To output the tokens without parsing:\n");
    printf("    $ dd --dump-tokens foo.dd\n");
    printf("To output the AST without compiling:\n");
    printf("    $ dd --dump-ast foo.dd\n");
    printf("To print this message:\n");
    printf("    $ dd --help\n\n");
}

extern Stack *syntax_stack;

extern int yyparse(void);
extern FILE *yyin;
extern int yylex(void);

typedef enum
{
    TOKENIZE,
    PARSE,
    EMIT_ASM,
} stage_t;

int main(int argc, char *argv[])
{
    ++argv, --argc; /* Skip over program name. */

    stage_t terminate_at = EMIT_ASM;

    check_target_architecture();

    char *file_name;

    if (argc == 1 && strcmp(argv[0], "--help") == 0)
    {
        print_help();
        return 0;
    }
    else if (argc == 1)
    {
        file_name = argv[0];
    }
    else if (argc == 2 && strcmp(argv[0], "--dump-tokens") == 0)
    {
        terminate_at = TOKENIZE;
        file_name = argv[1];
    }
    else if (argc == 2 && strcmp(argv[0], "--dump-ast") == 0)
    {
        terminate_at = PARSE;
        file_name = argv[1];
    }
    else
    {
        print_help();
        return 1;
    }

    int result;

    yyin = fopen(file_name, "r");

    if (yyin == NULL)
    {
        printf("Could not open file: '%s'\n", file_name);
        result = 2;
        goto cleanup_file;
    }

    char *filename = strrchr(file_name, '/');
    char output_file[100];
    if (filename == NULL)
        filename = file_name;
    else
        filename++;

    strcpy(filename + strlen(filename) - 3, ".asm");
    strcpy(output_file, "build/");
    strcat(output_file, filename);

    if (terminate_at == TOKENIZE)
    {
        int tokens;
        printf("Tokens \n");
        int token_count = 0;

        while ((tokens = yylex()) != 0)
        {
            printf("%-10sToken: %-4d\n", "", tokens);
            token_count++;
        }
        printf("\n");

        printf("\nTotal tokens: %d\n", token_count);

        result = 1;
        goto cleanup_file;
    }

    syntax_stack = stack_new();

    result = yyparse();

    if (result != 0)
    {
        goto cleanup_syntax;
    }

    Syntax *complete_syntax = stack_pop(syntax_stack);
    if (syntax_stack->size > 0)
    {
        warnx("Did not consume the whole syntax stack during parsing! Remaining:");
        while (syntax_stack->size > 0)
        {
            fprintf(stderr, "%s", syntax_type_name(stack_pop(syntax_stack)));
        }
    }
    printf("\n \n");
    if (terminate_at == PARSE)
    {
        printf("---AST---\n");
        print_syntax(complete_syntax);
    }
    else
    {
        // Perform semantic analysis
        SemanticAnalyzer *analyzer = semantic_analyzer_new();
        analyze_semantics(analyzer, complete_syntax);
        List *errors = get_semantic_errors(analyzer);
        if (list_length(errors) > 0) {
            printf("Semantic errors found:\n");
            for (int i = 0; i < list_length(errors); i++) {
                printf("%s\n", (char *)list_get(errors, i));
            }
            semantic_analyzer_free(analyzer);
            result = 1;
            goto cleanup_syntax;
        }
        semantic_analyzer_free(analyzer);
        write_assembly(complete_syntax, output_file);
        syntax_free(complete_syntax); // Move here

        printf("Written %s.\n", output_file);
    }

cleanup_syntax:
    stack_free(syntax_stack);

cleanup_file:
    if (yyin != NULL)
    {
        fclose(yyin);
    }

    return result;
}