/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_PARSER_H_INCLUDED
# define YY_YY_SRC_PARSER_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 7 "src/parser/parser.y"

    #include "ast/expr.h"
    #include "ast/stmt.h"
    #include "util/err.h"
    struct parser {
        char **lines;
        int error;  
        struct ast_stmt_list *program;
    };

#line 60 "src/parser/parser.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    GOTO = 258,                    /* GOTO  */
    GOSUB = 259,                   /* GOSUB  */
    ON = 260,                      /* ON  */
    IF = 261,                      /* IF  */
    THEN = 262,                    /* THEN  */
    LET = 263,                     /* LET  */
    DEF = 264,                     /* DEF  */
    FOR = 265,                     /* FOR  */
    TO = 266,                      /* TO  */
    STEP = 267,                    /* STEP  */
    NEXT = 268,                    /* NEXT  */
    PRINT = 269,                   /* PRINT  */
    INPUT = 270,                   /* INPUT  */
    RETURN = 271,                  /* RETURN  */
    NEQ = 272,                     /* NEQ  */
    LEQ = 273,                     /* LEQ  */
    GEQ = 274,                     /* GEQ  */
    NUMBER = 275,                  /* NUMBER  */
    FLOAT = 276,                   /* FLOAT  */
    NUM_VAR = 277,                 /* NUM_VAR  */
    STR_VAR = 278,                 /* STR_VAR  */
    FUNC_VAR = 279,                /* FUNC_VAR  */
    STRING = 280,                  /* STRING  */
    UMINUS = 281                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 28 "src/parser/parser.y"

    int int_val;
    float float_val;
    char *str_val;
    struct ast_expr *expr;
    struct ast_stmt *stmt;
    struct ast_stmt_list *stmt_list;
    struct ast_go_arg_list *goto_arg_list;
    struct ast_expr_arg_list *expr_arg_list;

#line 114 "src/parser/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (struct parser *p);

/* "%code provides" blocks.  */
#line 20 "src/parser/parser.y"

    void yyerror(struct parser *p, const char* message);
    #define YY_DECL \
        int yylex(struct parser *p)
    YY_DECL;
    struct parser *parse(FILE *file); 

#line 151 "src/parser/parser.h"

#endif /* !YY_YY_SRC_PARSER_PARSER_H_INCLUDED  */
