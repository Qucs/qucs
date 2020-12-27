/*
 * ccoupled_shunt_resonators.h - Capacitively coupled shunt resonators filter definition
 *
 * copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */
#ifndef CCOUPLED_SHUNT_RESONATOR_FILTER_H
#define CCOUPLED_SHUNT_RESONATOR_FILTER_H	


#include "tl_filter.h"

// Quarter wave side coupled ring filter
class CCoupled_Shunt_Resonator_Filter : public TL_Filter {
public:
  CCoupled_Shunt_Resonator_Filter();

  static QString* createSchematic(tFilter*);
};

#endif
