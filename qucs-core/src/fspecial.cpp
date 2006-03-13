/*
 * fspecial.cpp - special functions implementation
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: fspecial.cpp,v 1.1 2006-03-13 08:26:25 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "constants.h"
#include "fspecial.h"

#ifdef __MINGW32__
# define finite(x) _finite(x)
# ifndef isnan
# define isnan(x)  _isnan(x)
# endif
# ifndef isinf
# define isinf(x)  (!_finite(x) && !_isnan(x))
# endif
#endif
#if defined (__SVR4) && defined (__sun)
# define isinf(x) (!finite(x) && (x) == (x)) 
#endif

/* The function computes the complete elliptic integral of first kind
   K() and the second kind E() using the arithmetic-geometric mean
   algorithm (AGM) by Abramowitz and Stegun. */
void fspecial::ellip_ke (nr_double_t arg, nr_double_t &k, nr_double_t &e) {
  int iMax = 16;
  if (arg == 1.0) {
    k = NR_INF; // infinite
    e = 0;
  }
  else if (isinf (arg) && arg < 0) {
    k = 0;
    e = NR_INF; // infinite
  }
  else {
    nr_double_t a, b, c, f, s, fk = 1, fe = 1, t, da = arg;
    int i;
    if (arg < 0) {
      fk = 1 / sqrt (1 - arg);
      fe = sqrt (1 - arg);
      da = -arg / (1 - arg);
    }
    a = 1;
    b = sqrt (1 - da);
    c = sqrt (da);
    f = 0.5;
    s = f * c * c;
    for (i = 0; i < iMax; i++) {
      t = (a + b) / 2;
      c = (a - b) / 2;
      b = sqrt (a * b);
      a = t;
      f *= 2;
      s += f * c * c;
      if (c / a < NR_EPSI) break;
    }
    if (i >= iMax) {
      k = 0; e = 0;
    }
    else {
      k = M_PI_2 / a;
      e = M_PI_2 * (1 - s) / a;
      if (arg < 0) {
	k *= fk;
	e *= fe;
      }
    }
  }
}

const nr_double_t SN_ACC = 1e-5;	// Accuracy of sn(x) is SN_ACC^2
const nr_double_t K_ERR  = 1e-8;	// Accuracy of K(k)

// Computes Carlson's elliptic integral of the first kind
nr_double_t fspecial::ellip_rf (nr_double_t x, nr_double_t y, nr_double_t z) {
  nr_double_t al, av, dx, dy, dz, e2, e3;
  nr_double_t sx, sy, sz, xt, yt, zt;

  // constants
  const nr_double_t c1 = 1.0 / 24.0;
  const nr_double_t c2 = 0.1;
  const nr_double_t c3 = 3.0 / 44.0;
  const nr_double_t c4 = 1.0 / 14.0;

  xt = x;
  yt = y;
  zt = z;
  do {
    sx = sqrt (xt);
    sy = sqrt (yt);
    sz = sqrt (zt);
    al = sx * (sy + sz) + sy * sz;
    xt = 0.25 * (xt + al);
    yt = 0.25 * (yt + al);
    zt = 0.25 * (zt + al);
    av = (xt + yt + zt) / 3.0;
    dx = (av - xt) / av;
    dy = (av - yt) / av;
    dz = (av - zt) / av;
  }
  while (MAX (MAX (fabs (dx), fabs (dy)), fabs (dz)) > K_ERR);

  e2 = dx * dy - dz * dz;
  e3 = dx * dy * dz;
  return (1 + (c1 * e2 - c2 - c3 * e3) * e2 + c4 * e3) / sqrt (av);
}

