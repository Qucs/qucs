/*
 * taperedline.cpp - ideal tapered transmission line class implementation
 *
 * Copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 * $Id$
 *
 */



/* References
[1] Microwave Engineering. David M Pozar. John Wiley and Sons. 4th edition. Pages 261-267
[2] "Conversions between S, Z, Y, h, ABCD and T parameters which are Valid for Complex source and load impedances".
Dean A. Frickey. IEEE Transaction on Microwave Theory and Techniques. Vol 42. No 2. February 1994
*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "taperedline.h"
#include <matrix.h>

using namespace qucs;

taperedline::taperedline () : circuit (2) {
  type = CIR_TAPEREDLINE;
}
//------------------------------------------------------------------
// This function calculates the ABCD matrix of an arbitrarily tapered
// line by diving it into differential slots and calculating their ABCD matrix.
void taperedline::calcSparams(nr_double_t frequency)
{
  nr_double_t L = getPropertyDouble ("L");//Length
  nr_double_t Z1 = getPropertyDouble ("Z1");//Source impedance
  nr_double_t Z2 = getPropertyDouble ("Z2");//Load impedance
  nr_double_t alpha = getPropertyDouble ("Alpha");//Load impedance
  nr_double_t lambda = C0/frequency;
  nr_double_t lstep = 1e-3*lambda;
  nr_double_t Zi, beta;
  nr_complex_t a, b, c, d, gamma;
  matrix ABCD = eye(2);
  matrix ABCDaux = eye(2);
  for (nr_double_t l = lstep; l < L; l+=lstep)
  {
    if (!strcmp (getPropertyString ("Weighting"), "Exponential"))
    {
       Zi = calcExponential(l, L, Z1, Z2);
    }
    else
    {
        if (!strcmp (getPropertyString ("Weighting"), "Linear"))
        {
            Zi = calcLinear(l, L, Z1, Z2);
        }
        else
        {
           if (!strcmp (getPropertyString ("Weighting"), "Triangular"))
           {
              Zi = calcTriangular(l, L, Z1, Z2);
           }
           else//Klopfenstein
           {
              nr_double_t gamma_max = 0.05;
              Zi = calcKlopfenstein(l, L, Z1, Z2, gamma_max);
           }
        }
    }    
    beta = 2*pi*frequency/C0;
    gamma = nr_complex_t (alpha, beta);
    a = cosh(gamma*lstep);
    b = Zi*sinh(gamma*lstep);
    c = sinh(gamma*lstep)/Zi;
    d = cosh(gamma*lstep); 
    ABCDaux.set(0,0,a);
    ABCDaux.set(0,1,b);
    ABCDaux.set(1,0,c);
    ABCDaux.set(1,1,d);
    ABCD=ABCD*ABCDaux;
  }
  //Converting T matrix to S-parameters
  A=ABCD.get(0,0);
  B=ABCD.get(0,1);
  C=ABCD.get(1,0);
  D=ABCD.get(1,1);
}
//------------------------------------------------------------------
// Exponential impedance profile
nr_double_t taperedline::calcExponential(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2)
{
   nr_double_t a = (1/L)*std::log(Z2/Z1);
   return Z1*std::exp(a*l);
}

//------------------------------------------------------------------
// Linear impedance profile
nr_double_t taperedline::calcLinear(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2)
{
   return Z1+l*(Z2-Z1)/L;
}

//------------------------------------------------------------------
// Triangular impedance profile
nr_double_t taperedline::calcTriangular(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2)
{
   if (l < L/2)
   {
     return Z1*std::exp((2*l*l/(L*L))*std::log(Z2/Z1));
   }

   return Z1*std::exp(((4*l/L) - (2*l*l/(L*L)) -1)*std::log(Z2/Z1));
}

//------------------------------------------------------------------
// Klopfenstein impedance profile
nr_double_t taperedline::calcKlopfenstein(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2, nr_double_t gamma_max)
{
   nr_double_t gamma0 = 0.5*std::log(Z2/Z1);
   nr_double_t A = std::acosh(gamma0/gamma_max);
   return std::exp(0.5*std::log(Z1*Z2) + (gamma0/std::cosh(A))*A*A*phi(2*(l/L)-1, A));

}

//------------------------------------------------------------------
// Auxiliar function for Klopfenstein profile calculation
nr_double_t taperedline::phi(nr_double_t x, nr_double_t A)
{
   if (abs(x)<1e-4)
   {
      return 0;
   }
   if(abs(A)<1e-4)
   {
      return x/2.;
   }
   nr_double_t p=0;
   nr_double_t dy = 1e-3*x;
   for (nr_double_t y = 0; y < x; y+=dy)
   {
       p = p + (besseli(1, A*std::sqrt(1-y*y))/(A*std::sqrt(1-y*y)))*dy;
   }
   return p;

}

//------------------------------------------------------------------
// Modified Bessel function. It is calculated using its integral expression
nr_double_t taperedline::besseli(nr_double_t alpha, nr_double_t x)
{
 nr_double_t a=0, b=0;
 nr_double_t dtheta = 1e-1, dt = 10;
 for (nr_double_t theta = 0; theta<pi; theta+=dtheta) a+= std::exp(x*std::cos(theta))*std::cos(alpha*theta)*dtheta;
 for (nr_double_t t=0; t<1e4; t+=dt) b+= std::exp(-x*std::cosh(t) - alpha*t)*dt;
 return (a/pi)-b*std::sin(alpha/pi);

}
void taperedline::calcSP (nr_double_t frequency) {
  calcSparams(frequency);
  nr_double_t Z1 = getPropertyDouble ("Z1");
  nr_double_t Z2 = getPropertyDouble ("Z2");
  nr_complex_t S11 = (A*Z2+B -C*conj(Z1)*Z2-D*conj(Z1))/(A*Z2+B+C*Z1*Z2+D*Z1);
  nr_complex_t S12 = 2.*(A*D-B*C)*std::sqrt(Z1*Z2)/(A*Z2+B+C*Z1*Z2+D*Z1);
  nr_complex_t S21 = 2.*std::sqrt(Z1*Z2)/(A*Z2+B+C*Z1*Z2+D*Z1);
  nr_complex_t S22 = (-A*conj(Z2)+B-C*Z1*Z2+D*Z1)/(A*Z2+B+C*Z1*Z2+D*Z1);
  setS (NODE_1, NODE_1, S11); setS (NODE_2, NODE_2, S22);
  setS (NODE_1, NODE_2, S12); setS (NODE_2, NODE_1, S21);
}

void taperedline::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void taperedline::calcNoiseAC (nr_double_t) {
 nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4.0 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

void taperedline::initDC (void) {
  allocMatrixMNA ();
  // open circuit
  clearY ();
}

void taperedline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void taperedline::calcAC (nr_double_t frequency) {
  nr_double_t L = getPropertyDouble ("L");
  if (L != 0.0) {
/*
Conversions between S, Z, Y, h, ABCD and T parameters which are Valid for Complex source and load impedances.
Dean A. Frickey. IEEE Transaction on Microwave Theory and Techniques. Vol 42. No 2. February 1994
*/
    nr_complex_t y11 = D/B;
    nr_complex_t y12 = (B*C-A*D)/B;
    nr_complex_t y21 = -1./B;
    nr_complex_t y22 = A/B;
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
    setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
  }
}


// properties
PROP_REQ [] = {
  { "Z1", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "Z2", PROP_REAL, { 100, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 75e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "Weighting", PROP_STR, { PROP_NO_VAL, "Exponential" },
    PROP_RNG_STR4 ("Exponential", "Linear", "Triangular", "Klopfenstein") },
    PROP_NO_PROP };
PROP_OPT [] = {
  { "Alpha", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t taperedline::cirdef =
  { "TAPEREDLINE", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
