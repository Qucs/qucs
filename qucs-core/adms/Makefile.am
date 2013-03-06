##
## RCS Info
## $Id: Makefile.am 1218 2011-04-18 08:44:12Z r29173 $
## 
## Log
## $Log$
## Revision 1.15  2006/09/06 17:29:50  r29173
## introduced new elements admsttext/admstpath
## improved admsttext parsing
##
## Revision 1.14  2006/09/04 08:51:05  r29173
## tons of simplifications
##
## Revision 1.13  2006/03/23 07:51:55  r29173
## added .dtd and .xsl files to tarball distribution
##
## Revision 1.12  2005/05/09 14:38:31  r29173
## cleaned-up source directory structure
##
## Revision 1.11  2005/05/03 09:35:19  r29173
## cleaned-up header file dependencies
##
## Revision 1.10  2005/05/03 07:58:29  r29173
## moved admsNS into admsFile
##
## Revision 1.9  2005/05/03 07:40:27  r29173
## moved admsNS into admsFile
##
## Revision 1.8  2005/03/31 18:12:01  r29173
## migration to adms-2.x.x: converted to adms2 syntax
##
## Revision 1.7  2004/10/20 16:21:05  r29173
## removed file adms.iss from distribution
## added missing info field
##
## Revision 1.6  2004/08/20 14:07:52  r29173
## tests change in CVSROOT/loginfo
## pls Marat disregard!
##
## Revision 1.5  2004/08/19 16:50:18  r29173
## moved position of admsFile
##
## Revision 1.4  2004/08/03 17:01:22  r29173
## added adms.iss to the distribution
##
## Revision 1.3  2004/08/03 12:33:54  r29173
## import adms-1.21.0 from local CVS
##
## Revision 1.1.1.1  2004/05/21 12:20:01  r29173
## recreated cvs data structure (crashed after revision 1.13.0!)
##
## Revision 1.19  2004/05/17 09:13:01  r29173
## simplified handling of variable attributes
##
## Revision 1.18  2004/05/13 07:34:51  r29173
## continue removing GNode* structure
##
## Revision 1.17  2004/05/06 12:17:19  r29173
## started removing GNode* structure
##
## Revision 1.16  2004/05/04 08:07:10  r29173
## added code for @final
## removed libltdl
##
## Revision 1.15  2004/04/19 21:03:23  r29173
## removed subdir admsTree
##
## Revision 1.14  2004/04/19 14:52:20  r29173
## cleaned-up code
## added handling of \t in xml files
##
## Revision 1.13  2004/04/13 15:09:48  r29173
## some code clean-up
##
## Revision 1.12  2004/04/01 15:06:40  r29173
## added #ifdef NEWCALC to maintain old computation of VAL, DVAL
##
## Revision 1.11  2004/01/16 10:25:04  r29173
## moved code to admsXml
##
## Revision 1.10  2004/01/16 10:24:19  r29173
## moved code to admsXml
##
## Revision 1.9  2004/01/13 11:25:14  r29173
## changed coding style
##
## Revision 1.8  2004/01/07 15:34:10  r29173
## from now use of makefile.am files
##
## Revision 1.7  2004/01/05 11:35:51  r29173
## updated for cygwin
##
## Revision 1.6  2003/08/27 08:22:03  r29173
## removed simulator specific code (mica, spectre, ads)
##
## Revision 1.5  2003/06/23 13:19:16  r29173
## fixed order of module compilation
##
## Revision 1.4  2003/06/23 07:11:42  r29173
## fixed order of module compilation
##
## Revision 1.2  2003/05/21 14:20:55  r29173
## add rcs info
##
##

ACLOCAL_AMFLAGS=-I m4

generated_png=adms-scalar.png adms-element.png adms-enumeration.png adms-uid.png adms-alias.png

incfile=constants.vams disciplines.vams adms.implicit.xml dummy.va dummy.xml
incfilec=$(incfile:=.c)

EXTRA_DIST=make.win make.cyg \
	adms.xml admst.xml admstpath.xml \
	adms.css admst.dtd metaadms.dtd metaadmst.dtd metaadmstpath.dtd \
	admsstyle.xsl admstpathstyle.xsl admststyle.xsl \
	mkctext.pl mkelements.pl mkgrammar.pl adms.h adms.c \
	preprocessorYacc.h admsPreprocessor.h verilogaYacc.h admsVeriloga.h \
	$(incfile) $(incfilec) \
	mkimages.pl $(generated_png)

if MAINTAINER_MODE
#generated files: adms.h adms.c verilogaYacc.y admstpathYacc.y admstpathYacc.h $(incfilec)
$(generated_png): ${srcdir}/mkimages.pl
	$(PERL) ${srcdir}/mkimages.pl
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

CLEANFILES=.*.swp *\~ core
