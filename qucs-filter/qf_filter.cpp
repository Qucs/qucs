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
# include <config.h>
#endif

#include <math.h>
#include <iostream>
#include <sstream>
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
//  std::cerr << "New filter of " << n << "th order.\n";
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
  // c = [B(s)/s] (s^2 = - O^2)
  double c = ((BN << 1).evalX2 (pl)) / bdpl;
  pComp->val = c * Ws;
  qf_poly cS (c, 0, 0, 1);
  BN = BN - (BD * cS);		// B = B - cs
  BN.disp ("BN");
  BN.div (0, p);

  // Full removal of finite pole
  // c1 = (s B(s) / (s^2 + O^2)) @ s^2 = - O^2
  BN.disp ("BN");
  double c1 = (BN >> 1).evalX2 (pl) / bdpl;
  (pComp + 1)->val = c1;
  (pComp + 2)->val = -Ws / (c1 * pl);
  (pComp + 1)->val *= Ws;

  // 1/B = 1/B - (s/c1) / (s^2 + O^2)
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
      unit = "Ohm";
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
      std::cout << Comp[i].val * 1e6 << " u" << unit << '\n';
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

std::string qf_filter::to_spice (void) {
  std::ostringstream res;
  unsigned i = 0;

  res << "* SPICE filter\n";
  for (unsigned ic = 1, il = 1, ir = 1; i < ncomp; i++) {
    switch (Comp[i].comp) {
    case CAP:
      res << "C" << ic << "\t";
      res << Comp[i].node1 << "\t" << Comp[i].node2;
      res << "\t" << Comp[i].val << '\n';
      // Insert parallel resistor
      res << "R" << ir << "\t";
      res << Comp[i].node1 << "\t" << Comp[i].node2;
      res << "\t10G\n";
      ic++;
      ir++;
      break;
    case IND:
      res << "L" << il << "\t";
      res << Comp[i].node1 << "\t"
	<< Comp[i].node1 * 100 + Comp[i].node2 << "\t";
      res << Comp[i].val << "\n";
      il++;
      // Insert serial resistor
      res << "R" << ir << "\t";
      res << Comp[i].node1 * 100 + Comp[i].node2
	<< "\t" << Comp[i].node2 << "\t";
      res << "1u\n";
      ir++;
      break;
    case RES:
      res << "R" << ir << "\t";
      ir++;
      break;
    }
  }

  res << "RI\t10000\t1\t" << imp << "\n";
  res << "RL\t";

  if (Comp[i - 1].node2 == 0)
    res << Comp[i - 1].node1;
  else
    res << Comp[i - 1].node2;

  res << "\t0\t" << imp << "\n";
  res << "VCC\t10000\t0\tDC\t1\tAC\t1\n";
  res << ".END\n";
  return res.str();
}

void qf_filter::dump_spice (void) {
  std::cout << to_spice().c_str();
}

std::string qf_filter::to_qucs (void) {
  std::ostringstream res;
  return res.str();
}

