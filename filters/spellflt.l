%{

/*
 * $Header: /users/source/archives/vile.vcs/filters/RCS/spellflt.l,v 1.2 1999/04/17 14:21:20 tom Exp $
 *
 * Filter to add vile "attribution" sequences to misspelled words.
 */

#include <filters.h>

char *filter_name = "spell";

void
init_filter(int before GCC_UNUSED)
{
}

void
do_filter(FILE *input, FILE *output)
{
    FILE *fp;
    char *name = tmpnam((char *)0);
    char buffer[BUFSIZ];
    int ch;
    char *attr = class_attr(NAME_KEYWORD);

    if ((fp = fopen(name, "w")) == 0)
	return;
    while ((ch = fgetc(input)) != EOF && !feof(input))
	fputc(ch, fp);
    fclose(fp);

    sprintf(buffer, "ispell -x -l <%s", name);
    if ((fp = popen(buffer, "r")) != 0) {
	while (fgets(buffer, sizeof(buffer), fp)) {
	    unsigned len  = strlen(buffer);
	    while (len-- && isspace(buffer[len]))
		buffer[len] = 0;
	    if (*buffer)
		insert_keyword(buffer, attr, 0);
	}
	pclose(fp);
    }
    yyin = fopen(name, "r");
    yyout = output;

    while (yylex() > 0) {
    }

    remove(name);	/* works on Unix, to remove file before closing */
}

%}

ALPHA		[a-zA-Z\240-\377]
DIGIT		[0-9]
WORD		{ALPHA}({ALPHA}|{DIGIT})*

%%

{WORD}		{ WriteToken(keyword_attr(yytext)); }