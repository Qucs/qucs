/*
 * complex_missing_std.h - complex number class definitions for missing std functions
 *
 * Copyright (C) 2014 Richard Crozier <richard.crozier@yahoo.co.uk>
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


#ifndef __COMPLEX_MISSING_STD_H__
#define __COMPLEX_MISSING_STD_H__

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "complex.h"

/* -------------------------------------------------------
                    C++11 Functions
   ------------------------------------------------------- */
/* \bug already defined in complex.cpp ?
#ifndef HAVE_CXX_COMPLEX_ASIN
namespace std
{
nr_complex_t asin (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_ASINH
namespace std
{
nr_complex_t asinh (const nr_complex_t);
}
#endif

// overloaded math functions
#ifndef HAVE_CXX_COMPLEX_ACOS
namespace std
{
nr_complex_t acos (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_ACOSH
namespace std
{
nr_complex_t acosh (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_ATAN
namespace std
{
nr_complex_t atan (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_ATANH
namespace std
{
nr_complex_t atanh (const nr_complex_t);
}
#endif
*/

/* -------------------------------------------------------
        Functions Present in Standard Before C++11
   ------------------------------------------------------- */

#ifndef HAVE_CXX_COMPLEX_SIN
namespace std
{
nr_complex_t sin (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_SINH
namespace std
{
nr_complex_t sinh (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_COS
namespace std
{
nr_complex_t cos (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_COSH
namespace std
{
nr_complex_t cosh (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_TAN
namespace std
{
nr_complex_t tan (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_TANH
namespace std
{
nr_complex_t tanh (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_EXP
namespace std
{
nr_complex_t exp (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_LOG
namespace std
{
nr_complex_t log (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_LOG10
namespace std
{
nr_complex_t log10 (const nr_complex_t);
}
#endif

#ifndef HAVE_CXX_COMPLEX_NORM
namespace std
{
/*!\brief Compute euclidian norm of complex number
Compute \f$(\Re\mathrm{e}\;z )^2+ (\Im\mathrm{m}\;z)^2=|z|^2\f$
\param[in] z Complex number
\return Euclidian norm of z
*/
inline nr_double_t norm (const nr_complex_t z)
{
    nr_double_t r = real (z);
    nr_double_t i = imag (z);
    return r * r + i * i;
}
}
#endif

#ifndef HAVE_CXX_COMPLEX_POLAR
namespace std
{
/*!\brief Construct a complex number using polar notation
\param[in] mag Magnitude
\param[in] ang Angle
\return complex number in rectangular form
*/
inline nr_complex_t polar (const nr_double_t mag, const nr_double_t ang = 0.0)
{
    return nr_complex_t (mag * cos (ang), mag * sin (ang));
}
}
#endif

#ifndef HAVE_CXX_COMPLEX_POW
namespace std
{
/*!\brief Compute power function with real exponent
\param[in] z complex mantisse
\param[in] d real exponent
\return z power d (\f$z^d\f$)
*/
inline nr_complex_t pow (const nr_complex_t z, const nr_double_t d)
{
    return std::polar (pow (abs (z), d), arg (z) * d);
}
/*!\brief Compute power function with complex exponent but real mantisse
\param[in] d real mantisse
\param[in] z complex exponent
\return d power z (\f$d^z\f$)
*/
inline nr_complex_t pow (const nr_double_t d, const nr_complex_t z)
{
    return exp (z * log (d));
}
/*!\brief Compute complex power function
\param[in] z1 complex mantisse
\param[in] z2 complex exponent
\return d power z (\f$z_1^{z_2}\f$)
*/
inline nr_complex_t pow (const nr_complex_t z1, const nr_complex_t z2)
{
    return exp (z2 * log (z1));
}
}
#endif

#ifndef HAVE_CXX_COMPLEX_SQRT
namespace std
{
nr_complex_t sqrt (const nr_complex_t);
}
#endif



///* -------------------------------------------------------
//                    Missing Operators
//   ------------------------------------------------------- */
//
///*! \brief Equality of two complex
//    \todo Why not inline
//    \note Like equality of double this test
//    is meaningless in finite precision
//    Use instead fabs(x-x0) < tol
//*/
//bool operator==(const nr_complex_t z1, const nr_complex_t z2)
//{
//    return (real (z1) == real (z2)) && (imag (z1) == imag (z2));
//}
///*! \brief Inequality of two complex
//    \note Like inequality of double this test
//    is meaningless in finite precision
//    Use instead fabs(x-x0) > tol
//*/
//bool operator!=(const nr_complex_t z1, const nr_complex_t z2)
//{
//    return (real (z1) != real (z2)) || (imag (z1) != imag (z2));
//}
///*! \brief Superior of equal
//*/
//bool operator>=(const nr_complex_t z1, const nr_complex_t z2)
//{
//    return norm (z1) >= norm (z2);
//}
///*! \brief Inferior of equal
//*/
//bool operator<=(const nr_complex_t z1, const nr_complex_t z2)
//{
//    return norm (z1) <= norm (z2);
//}
///*! \brief Superior
//*/
//bool operator>(const nr_complex_t z1, const nr_complex_t z2)
//{
//    return norm (z1) > norm (z2);
//}
///*! \brief Inferior
//*/
//bool operator<(const nr_complex_t z1, const nr_complex_t z2)
//{
//    return norm (z1) < norm (z2);
//}
///*! \brief Modulo
//*/
//nr_complex_t operator%(const nr_complex_t z1, const nr_complex_t z2)
//{
//    return z1 - z2 * floor (z1 / z2);
//}
///*! \brief Modulo
//*/
//nr_complex_t operator%(const nr_complex_t z1, const nr_double_t r2)
//{
//    return z1 - r2 * floor (z1 / r2);
//}
///*! \brief Modulo
//*/
//nr_complex_t operator%(const nr_double_t r1, const nr_complex_t z2)
//{
//    return r1 - z2 * floor (r1 / z2);
//}

#endif // __COMPLEX_MISSING_STD_H__
