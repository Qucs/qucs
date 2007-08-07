/*
 * complex.cpp - complex number class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006 Gunther Kraut <gn.kraut@t-online.de>
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
 * $Id: complex.cpp,v 1.38 2007-08-07 20:43:00 ela Exp $
 *
 */

/*!\file complex.cpp
   
   Implement complex number class and functions
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <math.h>

#include "complex.h"
#include "consts.h"
#include "fspecial.h"

using namespace fspecial;

/*!\brief Constructor from a real imaginary pair 

   Construct a complex from a real part and an imaginary part 
   \todo Why not inline
*/
complex::complex (nr_double_t real, nr_double_t imag) {
  r = real;
  i = imag;
}


/*!\brief Default constructor 

   Initialize the complex number to \f$0 + 0 i\f$
   \todo Why not inline
*/
complex::complex () {
  r = 0.0;
  i = 0.0;
}


/*!\brief Copy constructor 
   \todo Why not inline
*/
complex::complex (const complex& z) {
  r = z.r;
  i = z.i;
}

/*!\brief Compute complex modulus 

   \return modulus of complex object
   \todo Why not inline
*/
nr_double_t complex::abs (void) {
  return xhypot (r, i);
}

/*!\brief Compute complex modulus 

   \param[in] z Complex number
   \return Modulus of z
   \todo Why not inline
*/
nr_double_t abs (const complex z) {
  return xhypot (z.r, z.i);
}

/*!\brief Compute complex modulus of real number

   \param[in] r Real number
   \return Modulus of r
   \todo Why not inline
*/
nr_double_t abs (const nr_double_t r) {
  return fabs (r);
}

/*!\brief Compute euclidian norm of complex number

   Compute \f$(\Re\mathrm{e}\;z )^2+ (\Im\mathrm{m}\;z)^2=|z|^2\f$
   \return Euclidian norm of z
   \todo Why not inline
*/
nr_double_t complex::norm (void) {
  return r * r + i * i;
}

/*!\brief Compute euclidian norm of complex number

   Compute \f$(\Re\mathrm{e}\;z )^2+ (\Im\mathrm{m}\;z)^2=|z|^2\f$
   \param[in] z Complex number
   \return Euclidian norm of z
   \todo Why not inline
*/
nr_double_t norm (const complex z) {
  return z.r * z.r + z.i * z.i;
}

/*!\brief Compute euclidian norm of real number

   Compute \f$r^2\f$
   \param[in] r Real number
   \return Euclidian norm of r
   \todo Why not inline
*/
nr_double_t norm (const nr_double_t r) {
  return r * r;
}

/*!\brief Compute argument of complex number

   \param[in] z Complex number
   \return Argument of z
   \todo lack of nr_double_t complex::arg(void)
   \todo Why not inline
*/
nr_double_t arg (const complex z) {
  return atan2 (z.i, z.r);
}

/*!\brief Real part of complex number

   \return Real part
   \todo Why not inline?
*/
nr_double_t complex::real (void) {
  return r;
}

/*!\brief Real part of complex number

   \param[in] z Complex number
   \return Real part of z
   \todo Why not inline?
*/
nr_double_t real (const complex z) {
  return z.r;
}

/*!\brief Real part of real number

   \param[in] r Real number
   \return Real part of r ie r
   \todo Why not inline?
*/
nr_double_t real (const nr_double_t r) {
  return r;
}

/*!\brief Imaginary part of complex number

   \return Imaginary part of complex object
   \todo Why not inline?
*/
nr_double_t complex::imag (void) {
  return i;
}

/*!\brief Imaginary part of complex number

   \param[in] z Complex number
   \return Imaginary part of z
   \todo Why not inline?
*/
nr_double_t imag (const complex z) {
  return z.i;
}

/*!\brief Imaginary part of complex number

   \param[in] r Real number
   \return Imaginary part of r
   \todo Why not inline?
*/
nr_double_t imag (const nr_double_t r) {
  return 0.0;
}

/*!\brief Conjugate of complex number

   \return Conjugate of complex object
   \todo Why not inline?
   \todo Why not operator ~
*/
complex complex::conj (void) {
  return complex (r, -i);
}

/*!\brief Conjugate of complex number

   \param[in] z Complex number
   \return Conjugate of complex z
   \todo Why not inline?
*/
complex conj (const complex z) {
  return complex (z.r, -z.i);
}

