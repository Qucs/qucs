/*
 * ohmmeter.cpp - AC/DC and Transient ohmmeter class implementation
 *
 * Copyright (C) 2016, Nelson Martins, macedo.pp@gmail.com
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt	
 *		
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
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
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "ohmmeter.h"

using namespace qucs;

ohmmeter::ohmmeter () : circuit (2) {
  type = CIR_OHMMETER;
  setProbe (true);
  setISource (true);
}

/*The ohmmeter calculates the impedance with a basic calculation of the ohm's law,it
 sets its current to 1A, meaning the voltage it measures equals the circuit impedance.
 https://en.wikipedia.org/wiki/Ohm%27s_law */

void ohmmeter::saveOperatingPoints (void) {
  nr_double_t Vr = real (getV (NODE_1) - getV (NODE_2));
  nr_double_t Vi = imag (getV (NODE_1) - getV (NODE_2));
  setOperatingPoint ("Vr", Vr);
  setOperatingPoint ("Vi", Vi);
}

/*It will act like a voltmeter on the first analysis of DC/TRANS simulation to test 
the existence of an external source*/
void ohmmeter::initDC (void) {
  nr_double_t i = 0;
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
  on=1;
}

/*if the ohmmeter didn't detect an external source in DC/TRANS, it will
 start the internal current source*/
void ohmmeter::initDC2 (void) {
  nr_double_t i = 1;
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void ohmmeter::calcOperatingPoints (void) {
  if(on>0)// Testing if the ohmmeter is already working or allowed to work
  {
    nr_double_t R = real (getV (NODE_1) - getV (NODE_2));
    nr_double_t Z = imag (getV (NODE_1) - getV (NODE_2));
    setOperatingPoint ("R", R);
    setOperatingPoint ("Z", Z);
  }
  else // if it is connected to an external source, it will show as NAN
  {
    nr_double_t R=NAN;
    setOperatingPoint ("R", R);
    setOperatingPoint ("Z", R);
  }

}

void ohmmeter::setstate(int a)
{
  on=a;
}
int ohmmeter::getstate()
{
  return on;
}
/*It will act like a voltmeter on the first analysis of AC simulation to test 
the existence of an external source*/
void ohmmeter::initAC (void) {
  nr_double_t a = 0;
  nr_double_t p = 0;
  nr_complex_t i = qucs::polar (a, deg2rad (p));
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
  on=1;
}
/*if the ohmmeter didn't detect an external source in AC, it will
 start the internal current source*/
void ohmmeter::initAC2 (void) {
  nr_double_t a = 1;
  nr_double_t p = 0;
  nr_complex_t i = qucs::polar (a, deg2rad (p));
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}


void ohmmeter::initTR (void) {
  initDC ();
}


//properties
PROP_REQ [] = { PROP_NO_PROP };
PROP_OPT [] = { PROP_NO_PROP };
struct define_t ohmmeter::cirdef =
  { "Ohmmeter", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
