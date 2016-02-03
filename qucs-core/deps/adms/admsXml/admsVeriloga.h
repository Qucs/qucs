/*
 * RCS Info
 * $Id: admsVeriloga.h 941 2008-03-27 17:38:57Z r29173 $
 * 
 * Log
 * $Log$
 * Revision 1.6  2006/07/27 13:51:26  r29173
 * moved code from xmlParser to admsImplicitTransforms
 * removed /simulator/function
 *
 * Revision 1.5  2006/01/13 10:07:04  r29173
 * cleaned-up code style
 *
 * Revision 1.4  2005/05/09 14:38:31  r29173
 * cleaned-up source directory structure
 *
 * Revision 1.3  2005/05/03 09:35:15  r29173
 * cleaned-up header file dependencies
 *
 * Revision 1.2  2004/08/23 09:24:31  r29173
 * removed unused function (adms_treedata_fatal)
 *
 * Revision 1.1  2004/08/03 12:33:55  r29173
 * import adms-1.21.0 from local CVS
 *
 * Revision 1.3  2004/08/03 09:23:11  r29173
 * renamed superelement into element
 *
 * Revision 1.2  2004/06/30 16:40:58  r29173
 * renamed all admsObject files to object<Filename>
 *
 * Revision 1.1.1.1  2004/05/21 12:20:01  r29173
 * recreated cvs data structure (crashed after revision 1.13.0!)
 *
 * Revision 1.8  2004/04/19 21:03:22  r29173
 * removed subdir admsTree
 *
 * Revision 1.7  2004/03/08 13:58:06  r29173
 * all code lower-cased
 *
 * Revision 1.6  2004/01/16 10:30:00  r29173
 * added accessors to yyin, yyout
 *
 * Revision 1.5  2004/01/06 14:13:13  r29173
 * fixed definition of win32_interface
 *
 * Revision 1.4  2004/01/06 12:35:40  r29173
 * fixed the use of globals: input file and output file
 *
 * Revision 1.3  2003/12/11 16:10:11  r29173
 * changed usage of win32_interface
 *
 * Revision 1.2  2003/05/21 14:18:02  r29173
 * add rcs info
 *
 */

#ifndef _admsveriloga_h
#define _admsveriloga_h

#include "adms.h"

void adms_veriloga_setint_yydebug (const int val);
void adms_veriloga_setfile_input (FILE *ifile);

void verilogaerror (const char *s);
int verilogalex ();
int verilogaparse ();

#endif /* _admsveriloga_h */
