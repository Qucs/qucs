/*
 * vector.cpp - vector class implementation
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: vector.cpp,v 1.4 2004/04/18 18:37:16 margraf Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "complex.h"
#include "object.h"
#include "logging.h"
#include "strlist.h"
#include "vector.h"

// Constructor creates an unnamed instance of the vector class.
vector::vector () : object () {
  capacity = size = 0;
  data = NULL;
  dependencies = NULL;
  origin = NULL;
}

// Constructor creates an named instance of the vector class.
vector::vector (char * n) : object (n) {
  capacity = size = 0;
  data = NULL;
  dependencies = NULL;
  origin = NULL;
}

/* The copy constructor creates a new instance based on the given
   vector object. */
vector::vector (const vector & v) : object (v) {
  size = v.size;
  capacity = v.capacity;
  data = (complex *) malloc (sizeof (complex) * capacity);
  memcpy (data, v.data, sizeof (complex) * size);
  dependencies = new strlist (*v.dependencies);
  origin = v.origin ? strdup (v.origin) : NULL;
}

// Destructor deletes a vector object.
vector::~vector () {
  if (data) free (data);
  if (dependencies) delete dependencies;
  if (origin) free (origin);
}

/* The function appends a new complex data item to the end of the
   vector and ensures that the vector can hold the increasing number
   of data items. */
void vector::add (complex c) {
  if (data == NULL) {
    size = 0; capacity = 64;
    data = (complex *) malloc (sizeof (complex) * capacity);
  }
  else if (size >= capacity) {
    capacity *= 2;
    data = (complex *) realloc (data, sizeof (complex) * capacity);
  }
  data[size++] = c;
}

/* This function appends the given vector to the vector. */
void vector::add (vector * v) {
  if (data == NULL) {
    size = 0; capacity = v->getSize ();
    data = (complex *) malloc (sizeof (complex) * capacity);
  }
  else if (size + v->getSize () > capacity) {
    capacity += v->getSize ();
    data = (complex *) realloc (data, sizeof (complex) * capacity);
  }
  for (int i = 0; i < v->getSize (); i++) data[size++] = v->get (i);
}

// Returns the complex data item at the given position.
complex vector::get (int i) {
  return data[i];
}

void vector::set (const complex z, int i) {
  data[i] = z;
}

// The function returns the current size of the vector.
int vector::getSize (void) {
  return size;
}

int vector::checkSizes (vector & v1, vector & v2) {
  if (v1.getSize () != v2.getSize ()) {
    logprint (LOG_ERROR, "vector '%s' and '%s' have different sizes\n",
	      v1.getName (), v2.getName ());
    return 0;
  }
  return 1;
}

vector * abs (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(abs(v.get(i)), i);
  return result;
}

vector * norm (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(norm(v.get(i)), i);
  return result;
}

vector * arg (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(arg(v.get(i)), i);
  return result;
}

vector * real (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(real(v.get(i)), i);
  return result;
}

vector * imag (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(imag(v.get(i)), i);
  return result;
}

vector * conj (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(conj(v.get(i)), i);
  return result;
}

vector * dB (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(10.0*log10(norm(v.get(i))), i);
  return result;
}

vector * sqrt (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(sqrt(v.get(i)), i);
  return result;
}

vector * exp (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(exp(v.get(i)), i);
  return result;
}

vector * ln (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(ln(v.get(i)), i);
  return result;
}

vector * log10 (vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(log10(v.get(i)), i);
  return result;
}

vector * modulo (vector & v, const complex z) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(modulo(v.get(i), z), i);
  return result;
}

vector * modulo (const complex z, vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(modulo(z, v.get(i)), i);
  return result;
}

vector * modulo (vector & v1, vector & v2) {
  if (v1.checkSizes (v1, v2)) {
    vector * result = new vector (v1);
    for (int i = 0; i < v1.getSize(); i++) result->set(modulo(v1.get(i), v2.get(i)), i);
    return result;
  }
  return &v1;
}

