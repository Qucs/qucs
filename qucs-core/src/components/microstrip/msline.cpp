/*
 * msline.cpp - microstrip transmission line class implementation
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
 * $Id: msline.cpp,v 1.6 2004/05/07 18:34:22 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "msline.h"

#define SQR(x)   ((x) * (x))
#define CUBIC(x) ((x) * (x) * (x))
#define QUAD(x)  ((x) * (x) * (x) * (x))

msline::msline () : circuit (2) {
  type = CIR_MSLINE;
}

void msline::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. L, W */
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t W = getPropertyDouble ("W");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t tand  = subst->getPropertyDouble ("tand");
  nr_double_t rho   = subst->getPropertyDouble ("rho");
  nr_double_t D     = subst->getPropertyDouble ("D");

  /* local variables */
  complex s11, s21, n, g;
  nr_double_t z, y, a, b, k0, ds, Kr, Rs, Ki, ac, ad, l0;

  // quasi-static effective dielectric constant of substrate + line
  ErEff = analyseEr (W, h, er);

  // quasi-static impedance of line
  ZlEff = analyseZl (W, h, er);

  // TODO: influence of t

  // analyse dispersion of Zl and Er
  analyseDispersion (W, h, er, frequency);

  // conductor losses: HAMMERSTAD and JENSEN
  Rs = sqrt (M_PI * frequency * MU0 * rho); // surface sheet resistance
  ds = rho / Rs; // skin depth
  // valid for t > 3 * ds
  if (t < 3 * ds) {
    logprint (LOG_STATUS, "line height t (%g) < 3 * skin depth (%g)\n", t, ds);
  }
  Ki = exp (-1.2 * pow (ZlEffFreq / (120 * M_PI), 0.7));
  Kr = 1 + M_2_PI * atan (1.4 * SQR (D / ds)); // D is RMS surface roughness
  ac = Rs / (ZlEffFreq * W) * Ki * Kr;

  // dielectric losses
  l0 = C0 / frequency;
  ad = M_PI * er / (er - 1) * (ErEffFreq - 1) / sqrt (ErEffFreq) * tand / l0;

  z = ZlEffFreq / z0;
  y = 1 / z;
  k0 = 2 * M_PI * frequency / C0;
  a = ac + ad;
  b = sqrt (ErEffFreq) * k0;
  g = rect (a, b);
  n = 2 * cosh (g * l) + (z + y) * sinh (g * l);
  s11 = (z - y) * sinh (g * l) / n;
  s21 = 2 / n;

  setS (1, 1, s11);
  setS (2, 2, s11);
  setS (1, 2, s21);
  setS (2, 1, s21);
}

/* This function returns the quasi-static impedance of a microstrip
   line based on the given line width, substrate height and the
   substrate's relative dielectrical constant. */
nr_double_t msline::analyseZl (nr_double_t W, nr_double_t h, nr_double_t er) {

  nr_double_t x, d, c, z, e, a, b;

  // often used in CAE
  c = 6 + (2 * M_PI - 6) * exp (-30.666 * h / W * 0.7528);
  e = (er + 1) / 2 + (er - 1) / 2 / sqrt (1 + 10 * h / W);
  d = 60 * log (c * h / W + sqrt (1 + SQR (2 * h / W)));
  z = d / sqrt (e);
  fprintf (stderr, "CAE z = %g\n", z);

  // SCHNEIDER
  a = 1 +
    1 / 49 *  log ((QUAD (W / h) + SQR (W / h / 52)) / 
		   (QUAD (W / h) + 0.432)) + 
    1 / 18.7 * log (1 + CUBIC (W / 18.1 / h));
  b = 0.564 * pow ((er - 0.9) / (er + 3), 0.053);
  e = (er + 1) / 2 + (er - 1) / 2 * pow (1 + 10 * h / W, -a * b);
  z = d / sqrt (e);
  fprintf (stderr, "SCHNEIDER z = %g\n", z);

  // WHEELER
  if (W / h < 3.3) {
    c = log (4 * h / W + sqrt (SQR (4 * h / W) + 2));
    z = c - 0.5 * (er - 1) / (er + 1) * (log (M_PI_2) + log (M_PI_4) / er);
    z = z * 120 / sqrt (2 * (er + 1));
  }
  else {
    c = 1.4516 + log (W / h / 2 + 0.94);
    d = 0.0823 * (er - 1) / SQR (er);
    x = 0.4413 + W / h / 2 + (er + 1) / 2 / M_PI / er * c + d;
    z = 188.5 / x / sqrt (er);
  }
  fprintf (stderr, "WHEELER z = %g\n", z);
  return z;
}

/* This function calculates the quasi-static value of the effective
   dielectric constant for the given microstrip line properties. */
nr_double_t msline::analyseEr (nr_double_t W, nr_double_t h, nr_double_t er) {

  nr_double_t ereff, a, b, d, e;

  /* these are valid for 0.05 < W/h < 10, 1 < er < 40 */

  // often used in CAE
  ereff = (er + 1) / 2 + (er - 1) / 2 / sqrt (1 + 10 * h / W);
  fprintf (stderr, "CAE er = %g\n", ereff);  

  // SCHNEIDER
  a = 1 +
    1 / 49 *  log ((QUAD (W / h) + SQR (W / h / 52)) / 
		   (QUAD (W / h) + 0.432)) + 
    1 / 18.7 * log (1 + CUBIC (W / 18.1 / h));
  b = 0.564 * pow ((er - 0.9) / (er + 3), 0.053);
  ereff = (er + 1) / 2 + (er - 1) / 2 * pow (1 + 10 * h / W, -a * b);
  fprintf (stderr, "SCHNEIDER er = %g\n", ereff);

  // WHEELER
  if (W / h < 1.3) {
    a = log (7 * h / W) + 1 / 32 * SQR (W / h);
    b = (er - 1) / (er + 1) / 2 * (0.4516 + 0.2416 / er);
    ereff = (1 + er) / 2 * SQR (a / (a - b));
  }
  else {
    d = (er - 1) / 2 / M_PI / er * (log (2.1349 * W / h + 4.0137) - 0.5169/er);
    e = W / h / 2 + M_1_PI * log (8.5397 * W / h + 16.0547);
    ereff = er * SQR ((e - d) / e);
  }
  fprintf (stderr, "WHEELER er = %g\n", ereff);
  
  return ereff;
}

