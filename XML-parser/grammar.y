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

    // "K" means Keyword.
    %token <type_cstr>     K_PROLOG_OPEN_BR
    %token <type_cstr>     K_PROLOG_CLOSE_BR

    %type <type_Attribute> attribute

%% 
    xml_file:   xml_prolog
            |   ' ' xml_prolog
            |   xml_file ' '
    ;

    xml_prolog: K_PROLOG_OPEN_BR _ NAME _ attribute _ attribute _ K_PROLOG_CLOSE_BR
                {
                    ValidateXmlProlog($3, $5, $7);
                }
    ;

    attribute: NAME _ '=' _ "'" ATTRIBUTE_VALUE "'"
               {
                $$.name = $1;
                $$.value = $6;
               }
             | NAME _ '=' _ '"' ATTRIBUTE_VALUE '"'
               {
                $$.name = $1;
                $$.value = $6;
               }
    ;

    // White space
    _:         /* empty */
            |   ' '
    ;
   
%% 

