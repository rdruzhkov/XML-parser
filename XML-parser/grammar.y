%{ 
    #include "C_str.h"
    #include "C_Attribute.h"
    #include "C_Attributes.h"
    #include "TagType.h"
    
#ifdef _DEBUG
    #define YYDEBUG 1
    yydebug = 1; 
#endif

    extern int g_line;
    
%} 
    
    %union { 
        char *              type_cstr;
        float               type_float;
        struct C_str          type_C_str;
        struct C_Attribute    type_C_Attribute;
        struct C_Attributes   type_C_Attributes;
    }

    %token <type_C_str>     ATTRIBUTE_VALUE
    %token <type_C_str>     NAME                 // Element or attribute name.
    %token <type_C_str>     ELEMENT_TEXT

    // "K" means Keyword.
    %token <type_cstr>     K_PROLOG_OPEN_BR     // "<?"
    %token <type_cstr>     K_PROLOG_CLOSE_BR    // "?>"

    %token <type_cstr>     K_ELEM_OPEN_TAG_OPEN_BRACKET     // "<"
    %token <type_cstr>     K_ELEM_CLOSE_TAG_OPEN_BRACKET    // "</"
    %token <type_cstr>     K_ELEM_TAG_CLOSE_BRACKET         // ">"
    %token <type_cstr>     K_ELEM_SELF_CLOSING_TAG          // "/>"


    %type <type_C_Attribute> attribute
    %type <type_C_Attribute> attributes

%% 
    xml_file:   element 
            |   xml_prolog element 
            |   element close_tags
            |   xml_prolog element close_tags
            |   close_tags
            |   element elements
            {
                PrintError("[E]: XML file can contain only one root element.\n");
            }
            |   xml_prolog element elements
            {
                PrintError("[E]: XML file can contain only one root element.\n");
            }
            |   element elements close_tags
            {
                PrintError("[E]: XML file can contain only one root element.\n");
            }
            |   xml_prolog element elements close_tags
            {
                PrintError("[E]: XML file can contain only one root element.\n");
            }
            | xml_prolog
            {
                PrintError("[E]: XML file must contain root element.\n");
            }
    ;

    xml_prolog: K_PROLOG_OPEN_BR NAME attributes K_PROLOG_CLOSE_BR
                {
                    ValidateProlog($2, & $3);

                    free_C_str(& $2);
                }
              | K_PROLOG_OPEN_BR NAME K_PROLOG_CLOSE_BR
                {
                    PrintError("[E]: XML prolog must contain attributes.\n");
                }
    ;

    element:    self_closing_tag
           |    open_tag close_tag
           |    open_tag element_body close_tag  
           |    open_tag error
                {
                    PrintError("[E] open_tag error.\n");
                }
           |    open_tag element_body error
                {
                    PrintError("[E] open_tag error.\n");
                }
    ;

    elements:   element
            |   elements element
    ;

    element_body:   ELEMENT_TEXT
                |   element
                |   element_body  ELEMENT_TEXT
                |   element_body element 
    ;

    self_closing_tag:    K_ELEM_OPEN_TAG_OPEN_BRACKET NAME K_ELEM_SELF_CLOSING_TAG
                         {
                            ValidateName($2);

                            free_C_str(& $2);
                         }
                    |    K_ELEM_OPEN_TAG_OPEN_BRACKET NAME attributes K_ELEM_SELF_CLOSING_TAG
                         {
                            ValidateName($2);
                            Validate_C_Attributes( & $3);

                            free_C_str(& $2);
                         }
    ;

    open_tag:   K_ELEM_OPEN_TAG_OPEN_BRACKET NAME K_ELEM_TAG_CLOSE_BRACKET
                {
                    ValidateName($2);
                    AddToTagList($2, Open);

                    free_C_str(& $2);
                }
            |   K_ELEM_OPEN_TAG_OPEN_BRACKET NAME attributes K_ELEM_TAG_CLOSE_BRACKET
                {
                    ValidateName($2);
                    Validate_C_Attributes( & $3);
                    AddToTagList($2, Open);

                    free_C_str(& $2);
                }
            |   K_ELEM_OPEN_TAG_OPEN_BRACKET NAME attributes error 
                {
                   ValidateName($2);
                   Validate_C_Attributes( & $3);
                   PrintError("[E,%d]: Open tag of element \"",$2.line);
                   PrintStr($2);
                   PrintError("\" doesn't have \">\".\n");

                   free_C_str(& $2);
                }
            |   K_ELEM_OPEN_TAG_OPEN_BRACKET NAME error 
                {
                   ValidateName($2);
                   PrintError("[E,%d]: Open tag of element \"",$2.line);
                   PrintStr($2);
                   PrintError("\" doesn't have \">\".\n");

                   free_C_str(& $2);
                }
            |   K_ELEM_OPEN_TAG_OPEN_BRACKET error NAME K_ELEM_TAG_CLOSE_BRACKET
                {
                    ValidateName($3);
                    AddToTagList($3, Open);

                    free_C_str(& $3);
                }
            |   K_ELEM_OPEN_TAG_OPEN_BRACKET error NAME attributes K_ELEM_TAG_CLOSE_BRACKET
                {
                    ValidateName($3);
                    Validate_C_Attributes( & $4);
                    AddToTagList($3, Open);

                    free_C_str(& $3);

                    PrintError("[E,%d]: Name \"", $3.line);
                    PrintStr($3);
                    PrintError("\" starts with invalid symbol sequence.\n");
                }
    ;

    close_tag:  K_ELEM_CLOSE_TAG_OPEN_BRACKET NAME K_ELEM_TAG_CLOSE_BRACKET
                {
                    ValidateName($2);
                    AddToTagList($2, Close);

                    free_C_str(& $2);
                }
             |  K_ELEM_CLOSE_TAG_OPEN_BRACKET NAME error 
                {
                   ValidateName($2);
                   PrintError("[E,%d]: Close tag of element \"",$2.line);
                   PrintStr($2);
                   PrintError("\" doesn't have \">\".\n");

                   free_C_str(& $2);
                }

    close_tags:  close_tag
              |  close_tags close_tag
    ;

    attributes: attribute
                {
                    Init_C_Atributes(& $$);
                    Add_C_Attribute(& $1, & $$);
                    free_C_str(& $1.name);
                    free_C_str(& $1.value);
                }
              | attributes attribute
                {
                    Add_C_Attribute(& $2, & $1);
                    free_C_str(& $2.name);
                    free_C_str(& $2.value);
                }
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

