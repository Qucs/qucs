/* [nepasimprimer]
 * RCS Info
 * $Id: admsXml.c 1184 2010-06-23 21:33:17Z r29173 $
 *
 * Log
 * $Log$
 * Revision 1.12  2006/09/08 16:03:46  r29173
 * set transform attributes as admsttext
 *
 * Revision 1.11  2006/08/30 02:57:12  r29173
 * added %() support
 *
 * Revision 1.10  2006/08/18 17:10:50  r29173
 * did lot of simplifications
 * added element quark (character array)
 *
 * Revision 1.9  2006/08/10 11:31:47  r29173
 * did lot of simplifications
 *
 * Revision 1.8  2006/08/04 17:15:31  r29173
 * rearranged code partionning into files
 *
 * Revision 1.7  2006/08/02 16:45:52  r29173
 * rearranged code partionning into files
 *
 * Revision 1.6  2005/04/25 12:29:33  r29173
 * fixed few memory leak issues (introduced adms.._free)
 *
 * Revision 1.5  2004/11/19 12:29:56  r29173
 * renamed _C into CAT
 * saved argc, argv into admsmain element
 * added adms:setenv transform
 *
 * Revision 1.4  2004/08/19 16:46:44  r29173
 * cleaned-up i/o file handling
 *
 * Revision 1.3  2004/08/03 12:33:55  r29173
 * import adms-1.21.0 from local CVS
 *
 * Revision 1.2  2004/05/26 13:02:47  r29173
 * added default values to all enumerations
 *
 * Revision 1.1.1.1  2004/05/21 12:20:01  r29173
 * recreated cvs data structure (crashed after revision 1.13.0!)
 *
 * Revision 1.67  2004/05/17 11:50:28  r29173
 * simplified handling of variable attributes
 *
 * Revision 1.66  2004/05/13 07:34:50  r29173
 * continue removing GNode* structure
 *
 * Revision 1.65  2004/05/05 10:19:30  r29173
 * started removing GNode* structure
 *
 * Revision 1.64  2004/05/04 08:07:10  r29173
 * added code for @final
 * removed libltdl
 *
 * Revision 1.63  2004/04/19 16:24:31  r29173
 * moved code of xparser to xmlParser
 *
 * Revision 1.62  2004/04/19 15:41:37  r29173
 * moved code of treeValidate to xmlParser
 *
 * Revision 1.61  2004/04/19 14:52:19  r29173
 * cleaned-up code
 * added handling of \t in xml files
 *
 * Revision 1.60  2004/03/30 09:06:34  r29173
 * added adms_message
 * added statistics
 * started noise implementation
 *
 * Revision 1.59  2004/03/25 20:14:49  r29173
 * splitted variable->scope into small pieces
 *
 * Revision 1.58  2004/03/19 12:45:35  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.57  2004/03/19 10:24:58  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.56  2004/03/08 13:58:07  r29173
 * all code lower-cased
 *
 * Revision 1.55  2004/03/08 08:24:20  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.54  2004/03/04 11:01:28  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.53  2004/03/02 13:42:00  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.52  2004/03/02 10:48:27  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.51  2004/02/25 11:34:23  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.50  2004/02/21 16:13:54  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.49  2004/02/20 22:22:18  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.48  2004/02/20 19:40:08  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.47  2004/02/20 19:05:53  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.46  2004/02/20 14:56:37  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.45  2004/02/20 13:45:30  r29173
 * start implementing support to new vla syntax
 *
 * Revision 1.44  2004/02/19 17:02:38  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.43  2004/02/19 01:25:11  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.42  2004/02/18 23:30:46  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.41  2004/02/18 00:43:01  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.40  2004/02/15 13:07:43  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.39  2004/02/14 22:46:49  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.38  2004/02/14 20:15:42  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.37  2004/02/13 14:28:39  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.36  2004/02/11 15:19:31  r29173
 * release 1.5.1
 *
 * Revision 1.35  2004/02/11 14:20:11  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.34  2004/02/10 16:44:30  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.33  2004/02/10 10:48:24  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.32  2004/02/06 16:27:19  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.31  2004/02/06 14:52:08  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.30  2004/02/05 21:37:03  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.29  2004/02/05 14:02:30  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.28  2004/02/04 20:49:12  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.27  2004/02/04 20:34:24  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.26  2004/02/03 12:48:29  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.25  2004/01/29 15:49:08  r29173
 * renamed accessors of xmlnodes
 *
 * Revision 1.24  2004/01/28 16:15:06  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.23  2004/01/28 10:37:29  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.22  2004/01/27 17:20:15  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.21  2004/01/27 13:46:14  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.20  2004/01/22 12:10:40  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.19  2004/01/21 14:35:45  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.18  2004/01/16 11:18:29  r29173
 * removed use of adms_xml_load_scope
 *
 * Revision 1.17  2004/01/16 10:24:02  r29173
 * added admsPreprocessor code
 * added admsSimulator code
 *
 * Revision 1.16  2004/01/13 11:25:52  r29173
 * added adms_message_usage
 *
 * Revision 1.15  2004/01/09 16:23:48  r29173
 * cleaned-up use of #include
 *
 * Revision 1.14  2004/01/08 10:28:49  r29173
 * moved code to always.[ch]
 *
 * Revision 1.13  2004/01/07 15:19:15  r29173
 * added command line arguments -h and -v
 *
 * Revision 1.12  2004/01/06 12:35:01  r29173
 * fixed the use of globals: input file and output file
 *
 * Revision 1.11  2004/01/05 11:40:14  r29173
 * added message when no arg specified at command line
 *
 * Revision 1.10  2003/12/15 21:47:14  r29173
 * started to create adms_object_new automatically
 *
 * Revision 1.9  2003/12/15 20:54:30  r29173
 * started to create adms_object_new automatically
 *
 * Revision 1.8  2003/12/12 14:33:27  r29173
 * changed construct (a!=NULL) into (a) or (a)?1:0
 *
 * Revision 1.7  2003/12/11 16:01:54  r29173
 * changed prefix [epst]_adms_ to [epst]_
 *
 * Revision 1.6  2003/12/05 13:47:14  r29173
 * used only one routine to create xml nodes
 *
 * Revision 1.5  2003/11/08 22:25:52  r29173
 * re-organized code (removed file simulatorLoad.c)
 *
 * Revision 1.4  2003/11/08 17:46:10  r29173
 * re-organized handling of xparsers
 *
 * Revision 1.3  2003/10/31 16:43:06  r29173
 * clean-up the way xparser works
 *
 * Revision 1.2  2003/05/21 14:18:02  r29173
 * add rcs info
 *
 */
 /*[nepasimprimer]*/
#include "admstpathYacc.h"
typedef void (*p_valueto) (p_adms myadms1,p_adms myadms2);
typedef void (*p_valuetobasicenumeration) (p_adms myadms,admse myinteger);
typedef void (*p_valuetobasicinteger) (p_adms myadms,int myinteger);
typedef void (*p_valuetobasicreal) (p_adms myadms,double myreal);
typedef void (*p_valuetobasicstring) (p_adms myadms,char *mystring);
/* examples: /a/b => \0,a,b and a/b/ => a,b,\0*/
static p_slist adms_split_new (const char* myname)
{
  p_slist mypath=NULL;
  const char* sj=myname;
  const char* si=myname;
  while(*sj!='\0')
  {
    if((*sj=='/')||(*sj=='\\'))
    {
      if(si==sj)
        adms_slist_push(&mypath,NULL);
      else
        adms_slist_push(&mypath,(p_adms)adms_m2nclone(si,sj));
      si=sj+1;
    }
    sj++;
  }
  adms_slist_push(&mypath,(p_adms)adms_m2nclone(si,sj));
  return adms_slist_reverse(mypath);
}
static void free_strlist (p_slist myli0)
{
  p_slist myli;
  for(myli=myli0;myli;myli=myli->next)
    free(myli->data);
  adms_slist_free(myli0);
}
static char* dirname (const char* myname)
{
  p_slist myli0=adms_split_new(myname);
  char* mydirname=NULL;
  p_slist myli=myli0;
  int first=1;
#if defined(ADMS_OS_MS)
  if((myli->data==NULL)&&myli->next&&(!strcmp((char*)(myli->next->data),"cygdrive")))
  {
    myli=myli->next->next;
    if(myli)
    {
      adms_k2strconcat(&mydirname,(char*)(myli->data));
      adms_k2strconcat(&mydirname,":/");
      myli=myli->next;
    }
    else
      adms_k2strconcat(&mydirname,ADMS_PATH_SEPARATOR);
  }
#endif
  for(;myli;myli=myli->next,first=0)
  {
    if(myli->data==NULL)
      adms_k2strconcat(&mydirname,ADMS_PATH_SEPARATOR);
    else if(myli->next==NULL)
    {
      if(!strcmp((char*)(myli->data),".")||!strcmp((char*)(myli->data),".."))
      {
        if(!first)
          adms_k2strconcat(&mydirname,ADMS_PATH_SEPARATOR);
        adms_k2strconcat(&mydirname,(char*)(myli->data));
      }
    }
    else
    {
      adms_k2strconcat(&mydirname,(char*)(myli->data));
      if(myli->next->next)
        adms_k2strconcat(&mydirname,ADMS_PATH_SEPARATOR);
    }
  }
  free_strlist(myli0);
  if(mydirname)
    return mydirname;
  else
    return adms_kclone(".");
}
static char* basename (const char* myname)
{
  p_slist myli0=adms_split_new(myname);
  char* mybasename=NULL;
  p_slist myli=adms_slist_last(myli0);
  if(!(!strcmp((char*)(myli->data),".")||!strcmp((char*)(myli->data),"..")))
    adms_k2strconcat(&mybasename,(char*)(myli->data));
  free_strlist(myli0);
  return mybasename;
}
static char* filename (const char* myname)
{
  char* myfilename=NULL;
  char* mybasename=basename(myname);
  adms_strconcat(&myfilename,dirname(myname));
  if(mybasename)
  {
    adms_k2strconcat(&myfilename,ADMS_PATH_SEPARATOR);
    adms_strconcat(&myfilename,mybasename);
  }
  return myfilename;
}
/*',,' becomes '' '' ''*/
/*'' becomes ''*/
/*'a\' 'b\' 'c' becomes 'a,b,c' - note: 'a\\' becomes 'a\' - strlen(delimiter) should be 1 */
static p_slist adms_strsplit (const char* string,const char* delimiter,int max_tokens)
{
  p_slist myl=NULL;
  p_slist myslash=NULL;
  p_slist myli0=NULL;
  p_slist myli;
  const char* myrem=string;
  char* occurence=strstr(myrem,delimiter);
  int dlen=strlen(delimiter);
  int stringlen=strlen(myrem);
  while(--max_tokens && occurence)
  {
    int len=(int)(occurence-myrem);
    char* str=adms_knclone(myrem,len);
    adms_slist_push(&myli0,(p_adms)str);
    myrem+=len+dlen;
    if(!*myrem)
      adms_slist_push(&myli0,(p_adms)adms_kclone(""));
    occurence=strstr(myrem,delimiter);
  }
  if(*myrem)
    adms_slist_push(&myli0,(p_adms)adms_knclone(myrem,stringlen-(int)(myrem-string)));
  if(stringlen==0)
    adms_slist_push(&myli0,(p_adms)adms_kclone(""));
  adms_slist_inreverse(&myli0);
  for(myli=myli0;myli;myli=myli->next)
  {
    char*s=(char*)(myli->data);
    int mylength=strlen(s);
    if(myli->next&&mylength&&(*(s+mylength-1)=='\\'))
    {
      *(s+mylength-1)=*delimiter;
      if(!myslash)
        myslash=myli;
    }
    else
    {
      if(myslash)
      {
        p_slist mylj;
        char* myconcat=NULL;
        for(mylj=myslash;mylj!=myli->next;mylj=mylj->next)
          adms_strconcat(&myconcat,(char*)(mylj->data));
        adms_slist_push(&myl,(p_adms)myconcat);
        myslash=NULL;
      }
      else
        adms_slist_push(&myl,myli->data);
    }
  }
  adms_slist_free(myli0);
  return adms_slist_reverse(myl);
}
static void adms_push_admst (p_admst myadmst,p_transform mytransform)
{
  adms_slist_push(&root()->_valueof,(p_adms)myadmst);
  adms_slist_push(&root()->_valueof,(p_adms)mytransform);
  if(myadmst && myadmst->_refd) myadmst->_refd++;
}
 /*[nepasimprimer]*/
