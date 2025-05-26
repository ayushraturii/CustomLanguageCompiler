#include <stdio.h>
#include "syntax.h"
#include "context.h" // Added to define Context type

#ifndef ASSEMBLY_HEADER
#define ASSEMBLY_HEADER

void check_target_architecture();
void emit_header(FILE *out, char *name);
void emit_insn(FILE *out, char *insn);
void emit_print(FILE *out); // Removed unused Context *ctx parameter
void write_header(FILE *out);
void write_footer(FILE *out);
void write_syntax(FILE *out, Syntax *syntax);
void write_assembly(Syntax *syntax, char *file_name);

#endif