/*
void qf_filter::to_Qucs (void) {
  double Value, Value2;

  // create the Qucs schematic
  std::cout << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  int x = 20;
  if(type != BANDPASS)  x += 40;
  std::cout << "<Components>\n";
  std::cout << "<Pac P1 1 " << x << " 320 18 -26 0 1 \"1\" 1 \""
            << imp << " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n";
  std::cout << "<GND * 1 " << x << " 350 0 0 0 0>\n";

  int yc = 320, yl = 240;
  unsigned int i, j=0;
  for(i = 0; i < o; i++) {
    x = 100 +((i+1) * 70);

    if(Comp[j].comp == CAP)
      std::cout << "<C C";
    else
      std::cout << "<L L";
    std::cout << j << " 1 " << x;

    if(Comp[j].node2 == 0)
      std::cout << " " << yc << " 17 -26 0 1";
    else
      std::cout << " " << yl << " -26 10 0 0";
    std::cout << " \"" << Comp[j].val;

    if(Comp[j].comp == CAP)
      std::cout << "F";
    else
      std::cout << "H";
    std::cout << "\" 1>\n";

    if(Comp[j].node2 == 0)
      std::cout << "<GND * 1 " << x << " " << (yc+30) << " 0 0 0 0>\n";

    j++;
    if(j >= ncomp) break;
    if(Comp[j].node1 != Comp[j-1].node1)  continue;
    if(Comp[j].node2 != Comp[j-1].node2)  continue;


    // second component parallel to the last
    if(Comp[j].comp == CAP)
      std::cout << "<C C";
    else
      std::cout << "<L L";
    std::cout << j << " 1 " << x;

    if(Comp[j].node2 == 0)
      std::cout << " " << (yc+60) << " 17 -26 0 1";
    else
      std::cout << " " << (yl-35) << " -26 -44 0 0";
    std::cout << " \"" << Comp[j].val;

    if(Comp[j].comp == CAP)
      std::cout << "F";
    else
      std::cout << "H";
    std::cout << "\" 1>\n";

    if(Comp[j].node2 == 0)
      std::cout << "<GND * 1 " << x << " " << (yc+90) << " 0 0 0 0>\n";

    j++;
  }


  if(o & 1)  x += 110;
  else  x += 70;
  std::cout << "<Pac P2 1 "<< x << " 320 18 -26 0 1 \"2\" 1 \""
            << imp << " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n"
            << "<GND * 1 " << x << " 350 0 0 0 0>\n";

  yc += 100;
  Value = f / 10.0;
  if((type == BANDPASS) || (type == BANDSTOP))
    Value2 = 10.0 * (f + bw);
  else
    Value2 = 10.0 *f;
  std::cout << "<.SP SP1 1 70 " << yc << " 0 50 0 0 \"log\" 1 \""
            << Value << "Hz\" 1 \"" << Value2
            << "Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n"
            << "<Eqn Eqn1 1 260 " << (yc+10)
            << " -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 \"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n"
            << "</Components>\n";

  std::cout << "<Wires>\n";

  // connect left source
  x = 20;
  if(type != BANDPASS)  x += 40;
  std::cout << "<" << x << " 240 " << x << " 290 \"\" 0 0 0>\n"
            << "<" << x << " 240 170 240 \"\" 0 0 0>\n";

  // wires down to shunt components
  for(i = 0; i < (o / 2) + 1; i++) {
    x = 170 + (i * 140);
    std::cout << "<" << x << " 240 " << x << " 290 \"\" 0 0 0>\n";
  }

  // horizontal wires for series components
  if(type == BANDPASS) {
    for(i = 0; i < (o / 2); i++) {
      x = 170 + (i * 140);
      std::cout << "<" << x << " 240 " << (x+20) << " 240 \"\" 0 0 0>\n"
                << "<" << (x-30) << " 290 " << x << " 290 \"\" 0 0 0>\n"
                << "<" << (x-30) << " 350 " << x << " 350 \"\" 0 0 0>\n";
    }
    if(o & 1)
      std::cout << "<" << (x+110) << " 290 " << (x+140) << " 290 \"\" 0 0 0>\n"
                << "<" << (x+110) << " 350 " << (x+140) << " 350 \"\" 0 0 0>\n";
  }
  else
    for(i = 0; i < (o / 2); i++) {
      x = 170 + (i * 140);
      std::cout << "<" << x << " 240 " << (x+40) << " 240 \"\" 0 0 0>\n"
                << "<" << (x+100) << " 240 " << (x+140) << " 240 \"\" 0 0 0>\n";
//      if(type == BANDSTOP)
        // connect parallel components
        std::cout << "<" << (x+40) << " 240 " << (x+40) << " 205 \"\" 0 0 0>\n"
                  << "<" << (x+100) << " 240 " << (x+100) << " 205 \"\" 0 0 0>\n";
    }

  // connect right source
  if(o & 1) {
    x  += 140 + 110;
    std::cout << "<" << x << " 240 " << x << " 290 \"\" 0 0 0>\n"
              << "<" << (x-110) << " 240 " << x << " 240 \"\" 0 0 0>\n";
  }
  std::cout << "</Wires>\n"
            << "<Diagrams>\n"
            << "</Diagrams>\n"
            << "<Paintings>\n";

  std::cout << "<Text 400 " << (yc+10) << " 12 #000000 0 \"Cauer ";

  switch(type) {
    case LOWPASS:
      std::cout << "low-pass filter\\n" << f << "Hz cutoff"; break;
    case HIGHPASS:
      std::cout << "high-pass filter\\n" << f << "Hz cutoff"; break;
    case BANDPASS:
      std::cout << "band-pass filter\\n" << f << "Hz..." << (f+bw) << "Hz"; break;
    case BANDSTOP:
      std::cout << "band-reject filter\\n" << f << "Hz..." << (f+bw) << "Hz"; break;
  }
  std::cout << ", PI-type,\\nimpedance matching " << imp << " Ohm\">\n";

  std::cout << "</Paintings>\n";
*/

void qf_filter::dump_qucs (void) {
  std::cout << to_qucs().c_str();
}
