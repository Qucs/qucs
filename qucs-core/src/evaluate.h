/*
 * evaluate.h - definitions for Qucs equation evaluations
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
 * $Id: evaluate.h,v 1.1 2004/03/21 18:55:48 ela Exp $
 *
 */

#ifndef __EVALUATE_H__
#define __EVALUATE_H__

namespace eqn {

class constant;

/* This class is merely a container for the applications which can be
   applied to equation constants. */
class evaluate
{
public:
  static constant * plus_d_d (constant *);
  static constant * plus_c_d (constant *);
  static constant * plus_d_c (constant *);
};

// Type of application function.
typedef constant * (* evaluator_t) (constant *);

// Structure defining an application.
struct application_t
{
  char * application; /* the name of the application     */
  int retval;         /* its return type                 */
  evaluator_t eval;   /* the actual application function */
  int nargs;          /* number of arguments             */
  int args[16];       /* the appropriate argument types  */
};

extern struct application_t applications[];

} /* namespace */

#endif /* __EVALUATE_H__ */
