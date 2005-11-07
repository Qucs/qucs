#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(qucs-doc,0.0.8,qucs-bugs@lists.sourceforge.net)
AC_CONFIG_SRCDIR([technical/technical.tex])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([no-define])
AM_MAINTAINER_MODE
AC_PREFIX_DEFAULT([/usr/local])
test "x$prefix" = xNONE && prefix="/usr/local"

AC_PATH_PROG(LATEX, latex, :)
AC_PATH_PROG(PDFLATEX, pdflatex, :)
AC_PATH_PROG(FIG2DEV, fig2dev, :)
AC_PATH_PROG(BIBTEX, bibtex, :)
AC_PATH_PROG(DVIPS, dvips, :)
AC_PATH_PROG(PS2EPS, ps2eps, :)
AC_PATH_PROG(EPSTOPDF, epstopdf, :)
AC_PATH_PROG(OCTAVE, octave, :)
AC_PATH_PROG(LATEX2HTML, latex2html, :)
AC_PATH_PROG(TEX2PDF, tex2pdf, :)

dnl Add here all your Makefiles. These are created by configure.
AC_CONFIG_FILES([Makefile
                 technical/Makefile
		 tutorial/Makefile
		 tutorial/dcstatic/Makefile
		 tutorial/dcstatic/pics/Makefile
		 tutorial/coupler/Makefile
		 tutorial/coupler/pics/Makefile
		 tutorial/modelbjt/Makefile
		 tutorial/modelbjt/pics/Makefile
		 tutorial/poweramp/Makefile
		 tutorial/poweramp/pics/Makefile
		 tutorial/lna/Makefile
		 tutorial/bjtbias/Makefile
		 tutorial/bjtbias/pics/Makefile])

AC_OUTPUT

# Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
