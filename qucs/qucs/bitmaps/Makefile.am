## Process this file with automake to produce Makefile.in
# 
# qucs/bitmaps/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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

XPMS = ac_voltage.xpm arrow.xpm capacitor.xpm dc_current.xpm dc_voltage.xpm  \
  inductor.xpm line.xpm msline.xpm polar.xpm rect.xpm rectangle.xpm          \
  resistor.xpm resistor_us.xpm smith.xpm source.xpm symtrans.xpm tabular.xpm \
  text.xpm tline.xpm transformer.xpm ground.xpm dc.xpm hb.xpm sparameter.xpm \
  sweep.xpm tran.xpm cccs.xpm ccvs.xpm vccs.xpm vcvs.xpm ac.xpm              \
  attenuator.xpm biast.xpm circulator.xpm dcblock.xpm dcfeed.xpm diode.xpm   \
  isolator.xpm mscorner.xpm mscross.xpm msstep.xpm mstee.xpm substrate.xpm   \
  port.xpm spfile1.xpm spfile2.xpm spfile3.xpm spfile4.xpm spfile5.xpm       \
  spfile6.xpm coplanar.xpm ellipse.xpm mscoupled.xpm gyrator.xpm             \
  pshifter.xpm

PNGS = fileopen.png filesave.png editdelete.png editcut.png editcopy.png     \
  deactiv.png bottom.png editpaste.png equation.png fileclose.png            \
  fileprint.png filesaveall.png gear.png ground.png mirror.png nodename.png  \
  pointer.png port.png rebuild.png redo.png rotate_ccw.png top.png undo.png  \
  viewmag+.png viewmag-.png viewmagfit.png wire.png filenew.png mirrory.png  \
  viewmag1.png 

EXTRA_DIST = $(XPMS) $(PNGS)

# installation of pictures
picdatadir = ${prefix}/share/qucs/bitmaps
picdata_DATA = $(EXTRA_DIST)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
