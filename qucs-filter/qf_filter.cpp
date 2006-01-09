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

#include <math.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "qf_poly.h"
#include "qf_filter.h"

// Constructor of a filter of nth order

// Just initialize a few things
qf_filter::qf_filter (int n) :
  type (LOWPASS), kind (UNDEF), o (n), f (0), bw (0), imp (1),
  ncomp (0), Comp (NULL) {
//  std::cerr << "New filter of " << n << "th order.\n" ;
}

// Default constructor
qf_filter::qf_filter () :
  type (LOWPASS), kind (UNDEF), o (0), f (0), bw (0), imp (1),
  ncomp (0), Comp (NULL) {
}

qf_filter::qf_filter (int n = 3, qfk k = UNDEF, qft t = LOWPASS) :
  type (t), kind (k), o (n), f (0), bw (0), imp (1), Comp (NULL) {
}

qf_filter::qf_filter (qfk k = UNDEF, qft t = LOWPASS,
		      double r = 1, double fc = 1, double b = 0) :
  type (t), kind (k), f (fc), bw (b), imp (r), Comp (NULL) {
}

qf_filter::qf_filter (qfk k = UNDEF, qft t = LOWPASS) :
  type (t), kind (k), o (0), f (0), bw (0), imp (1), ncomp (0), Comp (NULL) {
}

// Destructor of a filter
qf_filter::~qf_filter (void) {
//  std::cout << "Destructor qf_filter.\n" ;
  if (Comp != NULL)
    free (Comp);
}


// Extraction routines

// Extract finite attenuation pole
// Result is a parallel cap, and a serial resonator (L // C)
void qf_filter::extract_pole_pCsLC (double p, qfc * pComp, double Ws) {
  BN.disp ("BN");
  BD.disp ("BD");

  double pl = -p * p;
  double bdpl = BD.evalX2 (pl);

  // Partial removal of infinite pole (first // cap)
  // c = [B(s)/s] (s² = - Ω²)
  double c = ((BN << 1).evalX2 (pl)) / bdpl;
  pComp->val = c * Ws;

  qf_poly cS (c, 0, 0, 1);
  BN = BN - (BD * cS);		// B = B - c s 
  BN.disp ("BN");
  BN.div (0, p);

  // Full removal of finite pole
  // c1 = (s B(s) / (s² + Ω²)) @ s² = - Ω²
  BN.disp ("BN");
  double c1 = (BN >> 1).evalX2 (pl) / bdpl;
  (pComp + 1)->val = c1;
  (pComp + 2)->val = -Ws / (c1 * pl);
  (pComp + 1)->val *= Ws;

  // 1/B = 1/B - (s/c1) / (s² + Ω²)
  BD = BD - (BN >> 1) * (1 / c1);
  BD.div (0, p);

  BN.disp ("BN");
  BD.disp ("BD");
}

// Dumps a filter to std::cout
void qf_filter::dump_cout (void) {
  std::string unit;

  for (unsigned i = 0; i < ncomp; i++) {
    switch (Comp[i].comp) {
    case CAP:
      std::cout << "Cap. from node ";
      unit = "F";
      break;
    case IND:
      std::cout << "Ind. from node ";
      unit = "H";
      break;
    case RES:
      std::cout << "Res. from node ";
      unit = "Ω";
      break;
    }

    std::cout << Comp[i].node1 << " to ";

    if (Comp[i].node2 == 0)
      std::cout << "ground, value = ";
    else
      std::cout << "node " << Comp[i].node2 << ", value = ";

    if (Comp[i].val > 1)
      std::cout << Comp[i].val << " " << unit << '\n';
    else if (Comp[i].val >= 1e-3)
      std::cout << Comp[i].val * 1e3 << " m" << unit << '\n';
    else if (Comp[i].val >= 1e-6)
      std::cout << Comp[i].val * 1e6 << " µ" << unit << '\n';
    else if (Comp[i].val >= 1e-9)
      std::cout << Comp[i].val * 1e9 << " n" << unit << '\n';
    else if (Comp[i].val >= 1e-12)
      std::cout << Comp[i].val * 1e12 << " p" << unit << '\n';
    else if (Comp[i].val >= 1e-15)
      std::cout << Comp[i].val * 1e15 << " f" << unit << '\n';
    else
      std::cout << "0 " << unit << '\n';
  }
}

void qf_filter::dump_spice (void) {
  unsigned i = 0;

  for (unsigned ic = 1, il = 1, ir = 1; i < ncomp; i++) {
    switch (Comp[i].comp) {
    case CAP:
      std::cout << "C" << ic << "\t";
      std::cout << Comp[i].node1 << "\t" << Comp[i].node2;
      std::cout << "\t" << Comp[i].val << '\n';
      // Insert parallel resistor
      std::cout << "R" << ir << "\t";
      std::cout << Comp[i].node1 << "\t" << Comp[i].node2;
      std::cout << "\t10G\n";
      ic++;
      ir++;
      break;
    case IND:
      std::cout << "L" << il << "\t";
      std::cout << Comp[i].node1 << "\t"
	<< Comp[i].node1 * 100 + Comp[i].node2 << "\t";
      std::cout << Comp[i].val << "\n";
      il++;
      // Insert serial resistor
      std::cout << "R" << ir << "\t";
      std::cout << Comp[i].node1 * 100 + Comp[i].node2
	<< "\t" << Comp[i].node2 << "\t";
      std::cout << "1u\n";
      ir++;
      break;
    case RES:
      std::cout << "R" << ir << "\t";
      ir++;
      break;
    }
  }

  std::cout << "RI\t10000\t1\t" << imp << "\n";
  std::cout << "RL\t";

  if (Comp[i - 1].node2 == 0)
    std::cout << Comp[i - 1].node1;
  else
    std::cout << Comp[i - 1].node2;

  std::cout << "\t0\t" << imp << "\n";
  std::cout << "VCC\t10000\t0\tDC\t1\tAC\t1\n";
  std::cout << ".end\n";
}

void qf_filter::to_Qucs (void) {
}
