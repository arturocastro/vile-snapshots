%pointer
%x SETUP FLAGS VALUE OTHER COMMENT CODE PREPRO CMT_STANDARD CMT_TURBOC

%{

/*
 * $Id: iss-filt.l,v 1.18 2016/12/11 16:04:15 tom Exp $
 *
 * Filter to add vile "attribution" sequences to selected bits of Inno Setup
 * input text.
 */

#include <filters.h>
#include <fltstack.h>

DefineFilter(iss);

static const char *Action_attr;
static const char *Comment_attr;
static const char *Ident2_attr;
static const char *Number_attr;
static const char *Prepro_attr;
static const char *String_attr;

static const char *save_table;

%}

BLANK		[ \t]

IDENT		[[:alpha:]_][[:alnum:]_]*

SSTRING		\'([^'\r\n]|(''))*\'
DSTRING		\"([^"\r\n]|(""))*\"
STRING		{SSTRING}|{DSTRING}

HEXADECIMAL	\$[[:xdigit:]_]+
UNSIGNED	[[:digit:]]+
INTEGER		[-+]?{UNSIGNED}
REAL		[-+]?(({UNSIGNED}?\.{UNSIGNED})|({UNSIGNED}(\.{UNSIGNED}?)?))([eE]{INTEGER})?

CHARACTER	"#"{UNSIGNED}

NUMBER		{HEXADECIMAL}|{INTEGER}|{REAL}

%%

<SETUP,FLAGS,CODE>^"#"{IDENT} { WriteToken3(Prepro_attr, 1);
			  save_table = get_symbol_table();
			  push_state(PREPRO);
			  set_symbol_table("iss_cpp");
			  WriteToken2(get_keyword_attr(yytext+1), 1);
			}
<PREPRO>{STRING}	{ WriteToken(String_attr); }
<PREPRO>{NUMBER}	{ WriteToken(Number_attr); }
<PREPRO>{BLANK}		{ ECHO; }
<PREPRO>[\r\n]		{ ECHO;
			  pop_state();
			  set_symbol_table(save_table);
			  }
<PREPRO>.		{ ECHO; }

<SETUP,FLAGS,CODE>^"["[^]\n]*"]"	{ WriteToken(Action_attr);
			  if (!strcmp(yytext, "[Code]")) {
			    begin_state(CODE);
			    set_symbol_table("pas");
			  } else {
			    if (!strcmp(yytext, "[Setup]")) {
				begin_state(SETUP);
			    } else {
				begin_state(FLAGS);
			    }
			    set_symbol_table(default_table);
			  }
			}
<SETUP,FLAGS,CODE>{IDENT} { WriteToken(get_keyword_attr(yytext)); }

<SETUP>"="		{ ECHO; begin_state(VALUE); }
<SETUP,VALUE>{SSTRING}	{ WriteToken(String_attr); }
<SETUP,VALUE>{NUMBER}	{ WriteToken(Number_attr); }
<VALUE>[\n]		{ ECHO; begin_state(SETUP); }

<SETUP,FLAGS>"//"[^\r\n]*	{ WriteToken(Comment_attr); }
<SETUP,FLAGS>^";"[^\r\n]*	{ WriteToken(Comment_attr); }

<FLAGS>":"{BLANK}*	{ ECHO; begin_state(OTHER); }
<OTHER>[^;\r\n]+	{ WriteToken(String_attr); }
<OTHER>";"		{ ECHO; begin_state(FLAGS); }
<OTHER>[\n]		{ ECHO; begin_state(FLAGS); }

<CODE>{SSTRING}		{ WriteToken(String_attr); }
<CODE>{CHARACTER}	|
<CODE>{NUMBER}		{ WriteToken(Number_attr); }

<CODE>"{"		{ BeginQuote(CMT_TURBOC, Comment_attr); }
<CMT_TURBOC>"}"		{ FinishQuote(CODE); }
<CMT_TURBOC>\n		|
<CMT_TURBOC>.		{ flt_bfr_append(yytext, yyleng); }

<CODE>"(*"		{ BeginQuote(CMT_STANDARD, Comment_attr); }
<CMT_STANDARD>"*)"	{ FinishQuote(CODE); }
<CMT_STANDARD>\n	|
<CMT_STANDARD>.		{ flt_bfr_append(yytext, yyleng); }

<CODE>"//"[^\r\n]*	{ WriteToken(Comment_attr); /* Delphi */ }

<CODE>.			{ ECHO; }

%%

#include <fltstack.h>

static void
init_filter(int before GCC_UNUSED)
{
    (void) before;
}

static void
do_filter(FILE *inputs)
{
    InitLEX(inputs);

    Action_attr = class_attr(NAME_ACTION);
    Comment_attr = class_attr(NAME_COMMENT);
    Ident2_attr = class_attr(NAME_IDENT2);
    Number_attr = class_attr(NAME_NUMBER);
    Prepro_attr = class_attr(NAME_PREPROC);
    String_attr = class_attr(NAME_LITERAL);

    begin_state(FLAGS);
    RunLEX();
    flt_bfr_error();
    end_state();
}

#if NO_LEAKS
static void
free_filter(void)
{
    USE_LEXFREE;
}
#endif