/*!\brief Conjugate of real number

   \param[in] r Real number
   \return Conjugate of real r ie r
   \todo Why not inline?
*/
nr_double_t conj (const nr_double_t r) {
  return r;
}

/*!\brief Magnitude in dB

   Compute \f$10\log_{10} |z|^2=20\log_{10} |z|\f$
   \param[in] z complex number
   \return Magnitude in dB
   \todo Why not inline?
*/
nr_double_t dB (const complex z) {
  return 10.0 * log10 (norm (z));
}

/*!\brief Compute complex exponential

   \param[in] z complex number
   \return exponential of z
*/
complex exp (const complex z) {
  nr_double_t mag = exp (real (z));
  return complex (mag * cos (imag (z)), mag * sin (imag (z)));
}

/*!\brief Compute limited complex exponential

   \param[in] z complex number
   \return limited exponential of z
   \todo Change limexp(real) limexp(complex) file order
*/
complex limexp (const complex z) {
  nr_double_t mag = limexp (real (z));
  return complex (mag * cos (imag (z)), mag * sin (imag (z)));
}

/*!\brief Compute limited exponential

   Compute limited exponential:
   \f[
   \begin{cases}
   \exp r & \text{if } r < \text{M\_LIMEXP} \\
   \exp (\text{M\_LIMEXP})\left[1.0 + (r - \text{M\_LIMEXP})\right] &
        \text{else}
   \end{cases}
   \f]

   #M_LIMEXP is a constant
   \param[in] r real number
   \return limited exponential of r
   \todo Change limexp(real) limexp(complex) file order
   \todo Document #M_LIMEXP
*/
nr_double_t limexp (const nr_double_t r) {
  return r < M_LIMEXP ? exp (r) : exp (M_LIMEXP) * (1.0 + (r - M_LIMEXP));
}

/*!\brief Compute principal value of square root

    Compute the square root of a given complex number (except negative
    real), and with a branch cut along the negative real  axis.

   \param[in] z complex number
   \return principal value of square root z
*/
complex sqrt (const complex z) {
#if 0
  if (z.i == 0.0) {
    return z.r < 0.0 ? complex (0.0, sqrt (-z.r)) : complex (sqrt (z.r));
  } else {
    nr_double_t phi = arg (z);
    return polar (sqrt (abs (z)), phi / 2.0);
  }
#else /* better numerical behaviour, avoiding arg() and polar() */
  if (z.r == 0.0 && z.i == 0.0) {
    return complex (0.0, 0.0);
  } else {
    nr_double_t x = fabs (z.r);
    nr_double_t y = fabs (z.i);
    nr_double_t k;
    if (x >= y)  {
      nr_double_t t = y / x;
      k = sqrt (x) * sqrt (0.5 * (1.0 + sqrt (1.0 + t * t)));
    } else {
      nr_double_t t = x / y;
      k = sqrt (y) * sqrt (0.5 * (t + sqrt (1.0 + t * t)));
    }
    if (z.r >= 0.0) {
      return complex (k, z.i / (2.0 * k));
    } else {
      nr_double_t vi = (z.i >= 0) ? k : -k;
      return complex (z.i / (2.0 * vi), vi);
    }
  }
#endif
}

/*!\brief Compute principal value of natural logarithm of z

   \param[in] z complex number
   \return principal value of natural logarithm of z
*/
complex ln (const complex z) {
  nr_double_t phi = arg (z);
  return complex (log (abs (z)), phi);
}

/*!\brief Compute principal value of decimal logarithm of z

   \param[in] z complex number
   \return principal value of decimal logarithm of z
*/
complex log10 (const complex z) {
  nr_double_t phi = arg (z);
  return complex (log10 (abs (z)), phi * M_LOG10E);
}

/*!\brief Compute principal value of binary logarithm of z

   \param[in] z complex number
   \return principal value of binary logarithm of z
*/
complex log2 (const complex z) {
  nr_double_t phi = arg (z);
  return complex (log (abs (z)) * M_LOG2E, phi * M_LOG2E);
}

/*!\brief Compute power function with real exponent

   \param[in] z complex mantisse
   \param[in] d real exponent
   \return z power d (\f$z^d\f$)
*/
complex pow (const complex z, const nr_double_t d) {
  return polar (pow (abs (z), d), arg (z) * d);
}

