/*
 * mat.cpp - Matrix class implementation
 *
 * copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 *
 */



#include <mat.h>
Mat::Mat()
{ 
  rows = cols = 2;
  data = vec(4);
}


Mat::~Mat()
{
}

//Fills a matrix with 1
void Mat::ones()
{
  for (unsigned int r = 0; r < rows; r++)
     for (unsigned int c = 0; c < cols; c++) data[r * cols + c] = 1;
}

//Turns the matrix into the identity
void Mat::eye()
{
  for (unsigned int r = 0; r < rows; r++)
     for (unsigned int c = 0; c < cols; c++) (r==c) ? data[r * cols + c] = 1 : data[r * cols + c] = 0;
}

// mat * complex operation
Mat operator * (complex<double> z, Mat a) {
  Mat A;
  for (unsigned int r = 0; r < a.rows; r++)
      for (unsigned int c = 0; c<a.cols; c++) A(r, c) = z*a(r, c);
  return A;
}

// Scalar vectorial product
complex<double> ScalarProduct(vec U, vec V)
{
 if (V.size() != U.size()) return -1e30;
 complex<double> acc=0;
 for (unsigned int i = 0; i < U.size(); i++) acc += U[i]*V[i];
 return acc;
}


// Inner matrix product
Mat operator * (Mat a, Mat b) {

  Mat Res;
  for (unsigned int r = 0; r < Res.rows; r++)
     for (unsigned int c = 0; c < Res.cols; c++) Res(r, c) = ScalarProduct(a.getRow(r), b.getCol(c));

  return Res;
}

// Gets row r
vec Mat::getRow(unsigned int r)
{
   vec V(cols);
   for (unsigned int i = 0; i < cols; i++) V[i] = data[r * cols + i];
   return V;
}

//Gets the real part of a row
vector<double> Mat::getRowReal(unsigned int r)
{
   vector<double> V(cols);
   for (unsigned int i = 0; i < cols; i++) V[i] = real(data[r * cols + i]);
   return V;
}

//Gets the imaginary part of a row
vector<double> Mat::getRowImag(unsigned int r)
{
   vector<double> V(cols);
   for (unsigned int i = 0; i < cols; i++) V[i] = imag(data[r * cols + i]);
   return V;
}

// Gets row c
vec Mat::getCol(unsigned int c)
{
   vec V(rows);
   for (unsigned int i = 0; i < rows; i++) V[i] = data[i * cols + c];
   return V;
}

//Gets the real part of a column
vector<double> Mat::getColReal(unsigned int c)
{
   vector<double> V(rows);
   for (unsigned int i = 0; i < rows; i++) V[i] = real(data[i * cols + c]);
   return V;
}

//Gets the imaginary part of a columns
vector<double> Mat::getColImag(unsigned int c)
{
   vector<double> V(rows);
   for (unsigned int i = 0; i < rows; i++) V[i] = imag(data[i * cols + c]);
   return V;
}

// Prints the matrix content in the standard output
void Mat::print ()
{
  for (unsigned int r = 0; r < rows; r++)
  {
     for (unsigned int c = 0; c < cols; c++)
     {
         cout << data[r * cols + c] << " ";
     }
     cout << endl;
  }
}

