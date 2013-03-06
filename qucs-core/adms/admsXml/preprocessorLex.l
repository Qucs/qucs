%{

#include "adms.h"
#include "admsPreprocessor.h"
#include "preprocessorYacc.h"

#ifndef INITIAL
#  define INITIAL 0
#endif

#include "constants.vams.c"
#include "disciplines.vams.c"

#define yyflex_unput \
{\
  int i;\
  char *yycopy=strdup(yytext);\
  for ( i = yyleng - 1; i >= 0; --i )\
    unput(yycopy[i]);\
  free(yycopy);\
}

#define DONT_SKIPP (pproot()->skipp_text->data==INT2ADMS(0))

void adms_preprocessor_setfile_input (FILE *ifile)
{
  yyin = ifile;
}

FILE *adms_preprocessor_getfile_input (void)
{
  return yyin;
}

void adms_preprocessor_setfile_output (FILE *ifile)
{
  yyout = ifile;
}

FILE *adms_preprocessor_getfile_output (void)
{
  return yyout;
}

#define tkreturn(token) {pproot()->cr_scanner->cur_char_position+=yyleng; return token;}

static char* adms_preprocessor_lex_remove_leading_quote (char* name)
{
  char* unquoted_name;
  char* start;
  char* finish;
  start = name+1;
  finish = name + strlen(name);
  unquoted_name = adms_knclone (start, finish - start );
  return (unquoted_name);
}

static char* adms_preprocessor_lex_get_path (char* macro, const char begin, const char end)
{
  char* path;
  char* start=macro;
  char* finish;
  while( *start && (*start != begin) )
    start++;
  finish=++start;
  while( *finish && (*finish != end) )
    finish++;
  path=adms_knclone (start, finish - start );
  return path;
}

void preprocessorerror (const char *message)
{
  p_preprocessor cr_preprocessor=pproot()->cr_scanner;
  adms_message_fatal (("[%s:%i]:%s: %s\n",
    cr_preprocessor->filename,
    adms_preprocessor_get_line_position(cr_preprocessor,cr_preprocessor->cur_char_position-yyleng),
    message,
    cr_preprocessor->cur_message?cr_preprocessor->cur_message:"(unknown)"
  ))
}
static void adms_file_fprintf ( char *filename, const char *filedata)
{
  FILE *ofh = fopen (filename,"w");
  if (fprintf (ofh, filedata)==EOF)
    adms_message_fatal(("%s: cannot write file\n", filename))
  fclose (ofh);
}
static FILE* adms_file_open_read_with_path (const char* myfilename, p_slist mypath)
{
  p_slist l; for(l=mypath;l;l=l->next)
  {
    FILE* myfh=NULL;
    char* mypathname=strdup((char*)(l->data));
    adms_k2strconcat(&mypathname,ADMS_PATH_SEPARATOR);
    adms_k2strconcat(&mypathname,myfilename);
    if((myfh=fopen(mypathname,"r")))
      return myfh;
  }
  return NULL;
}
static void adms_preprocessor_lex_include_file (char* fileName)
{
  FILE*myfh;
  p_preprocessor scanner;
  char*message=NULL;
  adms_k2strconcat(&message,pproot()->cr_scanner->filename);
  adms_k2strconcat(&message,":");
  adms_strconcat(&message,adms_integertostring(adms_preprocessor_get_line_position (pproot()->cr_scanner, 0)));
  if(pproot()->cr_filename)
    free(pproot()->cr_filename);
  pproot()->cr_filename=strdup(fileName);
  if(!(myfh=adms_file_open_read_with_path(fileName,(p_slist)(pproot()->includePath))))
  {
    if(!strcmp(fileName,"discipline.h")||!strcmp(fileName,"disciplines.h")||!strcmp(fileName,"discipline.vams")||!strcmp(fileName,"disciplines.vams"))
    {
      adms_message_warning(("[%s]: standard vams file created (not found in -I path) ... '%s'\n",message,fileName))
      adms_file_fprintf(fileName,disciplines_vams);
      myfh=adms_file_open_read(fileName);
    }
    else if(!strcmp(fileName,"constant.h")||!strcmp(fileName,"constants.h")||!strcmp(fileName,"constant.vams")||!strcmp(fileName,"constants.vams"))
    {
      adms_message_warning(("[%s]: standard vams file created (not found in -I path) ... '%s'\n",message,fileName))
      adms_file_fprintf(fileName,constants_vams);
      myfh=adms_file_open_read(fileName);
    }
    else
      adms_message_fatal(("[%s]: failed to open file ... '%s'\n",message,fileName))
  }
  scanner=(p_preprocessor)malloc(sizeof(t_preprocessor));
  adms_message_verbose(("include file '%s'\n",fileName))
  scanner->buffer=NULL;
  scanner->fid=NULL;
  scanner->cur_continuator_position=NULL;
  pproot()->cr_scanner->buffer = (p_adms)YY_CURRENT_BUFFER;
  adms_slist_push(&pproot()->Scanner,(void*)pproot()->cr_scanner );
  yyin = myfh;
  scanner->fid = yyin;
  scanner->cur_line_position = 1;
  scanner->cur_char_position = 1;
  scanner->filename=strdup(pproot()->cr_filename);
  scanner->buffer = (void*) yy_create_buffer(yyin,YY_BUF_SIZE );
  pproot()->cr_scanner = scanner;
  yy_switch_to_buffer((YY_BUFFER_STATE)scanner->buffer );
  preprocessorlval.mystr = NULL;
  adms_k2strconcat(&preprocessorlval.mystr,"# 1 \"");
  adms_k2strconcat(&preprocessorlval.mystr,pproot()->cr_scanner->filename);
  adms_k2strconcat(&preprocessorlval.mystr,"\"\n");
  BEGIN( INITIAL );
  free(message);
}

static char *adms_preprocessor_lex_skipp_text ()
{
  char *c;
  int nbline=0;
  c = yytext;
  while (*c)
  {
    if(*c=='\n')
    {
      pproot()->cr_scanner->cur_char_position = 1;
      pproot()->cr_scanner->cur_line_position++;
      nbline++;
    }
    else
      pproot()->cr_scanner->cur_char_position++;
    c++;
  }
  if(nbline)
  {
    char*str=(char*)malloc(sizeof(char)*(nbline+1));
    memset(str,'\n',nbline);
    str[nbline] = '\0';
    return str;
  }
  else
    return strdup("");
}

%}

