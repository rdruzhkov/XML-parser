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
    xml_file:   element _
            |   xml_prolog element _
    ;

    xml_prolog: _ K_PROLOG_OPEN_BR _ NAME attribute attribute _ K_PROLOG_CLOSE_BR
                {
                    ValidateXmlProlog($4, $5, $6);
                }
    ;

    element:    self_closing_tag
           |    open_tag close_tag
           |    open_tag element_body close_tag    
    ;

    element_body:   _ ELEMENT_TEXT
                |   element
                |   element_body _ ELEMENT_TEXT
                |   element_body element 
    ;

    self_closing_tag:   _ K_ELEM_OPEN_TAG_OPEN_BRACKET NAME _ K_ELEM_SELF_CLOSING_TAG
                    |   _ K_ELEM_OPEN_TAG_OPEN_BRACKET NAME attributes _ K_ELEM_SELF_CLOSING_TAG
    ;

    open_tag:   _ K_ELEM_OPEN_TAG_OPEN_BRACKET NAME _ K_ELEM_TAG_CLOSE_BRACKET
            |   _ K_ELEM_OPEN_TAG_OPEN_BRACKET NAME attributes _ K_ELEM_TAG_CLOSE_BRACKET
    ;

    close_tag:  _ K_ELEM_CLOSE_TAG_OPEN_BRACKET NAME _ K_ELEM_TAG_CLOSE_BRACKET
    ;

    attributes: attribute
              | attributes attribute
    ;

    attribute: _ NAME _ '=' _ '\'' ATTRIBUTE_VALUE '\''
               {
                $$.name = $2;
                $$.value = $7;
               }
             | _ NAME _ '=' _ '"' ATTRIBUTE_VALUE '"'
               {
                $$.name = $2;
                $$.value = $7;
               }
    ;

    // White space
    _:         /* empty */
            |   spaces
    ;

    spaces: ' '
          | spaces ' '
    ;
   
%% 