/*!\brief Compute power function with complex exponent but real mantisse

   \param[in] d real mantisse
   \param[in] z complex exponent
   \return d power z (\f$d^z\f$)
*/
complex pow (const nr_double_t d, const complex z) {
  return exp (z * log (d));
}


/*!\brief Compute complex power function 

   \param[in] z1 complex mantisse
   \param[in] z2 complex exponent
   \return d power z (\f$z_1^{z_2}\f$)
*/
complex pow (const complex z1, const complex z2) {
  return exp (z2 * ln (z1));
}

/*!\brief Compute complex sinus

   \param[in] z complex angle
   \return sinus of z
*/
complex sin (const complex z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return (polar (exp (-i), r - M_PI_2) - polar (exp (i), -r - M_PI_2)) / 2.0;
}

/*!\brief Compute complex arc sinus

   \param[in] z complex arc
   \return arc sinus of z
*/
complex arcsin (const complex z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return complex (0.0, -1.0) * ln (rect (-i, r) + sqrt (1.0 - z * z));
}

/*!\brief Compute complex cosinus

   \param[in] z complex angle
   \return cosinus of z
*/
complex cos (const complex z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return (polar (exp (-i), r) + polar (exp (i), -r)) / 2.0;
}

/*!\brief Compute complex arc cosinus

   \param[in] z complex arc
   \return arc cosinus of z
*/
complex arccos (const complex z) {
#if 0
  return rect (0.0, -2.0) * ln (M_SQRT1_2 * (sqrt (z + 1.0) + sqrt (z - 1.0)));
#else
  complex y = sqrt (z * z - 1.0);
  if (z.r * z.i < 0.0) y = -y;
  return rect (0, -1.0) * ln (z + y);
#endif
}

/*!\brief Compute complex tangent

   \param[in] z complex angle
   \return tangent of z
*/
complex tan (const complex z) {
  nr_double_t r = 2.0 * real (z);
  nr_double_t i = 2.0 * imag (z);
  return rect (0.0, -1.0) + rect (0.0, 2.0) / (polar (exp (-i), r) + 1.0);
}

/*!\brief Compute complex arc tangent

   \param[in] z complex arc
   \return arc tangent of z
*/
complex arctan (const complex z) {
  return rect (0.0, -0.5) * ln (rect (0.0, 2.0) / (z + rect (0.0, 1.0)) - 1.0);
}

/*!\brief Compute complex arc tangent fortran like function
    
   atan2 is a two-argument function that computes the arctangent of y / x 
   given y and x, but with a range of \f$(-\pi;\pi]\f$

   \param[in] z complex angle
   \return arc tangent of z
*/
complex arctan2 (const complex y, const complex x) {
  complex a = arctan (y / x);
  return real (x) > 0.0 ? a : -a;
}

/*!\brief Compute complex cotangent

   \param[in] z complex angle
   \return cotangent of z
*/
complex cot (const complex z) {
  nr_double_t r = 2.0 * real (z);
  nr_double_t i = 2.0 * imag (z);
  return rect (0.0, 1.0) + rect (0.0, 2.0) / (polar (exp (-i), r) - 1.0);
}

/*!\brief Compute complex arc cotangent

   \param[in] z complex arc
   \return arc cotangent of z
*/
complex arccot (const complex z) {
  return rect (0.0, -0.5) * ln (rect (0.0, 2.0) / (z - rect (0.0, 1.0)) + 1.0);
}

/*!\brief Compute complex hyperbolic sinus

   \param[in] z complex angle
   \return hyperbolic sinus of z
*/
complex sinh (const complex z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return (polar (exp (r), i) - polar (exp (-r), -i)) / 2.0;
}

/*!\brief Compute complex argument hyperbolic sinus

   \param[in] z complex arc
   \return argument hyperbolic sinus of z
*/
complex arsinh (const complex z) {
  return ln (z + sqrt (z * z + 1));
}

/*!\brief Compute complex hyperbolic cosinus

   \param[in] z complex angle
   \return hyperbolic cosinus of z
*/
complex cosh (const complex z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return (polar (exp (r), i) + polar (exp (-r), -i)) / 2.0;
}

/*!\brief Compute complex argument hyperbolic cosinus

   \param[in] z complex arc
   \return argument hyperbolic cosinus of z
*/
complex arcosh (const complex z) {
  return ln (z + sqrt (z * z - 1.0));
}