// Compute the Jacobian elliptic functions sn (u,k), cn (u,k) and dn (u,k).
nr_double_t fspecial::ellip_sncndn (nr_double_t u, nr_double_t k,
		      nr_double_t& sn, nr_double_t& cn, nr_double_t& dn) {
  nr_double_t a, b, c, d;
  nr_double_t fn[14], en[14];
  int l;
  bool bo;

  d = 1 - k;
  if (k) {
    bo = (k < 0);
    if (bo) {
      k /= -1 / d;
      u *= (d = sqrt (d));
    }
    a = 1;
    dn = 1;
    for (int i = 1; i < 14; i++) {
      l = i;
      fn[i] = a;
      en[i] = (k = sqrt (k));
      c = 0.5 * (a + k);
      if (fabs (a - k) <= SN_ACC * a)
	break;
      k *= a;
      a = c;
    }
    u *= c;
    sn = sin (u);
    cn = cos (u);
    if (sn) {
      a = cn / sn;
      c *= a;
      for (int ii = l; ii > 0; ii--) {
	b = fn[ii];
	a *= c;
	c *= dn;
	dn = (en[ii] + a) / (b + a);
	a = c / b;
      }
      a = 1 / sqrt (c * c + 1);
      sn = (sn >= 0 ? a : -a);
      cn = sn * c;
    }
    if (bo) {
      a = dn;
      dn = cn;
      cn = a;
      sn /= d;
    }
  }
  else {
    cn = 1 / cosh (u);
    dn = cn;
    sn = tanh (u);
  }
  return sn;
}

#ifdef _MSC_VER

nr_double_t fspecial::round (nr_double_t arg) {
  return (arg > 0) ? floor (arg + 0.5) : ceil (arg - 0.5);
}

/* data for a Chebyshev series over a given interval */
struct cheb_series_t {
  nr_double_t * c;   /* coefficients                */
  int order;         /* order of expansion          */
  nr_double_t a;     /* lower interval point        */
  nr_double_t b;     /* upper interval point        */
};
typedef struct cheb_series_t cheb_series;

static nr_double_t cheb_eval (const cheb_series * cs, const nr_double_t x) {
  nr_double_t d  = 0.0;
  nr_double_t dd = 0.0;
  nr_double_t y  = (2.0 * x - cs->a - cs->b) / (cs->b - cs->a);
  nr_double_t y2 = 2.0 * y;
  for (int i = cs->order; i >= 1; i--) {
    nr_double_t t = d;
    d = y2 * d - dd + cs->c[i];
    dd = t;
  }
  d = y * d - dd + 0.5 * cs->c[0];
  return d;
}

/* Chebyshev fit for erfc ((t+1)/2), -1 < t < 1 */
static nr_double_t erfc_xlt1_data[20] = {
  1.06073416421769980345174155056,
 -0.42582445804381043569204735291,
  0.04955262679620434040357683080,
  0.00449293488768382749558001242,
 -0.00129194104658496953494224761,
 -0.00001836389292149396270416979,
  0.00002211114704099526291538556,
 -5.23337485234257134673693179020e-7,
 -2.78184788833537885382530989578e-7,
  1.41158092748813114560316684249e-8,
  2.72571296330561699984539141865e-9,
 -2.06343904872070629406401492476e-10,
 -2.14273991996785367924201401812e-11,
  2.22990255539358204580285098119e-12,
  1.36250074650698280575807934155e-13,
 -1.95144010922293091898995913038e-14,
 -6.85627169231704599442806370690e-16,
  1.44506492869699938239521607493e-16,
  2.45935306460536488037576200030e-18,
 -9.29599561220523396007359328540e-19
};
static cheb_series erfc_xlt1_cs = {
  erfc_xlt1_data,
  19,
  -1, 1
};

/* Chebyshev fit for erfc (x) * exp (x^2), 1 < x < 5, x = 2t + 3, -1 < t < 1 */
static nr_double_t erfc_x15_data[25] = {
  0.44045832024338111077637466616,
 -0.143958836762168335790826895326,
  0.044786499817939267247056666937,
 -0.013343124200271211203618353102,
  0.003824682739750469767692372556,
 -0.001058699227195126547306482530,
  0.000283859419210073742736310108,
 -0.000073906170662206760483959432,
  0.000018725312521489179015872934,
 -4.62530981164919445131297264430e-6,
  1.11558657244432857487884006422e-6,
 -2.63098662650834130067808832725e-7,
  6.07462122724551777372119408710e-8,
 -1.37460865539865444777251011793e-8,
  3.05157051905475145520096717210e-9,
 -6.65174789720310713757307724790e-10,
  1.42483346273207784489792999706e-10,
 -3.00141127395323902092018744545e-11,
  6.22171792645348091472914001250e-12,
 -1.26994639225668496876152836555e-12,
  2.55385883033257575402681845385e-13,
 -5.06258237507038698392265499770e-14,
  9.89705409478327321641264227110e-15,
 -1.90685978789192181051961024995e-15,
  3.50826648032737849245113757340e-16
};
static cheb_series erfc_x15_cs = {
  erfc_x15_data,
  24,
  -1, 1
};