/* This function calculates the frequency dependent value of the
   effective dielectric constant and the line impedance for the given
   frequency. */
nr_double_t msline::analyseDispersion (nr_double_t W, nr_double_t h,
				       nr_double_t er, nr_double_t frequency) {

  nr_double_t e, fp, f, g, z, p, p1, p2, p3, p4, fs, k, d;
  nr_double_t r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14;
  nr_double_t r15, r16, r17;

  // GETSINGER
  g = 0.6 + 0.009 * ZlEff;
  fp = C0 / 2 / h * ZlEff / MU0;
  f = g * SQR (frequency / fp);
  e = er - (er - ErEff) / (1 + f);
  fprintf (stderr, "GETSINGER1 e = %g\n", e);

  g = 0.6 + 0.009 * ZlEff;
  k = er / ErEff * g;
  f = 2 * MU0 * h * frequency / ZlEff;
  e = ErEff * (1 + k * SQR (f)) / (1 + g * SQR (f));
  fprintf (stderr, "GETSINGER2 e = %g\n", e);

  // YAMASHITA
  k = sqrt (er / ErEff);
  f = pow (4, 1.0 / 3.0) * h * frequency / C0 * sqrt (er - 1) *
    (0.5 + SQR (1 + 2 * log10 (1 + W / h)));
  e = ErEff * SQR ((1 + k * pow (f, 1.5)) / (1 + pow (f, 1.5)));
  fprintf (stderr, "YAMASHITA e = %g\n", e);

  // KOBAYASHI
  k = sqrt (ErEff / er);
  d = atan (er * sqrt ((ErEff - 1) / (er - ErEff)));
  f = 2 * M_PI * h * frequency / C0 * (1 + W / h) * sqrt (er - ErEff) / d;
  e = ErEff * SQR ((1 + SQR (f)) / (1 + k * SQR (f)));
  fprintf (stderr, "KOBAYASHI e = %g\n", e);

  // HAMMERSTAD and JENSEN
  g = SQR (M_PI) / 12 * (er - 1) / ErEff * sqrt (2 * M_PI * ZlEff / MU0);
  fp = C0 / 2 / h * ZlEff / MU0;
  f = g * SQR (frequency / fp);
  e = er - (er - ErEff) / (1 + f);
  z = ZlEff * sqrt (ErEff / e) * (e - 1) / (ErEff - 1);
  fprintf (stderr, "HAMMERSTAD e = %g, z = %g\n", e, z);

  // KIRSCHNING and JANSEN
  fs = frequency / 1e9 * h * 1e3;
  p1 = 0.27488 + (0.6315 + 0.525 / pow (1 + 0.0157 * fs, 20)) * W / h - 
    0.065683 * exp (-8.7513 * W / h);
  p2 = 0.33622 * (1 - exp (-0.03442 * er));
  p3 = 0.0363 * exp (-4.6 * W / h) * (1 - exp (-fs / 38.7 * 4.97));
  p4 = 1 + 2.751 * (1 - exp (-er / 15.916 * 8));
  p = p1 * p2 * pow ((0.1844 + p3 * p4) * fs, 1.5763);
  e = er - (er - ErEff) / (1 + p);

  r1 = 0.03891 * pow (er, 1.4);
  r2 = 0.267 * pow (W / h, 7);
  r3 = 4.766 * exp (-3.228 * pow (W / h, 0.641));
  r4 = 0.016 + pow (0.0514 * er, 4.524);
  r5 = pow (fs / 28.843, 12);
  r6 = 22.20 * pow (W / h, 1.92);
  r7 = 1.206 - 0.3144 * exp (-r1) * (1 - exp (-r2));
  r8 = 1 + 1.275 * (1 - exp (-0.004625 * r3 * 
			     pow (er, 1.674) * pow (fs / 18.365, 2.745)));
  r9 = 5.086 * r4 * r5 / (0.3838 + 0.386 * r4) * 
    exp (-r6) / (1 + 1.2992 * r5) * 
    pow (er - 1, 6) / (1 + 10 * pow (er - 1, 6));
  r10 = 0.00044 * pow (er, 2.136) + 0.0184;
  r11 = pow (fs / 19.47, 6) / (1 + 0.0962 * pow (fs / 19.47, 6));
  r12 = 1 / (1 + 0.00245 * SQR (W / h));
  r13 = 0.9408 * pow (e, r8) - 0.9603;
  r14 = (0.9408 - r9) * pow (ErEff, r8) - 0.9603;
  r15 = 0.707 * r10 * pow (fs / 12.3, 1.097);
  r16 = 1 + 0.0503 * SQR (er) * r11 * (1 - exp (- W / 15 / h * 6));
  r17 = r7 * (1 - 1.1241 * r12 / r16 * exp (-0.026 * pow (fs, 1.15656) - r15));
  z = ZlEff * pow (r13 / r14, r17);

  fprintf (stderr, "KIRSCHNING e = %g, z = %g\n", e, z);

  ZlEffFreq = z;
  ErEffFreq = e;
  return e;
}
