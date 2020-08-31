
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

	//int CountChar(char* str, char c);
	//bool IsWhiteSpace(char* str);
	//void Exit(int exitCode);

	//void str_Init(str *, const char *);

%}

%x PROLOG
%x BODY
%x DOUBLE_QUOTED
%x SINGLE_QUOTED

prologOpenBr	(\<\?)
prologCloseBr	(\?\>)

digit				[0-9]
float				{digit}+\.{digit}+
whiteSpace			[ \t\r\n]
name				[a-zA-Z_]+[a-zA-Z_0-9\.-]*
escapedCharacter	(&amp;|&lt;|&gt;|&quot;|&apos;) 
singleQuotedText	({escapedCharacter}|[^<&'])*
doubleQuotedText	({escapedCharacter}|[^<&"])*

%%

<INITIAL>[^\<]+					{
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
										printf("[E]: Trash data in the beggining of the file, starts at line %d.\n", trashStartLine);
								}	
<INITIAL>\<						{
									char c = Input();
									if( c == EOF)
									{
										printf("[E]: unexpected end of file\n");
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
									}
								}

<PROLOG>\?\>					{
									return K_PROLOG_CLOSE_BR;
								}
<PROLOG>{whiteSpace}+	{
									g_line += CountChar(yytext, '\n');
									return ' ';
								}
<PROLOG>{name}					{
									
									str_Init( & yylval.type_str, yytext);
									yylval.type_str.line = g_line;
									return NAME;
								}
<PROLOG>[=]						{
									return *yytext;
								}
<PROLOG>\"						{
									yy_push_state( DOUBLE_QUOTED );	
									return *yytext;
								}
<PROLOG>\"						{
									yy_push_state( DOUBLE_QUOTED );	
									return *yytext;
								}
<PROLOG>\'						{
									yy_push_state( SINGLE_QUOTED );	
									return *yytext;
								}

<DOUBLE_QUOTED>\"					{
										yy_pop_state();	
										return *yytext;
									}
<DOUBLE_QUOTED>{doubleQuotedText}	{
										str_Init( & yylval.type_str, yytext);
										yylval.type_str.line = g_line;
										return ATTRIBUTE_VALUE;
									}

<SINGLE_QUOTED>\'					{
										yy_pop_state();	
										return *yytext;
									}
<SINGLE_QUOTED>{singleQuotedText}	{
										str_Init( & yylval.type_str, yytext);
										yylval.type_str.line = g_line;
										return ATTRIBUTE_VALUE;
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