whitespace	 [ \t\v\f]*
Whitespace	 [ \t\v\f]+
newline		 \r?\n
ident		 $?[A-Za-z_][A-Za-z0-9_]*
notident	 [^`a-zA-Z_\r\n]+
notpragma	 [^#`a-zA-Z_\r\n]+
xpath	         \"[^\"]*\"
ipath	         \<[^>]+\>
continuator	 \\\r?\n
anystring  \"[^\"]*\"
b10_uint   [1-9][0-9]*
 
comment          \/\*[^\*]*\*+([^\*\/][^\*]*\*+)*\/
comment_oneline  \/\/[^\r\n]*


%x PRAGMA_PATH
%x PRAGMA_COMMENT
%x PRAGMA_NAME
%x PRAGMA_ERROR
%x PRAGMA_DEFINE_START
%x PRAGMA_DEFINE_IGNORE
%x PRAGMA_DEFINE_NOARG
%x PRAGMA_DEFINE_WITHARG
%x PRAGMA_DEFINE_TEXT_START
%x PRAGMA_DEFINE_TEXT
%x PRAGMA_DEFINE_WITHARG_COMMA
%x PRAGMA_DEFINE_WITHARG_ARGS
%x PRAGMA_SUBSTITUTOR_START
%x PRAGMA_SUBSTITUTOR_ARG
%x PRAGMA_SUBSTITUTOR_PAIR
%x PRAGMA_SUBSTITUTOR_ARGITEM
%x PRAGMA_SUBSTITUTOR_ARGCOMMA
%x PRAGMA_SUBSTITUTOR_ARGEND

