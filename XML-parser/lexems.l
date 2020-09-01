
%option noyywrap
%option stack

%{
	#include <stdbool.h> 
	#include <stdlib.h>

	#include "C_Structs.h"
	#include "y.tab.h"

	#define fileno _fileno

	int g_line = 1;
	int prevState;

	char Input();
	void Unput(char c);
	void ReadComment();

%}

%x PROLOG
%x BODY
%x DOUBLE_QUOTED
%x SINGLE_QUOTED
%x OPEN_TAG
%x CLOSE_TAG

digit				[0-9]
float				{digit}+\.{digit}+
whiteSpace			[ \t\r\n]
name				[a-zA-Z_]+[a-zA-Z_0-9\.-]*
escapedCharacter	(&amp;|&lt;|&gt;|&quot;|&apos;) 
singleQuotedText	({escapedCharacter}|[^<&'])*
doubleQuotedText	({escapedCharacter}|[^<&"])*
text (&amp;|&lt;|&gt;|&quot;|&apos;|[^<& \t\r\n])*
commentOpenTag		\<!\-\-	
%%

<INITIAL>[^\<]+		{
						bool isTrash = false;
						int trashStartLine;
						for(int i=0; yytext[i] != 0; i++)
						{
							if (yytext[i] == '\n')
								g_line++;
							else if (( isTrash == false  ) &&
										( yytext[i] != ' '  ) &&
										( yytext[i] != '\t' ) &&
										( yytext[i] != '\r' ))
							{
								isTrash = true;
								trashStartLine = g_line; 
							}
						}

						if(isTrash)
							PrintError("[E]: Trash data in the beggining of the file, starts at line %d.\n", trashStartLine);
					}	
<INITIAL>\<			{
						char c = Input();
						if( c == EOF)
						{
							PrintError("[E]: unexpected end of file\n");
							Exit(-1);
						}

						if(c == '?')
						{
							yy_push_state( PROLOG );
							return K_PROLOG_OPEN_BR;
						}
						else
						{
							yy_push_state( BODY );
							yy_push_state( OPEN_TAG );
							return K_ELEM_OPEN_TAG_OPEN_BRACKET;
						}
					}

<PROLOG>\?\>		{
						yy_push_state( BODY );
						return K_PROLOG_CLOSE_BR;
					}

<DOUBLE_QUOTED>\"						{
											yy_pop_state();	
											return *yytext;
										}
<DOUBLE_QUOTED>{doubleQuotedText}		{
											str_Init( & yylval.type_str, yytext);
											yylval.type_str.line = g_line;
											return ATTRIBUTE_VALUE;
										}

<SINGLE_QUOTED>\'						{
											yy_pop_state();	
											return *yytext;
										}
<SINGLE_QUOTED>{singleQuotedText}		{
											str_Init( & yylval.type_str, yytext);
											yylval.type_str.line = g_line;
											return ATTRIBUTE_VALUE;
										}	

<BODY>\<						{
									yy_push_state( OPEN_TAG );
									return K_ELEM_OPEN_TAG_OPEN_BRACKET;
								}
<BODY>\<\/						{
									yy_push_state( CLOSE_TAG );
									return K_ELEM_CLOSE_TAG_OPEN_BRACKET;							
								}
<BODY>{text}					{
									return ELEMENT_TEXT;
								}
<BODY>&							{
									PrintError("[E,%d]: \"&\" is not escaped or is part of not existing escape sequence.\n", g_line);
								}
<BODY>{commentOpenTag}			{
									ReadComment();
								}

<OPEN_TAG>\/\>			{
							yy_pop_state();	
							return K_ELEM_SELF_CLOSING_TAG;
						}

<OPEN_TAG,CLOSE_TAG>\>			{
									yy_pop_state();	
									return K_ELEM_TAG_CLOSE_BRACKET;
								}

<OPEN_TAG,CLOSE_TAG,PROLOG>:		{
										return *yytext;
									}

<PROLOG,OPEN_TAG,CLOSE_TAG>{name}		{
											str_Init( & yylval.type_str, yytext);
											yylval.type_str.line = g_line;
											return NAME;
										}
<PROLOG,OPEN_TAG,CLOSE_TAG>[=]			{
											return *yytext;
										}
<PROLOG,OPEN_TAG,CLOSE_TAG>\"			{
											yy_push_state( DOUBLE_QUOTED );	
											return *yytext;
										}
<PROLOG,OPEN_TAG,CLOSE_TAG>\'			{
											yy_push_state( SINGLE_QUOTED );	
											return *yytext;
										}

<PROLOG,OPEN_TAG,CLOSE_TAG,BODY>{whiteSpace}+		{
														g_line += CountChar(yytext, '\n');
														return ' ';
													}

<PROLOG,BODY,DOUBLE_QUOTED,SINGLE_QUOTED,OPEN_TAG,CLOSE_TAG>{commentOpenTag}		{
																						PrintError("[E,%d]: Comments are allowed only inside of element body.\n", g_line);
																						ReadComment();
																					}

%%

char Input()
{
	char c = input();
	if(c == '\n')
	{
		g_line++;
	}

	return c;
}

void Unput(char c)
{
	if(c == '\n')
	{
		g_line--;
	}

	unput( c );
}

void ReadComment()
{
	int commentStartLine = g_line;
										
	char c1 = 0, c2 = 0, c3 = 0;
	char c;

	while(1)
	{
		c = Input();
		if(c == EOF)
		{
			PrintError("[E,%d]: Not closed comment detected.\n", commentStartLine);
			return EOF;
		}

		c3 = c2;
		c2 = c1;
		c1 = c;

		if( c1 == '>' &&
			c2 == '-' && 
			c3 == '-' )
		{
			break;
		}
	}
}