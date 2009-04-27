/***************************************************************************
                               qf_poly.h
                             ----------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Vincent Habchi, F5RCS
    email                : 10.50@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _QF_POLY_H
#define _QF_POLY_H

/* Headers for R[X] arithmetic */

#include <limits>
#include "qf_common.h"

#undef  _QF_POLY_DEBUG

// Setup precision for roots search
# undef	  extended

# if defined (extended)
  # define lcplx      Lcplx
  # define Longdouble long double
# else
  # define  lcplx      Cplx  
  # define  Longdouble double
# endif

// A polynom is just a valarray of coefficients
// The 'k' and 'dg' variables are always updated and valid
// k == 0 means we are dealing with the null polynom  (P = 0)
// dg == 0 means a constant polynom (P = k, k real)

// The pointers may be obsoleted, in which case the array they point to 
// is freed and they are set to NULL which means representation invalid

class qf_poly {

 private :
  unsigned	dg;			      // Current degree
  qf_double_t	k;			      // Constant k
  qf_coeff	*cf;			      // Array of coefficients

  // Functions used by solve


 public :
  qf_poly		    (const qf_double_t&, const qf_double_t&, 
			     const qf_double_t&);
  qf_poly		    (qf_coeff*, const qf_double_t&,
			     const unsigned);
  qf_poly		    (const qf_poly&);	
  ~qf_poly		    (void);

    // access operators
  qf_poly&	operator =  (const qf_poly&);	      // P = Q

    // arithmetic operators

  qf_poly	operator -  (void);		      // Unary -
    
  qf_poly&	operator += (const qf_poly&);
  qf_poly&	operator += (const qf_double_t&);
  qf_poly&	operator -= (const qf_poly&);
  qf_poly&	operator -= (const qf_double_t&);
    
friend qf_poly	operator *  (qf_poly&, qf_poly&);
friend qf_poly	operator *  (const qf_poly&, const qf_double_t&);
  qf_poly&	operator *= (const qf_poly&);	      // P(X) = P(X)*Q(X)
  qf_poly&	operator *= (const qf_double_t&);

  qf_poly	operator << (const unsigned);	      // Basic div by X^n
  qf_poly	operator >> (const unsigned);	      // Multiply by X^n

  bool		operator == (const qf_poly&);	      // Test
  bool		operator != (const qf_poly&);	      // Test
  bool		is_null	    (void) {return (k == 0);}

  unsigned	deg	    (void) {return dg;}	      // Degree of poly
  unsigned	val	    (void);		      // Valuation
  void		fdeg	    (void);		      // Find the new order
  unsigned	fdeg	    (const qf_coeff*);	      // Other form
  qf_poly 	odd	    (void);		      // Odd part
  qf_poly 	even	    (void);		      // Even part
  void	 	slfodd	    (void);		      // Odd part
  void		slfeven	    (void);		      // Even part
  qf_poly 	mnx	    (void);		      // P(X) -> P(-X)
  qf_poly 	hsq	    (void);		      // P(X)*P(-X)
  qf_poly 	sqr	    (void);		      // Q(X) = P(X^2)
  qf_double_t	eval	    (const qf_double_t&);     // P(X = a) 
  qf_double_t	evalX2	    (const qf_double_t&);     // P(X^2 = a) 

  qf_poly	rvs	    (void);

  qf_double_t	pk	    (void) {return k;}	      // Return k factor
  qf_double_t	lnt	    (void);		      // Lowest non-zero term
  bool		div	    (const Cplx&);	      // Simple division
  void		hurw	    (void);		      // "Hurwitzes" polynom
  void		scale	    (const qf_double_t&);     // Scales polynom
  void		nrm	    (unsigned);		      // Scales to p[0]=1
  void		zerod	    (void);		      // P[dg(P)] = 0
  void		zerov	    (void);		      // P[val(P)] = 0
  void		cabs	    (void);		      // Q(X) = |a(i)| x^i
  bool		rroot	    (qf_double_t&);	      // Returns 1st real root
  qf_poly	intgrt	    (void);		      // Integrates
  qf_poly	rond	    (qf_poly&);		      // P(Q(X)) = Q o P(X)

  void		coeff	    (qf_roots&);	      // Compute cf from roots
  void		roots	    (qf_roots&);	      // Find all roots
  void		p_and_dp    (lcplx&, lcplx&, lcplx&); // P(z) & P'(z)
  bool		newton	    (lcplx&);		      // Roots enhancement

  void		disp	    (const string);	      // Prints P(X)
  void		disp_cf	    (void);
  void		disp_rt	    (void);
};

qf_poly		operator +  (qf_poly&, qf_poly&);
qf_poly		operator +  (qf_poly&, qf_double_t&);
qf_poly		operator -  (qf_poly&, qf_poly&);
qf_double_t	setroottol  (qf_double_t&);
qf_double_t	setrootprec (qf_double_t&);

#endif	// _QF_POLY_H
