/***************************************************************************
                               qf_poly.cpp
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


// Class for polynoms with real coefficients (R[X])
// Basic operations are covered
// It includes also an algorithm to find all the roots of a real
// polynom

#undef _QF_POLY_DEBUG
#undef _QF_POLY_ROOT_DEBUG

#include "qf_common.h"
#include "qf_matrix.h"
#include "qf_qr.h"
#include "qf_poly.h"

using namespace std;

// Some constants 
// The minimum storable number
const static qf_double_t  epsilon	= numeric_limits<qf_double_t>::min ();


// A polynom is essentially a structure with an order (max. index)
// and a table storing coefficients
// There are no uninitialized polynoms (no default ctor)

//  This ctor creates irreductible real polynoms
//  Id est either constants, monoms, or binoms

qf_poly::qf_poly (const qf_double_t& a, const qf_double_t& b, 
		  const qf_double_t& c) : dg(0), k(0), cf(NULL) {

#ifdef _QF_POLY_DEBUG
  std::cout << "Ctor qf_poly (a = " << a << ", b = " << b 
	    << ", c = " << c << ")\n";
#endif

  // Special cases
  if ((a == 0) && (b == 0)) {
    // Constant
    cf = new qf_coeff (c, 1);
    k = c;
    return;
  }

  if (a == 0) {
  // Monom (bx + c)
    dg = 1;
    cf = new qf_coeff (2);
    (*cf)[0] = c;
    (*cf)[1] = b;
    k = b;
    return;
  }

  dg = 2;
  cf = new qf_coeff (3);

  (*cf)[0] = c;
  (*cf)[1] = b;
  (*cf)[2] = a;
  k = a;
}

// Creates a polynom and instantiates it out of separate variables

qf_poly::qf_poly (qf_coeff *CF, const qf_double_t& K, 
		  const unsigned DG) : dg(0), k(0), cf(NULL) {

#ifdef _QF_POLY_DEBUG
  std::cout << "Ctor qf_poly (cf = " << CF << 
	    ", k = " << K << ", dg = " << DG << ")\n";
#endif

  dg = DG;
  k = K;
  cf = CF;
  
  if ((cf != NULL) && (k != (*cf)[dg])) k = (*cf)[dg];
  return;
}

// Copy constructor

qf_poly::qf_poly (const qf_poly& P) : dg (P.dg), k (P.k), cf (NULL) {

#ifdef _QF_POLY_DEBUG
  std::cout << "qf_poly copy ctor (...)\n";
#endif

  cf = new qf_coeff (*(P.cf));
}

// Destructor

  qf_poly::~qf_poly () {

#ifdef _QF_POLY_DEBUG
  std::cout << "qf_poly dtor ()\n";
#endif
  
  delete cf;
  return;
}

// Assignement operator
// We do copy the contents

qf_poly& qf_poly::operator = (const qf_poly& P) {

#ifdef	_QF_POLY_DEBUG
  std::cout << "qf_poly =\n";
#endif

  if (& P == this)	// Self copy, nothing to do
    return (*this);

//  if (rt != NULL) rt -> clear ();
  delete cf;

  dg = P.dg;
  k = P.k;
  cf = new qf_coeff (*(P.cf));

  return (*this);
}

// Basic functions.

// This function looks for the highest non-zero term and sets
// the 'd' variable accordingly, so that we do not perform useless 
// operations on 0s
// Note that the unused 0s are not freed. We cannot do that at that
// time without copying, which is a ** overhead

unsigned  qf_poly::fdeg (const qf_coeff *cf) {

  unsigned i = cf -> size () - 1;
  for (; i && ((*cf)[i] == 0); i--) ;

  return i;
}

// Useful after additions

void qf_poly::fdeg (void) {

  if (dg == 0) return;

  dg = fdeg (cf);
  k = (*cf)[dg];
}


// Arithmetical operations

// Negates (Unary minus : P -> -P)

qf_poly qf_poly::operator - (void) {

  qf_coeff *Cf = new qf_coeff (-(*cf));

  return qf_poly (Cf, -k, dg); // Please note "-k" rather than "k"
}

// Addition

qf_poly& qf_poly::operator += (const qf_poly &P) {

  // We perform addition by summing term by term each polynom
  // Therefore, if at least one is currently under factor representation
  // we must convert it back to standard form

  // Zero case
  if (P.k == 0)
    return (* this);

  // If (*this) has a higher degree than P, we add to a slice of (*this)
  if (dg >= P. dg) {
    (*cf)[slice (0, P.dg + 1, 1)] += *(P.cf);
    if (dg == P.dg) fdeg ();  // Highest order term might have been erased
  }

  // Else, we create a new array equal to P and add cf to a slice of it
  else {
    qf_coeff *new_cf = new qf_coeff (*(P.cf));

    (*new_cf)[slice (0, dg + 1, 1)] += *cf;
    
    delete cf;
    cf = new_cf;
    dg = P.dg;
  }

  // Update k, bail out
  k = (*cf)[dg];
  return (*this);
}

// Addition on the constant term

qf_poly& qf_poly::operator += (const qf_double_t& c) {

  (*cf)[0] += c;
  if (dg == 0) k = (*cf)[0];
  return (*this);
}

// Friend operator, relies on +=

qf_poly operator + (qf_poly & P, qf_poly & Q) {

  if (Q.deg () >= P.deg ()) {
    qf_poly R (Q);
    return R += P;
  }
  else {
    qf_poly R (P);
    return R += Q;
  }
}

// Subtraction
// Similar to +=

qf_poly& qf_poly::operator -= (const qf_poly& P) {

  if (P.k == 0)
    return (* this);

  if (dg >= P.dg) {
    (*cf)[slice (0, P.dg + 1, 1)] -= *(P.cf);
    fdeg ();
  }

  else {
    qf_coeff *new_cf = new qf_coeff (- *(P.cf));
    (*new_cf)[slice (0, dg + 1, 1)] += *cf;

    delete cf;
    cf = new_cf;
    dg = P.dg;
  }

  k = (*cf)[dg];
  return (*this);
}

// This function is useful for finding roots

qf_poly&  qf_poly::operator -= (const qf_double_t& r) {

  (*cf)[0] -= r;
  if (dg == 0) k -= r;
  return (* this);
}
  
// - operator, idem relies on -=

qf_poly operator - (qf_poly& P, qf_poly& Q) {

  if (P.deg () >= Q.deg ()) {
    qf_poly R (P);
    return R -= Q;
  }
  else {
    qf_poly R (-Q);
    return R += P;
  }
}

// Multiplication of two polynoms

qf_poly operator * (qf_poly& P, qf_poly& Q) {

  // Pathological cases (may happen)

  if (Q.k == 0) // Q = 0 absorbing element
    return Q;

  if (P.k == 0) // id.
    return P;

  if ((Q.dg == 0) && (Q.k == 1)) // Q = 1 neutral element
    return P;

  if ((P.dg == 0) && (P.k == 1)) // id.
    return Q;

  if ((Q.dg == 0) && (Q.k == -1)) // Invert polynom
    return qf_poly (-P);

  if ((P.dg == 0) && (P.k == -1)) // id.
    return -Q;

  if (P.dg == 0)		  // Other cases of constant factor
    return (Q * P.k);		  // Cf. infra

  if (Q.dg == 0)
    return (P * Q.k);

  // "Normal" cases

  unsigned    dmul = P.dg + Q.dg;    
  qf_coeff *  new_cf = new qf_coeff (dmul + 1);
  qf_double_t new_k = P.k * Q.k;

  for (unsigned i = 0; i <= Q.dg; i++)
    (*new_cf)[slice (i, P.dg + 1, 1)] += *(P.cf) * (*(Q.cf))[i];

  return qf_poly (new_cf, new_k, dmul);
}

qf_poly operator * (const qf_poly& P, const qf_double_t& m) {

  if (m == 0)
    return qf_poly (0.0, 0.0, 0.0); // 0

  if (m == 1)
    return P;

  qf_poly R (P);

  if (m == -1)
    return -R;

  if (R.cf != NULL)
    *(R.cf) *= m;

  R.k *= m;
  return R;
}
  
// Self-multiply 

qf_poly& qf_poly::operator *= (const qf_double_t& m) {

  if (m == 0) { // This destroys the polynom
    k = dg = 0;
    delete cf;
    *cf = qf_coeff (0.0, 1);
    return (*this);
  }

  if (m == 1)
    return (*this);

  *cf *= m;
  k *= m;
  return (*this);
}

qf_poly& qf_poly::operator *= (const qf_poly& P) {

  if (P.dg == 0) // Same as above
    return ((* this) *= P.k);

  unsigned new_dg = dg + P.dg;

  // Resizes the coefficient list
  qf_coeff *new_cf = new qf_coeff (new_dg + 1);

  for (unsigned i = 0; i <= P.dg; i++)
    (*new_cf)[slice (i, dg + 1, 1)] += *cf * (*(P.cf))[i];

  delete cf;	
  cf = new_cf;

  k *= P.k;
  dg = new_dg;
  return *this;
}

// Test

bool qf_poly::operator == (const qf_poly& P) {

  if (&P == this) return true;

  // Two polynoms can be equal only if their degree is the same
  if (dg != P.dg) return false;

  for (unsigned i = 0; i < cf -> size (); i++)
    if (((*cf)[i]) != (*(P.cf))[i]) return false;
  
  return true;
}

bool qf_poly::operator != (const qf_poly& P) {

  return ! (*this == P);
}

// Left shift n places
// This operation is perfectly defined if and only if the polynom being
// shifted is divisible by x^n (and thus has at least n zero roots)
// Otherwise, strange behaviour will occur and a warning is issued

qf_poly qf_poly::operator << (const unsigned n) {

  // Bizarre cases once more

  if (n == 0) return *this;
  if (n > dg) return qf_poly (0.0, 0.0, 0.0);
  if (dg == n) return qf_poly (0.0, 0.0, k);

  // Okay, proceed
  
  unsigned new_dg = dg - n;

  // Verifies operation is legal
  assert (((cf -> apply(fabs))[slice (0, n, 1)]).sum () == 0);
  qf_coeff *new_cf = new qf_coeff (cf -> shift (n));
  return qf_poly (new_cf, k, new_dg);
}

// Multiplies by x^n
// Unlike its siebling above, this function is perfectly defined

qf_poly qf_poly::operator >> (const unsigned n) {

  if (n == 0) return *this;
  if (k == 0) return qf_poly (0.0, 0.0, 0.0); // Zero is zero

  unsigned new_dg = dg + n;
  qf_coeff *new_cf = new qf_coeff (0.0, new_dg + 1);
  (*new_cf)[slice (n, dg + 1, 1)] = *cf;

  return qf_poly (new_cf, k, new_dg);
}

// Creates the odd part of a polynom

qf_poly qf_poly::odd (void) {

  if (dg == 0) return qf_poly (0.0, 0.0, 0.0); // If cst returns 0

  qf_coeff *new_cf = new qf_coeff (*cf);
  (*new_cf)[slice (0, dg / 2 + 1, 2)] = 0;
  unsigned new_dg = fdeg (new_cf);

  return qf_poly (new_cf, (*new_cf)[new_dg], new_dg);
}

// Idem, but modifies the polynom itself

void qf_poly::slfodd (void) {

  (*cf)[slice (0, dg / 2 + 1, 2)] = 0;
  fdeg ();  // If P is purely even, this might even be null!
}

// Creates the even part of a polynom

qf_poly qf_poly::even (void) {

  if (dg == 0) return (*this);	// If cst returns itself

  qf_coeff *new_cf = new qf_coeff (*cf);
  (*new_cf)[slice (1, (dg + 1) / 2, 2)] = 0;
  unsigned new_dg = fdeg (new_cf);
  return qf_poly (new_cf, (*new_cf)[new_dg], new_dg);
}

// Idem, but modifies the polynom itself

void qf_poly::slfeven (void) {

  if (dg == 0) return;

  (*cf)[slice (1, (dg + 1) / 2, 2)] = 0;
  fdeg ();
}

// computes Q(X) = P(-X)

qf_poly qf_poly::mnx (void) {

  // The odd coefficient are inverted, the even unchanged

  qf_coeff *new_cf = new qf_coeff (*cf);
  qf_coeff m1 (-1.0, (dg + 1) / 2);
  (*new_cf)[slice (1, (dg + 1) / 2, 2)] *= m1;

  qf_double_t kk = (dg % 2) ? -k : k;
  return qf_poly (new_cf, kk, dg);
}

// "Half square" : P(X)P(-X)

qf_poly qf_poly::hsq (void) {

  qf_poly P (*this);
  qf_poly Q (P. mnx ());
  P *= Q;

  // There can be no odd terms
  P. slfeven ();
  return P;
}

// This function produces, from an even polynom P(X), the polynom Q(X) which 
// verifies: P(X) = Q(X^2)
// e.g. if P(X) = X^4+X^2+2, Q(X)=X^2+X+2, P(X) = Q(X^2)

qf_poly qf_poly::sqr (void) {

  if (dg == 0) return (*this);

  // There cannot be any odd order term
  assert (((cf -> apply(fabs))[slice (1, (dg + 1) / 2, 2)]).sum () == 0);

  qf_coeff *new_cf = new qf_coeff ((*cf)[slice (0, dg / 2 + 1, 2)]);

  return qf_poly (new_cf, (*new_cf)[dg / 2], dg / 2);
}

// Eliminates a prime factor
// Beware! If this prime factor does not exist, the polynom is left untouched
// a warning is issued and the function returns false

bool qf_poly::div (const Cplx& z) {

  if (dg == 0) {
    std::cout << "Warning qf_poly::div () constant polynom.\n";
    return false;
  }

  if ((dg == 1) && (imag (z) != 0)) {
    std::cout << "Warning qf_poly::div () one real root but complex dividend\n";
    return false;
  }

  // k (X - r) / (X - r) = k
  if (dg == 1) {
    dg = 0;
    delete cf;
    cf = new qf_coeff (k, 1);
    return true;
  }

  // Proceed to general case.
  // If i = 0, we divide by (X - r)
  // If i != 0, we divide by (X^2 - 2rX + r^2+i^2), that is to say
  // by (X - (r + iI))(X - (r - iI)) where I^2 = -1

  qf_poly S (0.0, 0.0, 0.0);

  if (imag (z) == 0)	
    S = qf_poly (0.0, 1.0, -real (z));  // S = X - r

  else 
    S = qf_poly (1.0, -2 * real (z), norm (z));

  // Implement polynomial division by a polynom (x^n + ...)
  unsigned ds = S. deg ();
  qf_coeff * new_cf = new qf_coeff (dg - ds + 1);

  for (unsigned i = dg; i >= ds; i--) {
    (*new_cf)[i - ds] = (*cf)[i];
    (*cf)[slice (i - ds, ds, 1)] -= *(S.cf) * (*cf)[i];
  }

  delete cf;
  cf = new_cf;
  dg -= ds;
  return true;
}

// "Hurwitzes" a polynom. 
// Out of E(X), extract an E'(X) whose roots all have a negative real
// part, so that E(X)E(-X)=E'(X)E'(-X)

// Computes 

void qf_poly::hurw (void) {

#ifdef _QF_POLY_DEBUG
  disp ("Hurw");
#endif

  qf_roots rt;
  roots (rt);

  // Eliminates roots whose real part is positive
  qf_roots  rth;
  bool	    zero = false;
  unsigned  d = 0;

  for (qf_roots::iterator i = rt.begin (); i != rt.end (); i ++) {
    if (real (*i) > 0) continue;
    if ((norm (*i) == 0) && zero) {
      zero = false;
      continue;
    }
    else
      zero = true;

    rth. push_back (*i);
    d ++;
  }

  if (k < 0) k = -k;
  k = sqrt (k);

  dg = d;
  // Terms form is no more valid
  // Recomputes coefficients
  coeff (rth);
}

// Evaluates a polynom. Computes P(a) for real a

qf_double_t qf_poly::eval (const qf_double_t& a) {

  if (dg == 0) return k;	  // Constant
  
  if (a == 0) return (*cf)[0]; // Shortcut

  qf_double_t v = k;

  for (unsigned i = dg - 1; i >= 1; i --)
    v = v * a + (*cf)[i];

  v = v * a + (*cf)[0];
  return v;
}

// Evaluates a polynom P(X^2) for X^2 = c (c can be negative)

qf_double_t qf_poly::evalX2 (const qf_double_t& c) {

  return (sqr ()).eval (c);
}

// Reverses a polynom, that is to say, if P(X) is a polynom,
// Q(X) = X^n P(1/X)

qf_poly qf_poly::rvs (void) {

  qf_coeff *new_cf = new qf_coeff ((*cf)[slice (dg, dg + 1, -1)]);
  return qf_poly (new_cf, (*cf)[0], dg);
}

// Returns the valuation of the polynom (the degree of the first
// non-null term)

unsigned qf_poly::val (void) {

  if (k == 0) return -1;			    // val (P) = - inf

  for (unsigned i = 0; i <= dg; i++)
    if ((*cf)[i] != 0) return i;

  assert (false); // Inconsistency!
  return -1;
}

// Returns the "lowest non-zero term", that is P[val(P)]

qf_double_t qf_poly::lnt (void) {
  return (*cf)[val ()];
}

// Scales a polynom
// P(X) -> P(sX)

void  qf_poly::scale (const qf_double_t& s) {

  qf_double_t S = s;

  for (unsigned i = 1; i <= dg; i++) {
    (*cf)[i] *= S;
    S *= s;
  }

  k *= pow (s, static_cast <double> (dg));
}

// Normalize a polynom vis a vis its nth term
// i.e. P = P / p[n]

void  qf_poly::nrm (unsigned n) {

  qf_double_t s = (*cf)[n];

  assert (s != 0);
  (*cf) /= s;
  k /= s;
}

// Erase highest factor

void  qf_poly::zerod (void) {

  (*cf)[dg] = 0;
  fdeg ();
}

// Zeros lowest factor

void  qf_poly::zerov (void) {
  (*cf) [val ()] = 0;
}

// Absolute value of a polynom
// P(X) -> P'(X) where all coefficients are positive

void  qf_poly::cabs (void) {
  
  (*cf) = cf -> apply (fabs);
  k = fabs (k);
}

// Return the first real root of a polynom
// True : root found; false : root not found

bool  qf_poly::rroot (qf_double_t& rr) {

  if (dg == 0) return false;
  if (dg == 1) {rr = -(*cf)[0] / (*cf)[1]; return true;}

  qf_roots rt; 
  roots (rt);

  for (qf_roots::iterator i = rt.begin (); i != rt.end (); i++)
    if (imag (*i) == 0) {rr = fabs (real (*i)); return true;}

  return false;
}

// Integrates a polynom

qf_poly	qf_poly::intgrt (void) {

  qf_coeff *new_cf = new qf_coeff (dg + 2);
  (*new_cf)[slice (1, dg + 1, 1)] = *cf;

  for (unsigned i = dg + 1; i > 0; i --)
    (*new_cf)[i] /= i;
  
  k /= (dg + 1);

  return qf_poly (new_cf, k, dg + 1);
}

// Calculates P(Q(X)) = Q o P(X)

qf_poly	qf_poly::rond (qf_poly& Q) {

  qf_poly R (0, 0, (*cf)[dg]);

  for (int i = dg - 1; i >= 0; i --) {
    R *= Q;
    R += (*cf)[i];
  }

  return R;
}
  
// Pretty prints a polynom

void qf_poly::disp (const string name) {

  if (dg == 0) {    // Constant
    std::cout << name << "(x) = " << k << '\n';
    return;
  }

  std::cout << name << "(x) = ";
  disp_cf ();
}

void qf_poly::disp_cf (void) {

  if (dg == 0) {
    std::cout << k << '\n';
    return;
  }

  if ((*cf)[dg] < 0)
    std::cout << "-";

  if (fabs ((*cf)[dg]) != 1)
    std::cout << fabs ((*cf)[dg]) << ' ';

  if (dg == 1) {
    std::cout << "x ";
  }
  else {
    std::cout << "x^" << dg << ' ';

    for (unsigned i = dg - 1; i > 1; i--) {
      qf_double_t cur_p = (*cf)[i];

      if (cur_p == 0) // Null monome
	continue;

      if (cur_p > 0) std::cout << "+ ";
      else std::cout << "- ";

      if (fabs (cur_p) != 1)
	std::cout << fabs (cur_p);
      
      std::cout << " x^" << i << ' ';
    }

    if ((*cf)[1] != 0) {
      if ((*cf)[1] > 0) std::cout << "+ ";
      else std::cout << "- ";

      if (fabs ((*cf)[1]) != 1)
	std::cout << fabs ((*cf)[1]);
    
      std::cout << " x ";
    }
  }

  if ((*cf)[0] != 0) {
    if ((*cf)[0] > 0)
      std::cout << "+ ";
    else
      std::cout << "- ";

  std::cout << fabs ((*cf)[0]);

  }
  std::cout << '\n';
}

/* This function calculates P(X) = + a[i] X^i (sum form) out of the roots
  (product form) P(X) = k * (X - r[i])
*/

