
%option noyywrap
%option stack

%{
	#include <stdbool.h> 
	#include <stdlib.h>

	#include "C_str.h"
    #include "C_Attribute.h"
	#include "C_Attributes.h"
	#include "TagType.h"
	#include "y.tab.h"

	int g_line = 1;
	int commentStartLine;
	int cdataStartLine;
	int quotedStartLine;
	bool rootIsScanned = false;

	char Input();
	void Unput(char c);
#ifndef unix
	#define fileno _fileno
#endif
%}

%x PROLOG
%x BODY
%x DOUBLE_QUOTED
%x SINGLE_QUOTED
%x OPEN_TAG
%x CLOSE_TAG
%x EXPECTING_ROOT
%x EXPECTING_NAME
%x COMMENT
%x CDATA
%x DOCTYPE


digit				[0-9]
float				{digit}+\.{digit}+
whiteSpace			[ \t\r\n]
name				[a-zA-Z_]+[:a-zA-Z_0-9\.\-]*
escapedCharacter	(&amp;|&lt;|&gt;|&quot;|&apos;) 
singleQuotedText	({escapedCharacter}|[^<&'])*
doubleQuotedText	({escapedCharacter}|[^<&"])*
text				(&amp;|&lt;|&gt;|&quot;|&apos;|[^<& \t\r\n])*
commentOpenTag		\<!\-\-	
cdataOpenTag		\<\!\[CDATA\[	
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
							PrintError("[E,%d]: Trash data in the beggining of the file.\n", trashStartLine);
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
							rootIsScanned = true;
							yy_push_state( PROLOG );
							yy_push_state( EXPECTING_NAME );
							return K_PROLOG_OPEN_BR;
						}
						else
						{	
							Unput( c );
							Unput( '<' );
							yy_push_state( EXPECTING_ROOT );
						}
					}


<EXPECTING_ROOT>\<		{
							yy_push_state( BODY );
							yy_push_state( OPEN_TAG );
							yy_push_state( EXPECTING_NAME );
							return K_ELEM_OPEN_TAG_OPEN_BRACKET;
						}
<EXPECTING_ROOT><<EOF>> {
							if(!rootIsScanned)
							{
								PrintError("[E]: File is empty.\n");
								Exit(-1);
							}
						}
<EXPECTING_ROOT>\<\!DOCTYPE	{	
								yy_push_state( DOCTYPE );
								return K_DOCTYPE_START;
							}

<PROLOG>\?\>		{
						yy_push_state( EXPECTING_ROOT );
						return K_PROLOG_CLOSE_BR;
					}

<DOUBLE_QUOTED>\"						{
											yy_pop_state();	
											quotedStartLine = g_line;
											return *yytext;
										}
<DOUBLE_QUOTED><<EOF>>					{
											PrintError("[E,%d]: Not closed double quoted section detected.\n", quotedStartLine);
											Exit(-1);
										}
<DOUBLE_QUOTED>{doubleQuotedText}		{
											str_Init( & yylval.type_C_str, yytext);
											yylval.type_C_str.line = g_line;
											return ATTRIBUTE_VALUE;
										}

<SINGLE_QUOTED>\'						{
											yy_pop_state();	
											return *yytext;
										}
<SINGLE_QUOTED>{singleQuotedText}		{
											str_Init( & yylval.type_C_str, yytext);
											yylval.type_C_str.line = g_line;
											return ATTRIBUTE_VALUE;
										}
<SINGLE_QUOTED><<EOF>>					{
											PrintError("[E,%d]: Not closed single quoted section detected.\n", quotedStartLine);
											Exit(-1);
										}
<BODY>{cdataOpenTag}			{
									cdataStartLine = g_line;
									yy_push_state( CDATA );
								}
<BODY,OPEN_TAG,CLOSE_TAG>\<		{
									yy_push_state( BODY );
									yy_push_state( OPEN_TAG );
									yy_push_state( EXPECTING_NAME );
									return K_ELEM_OPEN_TAG_OPEN_BRACKET;
								}
<BODY,EXPECTING_ROOT,OPEN_TAG,CLOSE_TAG>\<\/		{
														yy_push_state( CLOSE_TAG );
														yy_push_state( EXPECTING_NAME );
														return K_ELEM_CLOSE_TAG_OPEN_BRACKET;							
													}
<BODY>{text}					{
									return ELEMENT_TEXT;
								}
<BODY>&							{
									PrintError("[E,%d]: \"&\" is not escaped or is part of not existing escape sequence.\n", g_line);
								}

<CDATA>[^\]]			{;}	
<CDATA>\]\]				{;}
<CDATA>\]				{;}
<CDATA><<EOF>>			{
							PrintError("[E,%d]: Not closed CDATA section detected.\n", cdataStartLine);
							Exit(-1);
						}
