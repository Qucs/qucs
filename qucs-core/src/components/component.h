/*
 * component.h - global component header file
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: component.h,v 1.5 2004-02-09 18:27:42 ela Exp $
 *
 */

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "ground.h"
#include "open.h"
#include "tee.h"
#include "cross.h"
#include "itrafo.h"

#include "resistor.h"
#include "capacitor.h"
#include "inductor.h"
#include "vccs.h"
#include "cccs.h"
#include "ccvs.h"
#include "vcvs.h"
#include "dcblock.h"
#include "dcfeed.h"
#include "biastee.h"
#include "pac.h"
#include "attenuator.h"
#include "circulator.h"
#include "isolator.h"
#include "trafo.h"
#include "strafo.h"
#include "vdc.h"
#include "idc.h"
#include "vac.h"
#include "phaseshifter.h"
#include "gyrator.h"
#include "tline.h"
#include "iprobe.h"

#include "diode.h"

#endif /* __COMPONENT_H__ */