%%

\"[^"]*\"  {
  preprocessorlval.mystr = strdup( yytext );
  tkreturn(TK_STRING)
}

{continuator} {
  p_continuator continuator;

  continuator=(p_continuator)malloc(sizeof(t_continuator));
  continuator->char_position = yyleng-1;
  continuator->line_position = pproot()->cr_scanner->cur_line_position;

  pproot()->cr_scanner->cur_line_position += 1;
  pproot()->cr_scanner->cur_char_position = 1;
  adms_slist_push(&(pproot()->cr_scanner->cur_continuator_position),(p_adms)continuator);

  tkreturn(TK_NOPRAGMA_CONTINUATOR)
}

 /* ignored sequences */

^#{Whitespace}{b10_uint}{whitespace}{anystring}{Whitespace}{newline} {
}

\r\n {
  preprocessorlval.mystr=strdup(yytext);
  return(TK_EOL);
}
\n {
  preprocessorlval.mystr=strdup(yytext);
  return(TK_EOL);
}
 
{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  preprocessorlval.mystr=strdup(yytext);
  return(TK_SPACE );
}

{comment} {
  preprocessorlval.mystr=adms_preprocessor_lex_skipp_text (); return(TK_COMMENT);
}

{comment_oneline} {
  preprocessorlval.mystr=adms_preprocessor_lex_skipp_text (); return(TK_COMMENT);
}

 /* pragma */

^{whitespace}[`]define {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  if ( DONT_SKIPP )
    {
      BEGIN( PRAGMA_DEFINE_START );
      tkreturn(TK_DEFINE)
    }
  else
      BEGIN( PRAGMA_DEFINE_IGNORE );
}
<PRAGMA_DEFINE_IGNORE>.* {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  BEGIN( INITIAL );
}
<PRAGMA_DEFINE_IGNORE>{newline} {
  pproot()->cr_scanner->cur_message=strdup("- `define: unexpected end-of-line");
  yyflex_unput
  BEGIN( INITIAL );
}

^{whitespace}[`]undef {
  if ( DONT_SKIPP )
    {
      BEGIN( PRAGMA_NAME );
      tkreturn(TK_UNDEF)
    }
  else
      BEGIN( PRAGMA_DEFINE_IGNORE );
}
^{whitespace}[`]ifdef {
  preprocessorlval.mystr=strdup("");;
  BEGIN( PRAGMA_NAME );
  tkreturn(TK_IFDEF)
}
^{whitespace}[`]ifndef {
  preprocessorlval.mystr=strdup("");;
  BEGIN( PRAGMA_NAME );
  tkreturn(TK_IFNDEF)
}
^{whitespace}[`]else {
  char* string=NULL;
  adms_k2strconcat(&string,"# ");
  adms_strconcat(&string,adms_integertostring(pproot()->cr_scanner->cur_line_position+1));
  adms_k2strconcat(&string," \"");
  adms_k2strconcat(&string,pproot()->cr_scanner->filename);
  adms_k2strconcat(&string,"\"");
  preprocessorlval.mystr=string;
  BEGIN( PRAGMA_COMMENT );
  tkreturn(TK_ELSE)
}
^{whitespace}[`]endif {
  char* string=NULL;
  adms_k2strconcat(&string,"# ");
  adms_strconcat(&string,adms_integertostring(pproot()->cr_scanner->cur_line_position+1));
  adms_k2strconcat(&string," \"");
  adms_k2strconcat(&string,pproot()->cr_scanner->filename);
  adms_k2strconcat(&string,"\"");
  preprocessorlval.mystr=string;
  BEGIN( PRAGMA_COMMENT );
  tkreturn(TK_ENDIF)
}
^{whitespace}[`]include {
  if(DONT_SKIPP)
    BEGIN(PRAGMA_PATH);
  else
    BEGIN(PRAGMA_DEFINE_IGNORE);
}