static void awrite (p_transform mytransform,p_admst mylhs,p_admst myrhsori)
{
  p_admst myrhs=aread(myrhsori);
  if(mylhs->_pseudo==admse__croix)
  {
    ((p_attribute)mylhs->_item.p)->_value=(p_adms)myrhs;
    if(myrhs) if(myrhs->_refd) myrhs->_refd++;
  }
  else if(!mylhs->_valueto)
  {
    adms_message_fatal_continue(("assignment not supported - lhs=%s rhs=%s\n",ns_etostr(mylhs->_pseudo),ns_etostr(myrhs->_pseudo)))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  else if(!myrhs)
  {
    ((p_valueto)mylhs->_valueto)(mylhs->_previous->_item.p,NULL);
  }
  else if(mylhs->_pseudo==admse_basicenumeration)
  {
    char* myvalues=aprintf(mytransform,myrhs);
    admse mye=ns_strtoe(myvalues);
    ((p_valuetobasicenumeration)mylhs->_valueto)(mylhs->_previous->_item.p,mye);
    free(myvalues);
  }
  else if(mylhs->_pseudo==admse_basicinteger)
  {
    char* myvalues=aprintf(mytransform,myrhs);
    int myi=adms_strtol(mytransform,myvalues);
    ((p_valuetobasicinteger)mylhs->_valueto)(mylhs->_previous->_item.p,myi);
    free(myvalues);
  }
  else if(mylhs->_pseudo==admse_basicreal)
  {
    char* myvalues=aprintf(mytransform,myrhs);
    double myr=adms_strtod(mytransform,myvalues);
    ((p_valuetobasicreal)mylhs->_valueto)(mylhs->_previous->_item.p,myr);
    free(myvalues);
  }
  else if(mylhs->_pseudo==admse_basicstring)
  {
    char* myvalues=aprintf(mytransform,myrhs);
    ((p_valuetobasicstring)mylhs->_valueto)(mylhs->_previous->_item.p,myvalues);
  }
  else if(mylhs->_pseudo==myrhs->_pseudo)
    ((p_valueto)mylhs->_valueto)(mylhs->_previous->_item.p,myrhs->_item.p);
  else if(mylhs->_pseudo==admse_empty)
  {
    if((myrhs->_pseudo==admse_basicenumeration)
    ||(myrhs->_pseudo==admse_basicinteger)
    ||(myrhs->_pseudo==admse_basicreal)
    ||(myrhs->_pseudo==admse_basicstring))
    {
      adms_message_fatal_continue(("assigment not supported - lhs=empty rhs=%s\n",ns_etostr(myrhs->_pseudo)))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    else
      ((p_valueto)mylhs->_valueto)(mylhs->_previous->_item.p,myrhs->_item.p);
  }
  else if(mylhs->_pseudo!=admse_empty && myrhs->_pseudo!=admse_empty && mylhs->_pseudo!=myrhs->_pseudo)
  {
    /* SRW -- Changed this to a verbose message.  This gets triggered when
    we replace a one-line conditional with a block, which must happen
    if we add a line such as for contribution splitting.  Since the
    operation is done below anyway, it shouldn't be considered as an
    error.

    adms_message_error_continue(("mismatch in assigment - lhs=%s rhs=%s\n",ns_etostr(mylhs->_pseudo),ns_etostr(myrhs->_pseudo)))
    adms_message_error(("see %s\n",adms_transform_uid(mytransform)))
    */
    adms_message_verbose(("mismatch in assigment - lhs=%s rhs=%s\n",ns_etostr(mylhs->_pseudo),ns_etostr(myrhs->_pseudo)))
    adms_message_verbose(("see %s\n",adms_transform_uid(mytransform)))
    ((p_valueto)mylhs->_valueto)(mylhs->_previous->_item.p,myrhs->_item.p);
  }
  else
  {
    ((p_valueto)mylhs->_valueto)(mylhs->_previous->_item.p,myrhs->_item.p);
  }
}
static void postx (p_transform mytransform)
{
  p_slist myli;
  postxx(mytransform);
  for(myli=mytransform->_templates;myli;myli=myli->next)
    postx((p_transform)myli->data);
  for(myli=mytransform->_children;myli;myli=myli->next)
    postx((p_transform)myli->data);
}
 /*[nepasimprimer]*/
static p_slist getlist_from_argv (const int argc,const char* *argv,const char* opt,char* argtype)
{
  p_slist mylist=NULL;
  int i;
  for(i=1;i<argc;i++)
  {
    char* myargvalue=NULL;
    const char* value=argv[i];
    if((strlen(value)==2)&&!strcmp(value,"-x"))
      adms_message_verbose(("command option %s: ignored\n",value))
    else if((strlen(value)==3)&&!strcmp(value,"-xv"))
      adms_message_verbose(("command option %s: ignored\n",value))
    else
    {
      if(!strcmp(opt,"-f"))
      {
        if((strlen(value)>2)&&(value[0]=='-'))
        {}
        else if((strlen(value)==2)&&(value[0]=='-'))
          i++;
        else
          myargvalue=adms_kclone(argv[i]);
      }
      if(!strcmp(opt,value))
      {
        if(i<argc-1)
          myargvalue=adms_kclone(argv[++i]);
        else
          adms_message_fatal(("option %s: no value found - expecting %s\n",opt,argtype))
      }
      else if((strlen(value)>2)&&!strncmp(opt,value,2))
        myargvalue=adms_knclone(value+2,strlen(value)-2);
      if(myargvalue)
      {
        char* myunixpath=filename(myargvalue);
        free(myargvalue);
        if(!strcmp(argtype,"file"))
        {
          if(!adms_file_isregular(myunixpath))
            adms_message_fatal(("option %s: cannot read file '%s'\n",opt,myunixpath))
        }
        else if(!strcmp(argtype,"directory"))
        {
          if(!adms_file_isdirectory(myunixpath))
            adms_message_fatal(("option %s: cannot read directory '%s'\n",opt,myunixpath))
        }
        adms_slist_push(&mylist,(p_adms)myunixpath);
        adms_message_verbose(("shift: %s %s\n",opt,myunixpath))
      }
    }
  }
  return adms_slist_reverse(mylist);
}
#include "admsPreprocessor.h"
#include "admsVeriloga.h"
 /*[nepasimprimer]*/
static void parseva (const int argc,const char** argv,char* myverilogamsfile)
{
  p_slist myli;
  char* mytmpverilogamsfile=NULL;
  adms_k2strconcat(&mytmpverilogamsfile,".");
  adms_strconcat(&mytmpverilogamsfile,basename(myverilogamsfile));
  adms_k2strconcat(&mytmpverilogamsfile,".adms");
  root()->_filename=basename(myverilogamsfile);
  root()->_fullfilename=adms_kclone(myverilogamsfile);
  root()->_curfilename=adms_kclone(myverilogamsfile);
  adms_message_info(("%sXml-%s (%s) %s %s\n",PACKAGE_NAME,PACKAGE_VERSION,GIT,__DATE__,__TIME__))
  /* preprocess input file */
  {
    p_preprocessor mypreprocessor=(p_preprocessor)malloc(sizeof(t_preprocessor));
    FILE* myverilogamsfilefh=adms_file_open_read(myverilogamsfile);
    FILE* ofh=fopen(mytmpverilogamsfile,"wb");
    if(!ofh)
      adms_message_fatal(("%s: failed to open file [write mode]\n",mytmpverilogamsfile))
    adms_preprocessor_setfile_input(myverilogamsfilefh);
    mypreprocessor->cur_line_position=1;
    mypreprocessor->cur_char_position=1;
    mypreprocessor->cur_message=NULL;
    mypreprocessor->fid=myverilogamsfilefh;
    mypreprocessor->filename=adms_kclone(myverilogamsfile);
    mypreprocessor->buffer=NULL;
    mypreprocessor->cur_continuator_position=NULL;
    adms_preprocessor_valueto_main((p_preprocessor_main)malloc(sizeof(t_preprocessor_main)));
    pproot()->Defined=NULL;
    pproot()->Scanner=NULL;
    pproot()->Text=NULL;
    pproot()->cr_filename=adms_kclone(myverilogamsfile);
    pproot()->cr_scanner=mypreprocessor;
    pproot()->error=0;
    adms_slist_push(&pproot()->skipp_text,(p_adms)(long)(0));
    pproot()->includePath=getlist_from_argv(argc,argv,"-I","directory");
    adms_slist_push(&pproot()->includePath,(p_adms)".");
    adms_preprocessor_get_define_from_argv(argc,argv);
    adms_preprocessor_define_add_default("insideADMS");
    adms_message_verbose(("create temporary file %s\n",mytmpverilogamsfile))
    (int) preprocessorparse();
    /* save preprocessed Verilog-AMS file */
    fputs("# 1 \"",ofh);
    fputs(pproot()->cr_scanner->filename,ofh);
    fputs("\"\n",ofh);
    adms_slist_inreverse(&pproot()->Text);
    for(myli=pproot()->Text;myli;myli=myli->next)
      fputs(((p_preprocessor_text)(myli->data))->_str,ofh);
    fclose(ofh);
    /* free preprocessor */
    free(mypreprocessor->filename);
    free(pproot()->cr_scanner);
    fclose(myverilogamsfilefh);
  }
  /* parses temporary file */
  {
    FILE* ifh=adms_file_open_read(mytmpverilogamsfile);
    if(root()->_dbg_vla==admse_yes)
      adms_veriloga_setint_yydebug(1);
    adms_veriloga_setfile_input(ifh);
    if(verilogaparse(root()))
      adms_message_fatal(("parse Verilog-AMS preprocessed file '%s' failed\n",mytmpverilogamsfile))
    else
      adms_message_verbose(("No error found during parsing\n"))
    fclose(ifh);
  }
  for(myli=root()->_module;myli;myli=myli->next)
    ((p_module)myli->data)->_evaluation=adms_evaluation_new();
  free(mytmpverilogamsfile);
  free(myverilogamsfile);
}
 /*[nepasimprimer]*/
static void Xattribute (p_transform mytransform,p_admst dot,p_admst dotdot)
{
}
static void Xnotadmst (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist myli;
  p_slist Attribute=mytransform->_attribute;
  fputs("<",OUT());
  fputs(mytransform->_name,OUT());
  while(Attribute)
  {
    p_attribute myattribute=(p_attribute)Attribute->data;
    char* value=tsprintf(dot,(p_text)myattribute->_value);
    fputs(" ",OUT());
    fputs(myattribute->_name,OUT());
    fputs("=\"",OUT());
    fputs(value,OUT());
    free(value);
    fputs("\"",OUT());
    Attribute=Attribute->next;
  }
  for(myli=mytransform->_children;myli;myli=myli->next)
  {
    p_transform mychild=(p_transform)myli->data;
    if(mychild->_callback==(void*)Xattribute)
    {
      char* name=tsprintf(dot,mychild->_textname);
      fputs(" ",OUT());
      fputs(name,OUT());
      free(name);
      fputs("=\"",OUT());
      xtraverse(mychild->_children,dot,dotdot);
      fputs("\"",OUT());
    }
  }
  fputs(">\n",OUT());
  xtraverse(mytransform->_children,dot,dotdot);
  fputs("</",OUT());
  fputs(mytransform->_name,OUT());
  fputs(">\n",OUT());
}
 /*[nepasimprimer]*/
static int myskipversion=0;
static void Xadmst (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(!myskipversion&&mytransform->_textversion)
  {
    char* installed=PACKAGE_VERSION;
    p_slist Installed=adms_strsplit(installed,".",3);
    int installed_major=strtol((char*)(Installed->data),(char* *)NULL,10);
    int installed_minor=strtol((char*)(Installed->next->data),(char* *)NULL,10);
    char* requested=tsprintf(dot,mytransform->_textversion);
    p_slist Requested=adms_strsplit(requested,".",3);
    int xmlrequested_major;
    int xmlrequested_minor;
    if(!(Requested&&Requested->next&&Requested->next->next))
      adms_message_fatal(("%s: invalid version number\n",adms_transform_uid(mytransform)))
    errno=0;
    xmlrequested_major=strtol((char*)(Requested->data),(char* *)NULL,10);
    xmlrequested_minor=strtol((char*)(Requested->next->data),(char* *)NULL,10);
    if(errno)
    {
      adms_message_fatal_continue(("'%s' invalid number - expecting for instance %s\n",requested,installed))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    if((xmlrequested_major>installed_major)||(xmlrequested_major==installed_major && xmlrequested_minor>installed_minor))
    {
      adms_message_fatal_continue(("adms version too old (requested by xml control file: %s installed adms version: %s)\n",requested,installed))
      adms_message_fatal_continue(("please install more recent version of adms\n"))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    if((xmlrequested_major<installed_major)||(xmlrequested_major==installed_major && xmlrequested_minor<installed_minor))
    {
      adms_message_warning(("adms version too recent (requested by xml control file: %s installed adms version: %s)\n",requested,installed))
      adms_message_warning(("please upgrade your xml control files or use other version of adms\n"))
      adms_message_warning(("see %s\n",adms_transform_uid(mytransform)))
    }
    free(requested);
    free_strlist(Installed);
    free_strlist(Requested);
  }
  xtraverse(mytransform->_children,dot,dotdot);
}
 /*[nepasimprimer]*/
static void Xforeach (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist myli;
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  for(myli=pselect->_admst;myli;myli=myli->next)
    xtraverse(mytransform->_children,(p_admst)myli->data,dot);
  free_ptraverse(pselect);
}
static void Xjoin (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist myli;
  char* separator=(mytransform->_textseparator)?tsprintf(dot,mytransform->_textseparator):NULL;
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  for(myli=pselect->_admst;myli;myli=myli->next)
  {
    xtraverse(mytransform->_children,(p_admst)myli->data,dot);
    if(myli->next&&separator)
      fputs(separator,OUT());
  }
  free(separator);
  free_ptraverse(pselect);
}
static void Xbreak (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  break_found=1;
}
static void Xvalueof (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  if(pselect->_admst)
  {
    p_slist myli=pselect->_admst;
    for(;myli;myli=myli->next)
    {
      p_admst myadmst=(p_admst)myli->data;
      adms_push_admst(myadmst,mytransform);
    }
  }
  else
    adms_push_admst(NULL,mytransform);
  free_ptraverse(pselect);
}
 /*[nepasimprimer]*/
/*bug: if-inside behaves like if-not-inside when 'select' is not a $var*/
static int Find (p_slist l,p_kadms data)
{
  while(l)
  {
    if(l->data==data)
      return 1;
    l=l->next;
  }
  return 0;
}
static int Xifinsidep_isempty (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_admst mystackinto=adms_pull_admst(mytransform);
  if(mystackinto)
  {
    if(mystackinto->_pseudo==admse_basiclist)
    {
      adms_message_fatal_continue(("'into' is a dollar variable - this is not supported\n"))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    else if(mystackinto->_pseudo==admse__ladms)
    {
      p_slist into=(p_slist)mystackinto->_item.p;
      p_ptraverse pselect=bar(dot,mytransform->_pathselect);
      p_slist lselect;
      for(lselect=pselect->_admst;lselect;lselect=lselect->next)
      {
        p_admst myadmst=(p_admst)lselect->data;
        if(myadmst->_pseudo==admse_basiclist)
        {
          adms_message_fatal_continue(("'select' is a dollar variable - this is not supported\n"))
          adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
        }
        else if(myadmst->_pseudo==admse__ladms)
        {
          p_slist mylii;
          for(mylii=(p_slist)myadmst->_item.p;mylii;mylii=mylii->next)
            if(!Find(into,mylii->data))
            {
              free_ptraverse(pselect);
              deref(mystackinto);
              return 0;
            }
        }
        else
          if(Find(into,myadmst->_item.p))
          {
            free_ptraverse(pselect);
            deref(mystackinto);
            return 0;
          }
      }
      free_ptraverse(pselect);
    }
    deref(mystackinto);
  }
  return 1;
}
 /*[nepasimprimer]*/
static void Xifinsidep (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(!Xifinsidep_isempty(mytransform,dot,dotdot))
    xtraverse(mytransform->_children,dot,dotdot);
}
static void Xifnotinsidep (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(Xifinsidep_isempty(mytransform,dot,dotdot))
    xtraverse(mytransform->_children,dot,dotdot);
}
 /*[nepasimprimer]*/
static int Xifinsidepath_isempty (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse plist=bar(dot,mytransform->_pathlist);
  p_slist linto;
  for(linto=plist->_admst;linto;linto=linto->next)
  {
    p_admst ainto=(p_admst)linto->data;
    if(ainto->_pseudo==admse_basiclist)
    {
      adms_message_fatal_continue(("'into' is a dollar variable - this is not supported\n"))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
    else if(ainto->_pseudo==admse__ladms)
    {
      p_slist into=(p_slist)ainto->_item.p;
      if(into)
      {
        p_ptraverse pselect=bar(dot,mytransform->_pathselect);
        p_slist lselect;
        for(lselect=pselect->_admst;lselect;lselect=lselect->next)
        {
          p_admst aselect=(p_admst)lselect->data;
          if(aselect->_pseudo==admse_basiclist)
          {
            adms_message_fatal_continue(("'select' is a dollar variable - this is not supported\n"))
            adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
          }
          else if(aselect->_pseudo==admse__ladms)
          {
            p_slist myli=(p_slist)aselect->_item.p;
            for(;myli;myli=myli->next)
              if(!Find(into,((p_admst)myli->data)->_item.p))
              {
                free_ptraverse(pselect);
                free_ptraverse(plist);
                return 0;
              }
          }
          else if(Find(into,aselect->_item.p))
          {
            free_ptraverse(pselect);
            free_ptraverse(plist);
            return 0;
          }
        }
        free_ptraverse(pselect);
      }
    }
  }
  free_ptraverse(plist);
  return 1;
}
static void Xifinsidepath (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(!Xifinsidepath_isempty(mytransform,dot,dotdot))
    xtraverse(mytransform->_children,dot,dotdot);
}
static void Xifnotinsidepath (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(Xifinsidepath_isempty(mytransform,dot,dotdot))
    xtraverse(mytransform->_children,dot,dotdot);
}
 /*[nepasimprimer]*/
static void Xchoose (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist myli;
  for(myli=mytransform->_children;myli;myli=myli->next)
  {
    p_transform mychild=(p_transform)myli->data;
    if(!strcmp(mychild->_name,"admst:when"))
    {
      p_ptraverse ptest=bar(dot,mychild->_pathtest);
      if(ptest->_admst&&((p_admst)ptest->_admst->data)->_item.p)
      {
        xtraverse(mychild->_children,dot,dotdot);
        free_ptraverse(ptest);
        return;
      }
      free_ptraverse(ptest);
    }
    else if(!strcmp(mychild->_name,"admst:otherwise"))
    {
      xtraverse(mychild->_children,dot,dotdot);
      return;
    }
    else
      adms_message_fatal(("%s: %s admst transform unexpected",adms_transform_uid(mytransform),adms_transform_uid(mychild)))
  }
}
 /*[nepasimprimer]*/
static void Xif (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  xtraverse(mytransform->_children,dot,dotdot);
}
 /*[nepasimprimer]*/
static void Xreturn (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  char* myname=tsprintf(dot,mytransform->_textname);
  char* mystring=tsprintf(dot,mytransform->_textstring);
  p_admst d=adms_admst_newpa(dot,dot,(p_adms)adms_return_new(myname,mystring));
  d->_hasnewbasicstring=2;
  free(myname);
  free(mystring);
  if(root()->_itransform)
  {
    d->_refd++;
    adms_slist_push(&((p_itransform)root()->_itransform->data)->_return,(p_adms)d);
    return;
  }
  adms_message_fatal_continue(("admst:return only allowed inside admst:template\n"))
  adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
}
 /*[nepasimprimer]*/
static void Xapplytemplates (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_slist myl0;
  for(myl0=pselect->_admst;myl0;myl0=myl0->next)
  {
    p_slist myl1;
    char* match=tsprintf((p_admst)myl0->data,mytransform->_textmatch);
    p_itransform myitransform;
    p_transform mytemplate=lookfortemplates(mytransform,match);
    if(!mytemplate)
    {
      adms_message_fatal_continue(("Template not found:\n"))
      adms_message_fatal_continue(("  <admst:template match=\"%s\"/>\n",match))
      adms_message_fatal(("  see:  %s\n",adms_transform_uid(mytransform)))
    }
    free(match);
    myitransform=adms_itransform_new(mytemplate);
    myitransform->_originalcall=root()->_itransform?((p_itransform)root()->_itransform->data)->_originalcall:mytransform;
    adms_slist_push(&root()->_itransform,(p_adms)myitransform);
    if(((p_admst)myl0->data)->_item.p)
    {
      p_slist l;
      xtraverse(mytemplate->_children,(p_admst)myl0->data,dot);
      for(l=myitransform->_variable;l;l=l->next)
      {
        p_slist li;
        for(li=((p_admstvariable)l->data)->_value;li;li=li->next)
          deref((p_admst)li->data);
        adms_admstvariable_free((p_admstvariable)l->data);
      }
    }
    adms_slist_pull(&root()->_itransform);
    adms_slist_push(&root()->_itransforminsideapplytemplate,(p_adms)myitransform);
    xtraverse(mytransform->_children,(p_admst)myl0->data,dot);
    break_found=0;
    for(myl1=myitransform->_return;myl1;myl1=myl1->next)
    {
      p_admst myadmst=(p_admst)myl1->data;
      myadmst->_refd--;
      if(myadmst->_refd==1)
      {
        adms_return_free((p_return)myadmst->_item.p);
        adms_admst_free(myadmst);
      }
    }
    adms_itransform_free(myitransform);
    adms_slist_pull(&root()->_itransforminsideapplytemplate);
  }
  free_ptraverse(pselect);
}
 /*[nepasimprimer]*/
static p_admstvariable getdollar (p_transform mytransform,p_admst dot)
{
  char* tname=tsprintf(dot,mytransform->_textname);
  p_admstvariable mydollar=lookup_dollar(tname);
  if(!mydollar)
  {
    mydollar=adms_admstvariable_new(tname);
    if(root()->_itransform)
    {
      p_itransform myitransform=(p_itransform)root()->_itransform->data;
      adms_slist_push(&myitransform->_variable,(p_adms)mydollar);
    }
    else
      adms_slist_push(&root()->_variable,(p_adms)mydollar);
  }
  free(tname);
  return mydollar;
}
static void derefprevious (p_slist v)
{
  p_slist l;
  for(l=v;l;l=l->next)
  {
    ((p_admst)l->data)->_refd--;
    deref((p_admst)l->data);
  }
  adms_slist_free(v);
}
static p_admstvariable getdollarnulled (p_transform mytransform,p_admst dot)
{
  p_admstvariable mydollar=getdollar(mytransform,dot);
  derefprevious(mydollar->_value);
  mydollar->_value=NULL;
  return mydollar;
}
static void Xvariablenull (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  (void)getdollarnulled(mytransform,dot);
}
static void Xvariablep (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  adms_slist_push(&getdollarnulled(mytransform,dot)->_value,(p_adms)adms_pull_admst(mytransform));
}
static void Xvariableconstant (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  adms_slist_push(&getdollarnulled(mytransform,dot)->_value,mytransform->_textstring->_token->data);
}
static void Xvariablestring (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_admstvariable mydollar=getdollar(mytransform,dot);
  p_slist mypreviousvalue=mydollar->_value;
  p_admst myadmst=adms_admst_newns(dot,dot,tsprintf(dot,mytransform->_textstring));
  mydollar->_value=NULL;
  adms_slist_push(&mydollar->_value,(p_adms)myadmst);
  if(myadmst->_refd) myadmst->_refd++;
  derefprevious(mypreviousvalue);
}
static void Xvariable (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist l;
  p_slist ll;
  p_admstvariable mydollar=getdollar(mytransform,dot);
  p_slist mypreviousvalue=mydollar->_value;
  p_ptraverse ppath=bar(dot,mytransform->_pathpath);
  mydollar->_value=ppath->_admst;
  for(l=mydollar->_value;l;l=l->next)
  {
    if(((p_admst)l->data)->_refd)
     ((p_admst)l->data)->_refd++;
    if(((p_admst)l->data)->_hasnewbasicstring!=2)
      ((p_admst)l->data)->_refd++;
  }
  for(l=ppath->_alladmst;l;l=l->next)
    for(ll=(p_slist)l->data;ll;ll=ll->next)
      deref((p_admst)ll->data);
  for(l=ppath->_aliasadmst;l;l=l->next)
    deref((p_admst)l->data);
  for(l=ppath->_admst;l;l=l->next)
    deref((p_admst)l->data);
  adms_slist_free(ppath->_alladmst);
  adms_slist_free(ppath->_aliasadmst);
  free(ppath);
  derefprevious(mypreviousvalue);
}
 /*[nepasimprimer]*/
static void Xvaluetop (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_slist myli;
  for(myli=pselect->_admst;myli;myli=myli->next)
    awrite(mytransform,(p_admst)myli->data,adms_pull_admst(mytransform));
  free_ptraverse(pselect);
}
static void Xvaluetopath (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_ptraverse ptstring=bar(dot,(p_path)mytransform->_textstring->_token->data);
  p_slist myli=pselect->_admst;
  p_slist mylii=ptstring->_admst;
  for(;myli&&mylii;myli=myli->next,mylii=mylii->next?mylii->next:mylii)
    awrite(mytransform,(p_admst)myli->data,(p_admst)mylii->data);
  free_ptraverse(ptstring);
  free_ptraverse(pselect);
}
static void Xvaluetonull (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_slist myli=pselect->_admst;
  for(;myli;myli=myli->next)
  {
    p_admst myadmst=(p_admst)myli->data;
    if(myadmst->_pseudo==admse_basiclist)
      ((p_admstvariable)myadmst->_item.p)->_value=NULL;
    else if(myadmst->_pseudo==admse__ladms || myadmst->_pseudo==admse__ladmst)
      awrite(mytransform,(p_admst)myli->data,NULL);
    else if(myadmst->_pseudo==admse__croix)
    {
      p_attribute myattribute=(p_attribute)myadmst->_item.p;
      myattribute->_value=NULL;
    }
  }
  free_ptraverse(pselect);
}
static void Xvaluetoconstant (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_slist myli;
  p_admst myconstant=(p_admst)mytransform->_textstring->_token->data;
  for(myli=pselect->_admst;myli;myli=myli->next)
    awrite(mytransform,(p_admst)myli->data,myconstant);
  free_ptraverse(pselect);
}
static void Xvaluetostring (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_slist l;
  char* tvalue=tsprintf(dot,mytransform->_textstring);
  p_admst myadmst=adms_admst_newns(dot,dot,tvalue);
  for(l=pselect->_admst;l;l=l->next)
    awrite(mytransform,(p_admst)l->data,myadmst);
  deref(myadmst);
  free_ptraverse(pselect);
}
static void Xvalueto (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_ptraverse ppath=bar(dot,mytransform->_pathpath);
  p_slist myli;
  p_slist mylii=ppath->_admst;
  if(mylii)
    for(myli=pselect->_admst;myli;myli=myli->next,mylii=mylii->next?mylii->next:mylii)
      awrite(mytransform,(p_admst)myli->data,(p_admst)mylii->data);
  else
    for(myli=pselect->_admst;myli;myli=myli->next)
      awrite(mytransform,(p_admst)myli->data,NULL);
  free_ptraverse(ppath);
  free_ptraverse(pselect);
}

 /*[nepasimprimer]*/
static void Xtext (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(mytransform->_textformat==NULL)
    return;
  if(mytransform->_pathselect)
  {
    p_ptraverse pselect=bar(dot,mytransform->_pathselect);
    p_slist myli;
    for(myli=pselect->_admst;myli;myli=myli->next)
      tprintf((p_admst)myli->data,mytransform->_textformat);
    free_ptraverse(pselect);
  }
  else
    tprintf(dot,mytransform->_textformat);
}
 /*[nepasimprimer]*/
static void Xmessage (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(mytransform->_pathselect)
  {
    p_ptraverse pselect=bar(dot,mytransform->_pathselect);
    p_slist myli;
    for(myli=pselect->_admst;myli;myli=myli->next)
    {
      char* text=tsprintf((p_admst)myli->data,mytransform->_textformat);
      adms_message_info(("%s",text))
      free(text);
    }
    free_ptraverse(pselect);
  }
  else
  {
    char* text=tsprintf(dot,mytransform->_textformat);
    adms_message_info(("%s",text))
    free(text);
  }
}
static void Xwarning (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(mytransform->_pathselect)
  {
    p_ptraverse pselect=bar(dot,mytransform->_pathselect);
    p_slist myli;
    for(myli=pselect->_admst;myli;myli=myli->next)
    {
      char* text=tsprintf((p_admst)myli->data,mytransform->_textformat);
      adms_message_warning(("%s",text))
      free(text);
    }
    free_ptraverse(pselect);
  }
  else
  {
    char* text=tsprintf(dot,mytransform->_textformat);
    adms_message_warning(("%s",text))
    free(text);
  }
}
static void Xerror (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(mytransform->_pathselect)
  {
    p_ptraverse pselect=bar(dot,mytransform->_pathselect);
    p_slist myli;
    for(myli=pselect->_admst;myli;myli=myli->next)
    {
      char* text=tsprintf((p_admst)myli->data,mytransform->_textformat);
      adms_message_error(("%s",text))
      free(text);
    }
    free_ptraverse(pselect);
  }
  else
  {
    char* text=tsprintf(dot,mytransform->_textformat);
    adms_message_error(("%s",text))
    free(text);
  }
}
static void Xfatal (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(mytransform->_pathselect)
  {
    p_ptraverse pselect=bar(dot,mytransform->_pathselect);
    p_slist myli;
    for(myli=pselect->_admst;myli;myli=myli->next)
    {
      char* text=tsprintf((p_admst)myli->data,mytransform->_textformat);
      adms_message_fatal_continue(("%s",text))
      free(text);
    }
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    free_ptraverse(pselect);
  }
  else
  {
    char* text=tsprintf(dot,mytransform->_textformat);
    adms_message_fatal_continue(("%s",text))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    free(text);
  }
}
 /*[nepasimprimer]*/
static void Xnewarguments (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_admst myadmst;
  int size=adms_slist_length(mytransform->_textarguments);
  p_slist* Arguments=(p_slist*)calloc(size,sizeof(p_admst));
  char* tdatatype=tsprintf(dot,mytransform->_textdatatype);
  p_new new=!strcmp(tdatatype,"quark")?adms_admsxml_new(mytransform,"basicstring"):adms_admsxml_new(mytransform,tdatatype);
  int i=0;
  p_slist myli;
  free(tdatatype);
  for(myli=mytransform->_textarguments;myli;myli=myli->next)
  {
    p_text myargname=(p_text)myli->data;
    if(myargname->_admse==admse__path)
    {
      p_ptraverse myptraverse=bar(dot,(p_path)myargname->_token->data);
      Arguments[i]=myptraverse->_admst;
      adms_slist_free(myptraverse->_alladmst);
      free(myptraverse);
    }
    else if(myargname->_admse==admse__p)
    {
      Arguments[i]=NULL;
      adms_slist_push(&Arguments[i],(p_adms)adms_pull_admst(mytransform));
    }
    else
    {
      p_admst mynewadmst=adms_admst_newns(dot,dot,tsprintf(dot,myargname));
      Arguments[i]=NULL;
      adms_slist_push(&Arguments[i],(p_adms)mynewadmst);
    }
    i++;
  }
  myadmst=new(mytransform,dot,Arguments,size);
  xtraverse(mytransform->_children,myadmst,dot);
  deref(myadmst);
  free(Arguments);
}
static void Xnew (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_admst myadmst;
  int size=mytransform->_pathinputs?adms_slist_length(mytransform->_pathinputs):1;
  p_slist* inputs=(p_slist*)calloc(size,sizeof(p_admst));
  p_ptraverse* ptraverseinputs=(p_ptraverse*)calloc(size,sizeof(p_ptraverse));
  char* tdatatype=tsprintf(dot,mytransform->_textdatatype);
  p_new new=!strcmp(tdatatype,"quark")?adms_admsxml_new(mytransform,"basicstring"):adms_admsxml_new(mytransform,tdatatype);
  int i=0;
  p_slist myli;
  free(tdatatype);
  for(myli=mytransform->_pathinputs;myli;myli=myli->next)
  {
    p_ptraverse myptraverse=bar(dot,(p_path)myli->data);
    inputs[i]=myptraverse->_admst;
    ptraverseinputs[i]=myptraverse;
    i++;
  }
  myadmst=new(mytransform,dot,inputs,size);
  xtraverse(mytransform->_children,myadmst,dot);
  deref(myadmst);
  for(i=0;i<size;i++)
    free_ptraverse(ptraverseinputs[i]);
  free(inputs);
}
 /*[nepasimprimer]*/
static void Xpush (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist myli;
  p_slist mylii;
  p_ptraverse pinto=bar(dot,mytransform->_pathinto);
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  for(myli=pinto->_admst;myli;myli=myli->next)
  {
    p_admst myadmsti=(p_admst)myli->data;
    if(myadmsti->_pseudo==admse_basiclist)
    {
      p_slist mylistinto=((p_admstvariable)myadmsti->_item.p)->_value;
      for(mylii=pselect->_admst;mylii;mylii=mylii->next)
      {
        p_admst myadmstii=aread((p_admst)mylii->data);
        adms_slist_push(&mylistinto,(p_adms)myadmstii);
        if(myadmstii->_refd) myadmstii->_refd++;
      }
      ((p_admstvariable)myadmsti->_item.p)->_value=mylistinto;
    }
    else if(myadmsti->_pseudo==admse__ladmst)
    {
      p_slist mylistinto=(p_slist)myadmsti->_item.p;
      for(mylii=pselect->_admst;mylii;mylii=mylii->next)
      {
        p_admst myadmstii=aread((p_admst)mylii->data);
        adms_slist_push(&mylistinto,(p_adms)myadmstii);
        if(myadmstii->_refd) myadmstii->_refd++;
      }
      ((p_valueto)myadmsti->_valueto)(myadmsti->_previous->_item.p,(p_adms)mylistinto);
    }
    else if(myadmsti->_pseudo==admse__ladms)
    {
      p_slist mylistinto=(p_slist)myadmsti->_item.p;
      for(mylii=pselect->_admst;mylii;mylii=mylii->next)
      {
        p_admst myadmstii=aread((p_admst)mylii->data);
        if(myadmstii->_pseudo==admse_basicstring)
          adms_slist_push(&mylistinto,(p_adms)adms_kclone(myadmstii->_item.s));
        else
          adms_slist_push(&mylistinto,myadmstii->_item.p);
      }
      ((p_valueto)myadmsti->_valueto)(myadmsti->_previous->_item.p,(p_adms)mylistinto);
    }
  }
  free_ptraverse(pselect);
  free_ptraverse(pinto);
}

static void Xpushonduplicate (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist myli;
  p_slist mylii;
  p_ptraverse pinto=bar(dot,mytransform->_pathinto);
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  for(myli=pinto->_admst;myli;myli=myli->next)
  {
    p_admst myadmsti=(p_admst)myli->data;
    if(myadmsti->_pseudo==admse_basiclist)
    {
      p_slist mylistinto=((p_admstvariable)myadmsti->_item.p)->_value;
      for(mylii=pselect->_admst;mylii;mylii=mylii->next)
      {
        p_admst myadmstii=aread((p_admst)mylii->data);
        admse mytype0=myadmstii->_pseudo;
        int found=0;
        p_adms myadms0=myadmstii->_item.p;
        p_cmp mycmp=adms_admsxml_cmp(myadmstii);
        p_slist myliii;
        for(myliii=mylistinto;myliii&&!found;myliii=myliii->next)
        {
          p_admst myadmst1=(p_admst)myliii->data;
          p_adms myadms1=myadmst1->_item.p;
          admse mytype1=myadmst1->_pseudo;
          found=(mytype0==mytype1)&&(!mycmp(myadms0,myadms1));
        }
        if(!found)
        {
          adms_slist_push(&mylistinto,(p_adms)myadmstii);
          if(myadmstii->_refd) myadmstii->_refd++;
        }
      }
      ((p_admstvariable)myadmsti->_item.p)->_value=mylistinto;
    }
    else if(myadmsti->_pseudo==admse__ladmst)
    {
      p_slist mylistinto=(p_slist)myadmsti->_item.p;
      for(mylii=pselect->_admst;mylii;mylii=mylii->next)
      {
        p_admst myadmstii=aread((p_admst)mylii->data);
        admse mytype0=myadmstii->_pseudo;
        int found=0;
        p_adms myadms0=myadmstii->_item.p;
        p_cmp mycmp=adms_admsxml_cmp(myadmstii);
        p_slist myliii;
        for(myliii=mylistinto;myliii&&!found;myliii=myliii->next)
        {
          p_admst myadmst1=(p_admst)myliii->data;
          p_adms myadms1=myadmst1->_item.p;
          admse mytype1=myadmst1->_pseudo;
          found=(mytype0==mytype1)&&(!mycmp(myadms0,myadms1));
        }
        if(!found)
        {
          adms_slist_push(&mylistinto,(p_adms)myadmstii);
          if(myadmstii->_refd) myadmstii->_refd++;
        }
      }
      ((p_valueto)myadmsti->_valueto)(myadmsti->_previous->_item.p,(p_adms)mylistinto);
    }
    else if(myadmsti->_pseudo==admse__ladms)
    {
      p_slist mylistinto=(p_slist)myadmsti->_item.p;
      for(mylii=pselect->_admst;mylii;mylii=mylii->next)
      {
        p_admst myadmstii=aread((p_admst)mylii->data);
        admse mytype0=myadmstii->_pseudo;
        int found=0;
        p_adms myadms0=myadmstii->_item.p;
        p_cmp mycmp=adms_admsxml_cmp(myadmstii);
        p_slist myliii;
        for(myliii=mylistinto;myliii&&!found;myliii=myliii->next)
        {
          p_adms myadms1=myliii->data;
          admse mytype1=myadms1?myadms1->_datatypename:admse_empty;
          found=(mytype0==mytype1)&&(!mycmp(myadms0,myadms1));
        }
        if(!found)
        {
          if(myadmstii->_pseudo==admse_basicstring)
            adms_slist_push(&mylistinto,(p_adms)adms_kclone(myadmstii->_item.s));
          else
            adms_slist_push(&mylistinto,myadmstii->_item.p);
        }
      }
      ((p_valueto)myadmsti->_valueto)(myadmsti->_previous->_item.p,(p_adms)mylistinto);
    }
  }
  free_ptraverse(pselect);
  free_ptraverse(pinto);
}

static int oncompare (p_ptraverse poncompareinto0, p_ptraverse poncompareinto1)
{
  p_slist myl0=poncompareinto0->_admst;
  p_slist myl1=poncompareinto1->_admst;
  int allthesame=1;
  if(adms_slist_length(myl0)!=adms_slist_length(myl1))
    allthesame=0;
  else
  {
    for(;myl0&&allthesame;myl0=myl0->next,myl1=myl1->next)
    {
      p_admst myadmstcmp0=(p_admst)myl0->data;
      p_admst myadmstcmp1=(p_admst)myl1->data;
      allthesame=(myadmstcmp0->_item.p==myadmstcmp1->_item.p);
    }
  }
  free_ptraverse(poncompareinto1);
  return allthesame;
}
static int oncompare1 (p_ptraverse poncompareinto0, p_admst ai)
{
  int found=0;
  p_slist lii=(p_slist)ai->_item.p;
  for(;lii&&!found;lii=lii->next)
  {
    p_admst d=adms_admst_newpa(ai,ai,lii->data);
    p_ptraverse poncompareinto1=bar(d,poncompareinto0->_transform->_pathoncompare);
    found=oncompare(poncompareinto0,poncompareinto1);
    adms_admst_free(d);
  }
  return found;
}
static void Xpushoncompare (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_slist myli;
  p_slist mylii;
  p_ptraverse pinto=bar(dot,mytransform->_pathinto);
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  for(myli=pinto->_admst;myli;myli=myli->next)
  {
    p_admst myadmsti=(p_admst)myli->data;
    if(myadmsti->_pseudo==admse_basiclist)
    {
      p_slist mylistinto=((p_admstvariable)myadmsti->_item.p)->_value;
        for(mylii=pselect->_admst;mylii;mylii=mylii->next)
        {
          p_admst myadmstii=aread((p_admst)mylii->data);
          p_ptraverse poncompareinto0=bar(myadmstii,mytransform->_pathoncompare);
          int found=0;
          p_slist myliii;
          for(myliii=mylistinto;myliii&&!found;myliii=myliii->next)
            found=oncompare(poncompareinto0,bar((p_admst)myliii->data,mytransform->_pathoncompare));
          if(!found)
          {
            adms_slist_push(&mylistinto,(p_adms)myadmstii);
            if(myadmstii->_refd) myadmstii->_refd++;
          }
          free_ptraverse(poncompareinto0);
        }
      ((p_admstvariable)myadmsti->_item.p)->_value=mylistinto;
    }
    else if(myadmsti->_pseudo==admse__ladmst)
    {
      p_slist mylistinto=(p_slist)myadmsti->_item.p;
        for(mylii=pselect->_admst;mylii;mylii=mylii->next)
        {
          p_admst myadmstii=aread((p_admst)mylii->data);
          p_ptraverse poncompareinto0=bar(myadmstii,mytransform->_pathoncompare);
          int found=0;
          p_slist myliii;
          for(myliii=mylistinto;myliii&&!found;myliii=myliii->next)
            found=oncompare(poncompareinto0,bar((p_admst)myliii->data,mytransform->_pathoncompare));
          if(!found)
          {
            adms_slist_push(&mylistinto,(p_adms)myadmstii);
            if(myadmstii->_refd) myadmstii->_refd++;
          }
          free_ptraverse(poncompareinto0);
        }
      ((p_valueto)myadmsti->_valueto)(myadmsti->_previous->_item.p,(p_adms)mylistinto);
    }
    else if(myadmsti->_pseudo==admse__ladms)
    {
      p_slist mylistinto=(p_slist)myadmsti->_item.p;
      for(mylii=pselect->_admst;mylii;mylii=mylii->next)
      {
        p_admst myadmstii=aread((p_admst)mylii->data);
        p_ptraverse poncompareinto0=bar(myadmstii,mytransform->_pathoncompare);
        int found=0;
        found=oncompare1(poncompareinto0,myadmsti);
        if(!found)
        {
          if(myadmstii->_pseudo==admse_basicstring)
            adms_slist_push(&mylistinto,(p_adms)adms_kclone(myadmstii->_item.s));
          else
            adms_slist_push(&mylistinto,myadmstii->_item.p);
        }
        free_ptraverse(poncompareinto0);
      }
      ((p_valueto)myadmsti->_valueto)(myadmsti->_previous->_item.p,(p_adms)mylistinto);
    }
  }
  free_ptraverse(pselect);
  free_ptraverse(pinto);
}
 /*[nepasimprimer]*/
static void Xreset (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_slist myli=pselect->_admst;
  for(;myli;myli=myli->next)
  {
    p_admst myadmst=(p_admst)myli->data;
    if(myadmst->_pseudo==admse_basiclist)
      ((p_admstvariable)myadmst->_item.p)->_value=NULL;
    else if(myadmst->_pseudo==admse__ladms || myadmst->_pseudo==admse__ladmst)
      awrite(mytransform,(p_admst)myli->data,NULL);
  }
  free_ptraverse(pselect);
}
 /*[nepasimprimer]*/
static void Xreverse (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  p_slist myli=pselect->_admst;
  for(;myli;myli=myli->next)
  {
    p_admst myadmst=(p_admst)myli->data;
    if(myadmst->_pseudo==admse_basiclist)
      adms_slist_inreverse(&((p_admstvariable)myadmst->_item.p)->_value);
    else if(myadmst->_pseudo==admse__ladms || myadmst->_pseudo==admse__ladmst)
    {
      p_slist mylist=(p_slist)myadmst->_item.p;
      adms_slist_inreverse(&mylist);
      ((p_valueto)myadmst->_valueto)(myadmst->_previous->_item.p,(p_adms)mylist);
    }
  }
  free_ptraverse(pselect);
}
 /*[nepasimprimer]*/
static void Xread (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  char* myfilename=tsprintf(dot,mytransform->_textfile);
  parseva(1,NULL,myfilename);
}
 /*[nepasimprimer]*/
static void Xopen (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  char* myfilename=tsprintf(dot,mytransform->_textfile);
  FILE*ofh;
  if(!(ofh=fopen(myfilename,"wb")))
    adms_message_fatal(("%s: failed to open file [write mode]\n",myfilename))
  if(ofh)
    adms_slist_push(&globalfileoutputlist,(p_adms)ofh);
  else
   adms_message_fatal(("%s: cannot open file.\n",adms_transform_uid(mytransform)))
  xtraverse(mytransform->_children,dot,dotdot);
  fclose(OUT());
  adms_slist_pull(&globalfileoutputlist);
  free(myfilename);
}
 /*[nepasimprimer]*/
static void Xsetenv (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  char* myname=tsprintf(dot,mytransform->_textname);
  char* mystring=tsprintf(dot,mytransform->_textstring);
  if(!adms_setenv(myname,mystring))
  {
    adms_message_fatal(("cannot set shell variable: '%s=%s'\n",myname,mystring))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  free(myname);
  free(mystring);
}
 /*[nepasimprimer]*/
static void Xcount (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  p_ptraverse pselect=bar(dot,mytransform->_pathselect);
  adms_push_admst(adms_admst_newbi(dot,dot,adms_slist_length(pselect->_admst)),mytransform);
  free_ptraverse(pselect);
}
 /*[nepasimprimer]*/
static void Xgetenv (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  char* myname=tsprintf(dot,mytransform->_textname);
  const char* myenv=getenv(myname);
  if(myenv==NULL)
  {
    adms_message_fatal_continue(("missing shell variable '%s'\n",myname))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  adms_push_admst(adms_admst_newns(dot,dot,adms_kclone(myenv)),mytransform);
  free(myname);
}
 /*[nepasimprimer]*/
static void Xcopy (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  FILE*fromfh;
  FILE*tofh;
  char buf[1024];
  char* tfrom=tsprintf(dot,mytransform->_textfrom);
  char* tto=tsprintf(dot,mytransform->_textto);
  char* myfromfile=filename(tfrom);
  char* myfrombasename=basename(myfromfile);
  char* myfromdirname=dirname(myfromfile);
  char* mytopath=filename(tto);
  char* mytofilename=NULL;
  char* mytodirname=NULL;
  free(tfrom);
  free(tto);
  if(!adms_file_isregular(myfromfile))
    adms_message_fatal(("%s: from=`%s' file not found\n",adms_transform_uid(mytransform),myfromfile))
  if(adms_file_isdirectory(mytopath))
  {
    adms_k2strconcat(&mytodirname,mytopath);
    adms_k2strconcat(&mytofilename,mytopath);
    adms_k2strconcat(&mytofilename,ADMS_PATH_SEPARATOR);
    adms_k2strconcat(&mytofilename,myfrombasename);
  }
  else
  {
    mytodirname=dirname(mytopath);
    if(adms_file_isdirectory(mytodirname))
      adms_k2strconcat(&mytofilename,mytopath);
    else
      adms_message_fatal(("%s: to=`%s' invalid directory\n",adms_transform_uid(mytransform),mytodirname))
  }
  if(!strcmp(myfromdirname,mytodirname))
    adms_message_fatal(("%s: file '%s' copied to itself!\n",adms_transform_uid(mytransform),myfromfile))
  fromfh=fopen(myfromfile,"rb");
  tofh=fopen(mytofilename,"wb");
  if(!fromfh)
    adms_message_fatal(("%s: from=`%s' cannot open file [read mode]\n",adms_transform_uid(mytransform),myfromfile))
  if(!tofh)
    adms_message_fatal(("%s: to=`%s' cannot open file [write mode]\n",adms_transform_uid(mytransform),mytopath))
  while(!feof(fromfh))
  {
    size_t newbytes=fread(buf,sizeof(char),sizeof(buf),fromfh);
    if(ferror(fromfh))
      adms_message_fatal(("%s: from=`%s' cannot read file contents\n",adms_transform_uid(mytransform),myfromfile))
    fwrite(buf,sizeof(char),newbytes,tofh);
  }
  fclose(tofh);
  fclose(fromfh);
  adms_message_verbose(("%s: copy file '%s' to '%s'\n",adms_transform_uid(mytransform),myfromfile,mytofilename))
  free(myfromfile);
  free(myfromdirname);
  free(myfrombasename);
  free(mytopath);
  free(mytodirname);
  free(mytofilename);
}
 /*[nepasimprimer]*/
struct sa {char *e1; char *nom; char *e2; /*=*/ char *e3; char *valeur; struct sa *X;};
struct sb {char *nom; char* e; struct sa *a0; struct sa *a;};
static void texttoxml (const char*s)
{
  adms_message_admstdbg_impl("%s",s);
}
static char* xmltotxt (char*s)
{
  char*s0=s;
  char*s1=s;
  while(*s)
  {
    if(!strncmp(s,"\r\n",2))
      *s1='\n',s+=2;
    else if(!strncmp(s,"&quot;",6))
      *s1='\"',s+=6;
    else if(!strncmp(s,"&amp;",5))
      *s1='&',s+=5;
    else if(!strncmp(s,"&lt;",4))
      *s1='<',s+=4;
    else if(!strncmp(s,"&gt;",4))
      *s1='>',s+=4;
    else if(!strncmp(s,"&apos;",6))
      *s1='\'',s+=6;
    else
      *s1=*s,s++;
    s1++;
  }
  *s1='\0';
  s0=realloc(s0,(s1-s0+1)*sizeof(char));
  return s0;
}
static int xmlstrncpy (char*d,const char*o,int n)
{
  int l=0;
  char*d0=d;
  while(*o&&n)
  {
    if(!strncmp(o,"\r\n",2))
      l++,*d='\n',o+=2,n-=2;
    else if(!strncmp(o,"\n",1))
      l++,*d='\n',o+=1,n-=1;
    else
      *d=*o,o++,n--;
    d++;
  }
  *d='\0';
  d0=realloc(d0,(d-d0+1)*sizeof(char));
  return l;
}
static char* xmlmemcpy (char*d,const char*o,int n,int *nl)
{
  char*d0=d;
  while(*o&&n)
  {
    if(!strncmp(o,"\r\n",2))
      *nl=*nl+1,*d=*o,d++,o++,n--;
    else if(!strncmp(o,"\n",1))
      *nl=*nl+1;
    *d=*o,d++,o++,n--;
  }
  *d='\0';
  return d0;
}
static int xmlisspace (const char c,int* L)
{
  if(c=='\n')
    {*L+=1;return 1;}
  else
    return isspace(c);
}
 /*[nepasimprimer]*/
#define FRfr_XMLFATAL0 "ben alors! ne commence pas par un nom\n"
#define FRfr_XMLFATAL1 "ben alors! et le nom d'attribut devant le signe =.\n"
#define FRfr_XMLFATAL2 "ben alors! le charactere = ?\n"
#define FRfr_XMLFATAL3 "ben alors! le charactere \" ?\n"
#define FRfr_XMLFATAL4 "ben alors! la valeur n'est pas terminee par le charactere \" ?\n"
#define ANcn_XMLFATAL0 "%s:%i: tag doesn't start with a name\n",monfichier,*L
#define ANcn_XMLFATAL1 "%s:%i: attribute name expected before character =\n",monfichier,*L
#define ANcn_XMLFATAL2 "%s:%i: inside tag character \'=\' expected\n",monfichier,*L
#define ANcn_XMLFATAL3 "%s:%i: inside tag character \'\"\' expected\n",monfichier,*L
#define ANcn_XMLFATAL4 "%s:%i: unterminated attribute value - character \" expected\n",monfichier,*L
#define XMLFATAL0 ANcn_XMLFATAL0
#define XMLFATAL1 ANcn_XMLFATAL1
#define XMLFATAL2 ANcn_XMLFATAL2
#define XMLFATAL3 ANcn_XMLFATAL3
#define XMLFATAL4 ANcn_XMLFATAL4
struct sb* balisenouveau(const char **cp,int* L,const char *monfichier)
{
  struct sb* balisep=malloc(sizeof(struct sb));
  struct sb balise;
  const char*c=*cp;
  const char*c0=c;
  balise.nom=NULL;
  balise.e=NULL;
  balise.a0=NULL;
  balise.a=NULL;
  for(;*c&&!isspace(*c)&&strncmp(c,">",1)&&strncmp(c,"/>",2);c++);
  if(c-c0)
    balise.nom=malloc(sizeof(char)*(c-c0+1)),
    balise.e=NULL,
    balise.a0=NULL,
    memcpy(balise.nom,c0,c-c0),balise.nom[c-c0]='\0';
  else
    adms_message_fatal((XMLFATAL0))
  while(*c)
  {
    char*e1;
    c0=c;for(;*c&&xmlisspace(*c,L);c++);
    if(!strncmp(c,">",1)||!strncmp(c,"/>",2)||!*c)
    {
      balise.e=malloc(sizeof(char)*(c-c0+1)),
      strncpy(balise.e,c0,c-c0),
      balise.e[c-c0]='\0';
      break;
    }
    e1=malloc(sizeof(char)*(c-c0+1));
    strncpy(e1,c0,c-c0);
    e1[c-c0]='\0';
    c0=c;for(;*c&&!isspace(*c)&&(*c!='=');c++);
    if(c==c0)
      adms_message_fatal((XMLFATAL1))
    if(balise.a0)
      balise.a=balise.a->X=malloc(sizeof(struct sa)),
      balise.a->X=NULL;
    else
      balise.a=balise.a0=malloc(sizeof(struct sa)),
      balise.a->X=NULL;
    balise.a->nom=malloc(sizeof(char)*(c-c0+1));
    balise.a->e1=e1;
    memcpy(balise.a->nom,c0,c-c0),balise.a->nom[c-c0]='\0';
    c0=c;for(;*c&&xmlisspace(*c,L);c++);
    balise.a->e2=malloc(sizeof(char)*(c-c0+1)),
    strncpy(balise.a->e2,c0,c-c0),
    balise.a->e2[c-c0]='\0';
    if(*c!='=')
      adms_message_fatal((XMLFATAL2))
    c++;
    c0=c;for(;*c&&xmlisspace(*c,L);c++);
    balise.a->e3=malloc(sizeof(char)*(c-c0+1)),
    strncpy(balise.a->e3,c0,c-c0),
    balise.a->e3[c-c0]='\0';
    if(*c!='"')
      adms_message_fatal((XMLFATAL3))
    c++;
    c0=c;for(;*c&&(*c!='\"');c++);
    if(*c!='\"')
      adms_message_fatal((XMLFATAL4))
    balise.a->valeur=malloc(sizeof(char)*(c-c0+1));
    balise.a->valeur=xmlmemcpy(balise.a->valeur,c0,c-c0,L);
    c++;
  }
  *balisep=balise;
  *cp=c;
  return balisep;
}
typedef struct sx* px;
struct sx { int l; int c; const char* f; char *contenu; struct sb* bp;
  px r; px x; px X; px y; px Y0; px Y;
  void (*adms) (const px xp);
  void (*dbg) (const px xp);
};
px noeud_courant=NULL;
static void racinedbg (const px xp)
{
  px Y;
  for(Y=xp->Y0;Y;Y=Y->X)
    (Y->dbg)(Y);
}
static void instructiondbg (const px xp)
{
  adms_message_admstdbg_impl("<!--%s:%i--> <?",xp->f,xp->l);
  texttoxml(xp->contenu);
  adms_message_admstdbg_impl("?>");
}
static void declarationdbg (const px xp)
{
  adms_message_admstdbg_impl("<!--%s:%i--> <?was_xml ",xp->f,xp->l);
  texttoxml(xp->contenu);
  adms_message_admstdbg_impl("?>");
}
static void typededocumentdbg (const px xp)
{
  adms_message_admstdbg_impl("<!--%s:%i--> <?was_DOCTYPE !DOCTYPE",xp->f,xp->l);
  texttoxml(xp->contenu);
  adms_message_admstdbg_impl("?>");
}
static void commentairedbg (const px xp)
{
  adms_message_admstdbg_impl("<!--%s:%i--> <!--",xp->f,xp->l);
  texttoxml(xp->contenu);
  adms_message_admstdbg_impl("-->");
}
static void debutbalisedbg (const px xp)
{
  if(!strcmp(xp->bp->nom,"admst"))
    adms_message_admstdbg_impl("<!--%s:%i--> <?was_admst_begin ?>",xp->f,xp->l);
  else
  {
    adms_message_admstdbg_impl("<!--%s:%i--> <%s",xp->f,xp->l,xp->bp->nom);
    for(xp->bp->a=xp->bp->a0;xp->bp->a;xp->bp->a=xp->bp->a->X)
      adms_message_admstdbg_impl("%s",xp->bp->a->e1),
      adms_message_admstdbg_impl("%s",xp->bp->a->nom),
      adms_message_admstdbg_impl("%s",xp->bp->a->e2),
      adms_message_admstdbg_impl("="),
      adms_message_admstdbg_impl("%s",xp->bp->a->e3),
      adms_message_admstdbg_impl("\""),
      texttoxml(xp->bp->a->valeur),
      adms_message_admstdbg_impl("\"");
    adms_message_admstdbg_impl("%s",xp->bp->e);
    adms_message_admstdbg_impl(">");
  }
  for(xp->Y=xp->Y0;xp->Y;xp->Y=xp->Y->X)
    (xp->Y->dbg)(xp->Y);
}
static void balisefeuilledbg (const px xp)
{
  adms_message_admstdbg_impl("<!--%s:%i--> <%s",xp->f,xp->l,xp->bp->nom);
  for(xp->bp->a=xp->bp->a0;xp->bp->a;xp->bp->a=xp->bp->a->X)
    adms_message_admstdbg_impl("%s",xp->bp->a->e1),
    adms_message_admstdbg_impl("%s",xp->bp->a->nom),
    adms_message_admstdbg_impl("%s",xp->bp->a->e2),
    adms_message_admstdbg_impl("="),
    adms_message_admstdbg_impl("%s",xp->bp->a->e3),
    adms_message_admstdbg_impl("\""),
    texttoxml(xp->bp->a->valeur),
    adms_message_admstdbg_impl("\"");
  adms_message_admstdbg_impl("%s",xp->bp->e);
  adms_message_admstdbg_impl("/>");
}
static void findebalisedbg (const px xp)
{
  if(!strcmp(xp->bp->nom,"admst"))
    adms_message_admstdbg_impl("<!--%s:%i--> <?was_admst_end ?>",xp->f,xp->l);
  else
  {
    adms_message_admstdbg_impl("<!--%s:%i--> </",xp->f,xp->l);
    adms_message_admstdbg_impl("%s",xp->bp->nom);
    adms_message_admstdbg_impl("%s",xp->bp->e);
    adms_message_admstdbg_impl(">");
  }
}
static void textedbg (const px xp)
{
  texttoxml(xp->contenu);
}

static void xmlhook_start (const px xp);
static void xmlhook_end (const px xp);
static void xmlhook_text (const px xp);

static void racineadms (const px xp)
{
  px Y;
  for(Y=xp->Y0;Y;Y=Y->X)
    (Y->adms)(Y);
}
static void instructionadms (const px xp) {}
static void declarationadms (const px xp) {}
static void typededocumentadms (const px xp) {}
static void commentaireadms (const px xp) {}
static void debutbaliseadms (const px xp)
{
  xmlhook_start(xp);
  for(xp->Y=xp->Y0;xp->Y;xp->Y=xp->Y->X)
    (xp->Y->adms)(xp->Y);
}
static void balisefeuilleadms (const px xp)
{
  xmlhook_start(xp);
  xmlhook_end(xp);
}
static void findebaliseadms (const px xp)
{
  xmlhook_end(xp);
}
static void texteadms (const px xp)
{
  xmlhook_text(xp);
}
struct sx racine={0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,racineadms,racinedbg};
#define noeuds n(instruction) n(declaration) n(typededocument) n(commentaire) n(debutbalise) n(balisefeuille) n(findebalise) n(texte)
#undef n
#define n(I) static px I##new (char* contenu,struct sb* bp,const char*monfichier,const px y,const int l,const int c) \
{\
  px xp=malloc(sizeof(struct sx)); \
  xp->l=l; \
  xp->c=c; \
  xp->f=monfichier; \
  xp->y=y; \
  xp->Y0=NULL; \
  xp->Y=NULL; \
  xp->x=NULL; \
  xp->X=NULL; \
  xp->contenu=contenu; \
  xp->bp=bp; \
  xp->dbg= I## dbg; \
  xp->adms= I## adms; \
  if(y->Y0==NULL) y->Y=y->Y0=xp; \
  else {y->Y=y->Y->X=xp;} \
  return xp; \
}
noeuds
#undef n
 /*[nepasimprimer]*/
static void arbre (const char *s,const char*monfichier)
{
  int L=1;
  noeud_courant=&racine;
  for(;*s;s++)
  {
    if(0) {}
    else if(!strncmp(s,"<?admst-skip-implicit?>",23))
    {
      const char*s0=s;
      int l;
      char*contenu;
      s+=23;
      l=s-s0;
      contenu=malloc(sizeof(char)*(l+1));
      declarationnew(contenu,NULL,monfichier,noeud_courant,L,s-s0);
      L+=xmlstrncpy(contenu,s0,l);
    }
    else if(!strncmp(s,"<?xml",5))
    {
      const char*s0;
      s+=5;
      for(s0=s;*s&&!((s>s0+0)&&!strncmp(s-1,"?>",2));s++);
      if(*s)
      {
        int l=s-s0-1;
        char*contenu=malloc(sizeof(char)*(l+1));
        declarationnew(contenu,NULL,monfichier,noeud_courant,L,s-s0);
        L+=xmlstrncpy(contenu,s0,l);
      }
      else
        adms_message_fatal(("%s:%i: xml declaration unterminated '%.20s'\n",monfichier,L,s0))
    }
    else if(!strncmp(s,"<?",2))
    {
      const char*s0;
      s+=2;
      for(s0=s;*s&&!((s>s0+0)&&!strncmp(s-1,"?>",2));s++);
      if(*s)
      {
        int l=s-s0-1;
        char*contenu=malloc(sizeof(char)*(l+1));
        instructionnew(contenu,NULL,monfichier,noeud_courant,L,s-s0);
        L+=xmlstrncpy(contenu,s0,l);
      }
      else
        adms_message_fatal(("%s:%i: xml instruction declaration unterminated '%.20s'\n",monfichier,L,s0))
    }
    else if(!strncmp(s,"<!--",4))
    {
      const char*s0;
      s+=4;
      for(s0=s;*s&&!((s>s0+1)&&!strncmp(s-2,"-->",3));s++);
      if(*s)
      {
        int l=s-s0-2;
        char*contenu=malloc(sizeof(char)*(l+1));
        commentairenew(contenu,NULL,monfichier,noeud_courant,L,s-s0);
        L+=xmlstrncpy(contenu,s0,l);
      }
      else
        adms_message_fatal(("%s:%i: xml comment declaration unterminated '%.20s'\n",monfichier,L,s0))
    }
    else if(!strncmp(s,"<!DOCTYPE",9))
    {
      const char*s0;
      s+=9;
      for(s0=s;*s&&*s!='>';s++);
      if(*s)
      {
        int l=s-s0-0;
        char*contenu=malloc(sizeof(char)*(l+1));
        typededocumentnew(contenu,NULL,monfichier,noeud_courant,L,s-s0);
        L+=xmlstrncpy(contenu,s0,l);
      }
      else
        adms_message_fatal(("%s:%i: xml doctype declaration unterminated '%.20s'\n",monfichier,L,s0))
    }
    else if(!strncmp(s,"</",2))
    {
      struct sb* balisep=malloc(sizeof(struct sb));
      int L0=L;
      const char*s0;
      s+=2;
      for(s0=s;*s&&!isspace(*s)&&*s!='>';s++);
      if(s-s0)
        balisep->nom=malloc(sizeof(char)*(s-s0+1)),
        balisep->e=NULL,
        balisep->a0=NULL,
        memcpy(balisep->nom,s0,s-s0),balisep->nom[s-s0]='\0';
      else
        adms_message_fatal(("%s:%i: end tag unterminated '%.20s'\n",monfichier,L0,s0))
      s0=s;for(;*s&&xmlisspace(*s,&L);s++);
      balisep->e=malloc(sizeof(char)*(s-s0+1)),
      strncpy(balisep->e,s0,s-s0),
      balisep->e[s-s0]='\0';
      if(*s&&*s=='>')
      {
        findebalisenew(NULL,balisep,monfichier,noeud_courant,L0,s-s0);
        if(!noeud_courant->bp)
          adms_message_fatal(("%s:%i: end tag '%s' floating\n",monfichier,L0,balisep->nom))
        if(strcmp(noeud_courant->bp->nom,balisep->nom))
        {
          adms_message_fatal_continue(("%s:%i: end tag mismatch '%s'\n",monfichier,L0,balisep->nom))
          adms_message_fatal(("%s:%i: current open tag '%s'\n",monfichier,noeud_courant->l,noeud_courant->bp->nom))
        }
        noeud_courant=(noeud_courant&&(noeud_courant!=&racine))?noeud_courant->y:NULL;
      }
      else
        adms_message_fatal(("%s:%i: end tag unterminated '%.20s'\n",monfichier,L0,s0))
    }
    else if(*s=='<')
    {
      int L0=L;
      const char*s0=++s;
      struct sb* bp=balisenouveau(&s,&L,monfichier);
      if(!strncmp(s,"/>",2))
        s++,
        balisefeuillenew(NULL,bp,monfichier,noeud_courant,L0,s-s0);
      else if(!strncmp(s,">",1))
        noeud_courant=debutbalisenew(NULL,bp,monfichier,noeud_courant,L0,s-s0);
      else
        adms_message_fatal(("%s:%i: tag badly formed '%.20s'\n",monfichier,L0,s0))
    }
    else
    {
      const char*s0;
      for(s0=s;*s&&*s!='<';s++);
      if(s>s0)
      {
        char*contenu=malloc(sizeof(char)*(s-s0+1));
        textenew(contenu,NULL,monfichier,noeud_courant,L,s-s0);
        noeud_courant->Y->contenu=xmlmemcpy(contenu,s0,s-s0,&L);
        s--;
      }
    }
  }
}
 /*[nepasimprimer]*/
#undef T
#define T(p) !strcmp(mytransform->_name,#p)
static void setcallback (p_transform mytransform)
{
  if(T(admst:if-inside))
  {
    if(mytransform->_textlist->_admse==admse__p) mytransform->_callback=(void*)Xifinsidep,mytransform->_id="Xifinsidep";
    else if(mytransform->_textlist->_admse==admse__path) mytransform->_callback=(void*)Xifinsidepath,mytransform->_id="Xifinsidepath";
    else
    {
      adms_message_fatal_continue(("'list' should be exactly '%%p' or '%%(...)'\n"))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
  }
  else if(T(admst:if-not-inside))
  {
    if(mytransform->_textlist->_admse==admse__p) mytransform->_callback=(void*)Xifnotinsidep,mytransform->_id="Xifnotinsidep";
    else if(mytransform->_textlist->_admse==admse__path) mytransform->_callback=(void*)Xifnotinsidepath,mytransform->_id="Xifnotinsidepath";
    else
    {
      adms_message_fatal_continue(("'list' should be exactly '%%p' or '%%(...)'\n"))
      adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    }
  }
  else if(T(admst:variable))
  {

    if(mytransform->_textselect)
    {
      if(mytransform->_textselect->_admse==admse__p)
        mytransform->_callback=(void*)Xvariablep,
        mytransform->_id="Xvariablep";
      else if(mytransform->_textselect->_admse==admse__path)
        mytransform->_callback=(void*)Xvariable,
        mytransform->_id="Xvariable",
        mytransform->_pathpath=(p_path)mytransform->_textselect->_token->data;
      else if((adms_slist_length(mytransform->_textselect->_token)==1)&&(mytransform->_textselect->_token->data->_datatypename==admse_admst))
        mytransform->_callback=(void*)Xvariableconstant,
        mytransform->_id="Xvariableconstant",
        mytransform->_textstring=mytransform->_textselect;
      else
        mytransform->_callback=(void*)Xvariablestring,
        mytransform->_id="Xvariablestring",
        mytransform->_textstring=mytransform->_textselect;
    }
    else if(mytransform->_textstring)
    {
      if((adms_slist_length(mytransform->_textstring->_token)==1)&&(mytransform->_textstring->_token->data->_datatypename==admse_admst))
        mytransform->_callback=(void*)Xvariableconstant,
        mytransform->_id="Xvariableconstant";
      else
        mytransform->_callback=(void*)Xvariablestring,
        mytransform->_id="Xvariablestring";
    }
    else if(mytransform->_pathpath)
      mytransform->_callback=(void*)Xvariable,
      mytransform->_id="Xvariable";
    else
      mytransform->_callback=(void*)Xvariablenull,
      mytransform->_id="Xvariablenull";
  }
  else if(T(admst:value-to))
  {
    if(mytransform->_textstring)
    {
      if(mytransform->_textstring->_admse==admse__p)
        mytransform->_callback=(void*)Xvaluetop,
        mytransform->_id="Xvaluetop";
      else if(mytransform->_textstring->_admse==admse__path)
        mytransform->_callback=(void*)Xvaluetopath,
        mytransform->_id="Xvaluetopath";
      else if((adms_slist_length(mytransform->_textstring->_token)==1)&&(mytransform->_textstring->_token->data->_datatypename==admse_admst))
        mytransform->_callback=(void*)Xvaluetoconstant,
        mytransform->_id="Xvaluetoconstant";
      else
        mytransform->_callback=(void*)Xvaluetostring,
        mytransform->_id="Xvaluetostring";
    }
    else if(mytransform->_pathpath)
      mytransform->_callback=(void*)Xvalueto,
      mytransform->_id="Xvalueto";
    else
      mytransform->_callback=(void*)Xvaluetonull,
      mytransform->_id="Xvaluetonull";
  }
  else if(T(admst:text)) mytransform->_callback=(void*)Xtext,mytransform->_id="Xtext";
  else if(T(admst:new)&&((mytransform->_textarguments))) mytransform->_callback=(void*)Xnewarguments,mytransform->_id="Xnewarguments";
  else if(T(admst:new)&&((mytransform->_pathinputs))) mytransform->_callback=(void*)Xnew,mytransform->_id="Xnew";
  else if(T(admst)) mytransform->_callback=(void*)Xadmst,mytransform->_id="Xadmst";
  else if(T(admst:for-each)) mytransform->_callback=(void*)Xforeach,mytransform->_id="Xforeach";
  else if(T(admst:join)) mytransform->_callback=(void*)Xjoin,mytransform->_id="Xjoin";
  else if(T(admst:break)) mytransform->_callback=(void*)Xbreak,mytransform->_id="Xbreak";
  else if(T(admst:value-of)) mytransform->_callback=(void*)Xvalueof,mytransform->_id="Xvalueof";
  else if(T(admst:choice)||T(admst:choose)) mytransform->_callback=(void*)Xchoose,mytransform->_id="Xchoose";
  else if(T(admst:when)) {mytransform->_id="Xnull_when";}
  else if(T(admst:otherwise)) {mytransform->_id="Xnull_otherwise";}
  else if(T(admst:if)) mytransform->_callback=(void*)Xif,mytransform->_id="Xif";
  else if(T(admst:templates)||T(admst:template)) {mytransform->_id="Xnull_template";}
  else if(T(admst:apply-template)||T(admst:apply-templates)) mytransform->_callback=(void*)Xapplytemplates,mytransform->_id="Xapplytemplates";
  else if(T(admst:return)) mytransform->_callback=(void*)Xreturn,mytransform->_id="Xreturn";
  else if(T(admst:attribute)) mytransform->_callback=(void*)Xattribute,mytransform->_id="Xattribute";
  else if(T(admst:push))
  {
    if(mytransform->_pathoncompare)
      mytransform->_callback=(void*)Xpushoncompare,mytransform->_id="Xpushoncompare";
    else if(mytransform->_textonduplicate&&!strcmp(tsprintf(NULL,mytransform->_textonduplicate),"ignore"))
      mytransform->_callback=(void*)Xpushonduplicate,mytransform->_id="Xpushonduplicate";
    else
      mytransform->_callback=(void*)Xpush,mytransform->_id="Xpush";
  }
  else if(T(admst:reset)) mytransform->_callback=(void*)Xreset,mytransform->_id="Xreset";
  else if(T(admst:count)) mytransform->_callback=(void*)Xcount,mytransform->_id="Xcount";
  else if(T(admst:reverse)) mytransform->_callback=(void*)Xreverse,mytransform->_id="Xreverse";
  else if(T(admst:read)) mytransform->_callback=(void*)Xread,mytransform->_id="Xread";
  else if(T(admst:open)) mytransform->_callback=(void*)Xopen,mytransform->_id="Xopen";
  else if(T(admst:setenv)) mytransform->_callback=(void*)Xsetenv,mytransform->_id="Xsetenv";
  else if(T(admst:getenv)) mytransform->_callback=(void*)Xgetenv,mytransform->_id="Xgetenv";
  else if(T(admst:copy)) mytransform->_callback=(void*)Xcopy,mytransform->_id="Xcopy";
  else if(T(admst:assert)) mytransform->_callback=(void*)Xassert,mytransform->_id="Xassert";
  else if(T(admst:message)) mytransform->_callback=(void*)Xmessage,mytransform->_id="Xmessage";
  else if(T(admst:warning)) mytransform->_callback=(void*)Xwarning,mytransform->_id="Xwarning";
  else if(T(admst:error)) mytransform->_callback=(void*)Xerror,mytransform->_id="Xerror";
  else if(T(admst:fatal)) mytransform->_callback=(void*)Xfatal,mytransform->_id="Xfatal";
  else adms_message_fatal(("%s: unknown transform\n",adms_transform_uid(mytransform)))
}
static void setkeeplist (p_transform mytransform)
{
  if((mytransform->_callback==(void*)Xvalueof)     || (mytransform->_callback==(void*)Xifinsidep) || (mytransform->_callback==(void*)Xifnotinsidep) ||
     (mytransform->_callback==(void*)Xvaluetonull) || (mytransform->_callback==(void*)Xreset)     || (mytransform->_callback==(void*)Xreverse))
    mytransform->_pathselect->_keeplist=1;
  else if((mytransform->_callback==(void*)Xpush)||(mytransform->_callback==(void*)Xpushoncompare)||(mytransform->_callback==(void*)Xpushonduplicate))
    mytransform->_pathinto->_keeplist=1;
  else if((mytransform->_callback==(void*)Xifinsidepath) || (mytransform->_callback==(void*)Xifnotinsidepath))
    mytransform->_pathlist->_keeplist=1;
}

static void xmlhook_start (const px xp)
{
  int l=xp->l;
  const char* admstfile=xp->f;
  char* mybasename=basename(admstfile);
  p_slist Transform=root()->_transform;
  p_transform mytransform;
  struct sb* bp=xp->bp;
  const char* xname=bp->nom;
  if(!strcmp(xname,"admst"))
    mytransform=adms_transform_new(admstfile,mybasename,l,xname,NULL);
  else
  {
    p_transform parent=(p_transform)Transform->data;
    mytransform=adms_transform_new(admstfile,mybasename,l,xname,parent);
    if(T(admst:templates)||T(admst:template))
      adms_slist_push(&parent->_templates,(p_adms)mytransform);
    else
      adms_slist_push(&parent->_children,(p_adms)mytransform);
  }
  free(mybasename);
  adms_slist_push(&root()->_transform,(p_adms)mytransform);
  if(is_admst(xname))
  {
    for(bp->a=bp->a0;bp->a;bp->a=bp->a->X)
    {
      const char* aname=bp->a->nom;
      const char* avalue=xmltotxt(bp->a->valeur);
      if(((strlen(aname)==5)&&!strncmp(aname,"xmlns",5))||((strlen(aname)>5)&&!strncmp(aname,"xmlns:",6)))
      {}
      else if(!strcmp(aname,"arguments"))
      {
        p_slist myli=adms_strsplit(avalue,",",1000);
        p_slist myli0=myli;
        for(;myli;myli=myli->next)
          adms_slist_push(&mytransform->_textarguments,(p_adms)tparse(mytransform,aname,(char*)(myli->data)));
        adms_slist_inreverse(&mytransform->_textarguments);
        free_strlist(myli0);
      }
      else if(!strcmp(aname,"inputs"))
      {
        p_slist myli=adms_strsplit(avalue,",",1000);
        p_slist myli0=myli;
        for(;myli;myli=myli->next)
          adms_slist_push(&mytransform->_pathinputs,(p_adms)pparse(mytransform,aname,(char*)(myli->data)));
        adms_slist_inreverse(&mytransform->_pathinputs);
        free_strlist(myli0);
      }
      else if(!strcmp(aname,"oncompare"))
        mytransform->_pathoncompare=pparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"test"))
        mytransform->_pathtest=pparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"path"))
        mytransform->_pathpath=pparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"into"))
        mytransform->_pathinto=pparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"select"))
      {
        if(!strcmp(xname,"admst:variable"))
          mytransform->_textselect=tparse(mytransform,aname,avalue);
        else
          mytransform->_pathselect=pparse(mytransform,aname,avalue);
      }
      else if(!strcmp(aname,"string")||!strcmp(aname,"value"))
        mytransform->_textstring=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"match"))
        mytransform->_textmatch=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"required"))
        mytransform->_textrequired=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"separator"))
        mytransform->_textseparator=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"from"))
        mytransform->_textfrom=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"to"))
        mytransform->_textto=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"list"))
      {
        mytransform->_textlist=tparse(mytransform,aname,avalue);
        mytransform->_pathlist=(p_path)mytransform->_textlist->_token->data;
      }
      else if(!strcmp(aname,"name"))
        mytransform->_textname=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"format"))
        mytransform->_textformat=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"onduplicate"))
        mytransform->_textonduplicate=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"file"))
        mytransform->_textfile=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"version"))
        mytransform->_textversion=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"datatype"))
        mytransform->_textdatatype=tparse(mytransform,aname,avalue);
      else if(!strcmp(aname,"mode"))
        adms_message_obsolete(("%s:%i: attribute %s=\"%s\" is ignored\n",admstfile,l,aname,avalue))
      else
        adms_message_fatal(("%s:%i: unknown attribute %s=\"%s\"\n",admstfile,l,aname,avalue))
    }
    setcallback(mytransform);
    return;
  }
  else
    mytransform->_callback=(void*)Xnotadmst,mytransform->_id="Xnotadmst";
  for(bp->a=bp->a0;bp->a;bp->a=bp->a->X)
  {
    const char* aname=bp->a->nom;
    const char* avalue=bp->a->valeur;
    p_attribute myattribute=adms_attribute_new(aname);
    myattribute->_value=(p_adms)tparse(mytransform,aname,avalue);
    adms_slist_push(&mytransform->_attribute,(p_adms)myattribute);
  }
  adms_slist_inreverse(&mytransform->_attribute);
}
static void xmlhook_end (const px xp)
{
  p_slist Transform=root()->_transform;
  p_transform mytransform=Transform?(p_transform)Transform->data:NULL;
  sanityxx (mytransform);
  if(is_admst(mytransform->_name))
    setkeeplist(mytransform);
  if(!T(admst))
    adms_slist_pull(&root()->_transform);
  adms_slist_inreverse(&mytransform->_children);
}
 /*[nepasimprimer]*/
