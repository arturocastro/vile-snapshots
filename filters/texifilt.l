%s TEXT NOTE

%{
/*
 * $Header: /users/source/archives/vile.vcs/filters/RCS/texifilt.l,v 1.1 2002/11/03 19:35:04 tom Exp $
 *
 * Filter to add vile "attribution" sequences to selected bits of texinfo input.
 */

#include <filters.h>

DefineFilter("texi");

static char *Action_attr;
static char *Comment_attr;
static char *Number_attr;
static char *String_attr;

static void
init_filter(int before GCC_UNUSED)
{
}

static void
do_filter(FILE *input)
{
    yyin = input;

    Action_attr  = class_attr(NAME_ACTION);
    Comment_attr = class_attr(NAME_COMMENT);
    Number_attr  = class_attr(NAME_NUMBER);
    String_attr  = class_attr(NAME_LITERAL);

    BEGIN(TEXT);
    while (yylex() > 0) {
    }
}

%}

SPACE		[ \t]

PUNCT		[!"'*-.:=?@^`{}~]

NAME		[a-zA-Z]+

INTEGER		[-+]?([0-9]+)
REAL		[-+]?([0-9]*\.[0-9]+)([eE][+-]?[0-9]+)?
NUMBER		({INTEGER}|{REAL})

%%

<TEXT>"@{C}"		|
<TEXT>"@"{SPACE}	|
<TEXT>[{}]		|
<TEXT>"@"{PUNCT}	{ WriteToken(String_attr); }

<TEXT>"@"{NAME}		{ char *attr = keyword_attr(yytext + 1);
			  char *mark = Action_attr;
			  if (attr == Comment_attr) {
			      BEGIN(NOTE);
			      mark = attr;
			  } else if (attr != 0) {
			      mark = attr;
			  }
			  flt_puts("@", 1, mark);
			  WriteToken2(keyword_attr(yytext + 1), 1);
			}

<NOTE>[^\n]*		{ WriteToken(Comment_attr); }
<NOTE>\n		{ ECHO; BEGIN(TEXT); }

<TEXT>{NUMBER}		{ WriteToken(Number_attr); }
<TEXT>.			{ ECHO; }