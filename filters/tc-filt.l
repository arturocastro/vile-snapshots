%s DESC GAPS CAPS VALS LINK

%{

/*
 * $Header: /users/source/archives/vile.vcs/filters/RCS/tc-filt.l,v 1.8 1999/08/29 20:04:54 tom Exp $
 *
 * Filter to add vile "attribution" sequences to selected bits of termcap
 */

#include <filters.h>

char *filter_name = "tc";

static char *Action_attr;
static char *Comment_attr;
static char *Number_attr;
static char *Keyword_attr;
static char *Ident_attr;
static char *String_attr;

static int commented;

static char *
check_name(char *name)
{
    char temp[3];
    char *attr;

    strncpy(temp, name, 2);
    temp[2] = 0;
    attr = keyword_attr(temp);
    if (attr == 0)
	attr = "";
    if (strcmp(attr, Keyword_attr))
    	attr = Action_attr;
    return attr;
}

static void
write_capname(char *name, int len)
{
    write_token(yyout, name, (len >= 2) ? 2 : len, check_name(name));
    if (len > 2) {
	char *attr = Action_attr;
	if (len == 3 && name[2] == '@')
	    attr = "";
	write_token(yyout, name + 2, len - 2, attr);
    }
}

static void
begin_value(char *name)
{
    if (!strcmp(name, "tc"))
	BEGIN(LINK);
    else
	BEGIN(VALS);
}

void
init_filter(int before GCC_UNUSED)
{
}

void
do_filter(FILE *input, FILE *output)
{
    yyin = input;
    yyout = output;

    Action_attr  = class_attr(NAME_ACTION);
    Comment_attr = class_attr(NAME_COMMENT);
    Keyword_attr = class_attr(NAME_KEYWORD);
    Number_attr  = class_attr(NAME_NUMBER);
    Ident_attr   = class_attr(NAME_IDENT);
    String_attr  = class_attr(NAME_LITERAL);

    while (yylex() > 0) {
    }
}

%}

BLANK		[ \t]
CAPCHAR		[^\.|# \t\n:=]
CAPNAME		{CAPCHAR}+
DOTNAME		(\.)+{CAPNAME}

ALIAS		[a-zA-Z_\.0-9-]+

NUMBER		"#"[0-9]+
STRING		(\\[:]|\\\n|[^:\n])*

%%

^"#"[^\n]*			{ WriteToken(Comment_attr); }
\n\n				{ ECHO; BEGIN(INITIAL); }

^({ALIAS}"|"|(\\\n\t)*)*	{ WriteToken(Ident_attr); BEGIN(DESC); }
^{ALIAS}:			{ WriteToken(Ident_attr); BEGIN(GAPS); }

<DESC>[^:\n]*			{ WriteToken(Comment_attr); BEGIN(GAPS); }
<GAPS,VALS>(:|:\\\n\t)*		{ commented=0; ECHO; BEGIN(CAPS); }
<CAPS>{CAPNAME}			{ write_capname(yytext, yyleng); begin_value(yytext); }
<CAPS>{DOTNAME}			{ WriteToken(Comment_attr); commented = 1; begin_value(yytext); }
<VALS>{NUMBER}			{ WriteToken(commented ? Comment_attr : Number_attr); BEGIN(GAPS); }
<VALS>{STRING}			{ WriteString(commented ? Comment_attr : String_attr); BEGIN(GAPS); }
<LINK>{STRING}			{ WriteString(commented ? Comment_attr : Ident_attr); BEGIN(GAPS); }