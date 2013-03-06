%{

#include "admsPreprocessor.h"

#define YYDEBUG 1
#define KS(s) adms_k2strconcat(&message,s);
#define KI(i) adms_k2strconcat(&message,adms_integertostring(i));
#define K0 KS("[") KS(pproot()->cr_scanner->filename) KS(":") \
  KI(adms_preprocessor_get_line_position(pproot()->cr_scanner,0)) KS("]: ")
#define DONT_SKIPP (pproot()->skipp_text->data==INT2ADMS(0))

p_slist continuatorList=NULL;
p_slist condistrue=NULL;

%}

%union{
  p_slist slist;
  char* mystr;
}

%token <mystr> TK_PRAGMA_NAME
%token <mystr> TK_IDENT
%token <mystr> TK_STRING
%token <mystr> TK_NOT_IDENT
%token <mystr> TK_ARG
%token <mystr> TK_ARG_NULL
%token <mystr> TK_SUBSTITUTOR_NOARG
%token <mystr> TK_SUBSTITUTOR_NULLARG
%token <mystr> TK_SUBSTITUTOR_NULLARG_ALONE
%token <mystr> TK_SUBSTITUTOR_WITHARG
%token <mystr> TK_SUBSTITUTOR_WITHARG_ALONE
%token <mystr> TK_CONTINUATOR
%token <mystr> TK_NOPRAGMA_CONTINUATOR
%token <mystr> TK_EOL
%token <mystr> TK_EOF
%token <mystr> TK_COMMENT
%token <mystr> TK_INCLUDE
%token <mystr> TK_SPACE

%token <mystr> TK_ERROR_PRAGMA_DEFINITION
%token <mystr> TK_ERROR_PRAGMA_NOT_FOUND
%token <mystr> TK_ERROR_UNEXPECTED_CHAR
%token <mystr> TK_ERROR_FILE_OPEN

%token TK_DEFINE
%token TK_DEFINE_END
%token TK_UNDEF
%token <mystr> TK_IFDEF
%token <mystr> TK_IFNDEF
%token <mystr> TK_ELSE
%token <mystr> TK_ENDIF

%type <slist> R_description
%type <slist> R_list_of_conditional
%type <slist> R_conditional
%type <slist> R_alternative
%type <slist> R_define_alternative

%type <slist> R_define_list_of_arg
%type <slist> R_pragma

%type <slist> R_define_text

%type <slist> R_define_notpragma
%type <slist> R_notpragma
%type <slist> R_arg_null
%type <slist> R_substitutor
%type <slist> R_substitutor_list_of_arg
%type <slist> R_list_of_arg
%type <slist> R_list_of_arg_with_comma
%type <slist> R_arg
%type <slist> R_other

%type <slist> R_include

%type <slist> R_if
%type <slist> R_ifn
%type <slist> R_else
%type <slist> R_endif

%type <mystr> R_substitutor_nullarg
%type <mystr> R_substitutor_witharg

%type <mystr> R_ifdef
%type <mystr> R_ifndef
%type <mystr> R_undef
%type <mystr> R_define

%start R_description

%%

R_description
        : R_list_of_conditional
          {
            pproot()->Text=$1;
          }
        ;

R_list_of_conditional
        : R_conditional
          {
            $$=$1;
          }
        | R_list_of_conditional R_conditional
          {
            $$=$2;
            adms_slist_concat(&($$),$1);
          }
        ;

