%{
#include "../syntax.h" // Added for Syntax type
#include "y.tab.h"

void comment();
void count();

void yyerror(const char *str);
%}

%option noyywrap

%%

"fun"    { count(); yylval.string = strdup(yytext); return FUN; }
"print"  { count(); return PRINT; }
"//"[^\n]*    { /* Discard comments. */ }
"/*"          { comment(); }
[ \t\n\f]+      { count(); /* Ignore whitespace */ }
"{"           { count(); return OPEN_BRACE; }
"}"           { count(); return CLOSE_BRACE; }
"("           { count(); return LB; }
")"           { count(); return RB; }
"["           { count(); return LSB; }
"]"           { count(); return RSB; }
"~"           { count(); return NT; }
"!"           { count(); return LN; }
"+"           { count(); return PLUS; }
"-"           { count(); return MINUS; }
"*"           { count(); return MULT; }
"<"           { count(); return LST; }
"&&"          { count(); return AD; }
"||"          { count(); return ORR; }
"=="          { count(); return EQ; }
">"           { count(); return GRT; }
">="          { count(); return GT_EQ; }
"<="          { count(); return LT_EQ; }
"="           { count(); return ASN; }
";"           { count(); return SEMICOLON; }
","           { count(); return COMMA; }
[0-9]+        {
                /* TODO: check numbers are in the legal range, and don't start with 0. */
                count(); yylval.string = strdup(yytext); return NUMBER;}
"if"          { count(); return IF; }
"else"        { count(); return ELSE; }
"return"      { count(); return RETURN; }
"var"         { count(); return TYPE; }
[a-zA-Z][_a-zA-Z0-9]* { count(); yylval.string = strdup(yytext); return IDENTIFIER; }
%%

#define INPUT_EOF 0

int column = 0;

void comment(void) {
    char c, prev = 0;
    while ((c = input()) != INPUT_EOF) {
        if (c == '/' && prev == '*')
            return;
        prev = c;
    }
    yyerror("unterminated comment");
}

void count() {
    int i;
    for (i = 0; yytext[i] != '\0'; i++)
        if (yytext[i] == '\n')
            column = 0;
        else if (yytext[i] == '\t')
            column += 8 - (column % 8);
        else
            column++;
    ECHO;
}

void yyerror(const char *str) {
    fprintf(stderr, "error: %s\n", str);
}