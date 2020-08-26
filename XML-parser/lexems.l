%option noyywrap
%{

	#include <stdlib.h>
	
	#include "y.tab.h"

	#define fileno _fileno
%}

%x TAG_OPENED
%s TAG_CLOSED

digit [0-9]

float {digit}+\.{digit}+

white_space [ \t\r]+

name [a-zA-Z_]+[a-zA-Z_0-9\.-]*

text (&amp;|&lt;|&gt;|&quot;|&apos;|[^<&])*

%%

{white_space} ;

<TAG_OPENED>\/	{ return *yytext;}

<TAG_OPENED>\>	{ 
					BEGIN(TAG_CLOSED); 
					return *yytext; 
				}

<TAG_OPENED>{name}  { 
						//printf("\n[D,L]: ELEMENT_NAME = \"%s\".\n", yytext);
						yylval.type_cstr = (char *)yytext;
						return ELEMENT_NAME; 
					}

<TAG_CLOSED>{text}  { 
						//printf("\n[D,L]: ELEMENT_TEXT = \"%s\".\n", yytext);
						yylval.type_cstr = yytext;
						return ELEMENT_TEXT; 
					}

\<			    { 
					BEGIN(TAG_OPENED); 
					return *yytext; 
				}



. { return *yytext;}

%%