R_conditional
        : R_if R_ifdef R_list_of_conditional R_else R_list_of_conditional R_endif
          {
            if(condistrue->data==INT2ADMS(1))
            {
              $$=$6;
              adms_slist_concat(&($$),$3);
            }
            else if(condistrue->data==INT2ADMS(0))
            {
              $$=$5;
              adms_slist_concat(&($$),$4);
            }
            adms_slist_pull(&pproot()->skipp_text);
            adms_slist_pull(&condistrue);
          }
        | R_if R_ifdef R_list_of_conditional R_endif
          {
            if(condistrue->data==INT2ADMS(1))
            {
              $$=$3;
              adms_slist_concat(&($$),$1);
            }
            else if(condistrue->data==INT2ADMS(0))
              $$=$4;
            adms_slist_pull(&pproot()->skipp_text);
            adms_slist_pull(&condistrue);
          }
        | R_ifn R_ifndef R_list_of_conditional R_else R_list_of_conditional R_endif
          {
            if(condistrue->data==INT2ADMS(1))
            {
               $$=$6;
               adms_slist_concat(&($$),$3);
            }
            else if(condistrue->data==INT2ADMS(0))
            {
              $$=$5;
              adms_slist_concat(&($$),$4);
            }
            adms_slist_pull(&pproot()->skipp_text);
            adms_slist_pull(&condistrue);
          }
        | R_ifn R_ifndef R_list_of_conditional R_endif
          {
            if(condistrue->data==INT2ADMS(1))
            {
              $$=$3;
              adms_slist_concat(&($$),$1);
            }
            else if(condistrue->data==INT2ADMS(0))
              $$=$4;
            adms_slist_pull(&pproot()->skipp_text);
            adms_slist_pull(&condistrue);
          }
        | R_alternative
          {
            $$=$1;
          }
        ;

R_if
        : TK_IFDEF
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        ;
R_ifn
        : TK_IFNDEF
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        ;
R_ifdef
        : TK_PRAGMA_NAME
          {
            $$=$1;
            if(!DONT_SKIPP)
            {
              adms_slist_push(&pproot()->skipp_text,INT2ADMS(1));
              adms_slist_push(&condistrue,INT2ADMS(-1));
            }
            else if(adms_preprocessor_identifier_is_def($1))
            {
              adms_slist_push(&condistrue,INT2ADMS(1));
              adms_slist_push(&pproot()->skipp_text,INT2ADMS(0));
            }
            else
            {
              adms_slist_push(&condistrue,INT2ADMS(0));
              adms_slist_push(&pproot()->skipp_text,INT2ADMS(1));
            }
          }
        ;
R_ifndef
        : TK_PRAGMA_NAME
          {
            $$=$1;
            if(!DONT_SKIPP)
            {
              adms_slist_push(&pproot()->skipp_text,INT2ADMS(1));
              adms_slist_push(&condistrue,INT2ADMS(-1));
            }
            else if(adms_preprocessor_identifier_is_ndef($1))
            {
              adms_slist_push(&condistrue,INT2ADMS(1));
              adms_slist_push(&pproot()->skipp_text,INT2ADMS(0));
            }
            else
            {
              adms_slist_push(&condistrue,INT2ADMS(0));
              adms_slist_push(&pproot()->skipp_text,INT2ADMS(1));
            }
          }
        ;

R_else
        : TK_ELSE
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
            if(condistrue->data==INT2ADMS(0))
              pproot()->skipp_text->data=INT2ADMS(0);
            else if(condistrue->data==INT2ADMS(1))
              pproot()->skipp_text->data=INT2ADMS(1);
            else
              pproot()->skipp_text->data=INT2ADMS(1);
          }
        ;

R_endif
        : TK_ENDIF
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        ;

