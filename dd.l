%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "dd.tab.h"
    #include "node.h"

    #define YY_NO_UNPUT // Disables an unused Lex function (unput) to avoid compiler warnings
%}

%%
"fun"   {return FUN;}
"var"   {return VAR;}
"return" {return RETURN;}
"if"    {return IF;}
"else"  {return ELSE;}
[a-zA-Z_][a-zA-Z0-9_] {yylval.str=strdup(yytext); return IDENTIFIER;}
[0-9]+ {yylval.num=atoi(yytext); return NUMBER;}
"(" {return LPAREN;}
")" {return RPAREN;}
"{" {return LBRACE;}
"}" {return RBRACE;}
";" {return SEMICOLON;}
"=" {return ASSIGN;}
"!" {return NOT;}
"~" {return BIT_NOT;}
"-" {return MINUS;}
"+" {return PLUS;}
"*" {return TIMES;}
"&&" {return AND;}
"||" {return OR;}
"==" {return EQ;}
">" {return GRT;}
"<" {return LST;}
">=" {return GT_EQ;}
"<=" {return LT_EQ;}
[ \t\n] ; // Ignore whitespace
. {printf("Unexpected character: %s\n", yytext);}

%%

int yywrap(){
    return 1;
}








