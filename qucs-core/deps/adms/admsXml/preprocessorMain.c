/*
 * RCS Info
 * $Id: preprocessorMain.c 1154 2008-11-04 10:25:43Z r29173 $
 * 
 * Log
 * $Log$
 * Revision 1.12  2005/06/17 16:44:25  r29173
 * debug memory allocation with valgrind
 *
 * Revision 1.11  2005/06/16 07:12:45  r29173
 * debug memory allocation with valgrind
 *
 * Revision 1.10  2005/05/09 14:38:30  r29173
 * cleaned-up source directory structure
 *
 * Revision 1.9  2005/05/03 09:35:15  r29173
 * cleaned-up header file dependencies
 *
 * Revision 1.8  2005/05/03 07:58:21  r29173
 * moved admsNS into admsFile
 *
 * Revision 1.7  2004/11/19 12:29:56  r29173
 * renamed _C into CAT
 * saved argc, argv into admsmain element
 * added adms:setenv transform
 *
 * Revision 1.6  2004/11/01 09:37:37  r29173
 * changed messaging macros (avoid useless computation of arguments)
 * improved the coding of transforms in xmlProgram.c
 *
 * Revision 1.5  2004/10/20 15:33:52  r29173
 * tons of changes
 * 1- introduced element whileloop (marat)
 * 2- reshaped the handling of the messaging system
 * 3- fixed bug in admsCheck/Makefile.am
 * 4- started implementation of implicit adms transforms
 *
 * Revision 1.4  2004/08/19 16:45:47  r29173
 * cleaned-up i/o file handling
 *
 * Revision 1.3  2004/08/03 12:33:55  r29173
 * import adms-1.21.0 from local CVS
 *
 * Revision 1.3  2004/07/09 14:39:11  r29173
 * removed data structure ns
 *
 * Revision 1.2  2004/05/26 13:02:48  r29173
 * added default values to all enumerations
 *
 * Revision 1.1.1.1  2004/05/21 12:20:01  r29173
 * recreated cvs data structure (crashed after revision 1.13.0!)
 *
 * Revision 1.14  2004/03/08 13:58:06  r29173
 * all code lower-cased
 *
 * Revision 1.13  2004/02/13 14:28:39  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.12  2004/02/06 09:06:03  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.11  2004/01/21 14:35:47  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.10  2004/01/09 16:23:47  r29173
 * cleaned-up use of #include
 *
 * Revision 1.9  2004/01/08 10:28:50  r29173
 * moved code to always.[ch]
 *
 * Revision 1.8  2004/01/06 12:35:29  r29173
 * fixed the use of globals: input file and output file
 *
 * Revision 1.7  2003/12/12 15:08:35  r29173
 * changed construct (a==NULL) to (!a)
 *
 * Revision 1.6  2003/12/12 14:43:38  r29173
 * changed construct (a==NULL) to (!a)
 *
 * Revision 1.5  2003/12/12 14:33:26  r29173
 * changed construct (a!=NULL) into (a) or (a)?1:0
 *
 * Revision 1.4  2003/12/11 20:14:07  r29173
 * cleaned-up after compilation with CFLAG+=-Wall
 *
 * Revision 1.3  2003/12/11 16:01:53  r29173
 * changed prefix [epst]_adms_ to [epst]_
 *
 * Revision 1.2  2003/05/21 14:18:01  r29173
 * add rcs info
 *
 */

#include "admsPreprocessor.h"

#define strdup_before_ptr(s,sptr,offset) adms_knclone(s+offset,sptr-s-offset)
#define strdup_after_ptr(s,sptr,offset)  adms_knclone(sptr+offset,strlen(s)-(sptr-s)-offset)