/*!\brief Compute complex argument hyperbolic secant

   \param[in] z complex arc
   \return argument hyperbolic secant of z
   \todo for symetry reason implement sech
*/
complex arsech (const complex z) {
  return ln ((1.0 + sqrt (1.0 - z * z)) / z);
}

/*!\brief Compute complex hyperbolic tangent

   \param[in] z complex angle
   \return hyperbolic tangent of z
*/
complex tanh (const complex z) {
  nr_double_t r = 2.0 * real (z);
  nr_double_t i = 2.0 * imag (z);
  return 1.0 - 2.0 / (polar (exp (r), i) + 1.0);
}

/*!\brief Compute complex argument hyperbolic tangent

   \param[in] z complex arc
   \return argument hyperbolic tangent of z
*/
complex artanh (const complex z) {
  return 0.5 * ln ( 2.0 / (1.0 - z) - 1.0);
}

/*!\brief Compute complex hyperbolic cotangent

   \param[in] z complex angle
   \return hyperbolic cotangent of z
*/
complex coth (const complex z) {
  nr_double_t r = 2.0 * real (z);
  nr_double_t i = 2.0 * imag (z);
  return 1.0 + 2.0 / (polar (exp (r), i) - 1.0);
}

/*!\brief Compute complex argument hyperbolic cotangent

   \param[in] z complex arc
   \return argument hyperbolic cotangent of z
*/
complex arcoth (const complex z) {
  return 0.5 * ln (2.0 / (z - 1.0) + 1.0);
}

/*!\brief Converts impedance to reflexion coefficient

   \param[in] z impedance
   \param[in] zref normalisation impedance
   \return reflexion coefficient
*/
complex ztor (const complex z, complex zref) {
  return (z - zref) / (z + zref);
}

/*!\brief Converts admittance to reflexion coefficient
   \param[in] y admitance
   \param[in] zref normalisation impedance
   \return reflexion coefficient
*/
complex ytor (const complex y, complex zref) {
  return (1 - y * zref) / (1 + y * zref);
}

/*!\brief Converts reflexion coefficient to impedance
   \param[in] r reflexion coefficient
   \param[in] zref normalisation impedance
   \return impedance
*/
complex rtoz (const complex r, complex zref) {
  return zref * (1 + r) / (1 - r);
}

/*!\brief Converts reflexion coefficient to admittance
   \param[in] r reflexion coefficient
   \param[in] zref normalisation impedance
   \return admittance
*/
complex rtoy (const complex r, complex zref) {
  return (1 - r) / (1 + r) / zref;
}

/*!\brief Complex floor 

    floor is the largest integral value not greater than argument
    Apply floor to real and imaginary part 
    \param[in] z complex number
    \return floored complex number
    \todo Why not inline?
    \todo Move near ceil
*/
complex floor (const complex z) {
  return rect (floor (real (z)), floor (imag (z)));
}

/*!\brief complex signum function 
   
    compute \f[
    \mathrm{signum}\;z= \mathrm{signum} (re^{i\theta})
                     = \begin{cases}
		       0 & \text{if } z=0 \\
		       e^{i\theta} & \text{else}
		       \end{cases}
	    \f]
   \param[in] z complex number
   \return signum of z
   \todo Better implementation z/abs(z) is not really stable
*/
complex signum (const complex z) {
  if (z == 0) return 0;
  return z / abs (z);
}

/*!\brief complex sign function 
   
    compute \f[
    \mathrm{sign}\;z= \mathrm{sign} (re^{i\theta})
                     = \begin{cases}
		       1 & \text{if } z=0 \\
		       e^{i\theta} & \text{else}
		       \end{cases}
	    \f]
   \param[in] z complex number
   \return sign of z
   \todo Better implementation z/abs(z) is not really stable
*/
complex sign (const complex z) {
  if (z == 0) return complex (1);
  return z / abs (z);
}

/*!\brief Euclidean distance function

   The xhypot() function returns \f$\sqrt{a^2+b^2}\f$.  
   This is the length of the hypotenuse of a right-angle triangle with sides 
   of length a and b, or the distance
   of the point (a,b) from the origin.

   \param[in] a first length
   \param[in] b second length
   \return Euclidean distance from (0,0) to (a,b): \f$\sqrt{a^2+b^2}\f$
*/
nr_double_t xhypot (const nr_double_t a, const nr_double_t b) {
  nr_double_t c = fabs (a);
  nr_double_t d = fabs (b);
  if (c > d) {
    nr_double_t e = d / c;
    return c * sqrt (1 + e * e);
  }
  else if (d == 0)
    return 0;
  else {
    nr_double_t e = c / d;
    return d * sqrt (1 + e * e);
  }
}

