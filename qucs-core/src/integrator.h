/*
 * integrator.h - integrator class definitions
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
 * $Id: integrator.h,v 1.1 2004/09/12 14:09:19 ela Exp $
 *
 */

#ifndef __INTEGRATOR_H__
#define __INTEGRATOR_H__

class integrator
{
 public:
  // constructor and destructor set
  integrator ();
  integrator (const integrator &);
  ~integrator ();

  // save-state variables for transient analysis
  nr_double_t getState (int, int n = 0);
  void setState (int, nr_double_t, int n = 0);
  void initStates (void);
  void clearStates (void);
  int getStates (void) { return states; }
  void setStates (int n) { states = n; }
  void nextState (void);
  void prevState (void);
  void fillState (int, nr_double_t);

  // integration specific
  typedef void (* integrate_func_t)
    (integrator *, int, nr_double_t, nr_double_t&, nr_double_t&);
  void setIntegration (integrate_func_t f) { integrate_func = f; }
  void integrate (int qstate, nr_double_t cap, nr_double_t& geq,
		  nr_double_t& ceq) {
    (*integrate_func) (this, qstate, cap, geq, ceq);
  }
  void setOrder (int o) { order = o; }
  int  getOrder (void) { return order; }
  void setCoefficients (nr_double_t * c) { coefficients = c; }
  nr_double_t * getCoefficients (void) { return coefficients; }

 private:
  nr_double_t * stateval;
  int states;
  int currentstate;
  int order;
  nr_double_t * coefficients;
  integrate_func_t integrate_func;
};

#endif /* __INTEGRATOR_H__ */