R_include
        : TK_INCLUDE
          {
            p_preprocessor_text newtext;
            newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        ;

R_undef
        : TK_UNDEF TK_PRAGMA_NAME
          {
            $$=$2;
          }
        ;

R_alternative
        : R_pragma
          {
            $$=$1;
          }
        | R_notpragma
          {
            $$=$1;
          }
        ;

R_pragma
        : R_include
          {
            $$=$1;
          }
        | R_define_alternative
          {
            $$=$1;
          }
        | R_undef
          {
            $$=NULL;
            if(DONT_SKIPP) adms_preprocessor_identifer_set_undef($1);
          }
        | TK_ERROR_UNEXPECTED_CHAR
          {
            $$=NULL;
          }
        | TK_ERROR_FILE_OPEN
          {
            char*message=NULL;
            $$=NULL;
            K0 KS(pproot()->cr_scanner->cur_message) KS("\n") 
            adms_preprocessor_add_message(message);
            free(pproot()->cr_scanner->cur_message);
            pproot()->cr_scanner->cur_message=NULL;
          }
        | TK_ERROR_PRAGMA_DEFINITION
          {
            char*message=NULL;
            $$=NULL;
            K0 KS("macro ") KS(pproot()->cr_scanner->cur_message) KS(" badly formed\n")
            adms_preprocessor_add_message(message);
            pproot()->error += 1;
            free(pproot()->cr_scanner->cur_message);
            pproot()->cr_scanner->cur_message=NULL;
          }
       ;

R_notpragma
	: R_substitutor
          {
            $$=$1;
          }
        | TK_NOPRAGMA_CONTINUATOR
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string("\n");
            adms_slist_push(&continuatorList,(p_adms)newtext);
            $$=NULL;
          }
        | TK_EOL
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string("\n");
            $$=adms_slist_new((p_adms)newtext);
            adms_slist_concat(&($$),continuatorList);
            continuatorList=NULL;
            ++pproot()->cr_scanner->cur_line_position;
            pproot()->cr_scanner->cur_char_position=1;
            pproot()->cr_scanner->cur_continuator_position=NULL;
          }
	| R_other
          {
            $$=$1;
          }
        | TK_ERROR_PRAGMA_NOT_FOUND
          {
            char*message=NULL;
            $$=NULL;
            K0 KS("macro ") KS(pproot()->cr_scanner->cur_message) KS(" is undefined\n")
            adms_preprocessor_add_message(message);
            pproot()->error += 1;
            free(pproot()->cr_scanner->cur_message);
            pproot()->cr_scanner->cur_message=NULL;
          }
	;

R_define_notpragma
	: R_substitutor
          {
            $$=$1;
          }
        | TK_CONTINUATOR
          {
            p_preprocessor_text newtext1=adms_preprocessor_new_text_as_string("\n");
            p_preprocessor_text newtext2=adms_preprocessor_new_text_as_string("");
            adms_slist_push(&continuatorList,(p_adms)newtext1);
            $$=adms_slist_new((p_adms)newtext2);
          }
        | TK_EOL
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
            adms_slist_concat(&($$),continuatorList);
            continuatorList=NULL;
            ++pproot()->cr_scanner->cur_line_position;
            pproot()->cr_scanner->cur_char_position=1;
            pproot()->cr_scanner->cur_continuator_position=NULL;
          }
	| R_other
          {
            $$=$1;
          }
        | TK_ERROR_PRAGMA_NOT_FOUND
          {
            char*message=NULL;
            $$=NULL;
            K0 KS("macro ") KS(pproot()->cr_scanner->cur_message) KS(" is undefined\n")
            adms_preprocessor_add_message(message);
            pproot()->error += 1;
            free(pproot()->cr_scanner->cur_message);
            pproot()->cr_scanner->cur_message=NULL;
          }
	;

