## Process this file with automake to produce Makefile.in
#
# Makefile.am
#
# Automake input file.
#
# Copyright (C) 2015 Felix Salfelder
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.
#

# will not work anytime soon.
AM_DISTCHECK_CONFIGURE_FLAGS=--disable-doc

if ENABLE_DOC
DOC_SUBDIR = qucs-doc
else
DOC_SUBDIR =
endif

SUBDIRS = examples qucs qucs-core $(DOC_SUBDIR)

EXTRA_DIST = bootstrap VERSION README.md NEWS.md CONTRIBUTING.md

# this is insane. the qucs testsuite needs to be
#  - either moved into qucs
#  - or get its own build system
dist-hook:
	cp -r $(abs_top_srcdir)/qucs-test $(distdir); \
	rm -rf $(distdir)/qucs-test/.git

all-local:
	:

  # Run Qucs-Test suite and collect coverage
  # Need to be on `script` section to raise error as failure
  # Use qucsator in-tree to generate gcov coverage for GCC (LINUX)
  #
MORE_PATH=$(abs_top_builddir)/qucs/qucs:$(abs_top_builddir)/qucs-core/src:$(abs_top_builddir)/qucs-core/src/converter

if QUCS_TEST
check-local: qucsatorcheck eqncheck qucscheck
endif

QUCSTEST_ENV = export PATH=$(MORE_PATH):$(PATH); \
               export QUCSCONV=qucsconv; \
               export QUCS_LIBDIR=$(abs_top_srcdir)/qucs/qucs-lib/library; \
               export QUCSATOR=qucsator;

if COND_MACOSX
macossuffix=_OSX
endif

# Run tests for Qucs (GUI)
# Schematic to Netlist conversion require QucsConv and Qucs component library,
# so use the prefix of the installed package.
qucscheck: $(PWD)/qucs-test
	$(QUCSTEST_ENV) \
	cd qucs-test && python run.py --qucs --exclude skip$(macossuffix).txt

# Run test on qucsator
qucsatorcheck: $(PWD)/qucs-test
	$(QUCSTEST_ENV) \
	cd qucs-test && python run.py --prefix $(abs_top_builddir)/qucs-core/src/ \
		--qucsator --exclude skip$(macossuffix).txt

# Run tests on Qucsator equation system
eqncheck: $(PWD)/qucs-test
	$(QUCSTEST_ENV) \
	cd qucs-test && python run_equations.py --prefix $(abs_top_builddir)/qucs-core/src/

# BUG should not be necessary, but is.
$(PWD)/qucs-test:
	what=qucs-test; \
	if [ -d $$what ]; then \
		touch $$what; \
	elif [ -d $(abs_top_srcdir)/$$what ]; then \
		cp -r $(abs_top_srcdir)/$$what . || exit 5; \
		chmod -R +w $$what || exit 6; \
	else \
		echo cannot find $$what; \
		exit 1; \
	fi

.PHONY: eqncheck qucscheck qucsatorcheck

distclean-local:
	if [ x$(srcdir) = x$(builddir) ]; then\
		echo not touching qucs-test; \
	else \
		echo removing qucs-test; \
		rm -rf qucs-test; \
	fi

#if DO_DOXYGEN
#-  # Publish HTML into Qucs/qucs-doxygen repo
#-  # See: http://awestruct.org/auto-deploy-to-github-pages/
#-  # Clone repo, put the html pages on its gh_pages branch
#-  # Credentials taken from the secure environment variables GIT_NAME, GIT_EMAIL and GH_TOKEN
#-  - if [[ $LINUX && $CLANG && $MASTER ]]; then
#-      git config --global user.name ${GIT_NAME} ;
#-      git config --global user.email ${GIT_EMAIL} ;
#-      git clone https://github.com/Qucs/qucs-doxygen.git qucs-doxygen ;
#-      cd qucs-doxygen ;
#-      git checkout gh-pages ;
#-      git rm -r * ;
#-      mv ../qucs/contrib/doxygen/html/ qucs/ ;
#-      mv ../qucs-doc/doxygen/html/ qucs-core/ ;
#-      git add --all . ;
#-      git commit -m "Auto-updating Doxygen developer documentation" ;
#-      git push https://${GH_TOKEN}@github.com/Qucs/qucs-doxygen gh-pages ;
#-    fi
#endif
