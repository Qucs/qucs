/*
 * RCS Info
 * $Id: admsCheck.c 1110 2008-10-14 13:40:29Z r29173 $
 *
 * Log
 * $Log$
 * Revision 1.17  2006/09/13 16:13:20  r29173
 * started migration to www.antlr.org tools
 *
 * Revision 1.16  2006/08/04 17:15:32  r29173
 * rearranged code partionning into files
 *
 * Revision 1.15  2006/06/16 12:24:25  r29173
 * ton of changes: memory clean-up
 *
 * Revision 1.14  2006/06/09 16:49:34  r29173
 * improved malloc management
 *
 * Revision 1.13  2006/01/26 13:39:25  r29173
 * fixed core dump when running admsCheck
 *
 * Revision 1.12  2005/06/21 08:02:28  r29173
 * renamed adms_set_admsmain to adms_valueto_admsmain
 *
 * Revision 1.11  2005/05/09 14:38:29  r29173
 * cleaned-up source directory structure
 *
 * Revision 1.10  2005/05/03 09:35:14  r29173
 * cleaned-up header file dependencies
 *
 * Revision 1.9  2005/05/03 07:58:19  r29173
 * moved admsNS into admsFile
 *
 * Revision 1.8  2005/04/01 07:25:44  r29173
 * renamed prefix .xsl to .xml
 *
 * Revision 1.7  2005/01/11 09:58:01  r29173
 * tons of changes - simplify accessor names (valueof, valueto)
 *
 * Revision 1.6  2004/11/19 12:29:56  r29173
 * renamed _C into CAT
 * saved argc, argv into admsmain element
 * added adms:setenv transform
 *
 * Revision 1.5  2004/11/11 13:45:03  r29173
 * replaced g_message by adms_message_info
 *
 * Revision 1.4  2004/11/01 09:37:36  r29173
 * changed messaging macros (avoid useless computation of arguments)
 * improved the coding of transforms in xmlProgram.c
 *
 * Revision 1.3  2004/08/03 12:33:54  r29173
 * import adms-1.21.0 from local CVS
 *
 * Revision 1.2  2004/08/03 10:05:46  r29173
 * removed superelement from admsgui
 * fixed some warnings issued during compilation
 *
 * Revision 1.1.1.1  2004/05/21 12:20:01  r29173
 * recreated cvs data structure (crashed after revision 1.13.0!)
 *
 * Revision 1.10  2004/03/11 16:25:14  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.9  2004/03/08 13:58:05  r29173
 * all code lower-cased
 *
 * Revision 1.8  2004/03/08 08:24:19  r29173
 * started implementing code builder in yacc parser
 *
 * Revision 1.7  2004/01/13 11:25:51  r29173
 * added adms_message_usage
 *
 * Revision 1.6  2004/01/09 16:23:45  r29173
 * cleaned-up use of #include
 *
 * Revision 1.5  2004/01/08 10:28:49  r29173
 * moved code to always.[ch]
 *
 * Revision 1.4  2004/01/05 11:07:51  r29173
 * change the way admsCheck.xml is downloaded
 *
 * Revision 1.3  2003/10/22 15:11:36  r29173
 * changed the way script "text2Ccode.pl" handles file name.
 *
 * Revision 1.2  2003/05/21 14:18:00  r29173
 * add rcs info
 *
 */


/*
  admsCheck

  Small program to perform a test run of adms using dummy.va and dummy.xml

  Used by 'make check' in the adms build system

*/

#include "adms.h"
/*constants*/
#include "dummy.xml.c"
#include "dummy.va.c"

char*mydata[]={
"dummy.va","dummy_va",
"dummy.xml","dummy_xml",
NULL
};

int main(const int argc, const char **argv)
{
  char**mydataptr;
  rootnew(adms_admsmain_new("admsmain"));
  adms_admsmain_valueto_filename(root(),adms_kclone("stdin"));
  adms_message_info(("Create files:\n"));
  for( mydataptr=mydata; *mydataptr; mydataptr++,mydataptr++)
  {
    char*savefilename=*mydataptr;
    char*symbolname=*(mydataptr+1);
    char* myfilename=strdup(savefilename);
    FILE* ofh=fopen(myfilename,"w");
    adms_message_info((" ... %s\n",myfilename));
    free(myfilename);
    if(!strcmp(symbolname,"dummy_va"))
      fprintf(ofh,"%s",dummy_va);
    else if(!strcmp(symbolname,"dummy_xml"))
      fprintf(ofh,"%s",dummy_xml);
    fclose(ofh);
  }
  adms_message_info(("How to use the example\n"));
  adms_message_info(("  admsXml dummy.va -e dummy.xml\n"));
  adms_message_info(("  This command parses file dummy.va\n"));
  adms_message_info(("  and interprets control file dummy.xml\n"));
  adms_message_info(("  File dummy.net is created.\n"));
  adms_message_info(("  (file .dummy.va.adms is created in background.)\n"));
  return 0;
}