extern int adms_preprocessor_leng;
static p_preprocessor_main adms_preprocessor_main;
void adms_preprocessor_valueto_main (p_preprocessor_main myadms_preprocessor_main)
{
  adms_preprocessor_main=myadms_preprocessor_main;
}
p_preprocessor_main pproot ()
{
  return adms_preprocessor_main;
}
void adms_preprocessor_get_define_from_argv (const int argc,const char** argv)
{
  int i;
  for (i=1; i<argc; i++)
  {
    char *value=strdup(argv[i]);
    char *defineName = NULL;
    char *defineValue = NULL;
    if ( (strlen(value)>2) && ((value[0])=='-') && ((value[1])=='D') )
    {
      char *equal = NULL;
      char *arg = strdup_after_ptr (value,value+2,0);
      char *argPtr = arg;
      while (*argPtr && (!equal))
      {
        if (*argPtr=='=')
          equal= argPtr;
        argPtr++;
      }
      if (*arg == '=')
        adms_message_verbose(("at command line argument, bad syntax ... '%s'\n",value))
      else if (equal == last_char (arg))
        defineName = strdup_before_ptr (arg,equal,0);
      else if (!equal)
        defineName = strdup(arg);
      else
      {
        defineName = strdup_before_ptr (arg,equal,0);
        defineValue = strdup_after_ptr (arg,equal,1);
      }
      if (defineName)
      {
        if (defineValue)
          adms_preprocessor_define_add_default_with_text (defineName, defineValue);
        else
          adms_preprocessor_define_add_default (defineName);
      }
      free(arg);
    }
    if (defineName)
      adms_message_verbose(("set pragma ... '%s'\n",defineName))
    free(value);
  }
}
int adms_preprocessor_add_message ( char *message)
{
  adms_message_info((message))
  return 0;
}
p_preprocessor_text adms_preprocessor_new_text_as_string(char *name)
{
  p_preprocessor_text text=(p_preprocessor_text)malloc(sizeof(t_preprocessor_text));
  text->_str = name;
  text->_isarg = 0;
  return text;
}
static int adms_preprocessor_pragma_substitute_text (p_preprocessor_substitutor substitutor)
{
  if (adms_preprocessor_pragma_define_is_def (substitutor->_pragma))
  {
    p_preprocessor_text space = adms_preprocessor_new_text_as_string(" ");
    p_slist l; for(l=substitutor->_pragma->text;l;l=l->next)
    {
      p_preprocessor_text text=(p_preprocessor_text)(l->data);
      p_slist arg=adms_slist_copy(substitutor->_pragma->arg);
      adms_slist_inreverse(&arg);
      substitutor->_cr_text=text;
      substitutor->_cr_newarg=adms_slist_copy(substitutor->_newarg);
      adms_slist_inreverse(&substitutor->_cr_newarg);
      if (text->_isarg==1)
      {
        p_slist ll; for(ll=arg;ll;ll=ll->next)
        {
          char* arg=(char*)ll->data;
          if(!strcmp(substitutor->_cr_text->_str,arg))
          {
            if(substitutor->_cr_newarg)
            {
              p_slist newarg=adms_slist_copy((p_slist)(substitutor->_cr_newarg->data));
              adms_slist_inreverse(&newarg);
              adms_slist_concat(&newarg,substitutor->_newtext);
              substitutor->_newtext=newarg;
            }
            else
              adms_slist_push(&(substitutor->_newtext),(p_adms)substitutor->_cr_text);
          }
          substitutor->_cr_newarg = substitutor->_cr_newarg->next;
        }
      }
      else
      {
        p_preprocessor_text newtext = adms_preprocessor_new_text_as_string(text->_str);
        newtext->_isarg = -1;
        adms_slist_push(&(substitutor->_newtext),(p_adms)newtext);
      }
      adms_slist_free(arg);
      adms_slist_free(substitutor->_cr_newarg);
    }
    adms_slist_push(&(substitutor->_newtext),(p_adms)space);
  }
  else
  {
    p_preprocessor_text name = adms_preprocessor_new_text_as_string(substitutor->_pragma->name);
    adms_slist_push(&(substitutor->_newtext),(p_adms)name);
  }
  return 0;
}
p_slist adms_preprocessor_new_text_as_substitutor (p_preprocessor_pragma_define define, p_slist newarg)
{
  p_preprocessor_substitutor substitutor=(p_preprocessor_substitutor)malloc(sizeof(t_preprocessor_substitutor));
  substitutor->_newtext=NULL;
  substitutor->_cr_text=NULL;
  substitutor->_cr_newarg=NULL;
  substitutor->_pragma = define;
  substitutor->_newarg = newarg;
  adms_preprocessor_pragma_substitute_text(substitutor);
  adms_slist_inreverse(&substitutor->_newtext);
  return substitutor->_newtext;
}
int adms_preprocessor_pragma_define_has_arg (p_preprocessor_pragma_define pragma)
{
  return (pragma->hasArg==1);
}
int adms_preprocessor_pragma_define_is_def (p_preprocessor_pragma_define pragma)
{
  return (pragma->isDefined==1);
}
int adms_preprocessor_identifier_is_def (char * name)
{
  int test;
  p_preprocessor_pragma_define pragma;
  if(
      (pragma = adms_preprocessor_pragma_define_exists (name))
      &&
      adms_preprocessor_pragma_define_is_def(pragma)
    )
    test = ( pragma->isDefined == 1 );
  else
    test = 0;
  return test;
}
p_preprocessor_pragma_define adms_preprocessor_identifer_set_undef (char * name)
{
  p_preprocessor_pragma_define pragma = adms_preprocessor_pragma_define_exists (name);
  if(!pragma)
    pragma = adms_preprocessor_define_add (name);
  pragma->isDefined = 0;
  return pragma;
}
int adms_preprocessor_identifier_is_ndef (char * name)
{
  return (!adms_preprocessor_identifier_is_def (name) );
}
int adms_preprocessor_pragma_define_is_ndef (p_preprocessor_pragma_define pragma)
{
  return (!adms_preprocessor_pragma_define_is_def(pragma));
}
int adms_preprocessor_pragma_define_has_noarg (p_preprocessor_pragma_define pragma)
{
  return (!adms_preprocessor_pragma_define_has_arg(pragma));
}
int adms_preprocessor_pragma_define_has_nullarg (p_preprocessor_pragma_define pragma)
{
  return adms_preprocessor_pragma_define_has_arg(pragma) && (adms_slist_length( pragma->arg )==0);
}
int adms_preprocessor_pragma_define_has_text (p_preprocessor_pragma_define pragma)
{
  return ((pragma->text)?1:0);
}
int adms_preprocessor_pragma_define_has_notext (p_preprocessor_pragma_define pragma)
{
  return (!adms_preprocessor_pragma_define_has_text(pragma));
}
int adms_preprocessor_pragma_define_has_noarg_and_notext (p_preprocessor_pragma_define pragma)
{
  int test;
  test = adms_preprocessor_pragma_define_has_noarg(pragma)
         &&
         adms_preprocessor_pragma_define_has_notext(pragma)
         ;
  return test;
}
int adms_preprocessor_pragma_define_has_noarg_and_text (p_preprocessor_pragma_define pragma)
{
  int test;
  test = adms_preprocessor_pragma_define_has_noarg(pragma)
         &&
         adms_preprocessor_pragma_define_has_text(pragma)
         ;
  return test;
}
int adms_preprocessor_pragma_define_has_nullarg_and_notext (p_preprocessor_pragma_define pragma)
{
  int test;
  test = adms_preprocessor_pragma_define_has_nullarg(pragma)
         &&
         adms_preprocessor_pragma_define_has_notext(pragma)
         ;
  return test;
}
int adms_preprocessor_pragma_define_has_nullarg_and_text (p_preprocessor_pragma_define pragma)
{
  int test;
  test = adms_preprocessor_pragma_define_has_nullarg(pragma)
         &&
         adms_preprocessor_pragma_define_has_text(pragma)
         ;
  return test;
}
int adms_preprocessor_pragma_define_has_arg_and_notext (p_preprocessor_pragma_define pragma)
{
  int test;
  test = adms_preprocessor_pragma_define_has_arg(pragma)
         &&
         adms_preprocessor_pragma_define_has_notext(pragma)
         ;
  return test;
}
int adms_preprocessor_pragma_define_has_arg_and_text (p_preprocessor_pragma_define pragma)
{
  int test;
  test = adms_preprocessor_pragma_define_has_arg(pragma)
         &&
         adms_preprocessor_pragma_define_has_text(pragma)
         ;
  return test;
}
static int adms_preprocessor_identifier_is_pragma_cb (p_preprocessor_pragma_define pragma, char * identifier)
{
  return (!!strcmp(pragma->name,identifier));
}
typedef int (*Binary) (const void* d1,const void* d2);
static p_slist adms_slist_find_custom (p_slist l,const void* data,Binary func)
{
  while(l)
  {
    if(!func(l->data,data))
       return l;
      l=l->next;
  }
  return ((void *)0);
}
p_preprocessor_pragma_define adms_preprocessor_pragma_define_exists (char * name)
{
  p_slist Data;
  p_preprocessor_pragma_define pragma;
  if((Data=adms_slist_find_custom ( pproot()->Defined,name,(Binary)adms_preprocessor_identifier_is_pragma_cb)))
    pragma=(p_preprocessor_pragma_define) Data->data;
  else
    pragma=NULL;
  return pragma;
}
p_preprocessor_pragma_define adms_preprocessor_define_add (char * name)
{
  p_preprocessor_pragma_define pragma;
  if ((pragma = adms_preprocessor_pragma_define_exists(name)))
  {
    if( pragma->isDefined == 1 )
      adms_message_warning(("pragma redefined ... '%s'\n",name))
    pragma->hasArg = 0;
    pragma->arg = NULL;
    pragma->text = NULL;
  }
  else
  {
    pragma=(p_preprocessor_pragma_define)malloc(sizeof(t_preprocessor_pragma_define));
    pragma->hasArg = 0;
    pragma->arg=NULL;
    pragma->text=NULL;
    pragma->name = name;
    adms_slist_push(&(pproot()->Defined),(p_adms)pragma);
  }
  pragma->isDefined = 1;
  return pragma;
}
int adms_preprocessor_define_add_default (char * name)
{
  p_preprocessor_pragma_define pragma;
  pragma = adms_preprocessor_define_add (name);
  adms_message_verbose(("define macro ... '%s'\n",name))
  assert(adms_preprocessor_pragma_define_has_noarg_and_notext(pragma));
  return 0;
}
int adms_preprocessor_define_add_default_with_text (char * name, char * value)
{
  p_slist Text;
  p_preprocessor_text text;
  p_preprocessor_pragma_define pragma;
  text = adms_preprocessor_new_text_as_string(value);
  Text = adms_slist_new((p_adms)text);
  pragma = adms_preprocessor_define_add_with_text (name, Text);
  adms_message_verbose(("define macro with value ... '%s=%s'\n",name, value))
  assert(adms_preprocessor_pragma_define_has_noarg_and_text(pragma));
  return 0;
}
p_preprocessor_pragma_define adms_preprocessor_undefine_add (char * name)
{
  p_preprocessor_pragma_define pragma;
  pragma = adms_preprocessor_pragma_define_exists(name);
  if(!pragma)
    {
      pragma=(p_preprocessor_pragma_define)malloc(sizeof(t_preprocessor_pragma_define));
      pragma->arg=NULL;
      pragma->text=NULL;
      pragma->name = name;
      adms_slist_push(&(pproot()->Defined),(p_adms)pragma);
    }
  pragma->isDefined = 0;
  return pragma;
}
p_preprocessor_pragma_define adms_preprocessor_define_add_with_arg (char * name, p_slist arg)
{
  p_preprocessor_pragma_define pragma;
  pragma = adms_preprocessor_define_add (name );
  pragma->hasArg = 1;
  pragma->arg = arg;
  pragma->text = NULL;
  return pragma;
}
p_preprocessor_pragma_define adms_preprocessor_define_add_with_text (char * name, p_slist text)
{
  p_preprocessor_pragma_define pragma;
  pragma = adms_preprocessor_define_add (name );
  pragma->hasArg = 0;
  pragma->arg = NULL;
  pragma->text = text;
  return pragma;
}
p_preprocessor_pragma_define adms_preprocessor_define_add_with_arg_and_text (char * name, p_slist arg, p_slist text)
{
  p_preprocessor_pragma_define pragma;
  p_slist l;
  pragma = adms_preprocessor_define_add_with_arg (name, arg );
  pragma->text = text;
  for(l=pragma->text;l;l=l->next)
  {
    p_preprocessor_text text=(p_preprocessor_text)(l->data);
    p_slist ll; for(ll=pragma->arg;ll;ll=ll->next)
    {
      if((text->_isarg != -1) && !strcmp(text->_str,(char*)(ll->data)))
        text->_isarg = 1;
    }
  }
  return pragma;
}
static p_continuator adms_preprocessor_get_line_position_cb (p_slist l,int mycharpos)
{
  while(l)
  {
    p_continuator mycontinuator=(p_continuator)(l->data);
    if(mycharpos<=mycontinuator->char_position)
       return mycontinuator;
    l=l->next;
  }
  return NULL;
}
int adms_preprocessor_get_line_position (p_preprocessor cr_preprocessor, int char_pos)
{
  int line_pos=cr_preprocessor->cur_line_position;
  p_continuator mycontinuator;
  p_slist Reversed=adms_slist_reverse(cr_preprocessor->cur_continuator_position);
  if((mycontinuator=adms_preprocessor_get_line_position_cb(Reversed,char_pos)))
    line_pos=mycontinuator->line_position;
  cr_preprocessor->cur_continuator_position = adms_slist_reverse(Reversed );
  return line_pos;
}