/* Chebyshev fit for erfc(x) * exp(x^2),
   5 < x < 10, x = (5t + 15)/2, -1 < t <  */
static nr_double_t erfc_x510_data[20] = {
  1.11684990123545698684297865808,
  0.003736240359381998520654927536,
 -0.000916623948045470238763619870,
  0.000199094325044940833965078819,
 -0.000040276384918650072591781859,
  7.76515264697061049477127605790e-6,
 -1.44464794206689070402099225301e-6,
  2.61311930343463958393485241947e-7,
 -4.61833026634844152345304095560e-8,
  8.00253111512943601598732144340e-9,
 -1.36291114862793031395712122089e-9,
  2.28570483090160869607683087722e-10,
 -3.78022521563251805044056974560e-11,
  6.17253683874528285729910462130e-12,
 -9.96019290955316888445830597430e-13,
  1.58953143706980770269506726000e-13,
 -2.51045971047162509999527428316e-14,
  3.92607828989125810013581287560e-15,
 -6.07970619384160374392535453420e-16,
  9.12600607264794717315507477670e-17
};
static cheb_series erfc_x510_cs = {
  erfc_x510_data,
  19,
  -1, 1
};

/* Estimates erfc (x) valid for 8 < x < 100, this is based on index 5725
   in Hart et al. */
static nr_double_t erfc8 (nr_double_t x) {
  static nr_double_t p[] = {
    2.97886562639399288862,
    7.409740605964741794425,
    6.1602098531096305440906,
    5.019049726784267463450058,
    1.275366644729965952479585264,
    0.5641895835477550741253201704
  };
  static nr_double_t q[] = {
    3.3690752069827527677,
    9.608965327192787870698,
    17.08144074746600431571095,
    12.0489519278551290360340491,
    9.396034016235054150430579648,
    2.260528520767326969591866945,
    1.0
  };
  nr_double_t n = 0.0, d = 0.0;
  int i;
  n = p[5];
  for (i = 4; i >= 0; --i) n = x * n + p[i];
  d = p[6];
  for (i = 5; i >= 0; --i) d = x * d + q[i];

  return exp (-x * x) * (n / d);
}

/* Abramowitz + Stegun, 7.1.5 */
static nr_double_t erfseries (nr_double_t x) {
  nr_double_t c = x;
  nr_double_t e = c;
  nr_double_t d;
  for (int k = 1; k < 30; ++k) {
    c *= -x * x / k;
    d  = c / (2.0 * k + 1.0);
    e += d;
  }
  return 2.0 / M_SQRTPI * e;
}

nr_double_t fspecial::erfc (nr_double_t x) {
  const nr_double_t ax = fabs (x);
  nr_double_t val;

  if (ax <= 1.0) {
    nr_double_t t = 2.0 * ax - 1.0;
    val = cheb_eval (&erfc_xlt1_cs, t);
  }
  else if (ax <= 5.0) {
    nr_double_t ex2 = exp (-x * x);
    nr_double_t t = 0.5 * (ax - 3.0);
    val = ex2 * cheb_eval (&erfc_x15_cs, t);
  }
  else if (ax < 10.0) {
    nr_double_t ex = exp(-x * x) / ax;
    nr_double_t t = (2.0 * ax - 15.0) / 5.0;
    val = ex * cheb_eval (&erfc_x510_cs, t);
  }
  else {
    val = erfc8 (ax);
  }
  return (x < 0.0) ? 2.0 - val : val;
}

nr_double_t fspecial::erf (nr_double_t x) {
  if (fabs (x) < 1.0) {
    return erfseries (x);
  }
  return 1.0 - erfc (x);
}

#endif /* _MSC_VER */