<PRAGMA_DEFINE_START>{ident}/{newline} {
  BEGIN( PRAGMA_DEFINE_NOARG );
  preprocessorlval.mystr=strdup(yytext );
  tkreturn(TK_PRAGMA_NAME)
}
<PRAGMA_DEFINE_START>{ident}/{Whitespace} {
  BEGIN( PRAGMA_DEFINE_NOARG );
  preprocessorlval.mystr=strdup(yytext );
  tkreturn(TK_PRAGMA_NAME)
}
<PRAGMA_DEFINE_START>{ident}/\( {
  BEGIN( PRAGMA_DEFINE_WITHARG );
  preprocessorlval.mystr=strdup(yytext );
  tkreturn(TK_PRAGMA_NAME)
}
<PRAGMA_DEFINE_START>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}
<PRAGMA_DEFINE_START>{newline} {
  pproot()->cr_scanner->cur_message=strdup("- pragma name: unexpected end-of-line");
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}
<PRAGMA_DEFINE_START>. {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - identifier was expected");
  pproot()->cr_scanner->cur_message=string;
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}

<PRAGMA_DEFINE_NOARG>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}
<PRAGMA_DEFINE_NOARG>{newline} {
  pproot()->cr_scanner->cur_message=strdup("- `define: unexpected end-of-line)");
  yyflex_unput
  BEGIN( INITIAL );
  tkreturn(TK_DEFINE_END)
}
<PRAGMA_DEFINE_NOARG>{continuator} {
  p_continuator continuator;

  continuator=(p_continuator)malloc(sizeof(t_continuator));
  continuator->char_position = yyleng-1;
  continuator->line_position = pproot()->cr_scanner->cur_line_position;

  pproot()->cr_scanner->cur_line_position += 1;
  pproot()->cr_scanner->cur_char_position = 1;
  adms_slist_push(&(pproot()->cr_scanner->cur_continuator_position),(p_adms)continuator);

  tkreturn(TK_CONTINUATOR)
}
<PRAGMA_DEFINE_NOARG>. {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - whitespace was expected");
  pproot()->cr_scanner->cur_message=string;
  yyflex_unput
  BEGIN( PRAGMA_DEFINE_TEXT_START );
}

<PRAGMA_DEFINE_WITHARG>\({whitespace}\) {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  BEGIN( PRAGMA_DEFINE_TEXT_START );
  tkreturn(TK_ARG_NULL)
}
<PRAGMA_DEFINE_WITHARG>\([^)]+{newline} {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - `define has no closing paren");
  pproot()->cr_scanner->cur_message=string;
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}
<PRAGMA_DEFINE_WITHARG>\({whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  BEGIN( PRAGMA_DEFINE_WITHARG_ARGS );
}

<PRAGMA_DEFINE_WITHARG_ARGS>{ident} {
  BEGIN( PRAGMA_DEFINE_WITHARG_COMMA );
  preprocessorlval.mystr=strdup(yytext );
  tkreturn(TK_ARG)
}
<PRAGMA_DEFINE_WITHARG_ARGS>. {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - `define has unexpected syntax");
  pproot()->cr_scanner->cur_message=string;
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}

<PRAGMA_DEFINE_WITHARG_COMMA>, {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}
<PRAGMA_DEFINE_WITHARG_COMMA>\) {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  BEGIN( PRAGMA_DEFINE_TEXT_START );
}
<PRAGMA_DEFINE_WITHARG_COMMA>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}
<PRAGMA_DEFINE_WITHARG_COMMA>. {
  yyflex_unput
  BEGIN( PRAGMA_DEFINE_WITHARG_ARGS );
}

