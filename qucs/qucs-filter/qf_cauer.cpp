/***************************************************************************
                               qf_cauer.cpp
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

// Elliptic (Cauer) filters, odd order

#include <math.h>
#include <stdlib.h>
#include <iostream>

#ifndef M_1_PI
#define M_1_PI    0.3183098861837906715377675267450287
#endif
#ifndef M_LN2
#define M_LN2     0.6931471805599453094172321214581766
#endif
#ifndef M_PI
#define M_PI      3.1415926535897932384626433832795029
#endif

#include "qf_poly.h"
#include "qf_filter.h"
#include "qf_cauer.h"

qf_cauer::qf_cauer (unsigned n, double r, double t) :
  qf_filter (CAUER, LOWPASS, 1, M_1_PI / 2, 0), rho (r) {
  o = n;
  th = sin (t);
  a = new double[o + 1];
  xfer ();
  values ();
  synth (LOWPASS);
}

qf_cauer::qf_cauer (double amin, double amax, double fc,
		    double fs, double r = 1, double b = 0,
		    qft type = LOWPASS) :
  qf_filter (CAUER, type, r, fc, b), a (NULL) {
  if (amin > amax)
    return;

  if ((amin > 3) || (amax < 3))
    return;

  if (((f > fs) && (type == LOWPASS)) || ((f < fs) && (type == HIGHPASS)))
    return;

  if (((type == BANDPASS) || (type == BANDSTOP))
      && (fabs (fs - (f * f) / fs) < bw))
    return;

  normalize (amin, amax, fs, type);
  xfer ();
  values ();
  synth (type);
}

qf_cauer::~qf_cauer (void) {
  if (a != NULL)
    delete[] a;
}

// Computes elliptic jacobi functions
// Adapted from: Numerical receipes in C, pp. 264 et seq.

static double FMAX (double x, double y) {
  return ((x > y) ? x : y);
}

// K (k)
static double K (double k) {
  double alamb, ave, delx, dely, delz, e2, e3;
  double sqrtx, sqrty, sqrtz, xt, yt, zt;

  // Constants
  const double THIRD = 1.0 / 3.0;
  const double C1 = 1.0 / 24.0;
  const double C2 = 0.1;
  const double C3 = 3.0 / 44.0;
  const double C4 = 1.0 / 14.0;

  if (k > 1) {
    std::cerr << "Invalid argument in qf_cauer::K (k): (k > 1).\n";
    exit (-1);
  }

  xt = 0;
  yt = 1 - k * k;
  zt = 1;

  do {
    sqrtx = sqrt (xt);
    sqrty = sqrt (yt);
    sqrtz = sqrt (zt);

    alamb = sqrtx * (sqrty + sqrtz) + sqrty * sqrtz;

    xt = 0.25 * (xt + alamb);
    yt = 0.25 * (yt + alamb);
    zt = 0.25 * (zt + alamb);

    ave = THIRD * (xt + yt + zt);

    delx = (ave - xt) / ave;
    dely = (ave - yt) / ave;
    delz = (ave - zt) / ave;

  }
  while (FMAX (FMAX (fabs (delx), fabs (dely)), fabs (delz)) > K_ERR1);

  e2 = delx * dely - delz * delz;
  e3 = delx * dely * delz;

  return (1 + (C1 * e2 - C2 - C3 * e3) * e2 + C4 * e3) / sqrt (ave);
}

// K'(k) = K(sqrt(1 - k²)) , even for small k's
static double Kp (double k) {
  double Kp;
  double f1 = 1, f2, w = 1;
  double kb = 1;

  Kp = f2 = 2 * M_LN2 - log (k);	// K' = ln (4 / k')

  while (kb > K_ERR2) {
//    std::cout << "Kb = " << kb << '\n' ;
    kb *= k * k;
    f1 *= (w / (w + 1));
    f2 -= 2 / (w * (w + 1));
    Kp += f1 * f2 * kb;
    w += 2;
  }
  return Kp;
}

static double sn (double x, double k) {
  double a, b, c, d, emc, u;
  double em[14], en[14];
  double sn, cn, dn;
  int i, ii, l;
  bool bo;

  emc = 1 - k * k;
  d = 1 - emc;
  u = x;

  if (emc) {
    bo = (emc < 0);
    if (bo) {
      emc /= -1 / d;
      u *= (d = sqrt (d));
    }
    a = 1;
    dn = 1;

    for (i = 1; i < 14; i++) {
      l = i;
      em[i] = a;
      en[i] = (emc = sqrt (emc));
      c = 0.5 * (a + emc);
      if (fabs (a - emc) <= SN_ACC * a)
	break;
      emc *= a;
      a = c;
    }

    u *= c;
    sn = sin (u);
    cn = cos (u);

    if (sn) {
      a = cn / sn;
      c *= a;
      for (ii = l; ii > 0; ii--) {
	b = em[ii];
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
  else sn = tanh (u);

  return sn;
}

// Arc sin in degrees
static double ASIND (double ang) {
  return (180 * asin (ang) / M_PI);
}

// Normalize the filter parameters to Z = 1 Ω and ω = 1 rad.s¯¹
// and computes order
void qf_cauer::normalize (double amin, double amax, double fs, qft type) {
  double Amax = pow (10, -amin / 10);
  double Amin = pow (10, -amax / 10);
  double Aemax = 1 - Amin;
  double Aemin = 1 - Amax;
  double sAmin = -10 * log10 (Aemax) + amin;
  double sAmax = -10 * log10 (Aemin) + amax;
  double sdiff = sAmax - sAmin;

#ifdef _QF_CAUER_DEBUG
  std::cout << "amin + aemin = " << sAmin << " dB\n";
  std::cout << "amax + aemax = " << sAmax << " dB\n";
  std::cout << "Δ(a) = " << sdiff << " dB\n";
#endif

  double kA = pow (10, -sdiff / 20);
  double KA;

  if (kA < 0.001)
    KA = Kp (kA) / K (kA);

  else
    KA = K (sqrt (1 - kA * kA)) / K (kA);

  rho = sqrt (Aemin);

  switch (type) {
  case LOWPASS:
    th = f / fs;
    break;
  case HIGHPASS:
    th = fs / f;
    break;
  case BANDPASS:
  case BANDSTOP:
    th = bw / fabs (fs - (f * f) / fs);
    break;
  }

  // Calculate order
  double Kth = K (th) / K (sqrt (1 - th * th));
  o = (unsigned) ceil (Kth * KA);
  if ((o % 2) == 0)
    o++;

#ifdef _QF_CAUER_DEBUG
  std::cout << "K'/K = " << Kth << ", K1/K'1 = " << KA << '\n';
  std::cout << "Order = " << o << '\n';
#endif

  a = new double[o + 1];
}

// A Cauer (or elliptic) filter has a symetric D(Ω)
// D(Ω) = F (Ω) / P (Ω) = K * Ω * π {(Ω² + a²(i)) / (a²(i)Ω² + 1)}
// So that it is Chebichev in the passband and in the stopband
void qf_cauer::xfer (void) {
  int m = (o - 1) / 2;
  double Ws = a[o] = sqrt (th);
  double k = K (th);
  int u;

#ifdef _QF_CAUER_DEBUG
  std::cerr << "Computing filter of order " << o << " with ";
  std::cerr << "ρ = " << rho << " and Θ = " << ASIND (th) << "°\n";
  std::cerr << "k = " << k << '\n';
#endif

  for (unsigned i = 1; i < o; i++) {
    double j = (double) i / (double) o;
    a[i] = Ws * sn (j * k, th);
#ifdef _QF_CAUER_DEBUG
    std::cerr << "a(" << i << ") = " << a[i] << '\n';
#endif
  }

#ifdef _QF_CAUER_DEBUG
  std::cerr << "Norm. puls. (Ws) = " << Ws << '\n';
#endif

  double delta = 1;
  for (u = 1; u < m + 2; u++)
    delta *= a[2 * u - 1] * a[2 * u - 1];
  delta /= Ws;
  double c = delta * sqrt (1 / (rho * rho) - 1);

#ifdef _QF_CAUER_DEBUG
  std::cerr << "Δ = " << delta << '\n';
  std::cerr << "c = " << c << '\n';
#endif

  // Computes D
  F = qf_poly (1, 0, 0, 1);	// F(X) = X
  P = qf_poly (c, 0, 0, 0);	// P(X) = c

  for (u = 1; u < m + 1; u++) {
    qf_poly MF (1, 0, a[2 * u] * a[2 * u], 2);
    qf_poly MP (a[2 * u] * a[2 * u], 0, 1, 2);

    MF.disp ("MF");
    MP.disp ("MP");
    F *= MF;
    P *= MP;
  }

  F.disp ("F");
  P.disp ("P");

  // E(x)E(-x) = F(x)F(-x) + P(x)P(-x)
  E = F.hsq () + P.hsq ();

  E.disp ("E");
  E.hurw ();
  E.disp ("E");

  BN = E.odd () + F.odd ();
  BD = E.even () - F.even ();
}

void qf_cauer::values (void) {

  ncomp = (3 * o) / 2;
  Comp = (qfc *) malloc (sizeof (qfc) * ncomp);

  // For each zero of transmission, we apply the method as in 
  // Saal & Ulbrich p. 288 or Zverev pp. 129 et seq.
  double Ws = sqrt (th);
  for (unsigned k = 0, l = 2; k < (ncomp - 1); k += 3) {
#ifdef _QF_CAUER_DEBUG
    std::cerr << "Pole (" << l << ") = " << (1 / (a[l] * Ws)) << "\n";
#endif
    extract_pole_pCsLC (1 / a[l], &(Comp[k]), Ws);

    // Zeros mangeling 
    l = o - l + 1;
    if (l < (o + 1) / 2)
      l += 2;
  }

  // Final removal of inifite pole
  Comp[ncomp - 1].val = Ws * BN.eval (1) / BD.eval (1);
}

void qf_cauer::synth (qft type) {
  double cnrm = 1 / (2 * M_PI * f * imp);
  double lnrm = imp / (2 * M_PI * f);
  unsigned i, node;

  switch (type) {
  case LOWPASS:
    for (i = 0, node = 1;;) {
      Comp[i].comp = CAP;	// Parallel capa first
      Comp[i].val *= cnrm;	// and last !
      Comp[i].node1 = node;
      Comp[i].node2 = 0;
      i++;
      if (i == ncomp)
	break;
      Comp[i].comp = CAP;	// Resonator cap
      Comp[i].val *= cnrm;
      Comp[i].node1 = node;
      Comp[i].node2 = node + 1;
      i++;
      Comp[i].comp = IND;
      Comp[i].val *= lnrm;
      Comp[i].node1 = node;
      Comp[i].node2 = ++node;
      i++;
    }
    break;
  case HIGHPASS:
    // First comes a serial cap
    Comp[0].comp = CAP;
    Comp[0].node1 = 1;
    Comp[0].node2 = 2;
    Comp[0].val = cnrm / Comp[0].val;
    for (i = 1, node = 2; i < ncomp;) {
      // double       temp ;
      // Then a serial resonant circuit
      Comp[i].comp = CAP;
      Comp[i].node1 = node;
      Comp[i].node2 = node + 1;
      Comp[i].val = cnrm / Comp[i].val;
      i++;
      Comp[i].comp = IND;
      Comp[i].node1 = node + 1;
      Comp[i].node2 = 0;
      Comp[i].val = lnrm / Comp[i].val;
      i++;
      // Then another serial cap
      Comp[i].comp = CAP;
      Comp[i].node1 = node;
      node += 2;
      Comp[i].node2 = node;
      Comp[i].val = cnrm / Comp[i].val;
      i++;
    }
    break;
  case BANDPASS: {
    // We double the number of components
    ncomp = 3 * o - 1;
    qfc * Comp2 = (qfc *) malloc (sizeof (double) * ncomp);
    double q = f / bw;

    for (unsigned i = 0, j = 0, node = 1;;) {
      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = 0;
      Comp2[j++].val = Comp[i].val * q * cnrm;

      Comp2[j].comp = IND;
      Comp2[j].node1 = node;
      Comp2[j].node2 = 0;
      Comp2[j++].val = lnrm / (q * Comp[i++].val);
      if (j == ncomp)
	break;

      double c = Comp[i].val;
      double l = Comp[i + 1].val;
      double iw2 = l * c;
#ifdef _QF_CAUER_DEBUG
      std::cout << "Ω(∞) = " << sqrt (1 / iw2) << '\n';
#endif
      double b = sqrt (1 + 4 * q * q * iw2);

#ifdef _QF_CAUER_DEBUG
      std::cout << "b = " << b << '\n';
#endif

      Comp2[j + 1].comp = IND;
      Comp2[j + 1].node1 = node;
      Comp2[j + 1].node2 = node + 1;
      Comp2[j + 1].val = (b - 1) / (q * c * 2 * b);

      Comp2[j + 3].comp = IND;
      Comp2[j + 3].node1 = node + 1;
      Comp2[j + 3].node2 = node + 2;
      Comp2[j + 3].val = (b + 1) / (q * c * 2 * b);

      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = node + 1;
      Comp2[j].val = 1 / Comp2[j + 3].val;

      Comp2[j + 2].comp = CAP;
      Comp2[j + 2].node1 = node + 1;
      Comp2[j + 2].node2 = node + 2;
      Comp2[j + 2].val = 1 / Comp2[j + 1].val;
      Comp2[j].val *= cnrm;
      Comp2[j + 2].val *= cnrm;
      Comp2[j + 1].val *= lnrm;
      Comp2[j + 3].val *= lnrm;
      j += 4;
      i += 2;
      node += 2;
    }

    delete[] Comp;
    Comp = Comp2;
    break;
  }
  case BANDSTOP: {
    ncomp = 3 * o - 1;
    qfc * Comp2 = (qfc *) malloc (sizeof (double) * ncomp);
    double q = f / bw;

    for (unsigned i = 0, j = 0, node = 1;;) {
      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = node + 1;
      Comp2[j++].val = cnrm * Comp[i].val / q;

      Comp2[j].comp = IND;
      Comp2[j].node1 = node + 1;
      Comp2[j].node2 = 0;
      Comp2[j++].val = lnrm * q / Comp[i++].val;
      if (j == ncomp)
	break;

      double c = Comp[i].val;
      double l = Comp[i + 1].val;
      double w2 = 1 / (l * c);
#ifdef _QF_CAUER_DEBUG
      std::cout << "Ω(∞) = " << sqrt (w2) << "; q = " << q << '\n';
#endif
      double b = sqrt (1 + 4 * q * q * w2);

#ifdef _QF_CAUER_DEBUG
      std::cout << "b = " << b << '\n';
#endif
      Comp2[j + 1].comp = IND;
      Comp2[j + 1].node1 = node;
      Comp2[j + 1].node2 = node + 2;
      Comp2[j + 1].val = l * (b - 1) / (2 * b * q);

      Comp2[j + 3].comp = IND;
      Comp2[j + 3].node1 = node + 2;
      Comp2[j + 3].node2 = node + 3;
      Comp2[j + 3].val = l * (b + 1) / (2 * b * q);

      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = node + 2;
      Comp2[j].val = 1 / Comp2[j + 3].val;

      Comp2[j + 2].comp = CAP;
      Comp2[j + 2].node1 = node + 2;
      Comp2[j + 2].node2 = node + 3;
      Comp2[j + 2].val = 1 / Comp2[j + 1].val;
      Comp2[j].val *= cnrm;
      Comp2[j + 2].val *= cnrm;
      Comp2[j + 1].val *= lnrm;
      Comp2[j + 3].val *= lnrm;
      j += 4;
      i += 2;
      node += 3;
    }

    delete[] Comp;
    Comp = Comp2;
    break;
  }
  }
}

void qf_cauer::dump () {
  if (Comp == NULL)
    return;			// Not initialized

  std::cout << "qf_cauer::dump ()\n";

  switch (type) {
  case LOWPASS:
    std::cout << "Lowpass ";
    break;
  case HIGHPASS:
    std::cout << "Highpass ";
    break;
  case BANDPASS:
    std::cout << "Bandpass ";
    break;
  case BANDSTOP:
    std::cout << "Bandstop ";
    break;
  }
  std::cout << "of order " << o << ", Θ = "
	    << ASIND (th) << "°, ρ = " << rho << '\n';
  dump_cout ();
}

void CC (void) {
  unsigned o;
  double t, r;

  do {
    std::cout << "Order : ";
    std::cin >> o;
    if (o == 0)
      return;
    std::cout << "Reflexion (%) : ";
    std::cin >> r;
    r /= 100.0;
    std::cout << "Angle (°) : ";
    std::cin >> t;
    t = M_PI * t / 180.0;
    qf_cauer F (o, r, t);
    F.dump ();
  }
  while (true);
}

#if TEST
int main (void) {
  double amin, amax, fc, fs, bw, r;

  while (true) {
    std::cout << "Enter cutoff (Hz) [0 to end]: ";
    std::cin >> fc;
    if (fc == 0)
      return 0;
    std::cout << "Enter ripple in passband (dB): ";
    std::cin >> amin;
    std::cout << "Enter stopband frequency (Hz): ";
    std::cin >> fs;
    std::cout << "Enter minimal attenuation in stopband (dB): ";
    std::cin >> amax;
    std::cout << "Enter bandwith (0 for high- or low-pass) (Hz): ";
    std::cin >> bw;
    std::cout << "Enter reference impedance (Ω): ";
    std::cin >> r;

    if (bw == 0) {
      if (fs > fc) {
	qf_cauer F (amin, amax, fc, fs, r, 0, LOWPASS);
	F.dump ();
      }
      else {
	qf_cauer F (amin, amax, fc, fs, r, 0, HIGHPASS);
	F.dump ();
      }
    }

    else {
      if (amin < amax) {
	qf_cauer F (amin, amax, fc, fs, r, bw, BANDPASS);
	F.dump ();
      }
      else {
	qf_cauer F (amax, amin, fc, fs, r, bw, BANDSTOP);
	F.dump ();
      }
    }
  }
}
#endif /* TEST */
