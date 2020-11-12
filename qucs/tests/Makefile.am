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

AM_CXXFLAGS = -fPIC

examplesdir=$(srcdir)/../examples

AM_DEFAULT_SOURCE_EXT = .cpp
check_LTLIBRARIES = helloworld.la element.la

helloworld_la_LDFLAGS = -shared -no-undefined -module -avoid-version -rpath /dev/null
element_la_LDFLAGS = -shared -no-undefined -module -avoid-version -rpath /dev/null
element_la_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) -I$(top_srcdir)/include

AM_TESTS_ENVIRONMENT = \
    export PATH='../qucs:$(PATH)' \
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
SH_LOG_FLAGS =
TEST_EXTENSIONS = .sh .prj

TESTS = \
	${check_PROGRAMS} \
	${SH_TESTS} \
	${PRJ_TESTS}


SH_TESTS = \
	helloworld.sh \
	element.sh \
	doesnotexist.sh \
	entries.sh \
	run.sh

PRJ_TESTS = \
	netlist_bug.prj \
	AC_bandpass.prj \
	AC_groupdelay_ac.prj \
	AC_SW_resonance.prj \
	DC_SP_LPF-Balun2.prj \
	DC_TR_active_mixer.prj \
	SP_mscoupler.prj \
	TR_resistor.prj \
	TR_sawtooth-1.prj

LATER_TESTS = \
	AC_SW_swr_meter.prj \
	AC_wattmeter.prj \
	DC_AC_active_bp.prj \
	DC_AC_active_lp.prj \
	DC_AC_bbv.prj \
	DC_AC_gain.prj \
	DC_AC_gyrator.prj \
	DC_AC_notch.prj \
	DC_AC_selective_amp.prj \
	DC_AC_SP_HF_BPF.prj \
	DC_AC_SP_Spiral_BPF.prj \
	DC_AC_SP_stab.prj \
	DC_SP_HICUM-fig10.prj \
	DC_SP_LPF-Balun3.prj \
	DC_SP_opamp_gyrator.prj \
	DC_SW_bridge.prj \
	DC_SW_bsim4v30nMOS_Ids_Vgs.prj \
	DC_SW_bsim4v30pMOS_Ids_Vgs.prj \
	DC_SW_charac.prj \
	DC_SW_curtice_1_tb1.prj \
	DC_SW_diff1.prj \
	DC_SW_diode.prj \
	DC_SW_fgummel.prj \
	DC_SW_preregulator.prj \
	DC_SW_rgummel.prj \
	DC_TR_SW_TestHBProbe.prj \
	SP_bpf_10Ghz.prj \
	SP_chebyshev1_5th.prj \
	SP_CIRCWG.prj \
	SP_elliptic_5th.prj \
	SP_fet_noise.prj \
	SP_fhr01fh.prj \
	SP_giacoletto.prj \
	SP_groupdelay_sp.prj \
	SP_microstrip.prj \
	SP_SmithChartTest.prj \
	SP_Sparam_diagrams.prj \
	SP_sparam.prj \
	SP_wilkinson.prj \
	TR_555_Fig7.prj \
	TR_555_Fig9.prj \
	TR_boostconverter.prj \
	TR_buckboost.prj \
	TR_buckconverter.prj \
	TR_chargepump.prj \
	TR_classic_osci.prj \
	TR_colpitts_base.prj \
	TR_colpitts.prj \
	TR_diode_hb.prj \
	TR_fullwaverectifier_1.prj \
	TR_fullwaverectifier_2.prj \
	TR_gilbert.prj \
	TR_lc_osc.prj \
	TR_lf_osci.prj \
	TR_mixer.prj \
	TR_multiplier.prj \
	TR_Puls3b.prj \
	TR_rf_osci.prj \
	TR_sawtooth-2.prj \
	TR_sawtooth-3.prj \
	TR_sawtooth-discreet.prj \
	TR_schmitt.prj \
	TR_single_balanced.prj \
	TR_supply.prj \
	TR_sym_osci.prj \
	TR_TTL_NAND_spice.prj

# something wrong. fix later.
XFAIL_TESTS = \
	TR_TTL_NAND_spice.prj \
	DC_SP_HICUM-fig10.prj \
	DC_SW_bsim4v30nMOS_Ids_Vgs.prj \
	DC_SW_bsim4v30pMOS_Ids_Vgs.prj


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
	cp -r $(abs_srcdir)/ref/*.{v,vs,net,dup} $(distdir)/ref

clean-local:
	rm -rf out/

EXTRA_DIST = \
	${SCHEMATICS:%=%.net.ref} \
	${SCHEMATICS:%=%.ref} \
	entries.ref \
	prj_log_compiler \
	$(SH_TESTS)

CLEANFILES = \
	*.sch.net \
	*.out *core

.PHONY: .P
${PRJ_TESTS:%.prj=%.log} : .P
${SH_TESTS:%.sh=%.log} : .P
