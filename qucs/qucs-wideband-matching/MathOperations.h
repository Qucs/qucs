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
 vector<double> ones(unsigned int);
 vector<double> operator * (double, vector<double>);
 vector<double> operator * (vector<double>, vector<double>);
 vector<double> operator + (double, vector<double>);
 vector<double> operator + (vector<double>, vector<double>);
 vector<double> operator - (double, vector<double>);
 vector<double> operator - (vector<double>, vector<double>);
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

#endif