/*!\brief Euclidean distance function for complex argument

   The xhypot() function returns \f$\sqrt{a^2+b^2}\f$.  
   This is the length of the hypotenuse of a right-angle triangle with sides 
   of length a and b, or the distance
   of the point (a,b) from the origin.

   \param[in] a first length
   \param[in] b second length
   \return Euclidean distance from (0,0) to (a,b): \f$\sqrt{a^2+b^2}\f$
*/
nr_double_t xhypot (const complex a, const complex b) {
  nr_double_t c = norm (a);
  nr_double_t d = norm (b);
  if (c > d)
    return abs (a) * sqrt (1 + d / c);
  else if (d == 0)
    return 0;
  else
    return abs (b) * sqrt (1 + c / d);
}

/*!\brief Euclidean distance function for a double b complex */
nr_double_t xhypot (const nr_double_t a, const complex b) {
  return xhypot (complex (a), b);
}

/*!\brief Euclidean distance function for b double a complex */
nr_double_t xhypot (const complex a, const nr_double_t b) {
  return xhypot (a, complex (b));
}

/*!\brief real signum function 
   
    compute \f[
    \mathrm{signum}\;d=
                     = \begin{cases}
		       O & \text{if } d=0 \\
		       1 & \text{if } d>0 \\
		       -1 & \text{if } d<0
		       \end{cases}
	    \f]
   \param[in] d real number
   \return signum of d
   \todo Move near complex signum
*/
nr_double_t signum (const nr_double_t d) {
  if (d == 0) return 0;
  return d < 0 ? -1 : 1;
}

/*!\brief real sign function 
   
    compute \f[
    \mathrm{sign}\;d=
                     = \begin{cases}
		       1 & \text{if } d\ge 0 \\
		       -1 & \text{if } d<0
		       \end{cases}
	    \f]
   \param[in] d real number
   \return sign of d
   \todo Move near complex sign
*/
nr_double_t sign (const nr_double_t d) {
  return d < 0 ? -1 : 1;
}

/*!\brief Cardinal sinus 
   
   Compute \f$\mathrm{sinc}\;z=\frac{\sin z}{z}\f$
   \param[in] z complex number
   \return cardianal sinus of z
   \todo Why not inline
*/
complex sinc (const complex z) {
  if (z == 0) return 1;
  return sin (z) / z;
}

/*!\brief Real cardinal sinus 
   
   Compute \f$\mathrm{sinc}\;d=\frac{\sin d}{d}\f$
   \param[in] d real number
   \return cardianal sinus of s
   \todo Why not inline
*/
nr_double_t sinc (const nr_double_t d) {
  if (d == 0) return 1;
  return sin (d) / d;
}

/*!\brief Construct a complex number using polar notation
   \param[in] mag Magnitude
   \param[in] ang Angle
   \return complex number in rectangular form
   \todo Why not inline
*/
complex polar (const nr_double_t mag, const nr_double_t ang) {
  return rect (mag * cos (ang), mag * sin (ang));
}

/*!\brief Construct a complex number using rectangular notation
   \param[in] x Real part
   \param[in] y Imagninary part
   \return complex number in rectangular form
   \todo Why not inline?
   \todo Move before polar
*/
complex rect (const nr_double_t x, const nr_double_t y) {
  return complex (x, y);
}
/*!\brief Extension of polar construction to complex
   \param[in] a Magnitude
   \param[in] p Angle
   \return complex number in rectangular form
   \bug Do not seems holomorph form of real polar
   \todo Move near real polar
*/
complex polar (const complex a, const complex p) {
  return a * exp (rect (p.i, -p.r));
}

/*!\brief Complex ceil
    Ceil is the smallest integral value not less than argument
    Apply ceil to real and imaginary part 
    \param[in] z complex number
    \return ceilled complex number
    \todo Why not inline?
*/
complex ceil (const complex z) {
  return rect (ceil (z.r), ceil (z.i));
}

