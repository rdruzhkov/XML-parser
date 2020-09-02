%{ 
    #include "C_Structs.h"
    
#ifdef _DEBUG
    #define YYDEBUG 1
    yydebug = 1; 
#endif
    
%} 
    
    %union { 
        char *              type_cstr;
        float               type_float;
        struct str          type_str;
        struct Attribute    type_Attribute;
    }

    %token <type_str>     ATTRIBUTE_VALUE
    %token <type_str>     NAME                 // Element or attribute name.
    %token <type_str>     ELEMENT_TEXT

    // "K" means Keyword.
    %token <type_cstr>     K_PROLOG_OPEN_BR     // "<?"
    %token <type_cstr>     K_PROLOG_CLOSE_BR    // "?>"

    %token <type_cstr>     K_ELEM_OPEN_TAG_OPEN_BRACKET     // "<"
    %token <type_cstr>     K_ELEM_CLOSE_TAG_OPEN_BRACKET    // "</"
    %token <type_cstr>     K_ELEM_TAG_CLOSE_BRACKET         // ">"
    %token <type_cstr>     K_ELEM_SELF_CLOSING_TAG          // "/>"


    %type <type_Attribute> attribute

%% 
    xml_file:   element 
            |   xml_prolog element 
            |   element close_tags
            {
                PrintError("[E] element close_tags.\n");
            }
            |   xml_prolog element close_tags
            {
                PrintError("[E]  xml_prolog element close_tags.\n");
            }
            |   close_tags
            {
                PrintError("[E] close_tags.\n");
            }
    ;

    xml_prolog: K_PROLOG_OPEN_BR NAME attribute attribute K_PROLOG_CLOSE_BR
                {
                    ValidateXmlProlog($2, $3, $4);
                }
    ;

    element:    self_closing_tag
           |    open_tag close_tag
           |    open_tag element_body close_tag  
           |    open_tag error
                {
                    PrintError("[E] open_tag error.\n");
                    yyerrok;
                }
           |    open_tag element_body error
                {
                    PrintError("[E] open_tag error.\n");
                    yyerrok;
                }
    ;

    element_body:   ELEMENT_TEXT
                |   element
                |   element_body  ELEMENT_TEXT
                |   element_body element 
    ;

    self_closing_tag:    K_ELEM_OPEN_TAG_OPEN_BRACKET NAME K_ELEM_SELF_CLOSING_TAG
                    |    K_ELEM_OPEN_TAG_OPEN_BRACKET NAME attributes K_ELEM_SELF_CLOSING_TAG
    ;

    open_tag:    K_ELEM_OPEN_TAG_OPEN_BRACKET NAME K_ELEM_TAG_CLOSE_BRACKET
                {
                    AddToTagList($2, Open);
                }
            |    K_ELEM_OPEN_TAG_OPEN_BRACKET NAME attributes K_ELEM_TAG_CLOSE_BRACKET
                {
                    AddToTagList($2, Open);
                }
    ;

    close_tag:  K_ELEM_CLOSE_TAG_OPEN_BRACKET NAME K_ELEM_TAG_CLOSE_BRACKET
                {
                    AddToTagList($2, Close);
                }

    close_tags:  close_tag
              |  close_tags close_tag
    ;

    attributes: attribute
              | attributes attribute
    ;

    attribute:  NAME '=' '\'' ATTRIBUTE_VALUE '\''
               {
                $$.name = $1;
                $$.value = $4;
               }
             |  NAME '=' '"' ATTRIBUTE_VALUE '"'
               {
                $$.name = $1;
                $$.value = $4;
               }
    ;
 
%% 

