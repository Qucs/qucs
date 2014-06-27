/*
 * real.h - some real valued function definitions
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __REAL_H__
#define __REAL_H__

#include <cmath>

namespace qucs {

// complex manipulations
nr_double_t   real (const nr_double_t);
nr_double_t   imag (const nr_double_t);
nr_double_t   norm (const nr_double_t);
nr_double_t   conj (const nr_double_t);
nr_double_t   abs (const nr_double_t);

/*!\brief Square a value

   \param[in] r Real number
   \return \f$x^2\f$
*/
inline nr_double_t  sqr (const nr_double_t r) {
  return r * r;
}

/*!\brief Quartic function

   \param[in] r Real number
   \return \f$x^4\f$
*/
inline nr_double_t  quadr (const nr_double_t r) {
  return r * r * r * r;
}

// extra math functions
nr_double_t limexp (const nr_double_t);
nr_double_t signum (const nr_double_t);
nr_double_t   sign (const nr_double_t);
nr_double_t xhypot (const nr_double_t, const nr_double_t);
nr_double_t   sinc (const nr_double_t);
nr_double_t    fix (const nr_double_t);
nr_double_t   step (const nr_double_t);

// functions introduced in C++ 11
nr_double_t  round (const nr_double_t);
nr_double_t  trunc (const nr_double_t);
nr_double_t  acosh (const nr_double_t);
nr_double_t  asinh (const nr_double_t);


inline nr_double_t coth (const nr_double_t d) {
  return 1.0 / std::tanh (d);
}

// more extra math functions
unsigned int factorial (unsigned int);


// std functions, these are provided here primarily for the use in the
// macros in evaluate.cpp, where it is much easier if there is a version
// in the qucs namespace. In general, throughout the sources, the std
// function should be used directly
inline nr_double_t sqrt (const nr_double_t d)
{
    return std::sqrt (d);
}

inline nr_double_t cos (const nr_double_t d)
{
    return std::cos (d);
}

inline nr_double_t cosh (const nr_double_t d)
{
    return std::cosh (d);
}

inline nr_double_t sech (const nr_double_t d)
{
    return  (1.0 / std::cosh (d));
}

inline nr_double_t cosech (const nr_double_t d)
{
    return  (1.0 / std::sinh (d));
}

inline nr_double_t sin (const nr_double_t d)
{
    return std::sin (d);
}

inline nr_double_t sinh (const nr_double_t d)
{
    return std::sinh (d);
}

inline nr_double_t tan (const nr_double_t d)
{
    return std::tan (d);
}

inline nr_double_t tanh (const nr_double_t d)
{
    return std::tanh (d);
}

inline nr_double_t log (const nr_double_t d)
{
    return std::log (d);
}

inline nr_double_t log10 (const nr_double_t d)
{
    return std::log10 (d);
}

inline nr_double_t exp (const nr_double_t d)
{
    return std::exp (d);
}

} // namespace qucs

// \bug ugly hack, inject missing functions in std namespace
// should check for availability
#if (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && __MAC_OS_X_VERSION_MAX_ALLOWED <= 1060)
namespace std {
  static inline nr_double_t round( nr_double_t x) {
     return qucs::round(x);
  }
  static inline nr_double_t trunc( nr_double_t x) {
     return qucs::trunc(x);
  }
  static inline nr_double_t erf( nr_double_t x) {
     return erf(x);
  }
  static inline nr_double_t acosh( nr_double_t x) {
     return qucs::acosh(x);
  }
  static inline nr_double_t asinh( nr_double_t x) {
     return qucs::asinh(x);
  }
} // namespace std
#endif


#endif /* __REAL_H__ */