/*!\brief Fix function 

    Fix is nearest integral value in direction of 0,
    \f[
    \operatorname{fix} d=\begin{cases}
    \operatorname{floor} d & \text{if } d > 0 \\
    \operatorname{ceil} d  & \text{else}
    \end{cases}
    \f]
    
    \param[in] d real number
    \return fixed complex number
    \todo Why not inline?
*/
nr_double_t fix (const nr_double_t d) {
  return (d > 0) ? floor (d) : ceil (d);
}

/*!\brief Complex ceil
    
    Apply fix to real and imaginary part 
    \param[in] z complex number
    \return fixed complex number
    \todo Why not inline?
    \todo why not using real fix
*/
complex fix (const complex z) {
  nr_double_t x = z.r;
  nr_double_t y = z.i;
  x = (x > 0) ? floor (x) : ceil (x);
  y = (y > 0) ? floor (y) : ceil (y);
  return rect (x, y);
}

/*!\brief Complex round
    round is the nearest integral value 
    Apply round to real and imaginary part 
    \param[in] z complex number
    \return rounded complex number
    \todo Why not inline?
*/
complex round (const complex z) {
  return rect (round (z.r), round (z.i));
}

/*!\brief Square of complex number
    
    \param[in] z complex number
    \return squared complex number
    \todo Why not inline?
*/
complex sqr (const complex z) {
  return rect (z.r * z.r - z.i * z.i, 2 * z.r * z.i);
}

/*!\brief Heaviside step function
   
   The Heaviside step function, H, also called unit step function, 
   is a discontinuous function whose value is zero for negative argument and 
   one for positive argument. For zero by convention, H(0)=0.5
   \param[in] d Heaviside argument
   \return Heaviside step
   \todo Create Heaviside alias
*/ 
nr_double_t step (const nr_double_t d) {
  nr_double_t x = d;
  if (x < 0.0)
    x = 0.0;
  else if (x > 0.0)
    x = 1.0;
  else
    x = 0.5;
  return x;
}

/*!\brief Heaviside step function for complex number
   
   Apply Heaviside to real and imaginary part
   \param[in] z Heaviside argument
   \return Heaviside step
   \todo Create Heaviside alias
   \todo Why not using real heaviside
*/ 
complex step (const complex z) {
  nr_double_t x = z.r;
  nr_double_t y = z.i;
  if (x < 0.0)
    x = 0.0;
  else if (x > 0.0)
    x = 1.0;
  else
    x = 0.5;
  if (y < 0.0)
    y = 0.0;
  else if (y > 0.0)
    y = 1.0;
  else
    y = 0.5;
  return rect (x, y);
}

/*!\brief Bessel function of first kind
   
   \param[in] n order 
   \param[in] z argument
   \return Bessel function of first kind of order n 
   \bug Not implemented
*/ 
complex jn (const int n, const complex z) {
  return rect (jn (n, z.r), 0);
}

/*!\brief Bessel function of second kind
   
   \param[in] n order 
   \param[in] z argument
   \return Bessel function of second kind of order n 
   \bug Not implemented
*/ 
complex yn (const int n, const complex z) {
  return rect (yn (n, z.r), 0);
}


/*!\brief Modified Bessel function of first kind
   
   \param[in] z argument
   \return Modified Bessel function of first kind of order 0
   \bug Not implemented
*/
complex i0 (const complex z) {
  return rect (i0 (z.r), 0);
}

/*!\brief Error function
   
   \param[in] z argument
   \return Error function
   \bug Not implemented
*/
complex erf (const complex z) {
  return rect (erf (z.r), 0);
}

/*!\brief Complementart error function
   
   \param[in] z argument
   \return Complementary error function
   \bug Not implemented
*/
complex erfc (const complex z) {
  return rect (erfc (z.r), 0);
}

/*!\brief Inverse of error function
   
   \param[in] z argument
   \return Inverse of error function
   \bug Not implemented
*/
complex erfinv (const complex z) {
  return rect (erfinv (z.r), 0);
}

/*!\brief Inverse of complementart error function
   
   \param[in] z argument
   \return Inverse of complementary error function
   \bug Not implemented
*/
complex erfcinv (const complex z) {
  return rect (erfcinv (z.r), 0);
}

/*!\brief Unary +
  \todo Why not inline 
*/
complex complex::operator+() {
  return complex (r, i);
}

