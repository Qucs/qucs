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
[3] "Handbook of Mathematical functions with Formulas, Graphs and Mathematical Tables". Milton Abramowitz. 
US department of commerce (1964). Link: http://people.math.sfu.ca/~cbm/aands/page_375.htm
*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "taperedline.h"

using namespace qucs;

taperedline::taperedline () : circuit (2) {
  type = CIR_TAPEREDLINE;
}
//------------------------------------------------------------------
// This function calculates the ABCD matrix of an arbitrarily tapered
// line by diving it into differential slots and calculating their ABCD matrix.
void taperedline::calcABCDparams(nr_double_t frequency)
{
  nr_double_t L = getPropertyDouble ("L");//Length
  nr_double_t Z1 = getPropertyDouble ("Z1");//Port 1 impedance
  nr_double_t Z2 = getPropertyDouble ("Z2");//Port 2 impedance
  nr_double_t alpha = getPropertyDouble ("Alpha");//Loss coefficient
  nr_double_t gamma_max = getPropertyDouble ("Gamma_max");;//Maximum ripple (Klopfenstein weighting only)
  nr_double_t lambda = C0/frequency;
  nr_double_t lstep = 3e-3*lambda;//Size of the differential elements
  nr_double_t Zi, beta;
  nr_complex_t a, b, c, d, gamma;
  matrix ABCD_ = eye(2);//Overall ABCD matrix
  matrix ABCDaux = eye(2);//Auxiliar matrix for performing the iterative product
  alpha = std::log(alpha) / 2.0;//The attenuation coefficient needs to be converted into Neper/m units
  for (nr_double_t l = lstep/2; l <= L-(lstep/2); l+=lstep)
  {
    // The line is discretized in finite elements. The size of these elements can be considered a differential
    // length since it is 1e-3*wavelength. Taking into account the cascading property of the ABCD matrix, the overall
    // ABCD matrix can be calculated as the product of the individual ABCD matrices.
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
              Zi = calcKlopfenstein(l, L, Z1, Z2, gamma_max);
           }
        }
    }    
    beta = 2*pi*frequency/C0;//Propagation constant
    gamma = nr_complex_t (alpha, beta);//Complex propagation constant
    // ABCD coefficients
    a = cosh(gamma*lstep);
    b = Zi*sinh(gamma*lstep);
    c = sinh(gamma*lstep)/Zi;
    d = cosh(gamma*lstep); 
    ABCDaux.set(0,0,a);
    ABCDaux.set(0,1,b);
    ABCDaux.set(1,0,c);
    ABCDaux.set(1,1,d);
    //Iterative product
    ABCD_=ABCD_*ABCDaux;

  }
  // Overall ABCD coefficients
  ABCD = ABCD_;
}

nr_double_t integrate_n (vector v) { /* using trapezoidal rule */
  nr_double_t result = 0.0;
  for (int i = 1; i < v.getSize () - 1; i++) result += norm (v.get (i));
  result += 0.5 * norm (v.get (0));
  result += 0.5 * norm (v.get (v.getSize () - 1));
  return result;
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
   return (std::exp(0.5*std::log(Z1*Z2) + (gamma0/std::cosh(A))*A*A*phi(2.*(l/L)-1., A)));
}

//------------------------------------------------------------------
// Auxiliar function for Klopfenstein profile calculation
nr_double_t taperedline::phi(nr_double_t x, nr_double_t A)
{
   if (abs(x)<1e-4) return 0;//phi(0, A) = 0
   if(abs(A)<1e-4) return x/2.;//phi(x, 0) = x/2
   if(abs(x-1)<1e-4) return (std::cosh(A)-1)/(A*A);//phi(1, A)=(cosh(A)-1)/A^2 
   nr_double_t sign;
   // phi(x, A) is defined for |x|<=1
   if (x > 1) x=1;
   if (x < -1) x=-1;

   if (x <= 0)//Integration direction
   {
     sign = -1;
   }
   else
   {
     sign = 1;
   }

   nr_double_t dy = abs(5e-3*x);//Differential step
   vector vIntegration(std::floor(abs(x)/dy));
   int i=0;
   for (nr_double_t y = 0; y <= abs(x); y+=dy)//Integration
   {
       vIntegration(i) = (besseli(1, A*std::sqrt(1-y*y))/(A*std::sqrt(1-y*y)))*dy;
       i++;
   }
   return sign*integrate_n(vIntegration);

}


//------------------------------------------------------------------
// Modified Bessel function. It is calculated using the infinite discrete
// summation formula. See "Handbook of Mathematical functions with Formulas, Graphs and
// Mathematical Tables". Milton Abramowitz. US department of commerce (1964). Page 375
// http://people.math.sfu.ca/~cbm/aands/page_375.htm
nr_double_t taperedline::besseli(nr_double_t alpha, nr_double_t x)
{
nr_double_t b=0;
for (int m = 0; m<15; m++)
{
	b+=std::pow(x/2, 2*m+alpha)*(1/(factorial(m)*std::tgamma(m+alpha+1)));
}
return b;
}

//------------------------------------------------------------------------
// This function calculates the factorial. In favour of efficiency, it implements a LUT for m < 30
long int taperedline::factorial(int m)
{
switch(m)
{
   case 0: return 1;
   case 1: return 1;
   case 2: return 2;
   case 3: return 6;
   case 4: return 24;
   case 5: return 120;
   case 6: return 720;
   case 7: return 5040;
   case 8: return 40320;
   case 9: return 362880;
   case 10: return 3628800;
   case 11: return 39916800;
   case 12: return 479001600;
   case 13: return 6227020800; 
   case 14: return 87178291200;
   case 15: return 1307674368000;
   case 16: return 20922789888000;
   case 17: return 355687428096000;
   case 18: return 6402373705728000;
   case 19: return 121645100408832000;
   case 20: return 2432902008176640000;
   default:
      long int k=1;
      for(int i =1; i<=m;i++)k*=i;
      return k;
}

}





void taperedline::calcSP (nr_double_t frequency) {
  calcABCDparams(frequency);
  matrix S = qucs::atos(ABCD, z0, z0);
  nr_complex_t S11 = S.get(0,0);
  nr_complex_t S12 = S.get(0,1);
  nr_complex_t S21 = S.get(1,0);
  nr_complex_t S22 = S.get(1,1);
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
  calcABCDparams(frequency);
  nr_double_t L = getPropertyDouble ("L");
  if (L != 0.0) {
/*
Conversions between S, Z, Y, h, ABCD and T parameters which are Valid for Complex source and load impedances.
Dean A. Frickey. IEEE Transaction on Microwave Theory and Techniques. Vol 42. No 2. February 1994
*/
    
    nr_complex_t y11 = ABCD.get(1,1)/ABCD.get(0,1);
    nr_complex_t y12 = det(ABCD)/ABCD.get(0,1);
    nr_complex_t y21 = -1./ABCD.get(0,1);
    nr_complex_t y22 = ABCD.get(0,0)/ABCD.get(0,1);
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y22);
    setY (NODE_1, NODE_2, y12); setY (NODE_2, NODE_1, y21);
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
  { "Gamma_max", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Alpha", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t taperedline::cirdef =
  { "TAPEREDLINE", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
