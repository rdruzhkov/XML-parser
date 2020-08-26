%{ 
    #define _CRT_SECURE_NO_WARNINGS

    #include <stdio.h>
    #include <string.h>
    
#ifdef _DEBUG
    #define YYDEBUG 1
    yydebug = 1; 
#endif

%} 

    %union {
        char *  type_cstr;
        float   type_float;
    }

    %token <type_cstr> CSTR
    %token <type_cstr> ELEMENT_NAME
    %token <type_cstr> XML_ENCODING
    %token <type_cstr> ELEMENT_TEXT
    %token <type_float> XML_VERSION

    %type <type_cstr> open_tag
    %type <type_cstr> close_tag

%% 
    xml_file:   root_element
            |   ' ' root_element
    ;

    root_element:   element
    ;

    element_body:   ELEMENT_TEXT
                |   element
                |   element_body ELEMENT_TEXT
                |   element_body element
    ;

    element:     open_tag close_tag
                 {
                    if(strcmp($1,$2))
                    {
                        printf("[E]: open_tag != close_tag\n");
                        YYERROR;  
                    }
                 }
           |     open_tag element_body close_tag    
                 {
                    if(strcmp($1,$3))
                    {
                        printf("[E]: open_tag != close_tag\n");
                        YYERROR;  
                    }
                 }
    ;                                                    

    open_tag:   '<' ELEMENT_NAME '>'
                {
                    $$ = malloc(strlen($2) + 1);
                    strcpy($$, $2);
                }
    ;

    close_tag:  '<' '/' ELEMENT_NAME '>'
                {
                    $$ = malloc(strlen($3) + 1);
                    strcpy($$, $3);
                }
    ;
   
%% 

