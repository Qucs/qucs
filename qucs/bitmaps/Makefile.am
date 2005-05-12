## Process this file with automake to produce Makefile.in
# 
# qucs/bitmaps/Makefile.am
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

# component pictures
XPMS = ac_voltage.png arrow.png capacitor.png dc_current.png dc_voltage.png   \
  inductor.png line.png msline.png polar.png rect.png rectangle.png           \
  resistor.png resistor_us.png smith.png source.png symtrans.png tabular.png  \
  text.png tline.png transformer.png gnd.png dc.png hb.png sparameter.png     \
  sweep.png tran.png cccs.png ccvs.png vccs.png vcvs.png ac.png cpwshort.png  \
  attenuator.png biast.png circulator.png dcblock.png dcfeed.png diode.png    \
  isolator.png mscorner.png mscross.png msstep.png mstee.png substrate.png    \
  subport.png spfile1.png spfile2.png spfile3.png spfile4.png spfile5.png     \
  spfile6.png coplanar.png ellipse.png mscoupled.png gyrator.png cpwgap.png   \
  pshifter.png iprobe.png noise_current.png noise_volt.png msmbend.png        \
  msopen.png filledellipse.png filledrect.png ac_current.png npn.png pnp.png  \
  nfet.png pfet.png nmosfet.png pmosfet.png dmosfet.png msgap.png npnsub.png  \
  pnpsub.png dmosfet_sub.png nmosfet_sub.png pmosfet_sub.png ellipsearc.png   \
  vpulse.png ipulse.png vrect.png irect.png msvia.png amplifier.png opamp.png \
  ysmith.png smithpolar.png polarsmith.png spicefile.png rect3d.png curve.png \
  cpwopen.png  

# toolbar pictures
PNGS = fileopen.png filesave.png editdelete.png editcut.png editcopy.png      \
  deactiv.png bottom.png editpaste.png equation.png fileclose.png pointer.png \
  fileprint.png filesaveall.png gear.png mirror.png nodename.png marker.png   \
  port.png rebuild.png redo.png rotate_ccw.png top.png undo.png viewmag1.png  \
  viewmag+.png viewmag-.png viewmagfit.png wire.png filenew.png mirrory.png

# application pictures
ICONS = big.button.qucs.xpm big.qucs.xpm tiny.button.qucs.xpm tiny.qucs.xpm

EXTRA_DIST = $(XPMS) $(PNGS) $(ICONS)

# installation of pictures
picdatadir = ${prefix}/share/qucs/bitmaps
picdata_DATA = $(EXTRA_DIST)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