static void dbx (p_transform mytransform)
{
  p_slist myli;
  adms_message_admstdbg_impl("<%s file=\"%s\">\n",mytransform->_id,adms_transform_uid(mytransform));
  dbxx(mytransform);
  for(myli=mytransform->_templates;myli;myli=myli->next)
    dbx((p_transform)myli->data);
  for(myli=mytransform->_children;myli;myli=myli->next)
    dbx((p_transform)myli->data);
  adms_message_admstdbg_impl("</%s>\n",mytransform->_id);
}
/*SNT becomes T
static const char* text_excluding_leadingspaces (const char* text)
{
  const char* cPtr=text;
  while(*cPtr&&isspace(*cPtr))
    cPtr++;
  if(*cPtr&&(*cPtr=='\n' || *cPtr=='\r'))
    cPtr++;
  return cPtr;
}
*/
/*[TNS]NS becomes [TNS]N */
static char* text_excluding_trailingspaces (const char* text)
{
  const char* cPtr=text;
  const char* nl=NULL;
  int t=0;
  int space=0;
  while(*cPtr)
  {
    if(!isspace(*cPtr))
    {t=1; space=0;}
    if(t&&(*cPtr=='\n' || *cPtr=='\r'))
    {nl=cPtr; space=0;}
    if(nl&&(*cPtr=='\n' || *cPtr=='\r'))
      space=1;
    cPtr++;
  }
  if(t&&space)
    return adms_knclone(text,nl-text);
  else if(t)
    return adms_kclone(text);
  else
    return NULL;
}
static void xmlhook_text (const px xp)
{
  const char* text=xmltotxt(xp->contenu);
  char* newtext=text_excluding_trailingspaces(text);
  if(root()->_transform&&newtext)
  {
    p_transform parent=(p_transform)root()->_transform->data;
    char* mybasename=basename(xp->f);
    p_transform mytransform=adms_transform_new(xp->f,mybasename,parent->_l,"admst:text",parent);
    free(mybasename);
    mytransform->_textformat=tparse(mytransform,"text()",newtext);
    mytransform->_callback=(void*)Xtext,mytransform->_id="Xtext";
    adms_slist_push(&parent->_children,(p_adms)mytransform);
  }
  free(newtext);
}
 /*[nepasimprimer]*/