void qf_poly::coeff (qf_roots& rt) {

  cf = new qf_coeff (dg + 1);
  (*cf)[0] = k;

  // Constant. Do nothing more
  if (dg == 0) return;

  // Monom k(X - r)
  if (dg == 1) {
    qf_roots::iterator i = rt.begin ();

    (*cf)[0] *= -real (*i);
    (*cf)[1] = k;
    return;
  }

  (*cf)[1] = 0;

  for (qf_roots::iterator i = rt.begin (); i != rt.end (); i++) {

    if (imag (*i) == 0) {	      // Real root

      qf_coeff new_cf (*cf);
      *cf *= -real (*i);
      *cf += new_cf.shift (-1);
    }
    else {				      // Complex conjugate root
     
      qf_double_t s = norm (*i);
      qf_double_t t = -2 * real (*i);
      qf_coeff new_cf (*cf);
      (*cf) *= s;
      (*cf) += new_cf.shift(-1) * t;
      (*cf) += new_cf.shift(-2);
      i++;
    }
  }
}

/* This function evaluates both P(z) and P'(z) for any complex z */

void  qf_poly::p_and_dp (lcplx& z, lcplx& pz, lcplx& ppz) {

  pz = (*cf)[dg];
  ppz = 0;

  // Synthetic division method

  for (int i = dg - 1; i >=0 ; i --) {
    ppz = ppz * z + pz ;
    pz = pz * z + (Longdouble) (*cf)[i];
  }
}