/*!\brief Unary -
  \todo Why not inline 
*/
complex complex::operator-() {
  return complex (-r, -i);
}

/*!\brief += operator for complex
  \todo Why not inline 
*/
complex& complex::operator+=(const complex z2) {
  r += z2.r;
  i += z2.i;
  return *this;
}

/*!\brief += operator for real
  \todo Why not inline 
*/
complex& complex::operator+=(const nr_double_t r2) {
  r += r2;
  return *this;
}

/*!\brief -= operator for complex
  \todo Why not inline 
*/
complex& complex::operator-=(const complex z2) {
  r -= z2.r;
  i -= z2.i;
  return *this;
}

/*!\brief -= operator for real
  \todo Why not inline 
*/
complex& complex::operator-=(const nr_double_t r2) {
  r -= r2;
  return *this;
}

/*!\brief *= operator for complex
  \todo Why not inline 
  \todo Add for real
*/
complex& complex::operator*=(const nr_double_t r2) {
  r *= r2;
  i *= r2;
  return *this;
}

/*!\brief /= operator for complex
  \todo Why not inline 
  \todo Add for real
*/
complex& complex::operator/=(const nr_double_t r2) {
  r /= r2;
  i /= r2;
  return *this;
}

/*!\brief Addition of two complex
  \todo Why not inline
*/
complex operator+(const complex z1, const complex z2) {
  return complex (z1.r + z2.r, z1.i + z2.i);
}

/*!\brief Addition of real and complex
  \todo Why not inline
*/
complex operator+(const nr_double_t r1, const complex z2) {
  return complex (r1 + z2.r, z2.i);
}

/*!\brief Addition of complex and real
  \todo Why not inline
*/
complex operator+(const complex z1, const nr_double_t r2) {
  return complex (z1.r + r2, z1.i);
}

/*!\brief Substraction of two complex
  \todo Why not inline
*/
complex operator-(const complex z1, const complex z2) {
  return complex (z1.r - z2.r, z1.i - z2.i);
}

/*!\brief Substraction of real and complex
  \todo Why not inline
*/
complex operator-(const nr_double_t r1, const complex z2) {
  return complex (r1 - z2.r, -z2.i);
}

/*!\brief Substraction of complex and real
  \todo Why not inline
*/
complex operator-(const complex z1, const nr_double_t r2) {
  return complex (z1.r - r2, z1.i);
}

/*!\brief Multiplication of complex and real
  \todo Why not inline
*/
complex operator*(const complex z1, const nr_double_t r2) {
  return complex (z1.r * r2, z1.i * r2);
}

/*!\brief Multiplication of real and complex
  \todo Why not inline
*/
complex operator*(const nr_double_t r1, const complex z2) {
  return complex (z2.r * r1, z2.i * r1);
}

/*!\brief Multiplication of two complex
  \todo Why not inline
*/
complex operator*(const complex z1, const complex z2) {
  return complex (z1.r * z2.r - z1.i * z2.i, z1.i * z2.r + z1.r * z2.i);
}

/*!\brief Equality of two complex
  \todo Why not inline
  \note Like equality of double this test 
        is meaningless in finite precision
	Use instead abs(x-x0) < tol
*/
bool operator==(const complex z1, const complex z2) {
  return (z1.r == z2.r) && (z1.i == z2.i);
}

/*!\brief Inequality of two complex
  \todo Why not inline
  \note Like inequality of double this test 
        is meaningless in finite precision
	Use instead abs(x-x0) > tol
*/
bool operator!=(const complex z1, const complex z2) {
  return (z1.r != z2.r) || (z1.i != z2.i);
}

/*!\brief Superior of equal
   \todo Why not inline
*/
bool operator>=(const complex z1, const complex z2) {
  return norm (z1) >= norm (z2);
}

/*!\brief Inferior of equal
   \todo Why not inline
*/
bool operator<=(const complex z1, const complex z2) {
  return norm (z1) <= norm (z2);
}

/*!\brief Superior
   \todo Why not inline
*/
bool operator>(const complex z1, const complex z2) {
  return norm (z1) > norm (z2);
}

/*!\brief Inferior 
   \todo Why not inline
*/
bool operator<(const complex z1, const complex z2) {
  return norm (z1) < norm (z2);
}