#if defined(STANDALONE)
static void creearbrex(const char*xcode,const char*myadmstfile)
#else
static void creearbrex(const char*myadmstfile)
#endif
{
    char* xdata0;
#if defined(STANDALONE)
    char* xfile=adms_kclone(myadmstfile);
    char* xdata=adms_kclone(xcode);
#else
/*read xfile*/
    char* xfile=filename(myadmstfile);
    FILE*xxfh=fopen(xfile,"rb");
    char buf[1024];
    char *xdata=NULL;
    size_t readbytes=0;
    if(!xxfh)
      adms_message_fatal(("%s: failed to open file [read mode]\n",xfile))
    if(!adms_file_isregular(xfile))
      adms_message_fatal(("%s: cannot read file - perhaps binary file\n",xfile))
    while(!feof(xxfh))
    {
      size_t newreadbytes=fread(buf,sizeof(char),sizeof(buf),xxfh);
      if(ferror(xxfh))
        adms_message_fatal(("%s: failed to read file\n",xfile))
      if(!(xdata=(char*)realloc(xdata,readbytes+newreadbytes)))
        adms_message_fatal(("%s: not enough memory available to read the file\n",xfile))
      memcpy(xdata+readbytes,buf,newreadbytes);
      readbytes+=newreadbytes;
    }
    fclose(xxfh);
    xdata=(char*)realloc(xdata,readbytes+1);
    *(xdata+readbytes)='\0';
#endif
    xdata0=xdata;
    racine.Y0=NULL;
    racine.Y=NULL;
    arbre(xdata,myadmstfile);
    free(xdata);
    free(xfile);
}
#if defined(STANDALONE)
int admsmain (const char*xcode,const char*xflag,const char*vafile)
{
  int argc=3;
  const char**argv=malloc(sizeof(char*)*argc);
#else
static void xbackup (const char* xfile,FILE*xfh)
{
  FILE*xxfh=fopen(xfile,"rb");
  char buf[1024];
  char *firstbyte=NULL;
  size_t readbytes=0;
  if(!xxfh)
    adms_message_fatal(("%s: failed to open file [read mode]\n",xfile))
  if(!adms_file_isregular(xfile))
    adms_message_fatal(("%s: cannot read file - perhaps binary file\n",xfile))
  while(!feof(xxfh))
  {
    size_t newreadbytes=fread(buf,sizeof(char),sizeof(buf),xxfh);
    if(ferror(xxfh))
      adms_message_fatal(("%s: failed to read file\n",xfile))
    if(!(firstbyte=(char*)realloc(firstbyte,readbytes+newreadbytes)))
      adms_message_fatal(("%s: not enough memory available to read the file\n",xfile))
    memcpy(firstbyte+readbytes,buf,newreadbytes);
    readbytes+=newreadbytes;
  }
  fclose(xxfh);
  firstbyte=(char*)realloc(firstbyte,readbytes+1);
  *(firstbyte+readbytes)='\0';
  fwrite(firstbyte,sizeof(char),readbytes,xfh);
  fflush(xfh);
  free(firstbyte);
}
int main (const int argc,const char**argv)
{
#endif
  char* myadmsimplicitxmlfile;
  char* mygetenv=getenv("adms_implicit_transforms");
  int myskipxmli=0;
  time_t mytime=time(NULL);
  p_slist myxargs;
  p_slist myli;
  FILE*xfh;
  char* xheader=NULL;
  const char* xinterface=".interface.xml";
#if defined(STANDALONE)
  argv[0]=adms_kclone("admsXml");
  argv[1]=adms_kclone(xflag);
  argv[2]=adms_kclone(vafile);
#endif
  /*NAN*/
  adms_NAN=adms_dzero/adms_dzero;
  /*admsmain*/
  rootnew(adms_admsmain_new("admsmain"));
  if(getenv("adms_info")    && !strcmp(getenv("adms_info"),    "no"))  root()->_info=admse_no;
  if(getenv("adms_usage")   && !strcmp(getenv("adms_usage"),   "no"))  root()->_usage=admse_no;
  if(getenv("adms_obsolete")&& !strcmp(getenv("adms_obsolete"),"no"))  root()->_obsolete=admse_no;
  if(getenv("adms_hint")    && !strcmp(getenv("adms_hint"),    "yes")) root()->_hint=admse_yes;
  if(getenv("adms_verbose") && !strcmp(getenv("adms_verbose"), "yes")) root()->_verbose=admse_yes;
  if(getenv("adms_debug")   && !strcmp(getenv("adms_debug"),   "yes")) root()->_debug=admse_yes;
  if(getenv("adms_dbg_vla") && !strcmp(getenv("adms_dbg_vla"), "yes")) root()->_dbg_vla=admse_yes;
  if(getenv("adms_dbg_xml") && !strcmp(getenv("adms_dbg_xml"), "yes")) root()->_dbg_xml=admse_yes;
  /*argc,argv*/
  {
    int i;
    root()->_argc=argc;
    root()->_argv=NULL;
    for(i=0;i<argc;i++)
    {
      if((strlen(argv[i])==2)&&!strcmp(argv[i],"-x"))
      {
        myskipxmli=1;
        adms_message_info(("-x: skipping any implicit xml scripts\n"))
      }
      else if((strlen(argv[i])==3)&&!strcmp(argv[i],"-xv"))
      {
        myskipversion=1;
        adms_message_info(("-xv: skipping version check\n"))
      }
      if((strlen(argv[i])==8)&&!strcmp(argv[i],"-compat2"))
        adms_message_warning(("backward compat ignored\n"))
      adms_slist_push(&root()->_argv,(p_adms)adms_kclone(argv[i]));
    }
  }
 /*[nepasimprimer]*/
/*main.simulator*/
  {
    char* sfullname=NULL;
    adms_k2strconcat(&sfullname,PACKAGE_NAME);
    adms_k2strconcat(&sfullname,"Xml-");
    if(getenv("adms_check"))
      adms_k2strconcat(&sfullname,"0.0.0 (shell variable 'adms_check' is set!)");
    else
      adms_k2strconcat(&sfullname,PACKAGE_VERSION);
    root()->_simulator=adms_simulator_new(sfullname);
    root()->_simulator->_fullname=adms_kclone(sfullname);
  }
 /*[nepasimprimer]*/
/*main.simulator.developer*/
  if(getenv("adms_check"))
    root()->_simulator->_developer=adms_kclone("0.0.0 (shell variable 'adms_check' is set)");
  else
  {
    char* sdeveloper=NULL;
    adms_k2strconcat(&sdeveloper,PACKAGE_NAME);
    adms_k2strconcat(&sdeveloper,"Xml");
    adms_k2strconcat(&sdeveloper,"-");
    adms_k2strconcat(&sdeveloper,PACKAGE_VERSION);
    adms_k2strconcat(&sdeveloper," (");
    adms_k2strconcat(&sdeveloper,__DATE__);
    adms_k2strconcat(&sdeveloper,"-");
    adms_k2strconcat(&sdeveloper,__TIME__);
    adms_k2strconcat(&sdeveloper,")");
    root()->_simulator->_developer=sdeveloper;
  }
 /*[nepasimprimer]*/
/*main.simulator.currentdate*/
  if(getenv("adms_check"))
    root()->_simulator->_currentdate=adms_kclone("0.0.0 (shell variable 'adms_check' is set)\n");
  else
  {
    char mybuffer[100];
    struct tm* mylocaltime=localtime(&mytime);
    strftime (mybuffer,100,"%a, %d %b %Y %H:%M:%S",mylocaltime);
    root()->_simulator->_currentdate=adms_kclone(mybuffer);
  }
 /*[nepasimprimer]*/
/*shell*/
  if(getenv("adms_dbg_pre"))
    adms_preprocessor_setint_yydebug(1);
  else
    adms_preprocessor_setint_yydebug(0);
 /*[nepasimprimer]*/
  if(
    (argc==1)
    ||
    ((argc==2)&&!strcmp(argv[1],"-h"))
    ||
    ((argc==2)&&!strcmp(argv[1],"--help"))
    ||
    ((argc==3)&&!strcmp(argv[1],"--")&&!strcmp(argv[2],"help"))
  )
  {
    adms_message_usage(("%sXml-%s (%s) ",PACKAGE_NAME,PACKAGE_VERSION,GIT))
    adms_message_usage_continue(("%s %s (os=%s compiler=%s)\n",__DATE__,__TIME__,ADMS_OS,ADMS_COMPILER))
    adms_message_usage(("%sXml source [options] -e script1 [-e script2 ...]\n",PACKAGE_NAME))
    adms_message_usage(("files:\n"))
    adms_message_usage(("  source: verilog-ams source code\n"))
    adms_message_usage(("  script1, ...: admst scripts (xml format)\n"))
    adms_message_usage(("options:\n"))
    adms_message_usage(("  -D NAME: predefine NAME as a macro, with definition `1'\n"))
    adms_message_usage(("  -D NAME=DEFINITION: predefine NAME as a macro, with definition DEFINITION\n"))
    adms_message_usage(("  -I DIR: Add directory DIR to search path for header files\n"))
    adms_message_usage(("  -x : ignore file .adms.implicit.xml\n"))
    adms_message_usage(("  -xv: do not check version number of scripts\n"))
    adms_message_usage(("shell variables:\n"))
    adms_message_usage(("- adms_info=\"yes\"|\"no\": print info [default=yes]\n"))
    adms_message_usage(("- adms_usage=\"yes\"|\"no\": print usage [default=yes]\n"))
    adms_message_usage(("- adms_verbose=\"yes\"|\"no\": print messages [default=no]\n"))
    adms_message_usage(("- adms_debug=\"yes\"|\"no\": print more messages [default=no]\n"))
    adms_message_usage(("- adms_dbg_vla=\"yes\"|\"no\": debug messages during vla parsing [default=no]\n"))
    adms_message_usage(("- adms_dbg_xml=\"yes\"|\"no\": debug messages during xml parsing [default=no]\n"))
    adms_message_usage(("- adms_dbg_pre=\"yes\"|\"no\": debug messages during vla preprocessing [default=no]\n"))
    adms_message_usage(("-i or --info: miscellaneous info (release, web site, mailing list)\n"))
    adms_message_usage(("-v or --version: version number\n"))
    adms_message_usage(("-h or --help: short help\n"))
  }
  else if(
    ((argc==2)&&!strcmp(argv[1],"-v"))
    ||
    ((argc==2)&&!strcmp(argv[1],"--version"))
    ||
    ((argc==3)&&!strcmp(argv[1],"--")&&!strcmp (argv[2],"version"))
  )
  {
    adms_message_usage(("<release name=\"%sXml\" version=\"%s\" date=\"%s\" time=\"%s\"/>\n",PACKAGE_NAME,PACKAGE_VERSION,__DATE__,__TIME__))
    return 0;
  }
  else if(
    ((argc==2)&&!strcmp(argv[1],"-i"))
    ||
    ((argc==2)&&!strcmp(argv[1],"--info"))
    ||
    ((argc==3)&&!strcmp(argv[1],"--")&&!strcmp(argv[2],"info"))
  )
  {
    adms_message_usage(("<info\n",PACKAGE_BUGREPORT))
    adms_message_usage(("  author=\"laurent lemaitre\"\n",PACKAGE_BUGREPORT))
    adms_message_usage(("  bug-report=\"%s\"\n",PACKAGE_BUGREPORT))
    adms_message_usage(("  home-page=\"https://sourceforge.net/projects/mot-adms/\"\n"))
    adms_message_usage(("  mailing-list=\"mot-adms-users@lists.sourceforge.net\">\n"))
    adms_message_usage(("  <release name=\"%s\" version=\"%s\" ",PACKAGE_NAME,PACKAGE_VERSION))
    adms_message_usage_continue(("git=\"%s\" date=\"%s\" time=\"%s\"/>\n",GIT,__DATE__,__TIME__))
    adms_message_usage(("</info>\n"))
    return 0;
  }
/*create implicit admst script*/
  {
    if(mygetenv)
    {
      myadmsimplicitxmlfile=filename(mygetenv);
      adms_message_info(("loading implicit xml script %s\n",mygetenv))
      adms_message_info(("(shell variable 'adms_implicit_transforms' has been set)\n"))
    }
    else
    {
#include "adms.implicit.xml.c"
      FILE*ofh;
      myadmsimplicitxmlfile=adms_kclone(".adms.implicit.xml");
      if(!(ofh=fopen(myadmsimplicitxmlfile,"wb")))
        adms_message_fatal(("%s: failed to open file [write mode]\n",myadmsimplicitxmlfile))
      fputs(adms_implicit_xml,ofh);
      fclose(ofh);
    }
  }
  if(
    (argc==1)
    ||
    ((argc==2)&&!strcmp(argv[1],"-h"))
    ||
    ((argc==2)&&!strcmp(argv[1],"--help"))
    ||
    ((argc==3)&&!strcmp(argv[1],"--")&&!strcmp(argv[2],"help"))
  )
    return 0;
 /*[nepasimprimer]*/
/*parse verilogams list*/
  {
    p_slist l=getlist_from_argv(argc,argv,"-f","file");
    for(myli=l;myli;myli=myli->next)
      parseva(argc,argv,(char*)(myli->data));
    adms_slist_free(l);
  }
/*add implicit admst script*/
  myxargs=getlist_from_argv(argc,argv,"-e","file");
#if defined(STANDALONE)
  adms_slist_push(&myxargs,(p_adms)adms_kclone("stdin"));
#endif
  adms_slist_push(&myxargs,(p_adms)myadmsimplicitxmlfile);
 /*[nepasimprimer]*/
/*traverse scripts*/
  if(!(xfh=fopen(xinterface,"wb")))
    adms_message_fatal(("%s: failed to open file [write mode]\n",xinterface))
  adms_k2strconcat(&xheader,"<!--\n");
  adms_k2strconcat(&xheader,"  File automatically created\n");
  adms_k2strconcat(&xheader,"  Command used:\n");
  adms_k2strconcat(&xheader,"  # release: ");
  adms_k2strconcat(&xheader,PACKAGE_VERSION);
  adms_k2strconcat(&xheader," ");
  adms_k2strconcat(&xheader,__DATE__);
  adms_k2strconcat(&xheader,"-");
  adms_k2strconcat(&xheader,__TIME__);
  adms_k2strconcat(&xheader,"\n");
  adms_k2strconcat(&xheader,"  ");
  adms_k2strconcat(&xheader,PACKAGE_NAME);
  adms_k2strconcat(&xheader,"Xml");
  adms_k2strconcat(&xheader," \\\n");
  if(myskipxmli)
    adms_k2strconcat(&xheader,"   -x \\\n");
  for(myli=myxargs;myli;myli=myli->next)
    if(myli!=myxargs)
    {
      const char* myadmstfile=(char*)(myli->data);
      adms_k2strconcat(&xheader,"   -e ");
      adms_k2strconcat(&xheader,myadmstfile);
      if(myli->next)
        adms_k2strconcat(&xheader," \\");
      adms_k2strconcat(&xheader,"\n");
    }
  adms_k2strconcat(&xheader,"-->\n\n");
  fwrite(xheader,sizeof(char),strlen(xheader)*sizeof(char),xfh);
#if defined(STANDALONE)
  fwrite(xcode,sizeof(char),strlen(xcode)*sizeof(char),xfh);
#endif
  fflush(xfh);
  free(xheader);
#if defined(STANDALONE)
#else
  for(myli=myxargs;myli;myli=myli->next)
    if(!myskipxmli || myli!=myxargs)
    {
      const char* myadmstfile=(char*)(myli->data);
      char* xfile=filename(myadmstfile);
      char* xlocalheader=NULL;
      adms_k2strconcat(&xlocalheader,"\n");
      adms_k2strconcat(&xlocalheader,"<?escript name=\"");
      adms_k2strconcat(&xlocalheader,myadmstfile);
      adms_k2strconcat(&xlocalheader,"\" ?>\n");
      fwrite(xlocalheader,sizeof(char),strlen(xlocalheader)*sizeof(char),xfh);
      fflush(xfh);
      free(xlocalheader);
      xbackup(xfile,xfh);
      free(xfile);
    }
#endif
  fclose(xfh);
/*db*/
    if(root()->_dbg_xml==admse_yes)
    {
      p_slist l;
      char*mydbgfile1=".admst1.xml";
      char*mydbgfile2=".admst2.xml";
      const char*admstdtduri="http://mot-adms.svn.sourceforge.net/viewvc/mot-adms/trunk/adms/admst.dtd";
#if defined(STANDALONE)
      creearbrex(xcode,".interface.xml");
#else
      creearbrex(".interface.xml");
#endif
      if(!(stdadmstdbgimpl=fopen(mydbgfile1,"wb")))
        adms_message_fatal(("%s: failed to open file [write mode]\n",mydbgfile1))
      adms_message_admstdbg_impl("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
      adms_message_admstdbg_impl("<!DOCTYPE admst PUBLIC \"-//adms//DTD admst 2.0//-\" \"%s\">\n",admstdtduri);
      adms_message_admstdbg_impl("<!-- xmllint -valid -noout %s -->\n",mydbgfile1);
      adms_message_admstdbg_impl("<admst xmlns:admst=\"%s\">\n",admstdtduri);
      racine.dbg(&racine);
      adms_message_admstdbg_impl("</admst>\n");
      fclose(stdadmstdbg);
      stdadmstdbgimpl=NULL;
      if(!(stdadmstdbgimpl=fopen(mydbgfile2,"wb")))
        adms_message_fatal(("%s: failed to open file [write mode]\n",mydbgfile2))
      adms_message_admstdbg_impl("<admst>\n");
      for(l=root()->_transform;l;l=l->next)
        dbx((p_transform)l->data);
      adms_message_admstdbg_impl("</admst>\n");
      fclose(stdadmstdbg);
      stdadmstdbgimpl=NULL;
      adms_message_info(("%s, %s: debug files created (note: shell variable 'adms_dbg_xml' is set)\n",mydbgfile1,mydbgfile2))
    }
 /*[nepasimprimer]*/
  for(myli=myxargs;myli;myli=myli->next)
    if(!myskipxmli || myli!=myxargs)
    {
      p_slist l;
#if defined(STANDALONE)
      creearbrex(xcode,(char*)myli->data);
#else
      creearbrex((char*)myli->data);
#endif
      adms_message_verbose(("-e file: %s\n",(char*)myli->data))
      racine.adms(&racine);
      if(root()->_dbg_xml==admse_yes)
      {
        char*mydbgfile=NULL;
        char* mybasename=basename((char*)myli->data);
        adms_k2strconcat(&mydbgfile,".");
        adms_k2strconcat(&mydbgfile,mybasename);
        if(!(stdadmstdbgimpl=fopen(mydbgfile,"wb")))
          adms_message_fatal(("%s: failed to open file [write mode]\n",mydbgfile))
        racine.dbg(&racine);
        free(mydbgfile);
        free(mybasename);
      }
      adms_message_verbose(("traverse: %s\n",(char*)myli->data))
      adms_slist_inreverse(&root()->_transform);
  /*postx*/
      for(l=root()->_transform;l;l=l->next)
        postx((p_transform)l->data);
  /*traverse*/
      for(l=root()->_transform;l;l=l->next)
        adms_slist_push(&root()->_invtransform,l->data);
      for(l=root()->_transform;l;l=l->next)
      {
        p_transform mytransform=(p_transform)l->data;
        p_admst myadmst=adms_admst_newpa(NULL,NULL,(p_adms)root());
        Xadmst(mytransform,myadmst,NULL);
        deref(myadmst);
      }
      if(root()->_valueof)
      {
        p_transform mytransform=(p_transform)root()->_valueof->data;
        adms_message_fatal_continue(("stack '%%s' is not empty! (%i element(s) left)\n",adms_slist_length(root()->_valueof)/2))
        adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
      }
      adms_slist_free(root()->_transform);
      root()->_transform=NULL;
      free(myli->data);
    }
  adms_slist_free(root()->_invtransform);
  adms_message_verbose(("%s: file created (all -e files in one file)\n",xinterface))
  for(myli=root()->_variable;myli;myli=myli->next)
  {
    p_admstvariable dollar=(p_admstvariable)myli->data;
    p_slist l;
    for(l=dollar->_value;l;l=l->next)
    {
      ((p_admst)l->data)->_refd--;
      deref((p_admst)l->data);
    }
    adms_admstvariable_free(dollar);
  }
  adms_slist_free(myxargs);
 /*[nepasimprimer]*/
  adms_message_info(("elapsed time: %g (second)\n",difftime(time(NULL),mytime)))
  adms_message_info(("admst iterations: %i (%i freed)\n",adms_global_nbadmstnew(),adms_global_nbadmstdestroy()))
  return 0;
}
#if defined (STANDALONE) && defined (APATHMAIN)
int main (int argc,char **argv)
{
  return apath_main (argc,argv);
}
#endif