<PRAGMA_DEFINE_TEXT_START>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  BEGIN( PRAGMA_DEFINE_TEXT );
}
<PRAGMA_DEFINE_TEXT_START>{continuator} {
  p_continuator continuator;

  continuator=(p_continuator)malloc(sizeof(t_continuator));
  continuator->char_position = yyleng-1;
  continuator->line_position = pproot()->cr_scanner->cur_line_position;

  pproot()->cr_scanner->cur_line_position += 1;
  pproot()->cr_scanner->cur_char_position = 1;
  adms_slist_push(&(pproot()->cr_scanner->cur_continuator_position),(p_adms)continuator);

  tkreturn(TK_CONTINUATOR)
}
<PRAGMA_DEFINE_TEXT_START>{newline} {
  yyflex_unput
  BEGIN( INITIAL );
  tkreturn(TK_DEFINE_END)
}
<PRAGMA_DEFINE_TEXT_START>. {
  yyflex_unput
  BEGIN( PRAGMA_DEFINE_TEXT );
}

<PRAGMA_DEFINE_TEXT>{Whitespace}/{newline} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}

<PRAGMA_DEFINE_TEXT>`{ident}/[ ]*\( {

  preprocessorlval.mystr=strdup(adms_preprocessor_lex_remove_leading_quote( yytext ));
  if(DONT_SKIPP)
  {
    p_preprocessor_pragma_define Define;
    if ((Define = adms_preprocessor_pragma_define_exists (preprocessorlval.mystr)))
    {
      if(adms_preprocessor_pragma_define_has_noarg(Define))
        tkreturn(TK_SUBSTITUTOR_NOARG)
      else if(adms_preprocessor_pragma_define_has_nullarg(Define))
        tkreturn(TK_SUBSTITUTOR_NULLARG)
      else
        tkreturn(TK_SUBSTITUTOR_WITHARG)
    }
    else
    {
       preprocessorlval.mystr = strdup(yytext);
       pproot()->cr_scanner->cur_message=strdup(yytext);
       tkreturn(TK_ERROR_PRAGMA_NOT_FOUND)
    }
  }
  else
    tkreturn(TK_IDENT)
}

<PRAGMA_DEFINE_TEXT>`{ident} {

  preprocessorlval.mystr=strdup(adms_preprocessor_lex_remove_leading_quote( yytext ));
  if(DONT_SKIPP)
  {
    p_preprocessor_pragma_define Define;
    if((Define=adms_preprocessor_pragma_define_exists (preprocessorlval.mystr)))
    {
      if(adms_preprocessor_pragma_define_has_noarg(Define))
        tkreturn(TK_SUBSTITUTOR_NOARG)
      else if(adms_preprocessor_pragma_define_has_nullarg(Define))
        tkreturn(TK_SUBSTITUTOR_NULLARG_ALONE)
      else
        tkreturn(TK_SUBSTITUTOR_WITHARG_ALONE)
    }
    else
    {
       preprocessorlval.mystr=strdup(yytext);
       pproot()->cr_scanner->cur_message=strdup(yytext);
       tkreturn(TK_ERROR_PRAGMA_NOT_FOUND)
    }
  }
  else
    tkreturn(TK_IDENT)
}

<PRAGMA_DEFINE_TEXT>{ident} {

  preprocessorlval.mystr = strdup(yytext);
  tkreturn(TK_IDENT)
}

<PRAGMA_DEFINE_TEXT>\(  {
  preprocessorlval.mystr = strdup(yytext);
  tkreturn('(')
}

<PRAGMA_DEFINE_TEXT>\)  {
  preprocessorlval.mystr = strdup(yytext);
  tkreturn(')')
}

<PRAGMA_DEFINE_TEXT>\,  {
  preprocessorlval.mystr = strdup(yytext);
  tkreturn(',')
}

<PRAGMA_DEFINE_TEXT>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
  preprocessorlval.mystr=strdup(yytext);
  return(TK_SPACE );
}

<PRAGMA_DEFINE_TEXT>{comment_oneline} {
  preprocessorlval.mystr=adms_preprocessor_lex_skipp_text (); return(TK_COMMENT);
}

<PRAGMA_DEFINE_TEXT>{comment} {
  preprocessorlval.mystr=adms_preprocessor_lex_skipp_text (); return(TK_COMMENT);
}

