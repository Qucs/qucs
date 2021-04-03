/*$Id: constant.h,v  2014/05/16 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * defined constants for just about everything
 */
//testing=trivial 2006.07.17
#ifndef CONSTANT_H
#define CONSTANT_H
#include "platform.h"
/*--------------------------------------------------------------------------*/
/* The names are chosen according to Verilog-A spec. */
/* Commented out means they are defined in a standard header, usually <math>*/
/* or there is some discrepancy of the correct value */

/* standard mathematical constants */
//double const M_PI	= 3.1415926535897932384626433832795028841972;
double const M_TWO_PI	= M_PI * 2;
//double const M_PI_2	= M_PI / 2;
//double const M_PI_4	= M_PI / 4;
//double const M_1_PI	= 1 / M_PI;
//double const M_2_PI	= 2 / M_PI;
//double const M_2_SQRTPI = 2 / sqrt(M_PI);

//double const M_E	= 2.7182818284590452354;
//double const M_LOG2E	= 1.4426950408889634074;  // log2(M_E)
//double const M_LOG10E	= 0.43429448190325182765; // log10(M_E)
//double const M_LN2	= 0.69314718055994530942; // log(2)
//double const M_LN10	= 2.3025809299404568402;  // log(10)

//double const M_SQRT2	= 1.4142135623730904880;  // sqrt(2)
//double const M_SQRT1_2  = 1 / M_SQRT_2;

/* extra mathematical constants */
//double const DTOR  = 0.0174532925199432957692369076848861271344;
//double const RTOD  = 57.2957795130823208768;
double const DTOR  = M_PI / 180;
double const RTOD  = 180 / M_PI;

/* standard physical constants ref:  http://physics.nist.gov */
double const P_Q	= 1.6021918e-19;   // charge of an electron, (spice)
//double const P_Q	= 1.60217653e-19;  // charge of an electron, (nist)
double const P_C	= 2.99792458e8;    // speed of light m/s
double const P_K	= 1.3806226e-23;   // Boltzmann's constant J/K (spice)
//double const P_K	= 1.3806505e-23;   // Boltzmann's constant J/K (nist)
double const P_H	= 6.6260693e-34;   // Planck's constant J-s
double const P_U0	= M_PI * 4.0e-7;   // permeability of vaccuum H/m
double const P_EPS0	= 8.854214871e-12; // permittivity of air F/m(spice)
//double const P_EPS0	= 8.854187817e-12; // permittivity of vaccuum F/m(nist)
//double const P_EPS0	= 1/(P_U0*P_C*P_C);// permittivity of vaccuum F/m(nist)
double const P_CELSIUS0	= 273.15;	   // 0 Celsius

/* extra physical constants */
double const P_EPS_SI	= 11.7*P_EPS0; // permittivity of silicon (1.0359e-10)
double const P_EPS_OX	= 3.9*P_EPS0;  // permittivity of oxide (3.45e-11)
double const P_K_Q	= P_K/P_Q;

/* dimension conversions.
 * Hopefully, all internal distances are in meters.
 * (except for some Berkeley models)
 * In some cases the user data is in other units
 */
double const CM2M  = 1e-2;		/*	centimeters to meters 	 */
double const CM2M2 = 1e-4;		/*          ...........  squared */
double const ICM2M = 1e2;		/* inverse  ...........    	 */
double const ICM2M2 = 1e4;		/* inverse  ...........  squared */
double const ICM2M3 = 1e6;		/* inverse  ...........  cubed   */
double const MICRON2METER = 1e-6;	/*	microns to meters	 */

//#ifdef HAS_NUMERIC_LIMITS
//double const MAXDBL    = std::numeric_limits<double>::max();
//#else
//double const MAXDBL    = DBL_MAX;
//#endif
//BUG// construction order bug? .. use #define

#define MAXDBL (DBL_MAX)

double const BIGBIG    =  (MAXDBL)*(.9247958);	/* unlikely number	  */
//double const OVERDUE   = -(MAXDBL)*(.9347958);/* unlikely number	  */
double const NEVER     =  (MAXDBL)*(.9447958);	/* unlikely number	  */
double const NOT_INPUT = -(MAXDBL)*(.9547658);	/* unlikely number	  */
double const NOT_VALID = -(MAXDBL)*(.9647958);	/* unlikely number	  */
double const LINEAR    = -(MAXDBL)*(.9747958);	/* unlikely number	  */

double const LOGBIGBIG = log(BIGBIG);

double const VOLTMIN   =  1.0e-50;
double const PWRMIN    =  1.0e-100;

const char TOKENTERM[] = ",=()[]";

enum {FILE_OK=0, FILE_BAD=-1};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
