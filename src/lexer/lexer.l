%{
    #include <string.h>
    #include "parser/parser.h"
    #include "util/err.h"
    #define YY_USER_ACTION yylloc.first_line = yylineno;

%}

%option noyywrap yylineno
%%
IF {
    return IF;
}
ON {
    return ON;
}
THEN {
    return THEN;
}
LET {
    return LET;
}
DEF {
    return DEF;
}
FOR {
    return FOR;
}
TO {
    return TO;
}
STEP {
    return STEP;
}
PRINT {
    return PRINT;
}
INPUT {
    return INPUT;
}
RETURN {
    return RETURN;
}
"GO"[ ]*"TO" {
    return GOTO;
}
"GO"[ ]*"SUB" {
    return GOSUB;
}
NEXT {
    yytext[strcspn(yytext, " ")] = 0;
    yylval.int_val = atoi(yytext);
    return NEXT;
}
[0-9]+ {
    yylval.int_val = atoi(yytext);
    return NUMBER;
}
[0-9]+[.][0-9]* {
    yylval.float_val = atof(yytext);
    return FLOAT;
}
[-+*/^=<>(),\n] {
    return yytext[0];
}
[A-Z][0-9]? {
    yylval.str_val = strdup(yytext);
    return NUM_VAR;
}
[A-Z]"$" {
    yylval.str_val = strdup(yytext);
    return STR_VAR;
}
[A-Z]{3} {
    yylval.str_val = strdup(yytext);
    return FUNC_VAR;
}
["][A-Z0-9 ()=]+["] {
    yytext[yyleng - 1] = 0;
    yylval.str_val = strdup(yytext + 1);
    return STRING;
}
"<=" {
    return LEQ;
}
">=" {
    return GEQ;
}
"<>" {
    return NEQ;
}

[A-Z]+ {
    compile_error("Invalid token '%s'", yylineno, p->lines, yytext);
    p->error = 1;
}
[ \t] ; 
%%