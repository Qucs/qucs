/*
 * libqucsator.h - qucsator library include file
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: libqucsator.h,v 1.3 2004/09/08 18:25:19 ela Exp $
 *
 */

#ifndef __LIBQUCSATOR_H__
#define __LIBQUCSATOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef double nr_double_t;

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "strlist.h"
#include "vector.h"
#include "matrix.h"
#include "matvec.h"
#include "dataset.h"
#include "exception.h"
#include "exceptionstack.h"
#include "check_dataset.h"
#include "check_touchstone.h"

#endif /* __LIBQUCSATOR_H__ */
