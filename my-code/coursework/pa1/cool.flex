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
#include <string>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE* fin; /* we read from this file */

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
  int string_index = 0;
  std::string string_value;
  char* unterminated_string_error_msg = "Unterminated string constant";
  char* eof_in_comment_msg = "EOF in comment";
  char* unmatched_closing_comment_msg = "Unmatched *)";
  char* invalid_symbol_msg = "0";
%}

/*
 * Define names for regular expressions here.
 */

CLASS (?i:class)
ELSE (?i:else)
FI (?i:fi)
IF (?i:if)
IN (?i:in)
INHERITS (?i:inherits)
ISVOID (?i:isvoid)
LET (?i:let)
LOOP (?i:loop)
POOL (?i:pool)
THEN (?i:then)
WHILE (?i:while)
CASE (?i:case)
ESAC (?i:esac)
NEW (?i:new)
OF (?i:of)
NOT (?i:not)

TRUE (t[Rr][Uu][Ee])
FALSE (f[Aa][Ll][Ss][Ee])


TYPEID ([A-Z][[:alnum:]_]*)|(SELF_TYPE)
OBJECTID [a-z][[:alnum:]_]*
SPECIAL_NOTATION [{}():;\.,<\*\+/\-\\=~@]
INT_CONST [0-9]+
FORBIDDEN_SYMBOLS [^{}():;\.,<\*\+/\-\\=~@[:alnum:]]

DARROW          =>
ASSIGN          <-
LE              <=
ONELINE_COMMENT --.*$
ONELINE_COMMENT_EOF --.*<<EOF>>

%s IN_COMMENT
%s IN_STRING
%s IN_ONELINE_COMMENT

%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */

<INITIAL>{
{DARROW}		    { return DARROW; }
{CLASS}         { return CLASS; }
{ELSE}          { return ELSE; }
{FI}            { return FI; }
{IF}            { return IF; }
{IN}            { return IN; }
{INHERITS}      { return INHERITS; }
{ISVOID}        { return ISVOID; }
{LET}           { return LET; }
{LOOP}          { return LOOP; }
{POOL}          { return POOL; }
{THEN}          { return THEN; }
{WHILE}         { return WHILE; }
{CASE}          { return CASE; }
{ESAC}          { return ESAC; }
{NEW}           { return NEW; }
{OF}            { return OF; }
{NOT}           { return NOT; }

{TRUE}          { cool_yylval.boolean = true; return BOOL_CONST; }
{FALSE}         { cool_yylval.boolean = false; return BOOL_CONST; }

{TYPEID}        { cool_yylval.symbol = new Entry(yytext, strlen(yytext), string_index++); return TYPEID; }
{OBJECTID}      { cool_yylval.symbol = new Entry(yytext, strlen(yytext), string_index++); return OBJECTID; }
{SPECIAL_NOTATION} { return yytext[0]; }
{ASSIGN}        { return ASSIGN; }
{LE}            { return LE; }

{INT_CONST}  { cool_yylval.symbol = new Entry(yytext, strlen(yytext), string_index++); return INT_CONST; }

[ \t\r\f\v]+
\n                {curr_lineno++;}
"(*"              BEGIN(IN_COMMENT);
"*)"              { 
                    cool_yylval.error_msg = unmatched_closing_comment_msg;
                    return ERROR; 
                  }
\"                BEGIN(IN_STRING);
--                BEGIN(IN_ONELINE_COMMENT);

{FORBIDDEN_SYMBOLS} { 
                      invalid_symbol_msg = yytext;
                      cool_yylval.error_msg = invalid_symbol_msg;
                      return ERROR; 
                    }
}

<IN_COMMENT>{
"*)"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        curr_lineno++;
<<EOF>>   {
            BEGIN(INITIAL);
            cool_yylval.error_msg = eof_in_comment_msg;
            return ERROR;
          }
}

<IN_ONELINE_COMMENT>{
"\n"      {
            curr_lineno++;
            BEGIN(INITIAL);
          }
<<EOF>>   {
            BEGIN(INITIAL);
          }
.
}

<IN_STRING>{
\\b                 { string_value += '\b'; }
\\t                 { string_value += '\t'; }
\\n                 { string_value += '\n'; }
\\\n                {
                      curr_lineno++;
                      string_value += '\n'; 
                    }
\\f                 { string_value += '\f'; }
\"                  {
                      cool_yylval.symbol = new Entry(string_value.data(), string_value.size(), string_index++);
                      string_value.clear();
                      BEGIN(INITIAL);
                      return STR_CONST;
                    }
\n                  {
                      curr_lineno++;
                      cool_yylval.error_msg=unterminated_string_error_msg;
                      string_value.clear();
                      BEGIN(INITIAL);
                      return ERROR;
                    }
.                   { string_value += yytext[0]; }
\\.                 { string_value += yytext[1]; }
}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */


%%