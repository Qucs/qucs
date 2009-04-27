/***************************************************************************
                               qf_filter.cpp
                             -----------------
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"

#undef	_QF_FILTER_DEBUG

// Constructors 
// Just initialize a few things

qf_filter::qf_filter (qf_spec* Psp): 

    Pspec  (Psp),
    E	  (0.0, 0.0, 1.0),
    F	  (0.0, 0.0, 1.0), 
    P	  (0.0, 0.0, 1.0), 
    BN	  (0.0, 0.0, 1.0),
    BD	  (0.0, 0.0, 1.0),
    BNc	  (0.0, 0.0, 1.0), 
    BDc	  (0.0, 0.0, 1.0),
    lcmp  ()
{}

// Copy ????

  qf_filter::qf_filter (const qf_filter &FL) : 
    Pspec  (FL. Pspec),
    E	  (FL.E),
    F	  (FL.F),
    P	  (FL.P),
    BN	  (FL.BN),
    BD	  (FL.BD),
    BNc	  (FL.BNc),
    BDc	  (FL.BDc),
    lcmp  (FL.lcmp) 
{}


// Out of polynoms P and F, calculate the susceptance B1 = BN/BD
// Of the filter

void qf_filter::scptX1	  (qf_poly & F,
			   qf_poly & P,
			   qf_poly & BN,
			   qf_poly & BD) {

  // Computes Hurwitz polynomial E
  // E(x)E(-x) = F(x)F(-x) + P(x)P(-x)
  qf_poly FH (F. hsq ());
  qf_poly PH (P. hsq ());

#ifdef _QF_FILTER_DEBUG
  FH.disp ("FH");
  PH.disp ("PH");
#endif

  E = FH + PH;
  E. hurw ();

#ifdef _QF_FILTER_DEBUG
  E. disp ("E");
#endif

  qf_poly EE (E. even ());
  qf_poly EO (E. odd ());
  qf_poly FE (F. even ());
  qf_poly FO (F. odd ());

  // BN must be of the same order as E
  BN = EO + FO;

  if (BN. deg () != E. deg ()) {
    BN = EE + FE;
    BD = EO - FO;
    return;
  }

  BD = EE - FE;
}

// Out of polynoms E and F, calculate the susceptance B1 = BN/BD
// Of the filter

void qf_filter::scptX2	  (qf_poly & E,
			   qf_poly & F,
			   qf_poly & BN,
			   qf_poly & BD) {

  qf_poly EE (E. even ());
  qf_poly EO (E. odd ());
  qf_poly FE (F. even ());
  qf_poly FO (F. odd ());

  // BN must be of the same order as E
  BN = EO + FO;

  if (BN. deg () != E. deg ()) {
    BN = EE + FE;
    BD = EO - FO;
    return;
  }

  BD = EE - FE;
}

// Out of polynoms P and E, calculate the susceptance B1 = BN/BD
// Of the filter

void qf_filter::scptX3	  (qf_poly & E,
			   qf_poly & P,
			   qf_poly & BN,
			   qf_poly & BD) {

  // Computes Hurwitz polynomial F
  // F(x)F(-x) = E(x)E(-x) - P(x)P(-x)
  qf_poly EH (E. hsq ());
  qf_poly PH (P. hsq ());

#ifdef	_QF_FILTER_DEBUG
  EH. disp ("EH");
  PH. disp ("PH");
#endif

  F = EH - PH;
  F. hurw ();

#ifdef _QF_FILTER_DEBUG
  F. disp ("F");
#endif

  qf_poly EE (E. even ());
  qf_poly EO (E. odd ());
  qf_poly FE (F. even ());
  qf_poly FO (F. odd ());

  // BN must be of the same order as E
  BN = EO + FO;

  if (BN. deg () != E. deg ()) {
    BN = EE + FE;
    BD = EO - FO;
    return;
  }

  BD = EE - FE;
}


// Extraction routines

// Extract finite attenuation pole (first version)
// Result is a shunt cap to ground, and a serial L||C
// Or, dual, a serial ind and a shunt L+C

bool qf_filter::pole_finite (qf_double_t p, 
			     qf_double_t Ws,
			     qf_trp*	 trp) { 

  // Partial removal of infinite pole (first // cap)
  // c = [B(s)/s] (s^2 = - W^2)

  unsigned BDd = BD.deg ();
  unsigned BNd = BN.deg ();
  bool	   is_neg = false;

  assert ((BDd + BNd) % 2 != 0);

  qf_double_t  pl = -p * p;			    // s^2 = - W^2
  qf_double_t  v;

  if (BDd % 2 == 0) {
    v = ((BN << 1).evalX2 (pl)) / (BD.evalX2 (pl));  // BN/s (s^2) / BD (s^2)
  }
  else
    v = BN.evalX2 (pl) / ((BD >> 1).evalX2 (pl));    // BN / sBD

  is_neg = (v < 0);

  trp -> v = v * Ws;

#ifdef _QF_FILTER_DEBUG
  std::cout << "Cp or Ls = " << v * Ws << '\n';
#endif

  qf_poly BDS (BD >> 1);	  // BD * s
  BDS *= v;			  // c * BD * s 
  BN -= BDS;			  // BN = BN - c * BD * s. 
#ifdef _QF_FILTER_DEBUG
  BN.disp("BN");
#endif
				  // BN is divisible by s^2+W^2
  Cplx pc (0, p);		  // Pole z = jW

  BN.div (pc);
  // Full removal of finite pole

  if (BDd % 2 == 0)
    v = (BN >> 1).evalX2 (pl) / BD.evalX2 (pl); // c1 = s * BN' / BD
  else
    v = BN.evalX2 (pl) / (BD << 1).evalX2 (pl);

  is_neg = (v < 0) || is_neg;	// Must remember if v is already true

  trp -> w1 = v * Ws;
  trp -> w2 = - Ws / (v * pl);

#ifdef _QF_FILTER_DEBUG
  std::cout << "C(s) = " << trp -> w1 << '\n';
  std::cout << "L(s) = " << trp -> w2 << '\n';
#endif

  // 1/B = 1/B - (s/c1) / (s^2 + W^2)

  qf_poly BNS (BN >> 1);
  BNS *= (1 / v);
  BD -= BNS;
  if (BD. deg () == 0)	// Removal of a final pole
    return is_neg;

  pc = Cplx (0, p);

  BD.div (pc);

#ifdef _QF_FILTER_DEBUG
  BN.disp ("BN");
  BD.disp ("BD");
#endif

  return is_neg;  // True if we have generated a negative comp
}

// Extract a single pole at infinity (shunt C)

bool qf_filter::pole_inf_c (qf_double_t& v, qf_double_t Ws) {

  // C = B(s) / s, s = INF
  
  if (BD. pk () == 0)
    v = BN. pk ();

  else {
    assert (BN.deg () == (BD.deg () + 1));
    v = BN.pk () / BD.pk ();
  }

#ifdef _QF_FILTER_DEBUG
  std::cout << "C(p) = " << v << endl;
#endif

  unsigned d = BN. deg ();
  qf_poly BDSc ((BD >> 1) * v);
  BN -= BDSc;    // B(s) = B(s) - C * s
  if (BN. deg () == d) BN. zerod ();  // Cancels roundoff errors

#ifdef _QF_FILTER_DEBUG
  BN.disp ("BN'");
  BD.disp ("BD");
#endif

  v *= Ws;
  return (v < 0);
}

// Extract a single pole at infinity (serial L)

bool qf_filter::pole_inf_l (qf_double_t& v, qf_double_t Ws) {

  // L = X(s) / s, s = INF
  
  if (BN. pk () == 0)
    v = BD. pk ();

  else {
    assert (BD.deg () == (BN.deg () + 1));
    v = BD.pk () / BN.pk ();
  }


#ifdef _QF_FILTER_DEBUG
  std::cout << "L(s) = " << v << endl;
#endif

  unsigned d = BD. deg ();
  qf_poly BNSc ((BN >> 1) * v);
  BD -= BNSc;    // B(s) = B(s) - C * s
  if (BD. deg () == d)	BD. zerod ();

#ifdef _QF_FILTER_DEBUG
  BN.disp ("BN'");
  BD.disp ("BD");
#endif

  v *= Ws;
  return (v < 0);
}

// Extract a single pole at zero (serial C)

bool qf_filter::pole_zero_c (qf_double_t& v, qf_double_t Ws) {

  // C = B(s) / s, s = 0
  assert (BD. val () == (BN. val () - 1));
  v = BN. lnt () / BD. lnt ();
 
#ifdef _QF_FILTER_DEBUG
  std::cout << "C(s) = " << v << endl;
#endif

  unsigned d = BD. val ();
  BD = ((BD >> 1) * v);
  BD -= BN;				  // BD' = v * s * BD - BN
  BN = (BN >> 1) * v;			  // BN' = v * s * BN
  if (BD. val () == d + 1) BN. zerov ();  // Cancels roundoff errors

#ifdef _QF_FILTER_DEBUG
  BN.disp ("BN'");
  BD.disp ("BD'");
#endif

  v *= Ws;
  return (v < 0);
}

// Extract a single pole at zero (shunt L)

bool qf_filter::pole_zero_l (qf_double_t& v, qf_double_t Ws) {

  // L = X(s) / s, s = 0
  assert (BN. val () == (BD. val () - 1));
  v = BD. lnt () / BN. lnt ();
 
#ifdef _QF_FILTER_DEBUG
  std::cout << "L(s) = " << v << endl;
#endif

  unsigned d = BN. val ();
  BN = ((BN >> 1) * v);
  BN -= BD;				  // BD' = v * s * BD - BN
  BD = (BD >> 1) * v;			  // BN' = v * s * BN
  if (BN. val () == d + 1) BD. zerov ();  // Cancels roundoff errors

#ifdef _QF_FILTER_DEBUG
  BN.disp ("BN'");
  BD.disp ("BD'");
#endif

  v *= Ws;
  return (v < 0);
}

// General synthesis routine for ladder filters
// These all poles filters are made of serial ind and shunt cap.
// n is the order of the filter
// s is a scale factor, useful in case of Bessel filters

bool qf_filter::synth_ladder (qf_double_t s) {

  qf_double_t val;
  unsigned    n = Pspec -> ord;

  if (! Pspec -> dual) {
    while (n) {
      pole_inf_c (val, 1);
      lcmp. insert (new qf_cap (val / s, true, true));
      n --;

      if (n == 0) break;

      pole_inf_l (val, 1);
      lcmp. insert (new qf_ind (val / s, false, false));
      n --;
    }
  }

  else {
    while (n) {
      pole_inf_c (val, 1);
      lcmp. insert (new qf_ind (val / s, false, false));
      n --;

      if (n == 0) break;

      pole_inf_l (val, 1);
      lcmp. insert (new qf_cap (val / s, true, true));
      n --;
    }
  }

  lcmp. insert (new qf_end ());
  return true;
}
