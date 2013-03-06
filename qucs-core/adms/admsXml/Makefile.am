##
## RCS Info
## $Id: Makefile.am 1181 2009-09-28 16:16:03Z r29173 $
## 
## Log
## $Log$
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

incfile=constants.vams disciplines.vams adms.implicit.xml dummy.va dummy.xml
incfilec=$(incfile:=.c)

if MAINTAINER_MODE
generated_FILES=adms.h adms.c verilogaYacc.y admstpathYacc.y admstpathYacc.h $(incfilec)
SUFFIXES=.vams .vams.c .va .va.c .xml .xml.c .y.in
$(incfilec):%.c:% ${srcdir}/mkctext.pl
	$(PERL) ${srcdir}/mkctext.pl $< $(top_srcdir)
admstpathYacc.y admstpathYacc.h adms.h adms.c: ${top_srcdir}/adms.xml ${top_srcdir}/admstpath.xml ${srcdir}/mkelements.pl
	$(PERL) ${srcdir}/mkelements.pl $(top_srcdir)
verilogaYacc.y: ${srcdir}/verilogaYacc.y.in ${srcdir}/mkgrammar.pl
	$(PERL) ${srcdir}/mkgrammar.pl ${srcdir}/verilogaYacc.y.in
# -b -> re-defines file-prefix (not y.tab.c)
# -d -> creates #define file
verilogaLex.c: verilogaLex.l
	flex -Pveriloga -overilogaLex.c verilogaLex.l
verilogaYacc.c: verilogaYacc.y verilogaLex.l
	bison -by -d -pveriloga verilogaYacc.y 
	mv -f y.tab.c verilogaYacc.c 
	mv -f y.tab.h verilogaYacc.h 
admstpathYacc.c: admstpathYacc.y admstpathYacc.h
	bison -by -padmstpath admstpathYacc.y 
	mv -f y.tab.c admstpathYacc.c 
preprocessorLex.c: preprocessorLex.l
	flex -Ppreprocessor -opreprocessorLex.c preprocessorLex.l
preprocessorYacc.c: preprocessorYacc.y preprocessorLex.l
	bison -by -d -ppreprocessor preprocessorYacc.y 
	mv -f y.tab.c preprocessorYacc.c 
	mv -f y.tab.h preprocessorYacc.h 
else
generated_FILES =
endif

dist_man_MANS = admsXml.1

lib_LTLIBRARIES=libadmsElement.la libadmsPreprocessor.la libadmsVeriloga.la libadmsAdmstpath.la

libadmsElement_la_SOURCES=adms.c
libadmsElement_la_CPPFLAGS=-DinsideElement
libadmsElement_la_LDFLAGS=-no-undefined

preprocessorLex.l: $(incfilec) 
libadmsPreprocessor_la_SOURCES=preprocessorYacc.c preprocessorLex.l preprocessorMain.c
libadmsPreprocessor_la_CPPFLAGS=-DinsidePreprocessor
libadmsPreprocessor_la_LDFLAGS=-no-undefined
libadmsPreprocessor_la_LIBADD=libadmsElement.la

libadmsVeriloga_la_SOURCES=verilogaYacc.c verilogaLex.l
libadmsVeriloga_la_CPPFLAGS=-DinsideVeriloga
libadmsVeriloga_la_LDFLAGS=-no-undefined
libadmsVeriloga_la_LIBADD=libadmsElement.la

libadmsAdmstpath_la_SOURCES=admstpathYacc.c admstpathYacc.h
libadmsAdmstpath_la_CPPFLAGS=-DinsideadmstpathYacc
libadmsAdmstpath_la_LDFLAGS=-no-undefined
libadmsAdmstpath_la_LIBADD=libadmsElement.la

#depends on adms.implicit.xml
bin_PROGRAMS=admsXml
noinst_PROGRAMS=admsCheck

admsXml_SOURCES=admsXml.c
admsXml_LDADD=-lm libadmsElement.la libadmsVeriloga.la libadmsPreprocessor.la libadmsAdmstpath.la

admsXml.c: adms.implicit.xml.c

admsCheck.c=$(incfilec)
admsCheck_SOURCES=admsCheck.c
admsCheck_LDADD=libadmsElement.la

EXTRA_DIST=mkctext.pl mkelements.pl mkgrammar.pl adms.h adms.c \
 preprocessorYacc.h admsPreprocessor.h verilogaYacc.h admsVeriloga.h \
 $(incfile) $(incfilec)

CLEANFILES=.*.swp *\~ core
