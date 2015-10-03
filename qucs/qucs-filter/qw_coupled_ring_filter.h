/*
 * qw_coupled_ring_filter.h - Quarter wavelength filter definition
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
#ifndef QW_COUPLED_RING_FILTER_H
#define QW_COUPLED_RING_FILTER_H


#include "tl_filter.h"

// Quarter wave side coupled ring filter
class QW_Coupled_Ring_Filter : public TL_Filter {
public:
  QW_Coupled_Ring_Filter();

  static QString* createSchematic(tFilter*, tSubstrate*, bool);
};

#endif