R_substitutor
        : TK_SUBSTITUTOR_NOARG
          {
            p_preprocessor_pragma_define Define=adms_preprocessor_pragma_define_exists($1);
            $$=adms_preprocessor_new_text_as_substitutor(Define,NULL);
          }
        | TK_SUBSTITUTOR_NULLARG_ALONE
          {
            p_preprocessor_pragma_define Define=adms_preprocessor_pragma_define_exists($1);
            $$=adms_preprocessor_new_text_as_substitutor(Define,NULL);
          }
        | R_substitutor_nullarg R_arg_null
          {
            p_preprocessor_pragma_define Define=adms_preprocessor_pragma_define_exists($1);
            $$=adms_preprocessor_new_text_as_substitutor(Define,NULL);
          }
        | R_substitutor_nullarg '(' R_substitutor_list_of_arg ')'
          {
            char*message=NULL;
            p_preprocessor_pragma_define Define=adms_preprocessor_pragma_define_exists($1);
            $$=adms_preprocessor_new_text_as_substitutor(Define, $3);
            K0 KS("arguments given to macro `") KS( Define->name) KS("\n")
            adms_preprocessor_add_message(message);
            pproot()->error += 1;
          }
        | TK_SUBSTITUTOR_WITHARG_ALONE
          {
            char*message=NULL;
            p_preprocessor_pragma_define Define=adms_preprocessor_pragma_define_exists($1);
            $$=adms_preprocessor_new_text_as_substitutor(Define,NULL);
            K0 KS("macro `") KS(Define->name) KS(" has no argument [") KI(adms_slist_length(Define->arg)) KS(" expected]\n")
            adms_preprocessor_add_message(message);
            pproot()->error += 1;
            adms_slist_push(&($$),(p_adms)$1);
          }
        | R_substitutor_witharg R_arg_null
          {
            char*message=NULL;
            p_preprocessor_pragma_define Define=adms_preprocessor_pragma_define_exists($1);
            $$=adms_preprocessor_new_text_as_substitutor(Define,NULL);
            K0 KS("macro `") KS(Define->name) KS(" has no argument [") KI(adms_slist_length(Define->arg)) KS(" expected]\n")
            adms_preprocessor_add_message(message);
            pproot()->error += 1;
          }
        | R_substitutor_witharg '(' R_substitutor_list_of_arg ')'
          {
            p_preprocessor_pragma_define Define=adms_preprocessor_pragma_define_exists($1);
            {
              if(adms_slist_length($3) == adms_slist_length(Define->arg))
              {
              }
              else if(adms_slist_length($3) > adms_slist_length(Define->arg))
              {
                if(adms_slist_length($3) == 1)
                {
                  char*message=NULL;
                  K0 KS("macro `") KS(Define->name) KS(" has one argument [") KI(adms_slist_length(Define->arg)) KS(" expected]\n")
                  adms_preprocessor_add_message(message);
                }
                else
                {
                  char*message=NULL;
                  K0 KS("macro `") KS(Define->name) KS(" has too many (") KI(adms_slist_length($3)) KS(") arguments\n") 
                  adms_preprocessor_add_message(message);
                }
                pproot()->error += 1;
              }
              else
              {
                if(adms_slist_length($3) == 1)
                {
                  char*message=NULL;
                  K0 KS("macro `") KS(Define->name) KS(" has one argument [") KI(adms_slist_length(Define->arg)) KS(" expected]\n")
                  adms_preprocessor_add_message(message);
                }
                else
                {
                  char*message=NULL;
                  K0 KS("macro `") KS(Define->name) KS(" has too few (") KI(adms_slist_length($3)) KS(") arguments\n")
                  adms_preprocessor_add_message(message);
                }
                pproot()->error += 1;
              }
            }
            $$=adms_preprocessor_new_text_as_substitutor(Define, $3);
          }
       ;

R_substitutor_nullarg
	: TK_SUBSTITUTOR_NULLARG TK_SPACE
          {
            $$=$1;
          }
	| TK_SUBSTITUTOR_NULLARG
          {
            $$=$1;
          }
	;

R_substitutor_witharg
	: TK_SUBSTITUTOR_WITHARG TK_SPACE
          {
            $$=$1;
          }
	| TK_SUBSTITUTOR_WITHARG
          {
            $$=$1;
          }
	;

R_arg_null
	: '(' ')'
          {
          }
	;

R_substitutor_list_of_arg
	: R_list_of_arg
          {
            $$=adms_slist_new((p_adms)$1);
          }
	| R_substitutor_list_of_arg ',' R_list_of_arg
          {
            adms_slist_push(&($1),(p_adms)$3);
            $$=$1;
          }
	;

R_list_of_arg
	: R_arg
          {
            $$=$1;
          }
	| R_list_of_arg R_arg
          {
            $$=$2;
            adms_slist_concat(&($$),$1);
          }
	;

R_list_of_arg_with_comma
	: R_list_of_arg
          {
            $$=$1;
          }
	| R_list_of_arg_with_comma ',' R_list_of_arg
          {
            p_preprocessor_text comma=adms_preprocessor_new_text_as_string(",");
            adms_slist_push(&($1),(p_adms)comma);
            $$=$3;
            adms_slist_concat(&($$),$1);
          }
	;