<PRAGMA_DEFINE_TEXT>.  {
  preprocessorlval.mystr = strdup(yytext);
  tkreturn(TK_NOT_IDENT)
}

<PRAGMA_DEFINE_TEXT>{continuator} {
  p_continuator continuator;

  continuator=(p_continuator)malloc(sizeof(t_continuator));
  continuator->char_position = yyleng-1;
  continuator->line_position = pproot()->cr_scanner->cur_line_position;

  pproot()->cr_scanner->cur_line_position += 1;
  pproot()->cr_scanner->cur_char_position = 1;
  adms_slist_push(&(pproot()->cr_scanner->cur_continuator_position),(p_adms)continuator);

  tkreturn(TK_CONTINUATOR)
}
<PRAGMA_DEFINE_TEXT>{newline} {
  yyflex_unput
  BEGIN( INITIAL );
  tkreturn(TK_DEFINE_END)
}

<PRAGMA_NAME>{ident} {
  BEGIN( PRAGMA_COMMENT );
  preprocessorlval.mystr=strdup(yytext );
  tkreturn(TK_PRAGMA_NAME)
}
<PRAGMA_NAME>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}
<PRAGMA_NAME>{newline} {
  pproot()->cr_scanner->cur_message=strdup("- unexpected end-of-line");
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}
<PRAGMA_NAME>. {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - pragma name was expected");
  pproot()->cr_scanner->cur_message=string;
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}
<PRAGMA_NAME>{continuator} {
  p_continuator continuator;

  continuator=(p_continuator)malloc(sizeof(t_continuator));
  continuator->char_position = yyleng-1;
  continuator->line_position = pproot()->cr_scanner->cur_line_position;

  pproot()->cr_scanner->cur_line_position += 1;
  pproot()->cr_scanner->cur_char_position = 1;
  adms_slist_push(&(pproot()->cr_scanner->cur_continuator_position),(p_adms)continuator);

  tkreturn(TK_CONTINUATOR)
}

<PRAGMA_PATH>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}
<PRAGMA_PATH>{xpath} {
  char* fileName;
  BEGIN( INITIAL );
  fileName = adms_preprocessor_lex_get_path(yytext,'"','"');
  adms_preprocessor_lex_include_file(fileName);
  tkreturn(TK_INCLUDE)
}
<PRAGMA_PATH>{ipath} {
  char* fileName;
  BEGIN( INITIAL );
  fileName = adms_preprocessor_lex_get_path(yytext,'<','>');
  adms_message_info(("%s: implicit path handled as explicit path!",fileName))
  adms_preprocessor_lex_include_file(fileName);
  tkreturn(TK_INCLUDE)
}
<PRAGMA_PATH>. {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - bad path");
  pproot()->cr_scanner->cur_message=string;
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}

<PRAGMA_COMMENT>{Whitespace} {
  pproot()->cr_scanner->cur_char_position+=yyleng; 
}
<PRAGMA_COMMENT>{comment_oneline} {
  preprocessorlval.mystr=adms_preprocessor_lex_skipp_text (); return(TK_COMMENT);
}
<PRAGMA_COMMENT>{comment} {
  preprocessorlval.mystr=adms_preprocessor_lex_skipp_text (); return(TK_COMMENT);
}
<PRAGMA_COMMENT>{continuator} {
  p_continuator continuator;

  continuator=(p_continuator)malloc(sizeof(t_continuator));
  continuator->char_position = yyleng-1;
  continuator->line_position = pproot()->cr_scanner->cur_line_position;

  pproot()->cr_scanner->cur_line_position += 1;
  pproot()->cr_scanner->cur_char_position = 1;
  adms_slist_push(&(pproot()->cr_scanner->cur_continuator_position),(p_adms)continuator);

  tkreturn(TK_CONTINUATOR)

}
<PRAGMA_COMMENT>{newline} {
  yyflex_unput
  BEGIN( INITIAL );
}
<PRAGMA_COMMENT>. {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - unknown error");
  pproot()->cr_scanner->cur_message=string;
  yyflex_unput
  BEGIN( PRAGMA_ERROR );
}

