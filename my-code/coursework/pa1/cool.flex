/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
%}

/*
 * Define names for regular expressions here.
 */

KEYWORD (?i:class|else|fi|if|in|inherits|isvoid|let|loop|pool|then|while|case|esac|new|of|not)
TRUE_FALSE (t[Rr][Uu][Ee])|(f[Aa][Ll][Ss][Ee])
SELFID self|SELF_TYPE
TYPEID [A-Z][[:alnum:]_]*
OBJECTID [a-z][[:alnum:]_]*
SPECIAL_NOTATION [{}():;\.,><=*+/\[\]"'-\\]
INT_CONST [0-9]+

DARROW          =>

%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */
{DARROW}		  { return (DARROW); }

{KEYWORD}     { printf("#%d %s\n", curr_lineno, yytext); }

{TRUE_FALSE}  { printf("#%d %s\n", curr_lineno, yytext); }

{SELFID}      { printf("#%d %s\n", curr_lineno, yytext); }

{TYPEID}      { printf("#%d TYPEID %s\n", curr_lineno, yytext); }

{OBJECTID}    { printf("#%d OBJECTID %s\n", curr_lineno, yytext); }

{SPECIAL_NOTATION} { printf("#%d %s\n", curr_lineno, yytext); }

{INT_CONST}   { printf("#%d INT_CONST %s\n", curr_lineno, yytext); }

/* Hangs when comment reaches EOF in test.cl */
"\(*"        {
                int c;

                for ( ; ; )
                    {
                    while ( (c = yyinput()) != '*' &&
                            c != EOF )
                        ;    /* eat up text of comment */

                    if ( c == '*' )
                        {
                        while ( (c = yyinput()) == '*' )
                            ;
                        if ( c == ')' )
                            break;    /* found the end */
                        }

                    if ( c == EOF )
                        {
                        printf("aaaa");
                        break;
                        }
                    }
                }

[ \t\r\f\v]+

\n          {curr_lineno++;}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */


%%
