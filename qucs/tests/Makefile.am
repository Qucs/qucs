# Copyright (C) 2017 Felix Salfelder
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

include ../autotools/common.mk

#AM_CXXFLAGS = -fPIC

examplesdir=$(srcdir)/../examples

AM_DEFAULT_SOURCE_EXT = .cpp
check_LTLIBRARIES = helloworld.la element.la
LIBQUCS = ${top_builddir}/src/libqucs.la

helloworld_la_LDFLAGS = -shared -no-undefined -module -avoid-version -rpath /dev/null
element_la_LDFLAGS = -shared -no-undefined -module -avoid-version -rpath /dev/null
element_la_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) -I$(top_srcdir)/include

AM_TESTS_ENVIRONMENT = \
    export PATH='../qucs:$(PATH)' \
    export QUCS_PLUGPATH='../' \
	 REDIRECT='exec 2>&9'\
    export QUCS='qucs' \
    _REDIR='1>&9 2>&10' \
    srcdir='$(srcdir)' \
    examplesdir='$(examplesdir)';
AM_TESTS_FD_REDIRECT = 9>&2

if COND_WIN32
helloworld.dll: %.dll: %.la
	[ -f $@ ] || $(LN_S) .libs/$$(. .libs/$<; echo $$dlname) .
helloworld.log: helloworld.dll
element.dll: %.dll: %.la
	[ -f $@ ] || $(LN_S) .libs/$$(. .libs/$<; echo $$dlname) .
element.log: element.dll
else
element.so helloworld.so: %.so: %.la
	[ -f $@ ] || $(LN_S) .libs/$$(. .libs/$<; echo $$dlname) .
helloworld.log: helloworld.so
element.log: element.so
endif

SH_LOG_COMPILER = $(SHELL)
PRJ_LOG_COMPILER = $(srcdir)/prj_log_compiler
QX_LOG_COMPILER = $(srcdir)/qx_log_compiler
SH_LOG_FLAGS =
TEST_EXTENSIONS = .sh .prj .qx

check_PROGRAMS = \
	geometry0 \
	model0 \
	model1 \
	wire0 \
	wire1

TESTS = \
	${check_PROGRAMS} \
	${SH_TESTS} \
	${PRJ_TESTS} \
	${QX_TESTS}

# something wrong. fix later.
XFAIL_TESTS = \
	TR_TTL_NAND_spice.prj \
	DC_SP_HICUM-fig10.prj \
	DC_SW_bsim4v30nMOS_Ids_Vgs.prj \
	DC_SW_bsim4v30pMOS_Ids_Vgs.prj

# model0_CPPFLAGS = ${AM_CPPFLAGS} -DUNTANGLE_QT
model0_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) # yikes.
model0_LDADD = ${LIBQUCS}
model0_LDADD += $(X11_LIBS) $(QT_LIBS) # yikes

# model1_CPPFLAGS = ${AM_CPPFLAGS} -DUNTANGLE_QT
model1_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) # yikes.
model1_LDADD = ${LIBQUCS}
model1_LDADD += $(X11_LIBS) $(QT_LIBS) # yikes

# wire0_CPPFLAGS = ${AM_CPPFLAGS} -DUNTANGLE_QT
wire0_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) # yikes.
wire0_LDADD = ${LIBQUCS}
wire0_LDADD += $(X11_LIBS) $(QT_LIBS) # yikes

wire1_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) # yikes.
wire1_LDADD = ${LIBQUCS}
wire1_LDADD += $(X11_LIBS) $(QT_LIBS) # yikes

geometry0_LDADD = ${LIBQUCS}
geometry0_LDADD += $(X11_LIBS) $(QT_LIBS) # yikes


if COND_WIN32
# it does not work on windoze
doesnotexist.log: SH_LOG_FLAGS=-c 'exit 77'
endif

dist-hook:
	for i in ${PRJ_TESTS}; do \
		mkdir -p ${distdir}/$$i; \
		cp $(srcdir)/$$i/*.cir $(distdir)/$$i; 2>/dev/null \
		cp $(srcdir)/$$i/*.sch $(distdir)/$$i; \
		cp $(srcdir)/$$i/*.sch.net $(distdir)/$$i; \
	done

	mkdir -p $(distdir)/ref
	cp $(abs_srcdir)/ref/*.{v,vs,net,dup,out} $(distdir)/ref

clean-local:
	rm -rf out/

out/%.qx.out: %.qx
	../cli/qucs < $< > $@

EXTRA_DIST += \
	prj_log_compiler \
	qx_log_compiler \
	$(SH_TESTS) \
	$(QX_TESTS)

CLEANFILES = \
	*.sch.net \
	*.out *core

.PHONY: .P
${PRJ_TESTS:%.prj=%.log} : .P
${SH_TESTS:%.sh=%.log} : .P
${QX_TESTS:%.qx=%.log} : .P
${check_PROGRAMS:%=%.log} : .P
