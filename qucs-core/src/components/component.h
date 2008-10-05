/*
 * component.h - component header file
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: component.h,v 1.61 2008-10-05 17:52:11 ela Exp $
 *
 */

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

#ifdef __MINGW32__
#define strcasecmp stricmp
#endif

#ifdef __MINGW32__
# define finite(x) _finite(x)
# ifndef isnan
# define isnan(x)  _isnan(x)
# endif
# ifndef isinf
# define isinf(x)  (!_finite(x) && !_isnan(x))
# endif
#endif

#ifdef __MINGW32__
#define acosh(x) log((x) + sqrt((x) * (x) - 1.0))
#endif

#if defined (__SVR4) && defined (__sun)
# define isinf(x) (!finite(x) && (x) == (x)) 
#endif

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "node.h"
#include "net.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "netdefs.h"

#endif /* __COMPONENT_H__ */