<PRAGMA_ERROR>.*{newline} {
  char* string=NULL;
  adms_k2strconcat(&string,"at character ");
  adms_k2strconcat(&string,yytext);
  adms_k2strconcat(&string," - unknown error");
  pproot()->cr_scanner->cur_message=string;
  tkreturn(TK_ERROR_PRAGMA_DEFINITION)
}

`{ident}/[ ]*\( {

  p_preprocessor_pragma_define Define;
  preprocessorlval.mystr = 
    strdup(adms_preprocessor_lex_remove_leading_quote( yytext));
  if(DONT_SKIPP)
  {
    if ((Define = adms_preprocessor_pragma_define_exists (preprocessorlval.mystr)))
    {
      if(adms_preprocessor_pragma_define_has_noarg(Define))
        tkreturn(TK_SUBSTITUTOR_NOARG)
      else if(adms_preprocessor_pragma_define_has_nullarg(Define))
        tkreturn(TK_SUBSTITUTOR_NULLARG)
      else
        tkreturn(TK_SUBSTITUTOR_WITHARG)
    }
    else
    {
      preprocessorlval.mystr = strdup(yytext);
      pproot()->cr_scanner->cur_message=strdup(yytext);
      tkreturn(TK_ERROR_PRAGMA_NOT_FOUND)
    }
  }
  else
    tkreturn(TK_IDENT)

}

`{ident} {

  preprocessorlval.mystr = 
    strdup(adms_preprocessor_lex_remove_leading_quote( yytext ));
  {
    p_preprocessor_pragma_define Define;
    if(DONT_SKIPP)
    {
      if((Define=adms_preprocessor_pragma_define_exists (preprocessorlval.mystr)))
      {
        if(adms_preprocessor_pragma_define_has_noarg(Define))
          tkreturn(TK_SUBSTITUTOR_NOARG)
        else if(adms_preprocessor_pragma_define_has_nullarg(Define))
          tkreturn(TK_SUBSTITUTOR_NULLARG_ALONE)
        else
          tkreturn(TK_SUBSTITUTOR_WITHARG_ALONE)
      }
      else
      {
         preprocessorlval.mystr = strdup(yytext);
         pproot()->cr_scanner->cur_message=strdup(yytext);
         tkreturn(TK_ERROR_PRAGMA_NOT_FOUND)
      }
    }
    else
      tkreturn(TK_IDENT)
  }

}

{ident} {

  preprocessorlval.mystr = strdup(yytext);
  tkreturn(TK_IDENT)

}

\(  {
  preprocessorlval.mystr = strdup(yytext);
  return '(';
}

\)  {
  preprocessorlval.mystr = strdup(yytext);
  return ')';
}

\,  {
  preprocessorlval.mystr = strdup(yytext);
  return ',';
}

.  {
  preprocessorlval.mystr = strdup(yytext);
  return TK_NOT_IDENT;
}

<<EOF>> {
  char* string=NULL;
  if( pproot()->Scanner != NULL )
    {
      yy_delete_buffer((YY_BUFFER_STATE)(pproot()->cr_scanner->buffer));
      free(pproot()->cr_scanner );
      pproot()->cr_scanner = (p_preprocessor) adms_slist_pull(&pproot()->Scanner );
      yyin = pproot()->cr_scanner->fid;
      yy_switch_to_buffer((YY_BUFFER_STATE)pproot()->cr_scanner->buffer );
    }
  else
    yyterminate ();
  adms_k2strconcat(&string,"# ");
  adms_strconcat(&string,adms_integertostring(pproot()->cr_scanner->cur_line_position+1));
  adms_k2strconcat(&string," \"");
  adms_k2strconcat(&string,pproot()->cr_scanner->filename);
  adms_k2strconcat(&string,"\"");
  preprocessorlval.mystr=string;
  tkreturn(TK_EOF)
}

%%
int yywrap (void) {return 1;}