/* This function implements the Newton root algorithm, which serves
   here to clean up the roots.
*/

bool  qf_poly::newton (lcplx& r) {

  lcplx		    p(numeric_limits<Longdouble>::max (), 0);
  lcplx		    pp, cp = r;
  bool		    retry = false;
  int		    fpcw, t_fpcw;

#ifdef	_QF_POLY_ROOT_DEBUG
  std::cout << "Enter Newton r = " << r << '\n';
#endif


/* On x86 + GCC, ensure that we compute Longdoubles in extended IEEE format */
#if defined (__i386__) && defined (__GNUC__) && defined (extended)
  asm(	"fstcw %0\n\t" : "=m" (fpcw));
  asm(	"fstcw %0\n\t" : "=m" (t_fpcw));
  asm(	"orw  $0x0300, %0\n\t" : "=m" (t_fpcw));
  asm(	"fldcw %0\n" : : "m" (t_fpcw));

#ifdef	_QF_POLY_ROOT_DEBUG
  std::cout << "FPU CW was: 0x" << setbase (16) << fpcw << endl;
  std::cout << "FPU CW is: 0x" << setbase (16) << t_fpcw << endl;
  std::cout << setbase (10);
#endif
#endif /* __i386 & __GNUC */


  while (abs (p) != 0) {
    lcplx old_p = p;
    lcplx old_r = r;
    p_and_dp (r, p, pp);

    Longdouble np = abs (p);
#ifdef	_QF_POLY_ROOT_DEBUG
    std::cout << "\t|p| = " << np <<'\n';
    std::cout << "\t|pp| = " << abs (pp) <<'\n';
#endif

    if ((np >= abs (old_p)) || (abs (pp) == 0)) {
      if (retry) {
	r = old_r;
#ifdef	_QF_POLY_ROOT_DEBUG
	std::cout << "Exit Newton (false) r = " << r << '\n';
#endif

/* Restore x87 FPU to previous mode */
#if defined (__i386__) && defined (__GNUC__) && defined (extended)
	asm(  "fldcw %0\n" :  : "m" (fpcw));
#endif
	return false;	// Derivate is 0, error
      }
      else  {
	retry = true;
	// Try to break out of a cycle by increasing r by a random quantity
	Longdouble ri = ((Longdouble) rand () / RAND_MAX) * np;
	r += ri;
	p = numeric_limits <Longdouble>::max ();
	pp = 1.0;
	continue;
      }
    }

    lcplx dz = -p / pp;

    if (abs (dz) == 0) break;

    r += dz;
  }

/* Restore x87 FPU to previous mode */
#if defined (__i386__) && defined (__GNUC__) && defined (extended)
  asm(	"fldcw %0\n" :	: "m" (fpcw));
#endif

#ifdef	_QF_POLY_ROOT_DEBUG
  std::cout << "Exit Newton r = " << r << '\n';
#endif
  return true;
}