/*!\brief Divide a complex by a double
   \todo why not inline
   \todo Put near *
*/
complex operator/(const complex z1, const nr_double_t r2) {
  return complex (z1.r / r2, z1.i / r2);
}

/*!\brief Divide a complex by a complex
   \todo Put near *
*/
complex operator/(const complex z1, const complex z2) {
#if 0
  nr_double_t n = norm (z2);
  return complex ((z1.r * z2.r + z1.i * z2.i) / n,
		  (z1.i * z2.r - z1.r * z2.i) / n);
#else /* avoid numerical overflow and underrun */
  nr_double_t r, i, n, d;
  if (fabs (z2.r) > fabs (z2.i)) {
    n = z2.i / z2.r;
    d = z2.r + z2.i * n;
    r = (z1.r + z1.i * n) / d;
    i = (z1.i - z1.r * n) / d;
  }
  else {
    n = z2.r / z2.i;
    d = z2.r * n + z2.i;
    r = (z1.r * n + z1.i) / d;
    i = (z1.i * n - z1.r) / d;
  }
  return complex (r, i);
#endif
}

/*!\brief Divide and assign
   \todo Put near *=
*/
complex& complex::operator/=(const complex z) {
#if 0
  nr_double_t n1, n2;
  n1 = norm (z);
  n2 = (r * z.r + i * z.i) / n1;
  i  = (i * z.r - r * z.i) / n1;
  r  = n2;
#else /* avoid numerical overflow and underrun */
  nr_double_t n, d, t;
  if (fabs (z.r) > fabs (z.i)) {
    n = z.i / z.r;
    d = z.r + z.i * n;
    t = (r + i * n) / d;
    i = (i - r * n) / d;
    r = t;
  }
  else {
    n = z.r / z.i;
    d = z.r * n + z.i;
    t = (r * n + i) / d;
    i = (i * n - r) / d;
    r = t;
  }
#endif
  return *this;
}

/*!\brief Divide a real by a complex
   \todo Put near *
*/
complex operator/(const nr_double_t r1, const complex z2) {
#if 0
  nr_double_t n = norm (z2);
  return complex (r1 * z2.r / n, -r1 * z2.i / n);
#else /* avoid numerical overflow and underrun */
  nr_double_t r, i, n, d;
  if (fabs (z2.r) > fabs (z2.i)) {
    n = z2.i / z2.r;
    d = z2.r + z2.i * n;
    r = r1 / d;
    i = -n * r1 / d;
  }
  else {
    n = z2.r / z2.i;
    d = z2.r * n + z2.i;
    r = r1 * n / d;
    i = -r1 / d;
  }
  return complex (r, i);
#endif
}

/*!\brief Modulo 
   \todo Why not inline 
*/
complex operator%(const complex z1, const complex z2) {
  return z1 - z2 * floor (z1 / z2);
}

/*!\brief Modulo 
   \todo Why not inline 
*/
complex operator%(const complex z1, const nr_double_t r2) {
  return z1 - r2 * floor (z1 / r2);
}

/*!\brief Modulo 
   \todo Why not inline 
*/
complex operator%(const nr_double_t r1, const complex z2) {
  return r1 - z2 * floor (r1 / z2);
}

/*!\brief Modulo and assign
   \todo Why not inline 
*/
complex& complex::operator%=(const complex z) {
  *this = *this % z;
  return *this;
}

/*!\brief Modulo and assign
   \todo Why not inline 
*/
complex& complex::operator%=(const nr_double_t r) {
  *this = *this % r;
  return *this;
}

/*!\brief Assign
   \todo Why not inline 
*/
complex& complex::operator=(const complex z) {
  r = z.r;
  i = z.i;
  return *this;
}

/*!\brief Assign with real 
   \todo Why not inline 
*/
complex& complex::operator=(const nr_double_t x) {
  r = x;
  i = 0.0;
  return *this;
}

/*!\brief Multiply and assign
   \todo Put near other *=
*/
complex& complex::operator*=(const complex z) {
  nr_double_t n;
  n = r * z.r - i * z.i;
  i = i * z.r + r * z.i;
  r = n;
  return *this;
}

#ifdef DEBUG
#include <stdio.h>
/*!\brief Print a complex number
   \todo Transform in inline
   \todo Add a debug method that is void if non debug
*/
void complex::print (void) {
  fprintf (stderr, "%+.2e,%+.2e ", (double) r, (double) i);
}
#endif /* DEBUG */
