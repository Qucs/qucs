/*
 * MathOperations.h - Math operations class definition
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



#ifndef MATHOPERATIONS_H
#define MATHOPERATIONS_H

#include <vector>
#include <complex>
#include <algorithm>
#include <iostream>
#include "mat.h"
using namespace std;
 const double pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798;
 vector<double> linspace(double, double, unsigned int);
 vector<complex<double>> ones(vector<complex<double>>);
 vector<complex<double>> Product(vector<complex<double>>, complex<double>);
 double min(vector<double>);
 double max(vector<double>);
 vector<double> abs(vector<complex<double>>);
 double mean(vector<double>);
 vector<double> real(vector<complex<double>>);
 vector<complex<double>>  interp(vector<double>, vector<complex<double>>, vector<double>);
 unsigned int closestIndex(std::vector<double>, double);
 unsigned int findMin(std::vector<double>);
 vector<double> getRealPart(vector<complex<double>>);
 vector<double> getImagPart(vector<complex<double>>);
 complex<double> conj(complex<double>);
 vector<complex<double>> conj(vector<complex<double>>);
 vector<double> sqrt(vector<double>);
 vector<double> ones(unsigned int);
 vector<double> operator * (double, vector<double>);
 vector<double> operator * (vector<double>, vector<double>);
 vector<complex<double>> operator * (double, vector<complex<double>>);
 vector<complex<double>> operator * (vector<complex<double>>, vector<complex<double>>);
 vector<complex<double>> operator / (vector<double>, vector<complex<double>>);
 vector<complex<double>> operator / (vector<complex<double>>, vector<complex<double>>);
 complex<double> operator / (double, complex<double>);
 vector<double> operator + (double, vector<double>);
 vector<double> operator + (vector<double>, vector<double>);
 vector<complex<double>> operator + (vector<double>, vector<complex<double>>);
 vector<double> operator - (double, vector<double>);
 vector<double> operator - (vector<double>, vector<double>);
 vector<complex<double>> operator - (vector<double>, vector<complex<double>>);
 vector<complex<double>> operator - (vector<complex<double>>, vector<complex<double>>);
 vector<complex<double>> operator + (vector<complex<double>>, vector<complex<double>>);
 void print(vector<double>);
 void print(vector<complex<double>>);
 vector<double> repvec(vector<double>, unsigned int);
 void setRow(vector<vector<double>> &, unsigned int,vector<double>);
 vector<double> getCol(vector<vector<double>>, unsigned int);
 vector<double> getRow(vector<vector<double>>, unsigned int);
 vector<complex<double>> SubVector(vector<complex<double>>, unsigned int, unsigned int);
 vector<double> SubVector(vector<double>, unsigned int, unsigned int);
 vector<double> centroid(vector<vector<double>>);
 vector<double> log(vector<double>);

#endif