vector * pow (vector & v, const complex z) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(pow(v.get(i), z), i);
  return result;
}

vector * pow (const complex z, vector & v) {
  vector * result = new vector (v);
  for (int i = 0; i < v.getSize(); i++) result->set(pow(z, v.get(i)), i);
  return result;
}

vector * pow (vector & v1, vector & v2) {
  if (v1.checkSizes (v1, v2)) {
    vector * result = new vector (v1);
    for (int i = 0; i < v1.getSize(); i++) result->set(pow(v1.get(i), v2.get(i)), i);
    return result;
  }
  return &v1;
}

vector& vector::operator=(const complex c) {
  for (int i = 0; i < getSize(); i++) data[i] = c;
  return *this;
}

vector& vector::operator+=(vector & v) {
  if (checkSizes (*this, v)) {
    for (int i = 0; i < getSize (); i++) data[i] += v.get (i);
  }
  return *this;
}

vector& vector::operator+=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] += c;
  return *this;
}

vector * operator+(vector & v1, vector & v2) {
  if (v1.checkSizes (v1, v2)) {
    vector * result = new vector (v1);
    *result += v2;
    return result;
  }
  return NULL;
}

vector * operator+(vector & v, const complex c) {
  vector * result = new vector (v);
  *result += c;
  return result;
}

vector& vector::operator-() {
  for (int i = 0; i < getSize (); i++) data[i] *= -1;
  return *this;
}

vector& vector::operator-=(vector & v) {
  if (checkSizes (*this, v)) {
    for (int i = 0; i < getSize (); i++) data[i] -= v.get (i);
  }
  return *this;
}

vector& vector::operator-=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] -= c;
  return *this;
}

vector * operator-(vector & v1, vector & v2) {
  if (v1.checkSizes (v1, v2)) {
    vector * result = new vector (v1);
    *result -= v2;
    return result;
  }
  return NULL;
}

vector * operator-(vector & v, const complex c) {
  vector * result = new vector (v);
  *result -= c;
  return result;
}

vector * operator-(const complex c, vector & v) {
  vector * result = new vector (v);
  *result *= -1.0;
  *result += c;
  return result;
}

vector& vector::operator*=(vector & v) {
  if (checkSizes (*this, v)) {
    for (int i = 0; i < getSize (); i++) data[i] *= v.get (i);
  }
  return *this;
}

vector& vector::operator*=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] *= c;
  return *this;
}

vector * operator*(vector & v1, vector & v2) {
  if (v1.checkSizes (v1, v2)) {
    vector * result = new vector (v1);
    *result *= v2;
    return result;
  }
  return NULL;
}

vector * operator*(vector & v, const complex c) {
  vector * result = new vector (v);
  *result *= c;
  return result;
}

vector& vector::operator/=(vector & v) {
  if (checkSizes (*this, v)) {
    for (int i = 0; i < getSize (); i++) data[i] /= v.get (i);
  }
  return *this;
}

vector& vector::operator/=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] /= c;
  return *this;
}

vector * operator/(vector & v1, vector & v2) {
  if (v1.checkSizes (v1, v2)) {
    vector * result = new vector (v1);
    *result /= v2;
    return result;
  }
  return NULL;
}

vector * operator/(vector & v, const complex c) {
  vector * result = new vector (v);
  *result /= c;
  return result;
}

vector * operator/(const complex c, vector & v) {
  vector * result = new vector (v);
  *result  = c;
  *result /= v;
  return result;
}

/* This function reverses the order of the data list. */
void vector::reverse (void) {
  complex * buffer = (complex *) malloc (sizeof (complex) * size);
  for (int i = 0; i < size; i++)
    buffer[i] = data[size - 1 - i];
  free (data);
  data = buffer;
  capacity = size;
}

// Sets the origin (the analysis) of the vector.
void vector::setOrigin (char * n) {
  if (origin) free (origin);
  origin = n ? strdup (n) : NULL;
}

// Returns the origin (the analysis) of the vector.
char * vector::getOrigin (void) {
  return origin;
}
