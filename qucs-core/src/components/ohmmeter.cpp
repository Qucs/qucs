/***************************************************************************
                                ohmmeter.cpp
                               -----------
    begin                : Fri Sep 25 2016
    copyright            : (C) 2016 by Nelson Martins
    email                : macedo.pp@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


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

void ohmmeter::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}
/*first analysis of DC/TRANS simulation when only act 
  like voltmeter to test if there is a external source*/
void ohmmeter::initDC (void) {
  nr_double_t i = 0;
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
  on=1;
}

/*if the ohmmeter didn't detect extern source in DC/TRANS simulation 
  while start the internal corrent source*/
void ohmmeter::initDC2 (void) {
  nr_double_t i = 1;
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void ohmmeter::calcOperatingPoints (void) {
  if(on>0)// meaning that is testing if the ohmmeter is allow to work or is already working
  {
    nr_double_t R = real (getV (NODE_1) - getV (NODE_2));
    setOperatingPoint ("R", R);
    nr_double_t Z = imag (getV (NODE_1) - getV (NODE_2));
    setOperatingPoint ("Z", Z);
  }
  else // in this case means that is conected to a external source
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
/*first analysis of AC simulation when only act 
  like voltmeter to test if there is a external source*/
void ohmmeter::initAC (void) {
  nr_double_t a = 0;
  nr_double_t p = 0;
  nr_complex_t i = qucs::polar (a, deg2rad (p));
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
  on=1;
}
/*if the ohmmeter didn't detect external source in AC simulation 
  while start the intern corrent source*/
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


// properties
PROP_REQ [] = { PROP_NO_PROP };
PROP_OPT [] = { PROP_NO_PROP };
struct define_t ohmmeter::cirdef =
  { "Ohmmeter", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