<CDATA>\]\]\>			{
							yy_pop_state();
						}

<OPEN_TAG>\/\>			{
							yy_pop_state();	
							return K_ELEM_SELF_CLOSING_TAG;
						}

<COMMENT>\-\-			{
							PrintError("[E,%d]: The string \"--\" is not permitted within comments.\n", g_line);
							Unput('-');
						}
<COMMENT><<EOF>>		{	
							PrintError("[E,%d]: Not closed comment detected.\n", commentStartLine);
							Exit(-1);
						}
<COMMENT>\-\-\>			{
							yy_pop_state();
						}
<COMMENT>[^\-]			{;}
<COMMENT>\-				{;}

<EXPECTING_NAME>{name}		{
								str_Init( & yylval.type_C_str, yytext);
								yylval.type_C_str.line = g_line;
								yy_pop_state();

								return NAME;
							}
<EXPECTING_NAME>.			{
								PrintError("[E,%d]: Invalid symbol \"%c\" for start of the name.\n", g_line, *yytext);
							}

<EXPECTING_ROOT,BODY>{commentOpenTag}			{
													commentStartLine = g_line;
													yy_push_state( COMMENT );
												}

<OPEN_TAG,CLOSE_TAG,DOCTYPE>\>			{
									yy_pop_state();	
									return K_ELEM_TAG_CLOSE_BRACKET;
								}

<OPEN_TAG,CLOSE_TAG,PROLOG>:		{ return *yytext; }

<PROLOG,OPEN_TAG,CLOSE_TAG,DOCTYPE>{name}		{
													str_Init( & yylval.type_C_str, yytext);
													yylval.type_C_str.line = g_line;
													return NAME;
												}
<PROLOG,OPEN_TAG,CLOSE_TAG>[=]			{ return *yytext; }
<PROLOG,OPEN_TAG,CLOSE_TAG>\"			{
											yy_push_state( DOUBLE_QUOTED );	
											return *yytext;
										}
<PROLOG,OPEN_TAG,CLOSE_TAG>\'			{
											yy_push_state( SINGLE_QUOTED );	
											return *yytext;
										}

<CDATA,EXPECTING_NAME,EXPECTING_ROOT,COMMENT,PROLOG,OPEN_TAG,CLOSE_TAG,BODY,DOCTYPE>{whiteSpace}+		{
																											g_line += CountChar(yytext, '\n');
																										}

<PROLOG,DOUBLE_QUOTED,SINGLE_QUOTED,OPEN_TAG,CLOSE_TAG,EXPECTING_NAME,DOCTYPE>{commentOpenTag}		{
																								commentStartLine = g_line;
																								PrintError("[E,%d]: Comments are allowed only inside of element body or just after prologue.\n", g_line);
																								yy_push_state( COMMENT );
																							}
<INITIAL,EXPECTING_ROOT,PROLOG,DOUBLE_QUOTED,SINGLE_QUOTED,OPEN_TAG,CLOSE_TAG,EXPECTING_NAME,DOCTYPE>{cdataOpenTag}		{
																													cdataStartLine = g_line;
																													PrintError("[E,%d]: CDATA sections are allowed only inside of element body.\n", g_line);
																													yy_push_state( CDATA );
																												}

<PROLOG,DOCTYPE,EXPECTING_NAME,BODY,DOUBLE_QUOTED,SINGLE_QUOTED,OPEN_TAG,CLOSE_TAG,EXPECTING_ROOT,COMMENT,CDATA>.	{
																														PrintError("[E,%d]: Wrong place for symbol \"%c\".\n", g_line, *yytext);
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