R_arg
        : TK_SPACE
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_COMMENT
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_EOL
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string("\n");
            ++pproot()->cr_scanner->cur_line_position;
            pproot()->cr_scanner->cur_char_position=1;
            pproot()->cr_scanner->cur_continuator_position=NULL;
            $$=adms_slist_new((p_adms)newtext);
          }
	| '(' R_list_of_arg_with_comma ')'
          {
            p_preprocessor_text lparen=adms_preprocessor_new_text_as_string("(");
            p_preprocessor_text rparen=adms_preprocessor_new_text_as_string(")");
            $$=$2;
            adms_slist_concat(&($$),adms_slist_new((p_adms)lparen));
            adms_slist_push(&($$),(p_adms)rparen);
          }
	| '(' ')'
          {
            p_preprocessor_text lparen=adms_preprocessor_new_text_as_string("(");
            p_preprocessor_text rparen=adms_preprocessor_new_text_as_string(")");
            $$=adms_slist_new((p_adms)lparen);
            adms_slist_push(&($$),(p_adms)rparen);
          }
        | TK_IDENT
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_STRING
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_NOT_IDENT
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
	| R_substitutor
          {
            $$=$1;
          }
	;

R_other
       	: '('
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string("(");
            $$=adms_slist_new((p_adms)newtext);
          }
	| ')'
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string(")");
            $$=adms_slist_new((p_adms)newtext);
          }
	| ','
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string(",");
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_IDENT
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_NOT_IDENT
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_STRING
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_SPACE
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_COMMENT
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
        | TK_EOF
          {
            p_preprocessor_text newtext=adms_preprocessor_new_text_as_string($1);
            $$=adms_slist_new((p_adms)newtext);
          }
       ;

R_define_alternative
        : R_define TK_DEFINE_END
          {
            p_preprocessor_pragma_define Define;
            if(DONT_SKIPP) Define=adms_preprocessor_define_add($1);
            $$=NULL;
          }
        | R_define R_define_text TK_DEFINE_END
          {
            p_preprocessor_pragma_define Define;
            if(DONT_SKIPP) Define=adms_preprocessor_define_add_with_text($1, $2);
            $$=NULL;
          }
        | R_define TK_ARG_NULL TK_DEFINE_END
          {
            p_preprocessor_pragma_define Define;
            if(DONT_SKIPP) Define=adms_preprocessor_define_add_with_arg($1, NULL);
            $$=NULL;
          }
        | R_define TK_ARG_NULL R_define_text TK_DEFINE_END
          {
            p_preprocessor_pragma_define Define;
            if(DONT_SKIPP) Define=adms_preprocessor_define_add_with_arg_and_text($1, NULL, $3);
            $$=NULL;
          }
        | R_define R_define_list_of_arg TK_DEFINE_END
          {
            p_preprocessor_pragma_define Define;
            if(DONT_SKIPP) Define=adms_preprocessor_define_add_with_arg($1, $2);
            $$=NULL;
          }
        | R_define R_define_list_of_arg R_define_text TK_DEFINE_END 
          {
            p_preprocessor_pragma_define Define;
            if(DONT_SKIPP) Define=adms_preprocessor_define_add_with_arg_and_text($1, $2, $3);
            $$=NULL;
          }
        ;

R_define
        : TK_DEFINE TK_PRAGMA_NAME
          {
            $$=$2;
          }
        ;

R_define_list_of_arg
        : TK_ARG
          {
            $$=adms_slist_new((p_adms)$1);
          }
        | R_define_list_of_arg TK_ARG
          {
            adms_slist_push(&($1),(p_adms)$2);
            $$=$1;
          }
       ;

R_define_text
        : R_define_notpragma
          {
            $$=$1;
          }
        | R_define_text R_define_notpragma
          {
            $$=$2;
            adms_slist_concat(&($$),$1);
          }
       ;

%%

int adms_preprocessor_getint_yydebug(void)
  {
    return yydebug;
  }
void adms_preprocessor_setint_yydebug(const int val)
  {
    yydebug=val;
  }
