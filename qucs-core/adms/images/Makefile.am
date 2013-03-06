##
## RCS Info
## $Id: Makefile.am 1111 2008-10-14 14:19:34Z r29173 $
## 
## Log
## $Log$
## Revision 1.1  2006/09/06 17:31:26  r29173
## added images in tarball distribution
##
## Revision 1.15  2006/08/04 17:15:31  r29173
## rearranged code partionning into files
##
## Revision 1.14  2006/08/02 16:45:53  r29173
## rearranged code partionning into files
##
## Revision 1.13  2005/11/01 14:09:09  r29173
## removed unused argument - fixed bug in Makefile.am
##
## Revision 1.12  2005/06/09 16:54:50  r29173
## deleted xmlParserNew.c -- SLOWSLOW
##
## Revision 1.11  2005/05/09 14:38:32  r29173
## cleaned-up source directory structure
##
## Revision 1.10  2005/05/03 07:58:21  r29173
## moved admsNS into admsFile
##
## Revision 1.9  2005/03/31 18:12:02  r29173
## migration to adms-2.x.x: converted to adms2 syntax
##
## Revision 1.8  2005/03/23 13:38:34  r29173
## migration to adms-2.x.x: added xpath parser
##
## Revision 1.7  2004/11/10 13:49:30  r29173
## added -lm (math library)
##
## Revision 1.6  2004/11/02 10:18:02  r29173
## fixed makefile dependy issues - now 'make distcheck' passes
##
## Revision 1.5  2004/10/22 17:16:08  marat_yakupov
## Fixed issue to allow compilation and prevent core dumps
##
## Revision 1.4  2004/10/20 15:33:51  r29173
## tons of changes
## 1- introduced element whileloop (marat)
## 2- reshaped the handling of the messaging system
## 3- fixed bug in admsCheck/Makefile.am
## 4- started implementation of implicit adms transforms
##
## Revision 1.3  2004/08/03 12:33:55  r29173
## import adms-1.21.0 from local CVS
##
## Revision 1.3  2004/07/06 19:17:57  r29173
## started implementing yacc code for element "nature"
## resumed transition GLib -> adms.xml
##
## Revision 1.2  2004/05/26 13:02:47  r29173
## added default values to all enumerations
##
## Revision 1.1.1.1  2004/05/21 12:20:01  r29173
## recreated cvs data structure (crashed after revision 1.13.0!)
##
## Revision 1.11  2004/04/21 14:25:34  r29173
## simplified xparser->_current_stack
##
## Revision 1.10  2004/03/23 13:05:51  r29173
## fixed name spelling (lowercase to uppercase)
##
## Revision 1.9  2004/03/08 08:24:20  r29173
## started implementing code builder in yacc parser
##
## Revision 1.8  2004/01/16 10:26:25  r29173
## moved code to admsXml
##
## Revision 1.7  2004/01/15 14:58:46  r29173
## removed -Wall flag
##
## Revision 1.6  2004/01/13 11:25:14  r29173
## changed coding style
##
## Revision 1.5  2004/01/05 11:35:51  r29173
## updated for cygwin
##
## Revision 1.4  2003/09/01 08:34:46  r29173
## subdir `data' removed
##
## Revision 1.3  2003/06/19 13:55:23  r29173
## fixed dependency (admsTree missing)
##
## Revision 1.2  2003/05/21 14:20:56  r29173
## add rcs info
##
##


generated_FILES=adms-scalar.png adms-element.png adms-enumeration.png adms-uid.png adms-alias.png
EXTRA_DIST=mkimages.pl $(generated_FILES)

if MAINTAINER_MODE
$(generated_FILES): ${srcdir}/mkimages.pl
	$(PERL) ${srcdir}/mkimages.pl
endif

CLEANFILES=core .*.swp *\~
