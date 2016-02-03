incfile=constants.vams disciplines.vams adms.implicit.xml dummy.va dummy.xml
incfilec=$(incfile:=.c)

generated_FILES=adms.h adms.c verilogaYacc.y admstpathYacc.y admstpathYacc.h $(incfilec) \
                verilogaLex.c verilogaYacc.c verilogaYacc.h admstpathYacc.c \
                preprocessorLex.c preprocessorYacc.c preprocessorYacc.h

SUFFIXES=.vams .vams.c .va .va.c .xml .xml.c .y.in

# use mkctext.pl to create C files containing the contents of each input file
# in a C character array. The output is used by make check only

# Perl XML::LibXml module are only required for maintainer
# Parser created by maintainer
if MAINTAINER_MODE

$(incfilec):%.c:% ${srcdir}/mkctext.pl
	$(PERL) ${srcdir}/mkctext.pl $< $(top_srcdir)

admstpathYacc.y admstpathYacc.h adms.h adms.c: ${top_srcdir}/adms.xml ${top_srcdir}/admstpath.xml ${srcdir}/mkelements.pl
	$(PERL) ${srcdir}/mkelements.pl $(top_srcdir)

verilogaYacc.y: ${srcdir}/verilogaYacc.y.in ${srcdir}/mkgrammar.pl
	$(PERL) ${srcdir}/mkgrammar.pl ${srcdir}/verilogaYacc.y.in

endif

dist_man_MANS = admsXml.1 admsCheck.1

noinst_LIBRARIES=libadmsElement.a libadmsPreprocessor.a libadmsVeriloga.a libadmsAdmstpath.a

libadmsElement_a_SOURCES=adms.c

preprocessorLex.l: $(incfilec) preprocessorYacc.c preprocessorYacc.h
preprocessorYacc.h: preprocessorYacc.c
libadmsPreprocessor_a_SOURCES=preprocessorLex.l preprocessorMain.c preprocessorYacc.h preprocessorYacc.y

verilogLex.l: $(incfilec) verilogaYacc.c verilogYacc.h
verilogYacc.h: verilogYacc.c
libadmsVeriloga_a_SOURCES=verilogaYacc.y verilogaLex.l verilogaYacc.h

admstpathYacc.y: admstpathYacc.h
libadmsAdmstpath_a_SOURCES= admstpathYacc.y admstpathYacc.h

#depends on adms.implicit.xml
bin_PROGRAMS=admsXml admsCheck

admsXml_SOURCES=admsXml.c
admsXml_LDADD=-lm libadmsElement.a libadmsVeriloga.a libadmsPreprocessor.a libadmsAdmstpath.a

admsXml.c: adms.implicit.xml.c

# Rules to create the admsCheck test program
admsCheck.c=$(incfilec)
admsCheck_SOURCES=admsCheck.c
admsCheck_LDADD=libadmsElement.a

EXTRA_DIST=mkctext.pl mkelements.pl mkgrammar.pl adms.h adms.c \
 preprocessorYacc.h admsPreprocessor.h verilogaYacc.h admsVeriloga.h \
 $(incfile) $(incfilec) verilogaYacc.y.in admstpath.dtd \
 CMakeLists.txt ../config.h basicLibrary.xml

CLEANFILES=.*.swp *\~ core $(generated_FILES)
