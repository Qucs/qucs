## Process this file with automake to produce Makefile.in
# 
# src/converter/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.  
#

EXTRA_DIST = parse_spice.y scan_spice.l

bin_PROGRAMS = qucsconv

INCLUDES = -I$(top_srcdir)/src

qucsconv_SOURCES = qucsconv.cpp parse_spice.cpp scan_spice.cpp \
	check_spice.cpp qucs_producer.cpp

noinst_HEADERS = tokens_spice.h check_spice.h qucs_producer.h

generated_FILES = parse_spice.cpp scan_spice.cpp tokens_spice.h

scan_spice.cpp: scan_spice.l
	$(LEX) -o$@ $<
	@tmpfile="tmp.cpp"; sedfile="tmp.sed"; \
	echo "/^#include <unistd\.h>/ c\\" > $$sedfile; \
	echo "#if HAVE_UNISTD_H\\" >> $$sedfile; \
	echo "#include <unistd.h>\\" >> $$sedfile; \
	echo "#endif" >> $$sedfile; \
	cat $@ | sed -f $$sedfile > $$tmpfile; \
	mv -f $$tmpfile $@; rm -f $$tmpfile $$sedfile

parse_spice.cpp: parse_spice.y
	$(YACC) -v -o $@ --defines=tokens_spice.h $<

CLEANFILES = *~ *.orig *.rej *.output
MAINTAINERCLEANFILES = Makefile.in $(generated_FILES)
DISTCLEANFILES = $(generated_FILES)