/* The function finds the complex roots of the polynom given by:
   p(x) = a_{n-1} * x^{n-1} + ... a_{2} * x^{2} + a_{1} * x + a_{0}
   The results are stored in the vector rst, real part followed by
   imaginary part for each complex root.  It return zero on success
   and non-zero otherwise. */

void qf_poly::roots (qf_roots& rt) {

  const qf_double_t prec = numeric_limits <qf_double_t>::epsilon () * 5;
  qf_qr		    H (dg);
  qf_roots	    est_rt;

  // Set up a suitable Hessenberg matrix H, computes its eigenvalues
  // through the QR algorithm. The eigenvalues of H are the roots of
  // the polynom, the roundoff errors are cleaned by a successive call
  // to newton ().
  assert (H. qr (*cf, est_rt));

#ifdef	_QF_POLY_ROOT_DEBUG
  cout << setprecision (12);
#endif
  // Calls newton () to refine roots and then store them
  for (qf_roots::iterator i = est_rt. begin (); i != est_rt. end (); i ++) {

    lcplx z (*i);
    newton (z);
    
    if (fabs (real (z)) < prec) {z = Cplx (0, imag (z));}
    if (fabs (imag (z)) < prec) {z = Cplx (real (z), 0);}

#ifdef	_QF_POLY_ROOT_DEBUG
    cout << "Root: " << setw (20) << left << z << '\n';
#endif

    rt.push_back (static_cast <Cplx> (z));
  }

#ifdef	_QF_POLY_ROOT_DEBUG
  cout << setprecision (4);
#endif
}
