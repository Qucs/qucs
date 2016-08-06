/*
 * MathOperations.cpp - Math operations class implementation
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

#include "MathOperations.h"

// Given two double parameters, it generates a N-dim vector whose values are equally spaced
vector<double> linspace(double y_ini, double y_end, unsigned int N) {
    double step = (y_end - y_ini) / (N-1);
    vector<double> result(N);
    double acc=y_ini;
    for (vector<double>::iterator it = result.begin(); it != result.end(); ++it)
    {  
        *it = acc;
        acc+=step;
    }
    return result;
}

// Fills a complex vector with 1
vector<complex<double>> ones (vector<complex<double>> X)
{
 vector<complex<double>>x(X.size());
 for (vector<complex<double>>::iterator it = x.begin(); it != x.end(); ++it) *it=1;
 return x;
}

// Dot vector product
vector<complex<double>> Product(vector<complex<double>> A, complex<double>B)
{ 
  vector<complex<double>>result(A.size());
  vector<complex<double>>::iterator ita = A.begin();

  for (vector<complex<double>>::iterator itres = result.begin(); ita != A.end(); ++ita, ++itres) *itres=(*ita)*B;
  return result;
}

// Finds the minimum of a real vector
double min(vector<double> A)
{
   return *min_element(A.begin(), A.end());
}

// Finds the maximum of a real vector
double max(vector<double> A)
{
   return *max_element(A.begin(), A.end());
}

// Interpolates a complex vector using linear interpolation
vector<complex<double>>  interp(vector<double> f1, vector<complex<double>> Z, vector<double> f2)
{
   vector<double> realpart = getRealPart(Z);
   vector<double> imagpart = getImagPart(Z);
   vector<complex<double>> Z_(f2.size());
  
   //Check f2 \in f1
   if ( (min(f2) < min(f1)) || (max(f2) > max(f1)) )
   {
    cout << "ERROR: Cannot interporlate a sequence defined outside the original one" << endl;
   }
   else
   {
        double realpart_, imagpart_;
	unsigned int i1, i2, nslots, aux=0;
	double step_r, step_i, a=0;
	for (int ind = 0; ind < f1.size()-1; ind++)
	{
          i1 = closestIndex(f2, f1[ind]);
	  i2 = closestIndex(f2, f1[ind+1]);
	  nslots = i2-i1;

          step_r = (realpart[ind + 1] - realpart[ind])/nslots;
          step_i = (imagpart[ind + 1] - imagpart[ind])/nslots;
	
	  for (unsigned int i = a; i <= nslots; i++, aux++)
	  {
	    realpart_ = realpart[ind] + i*step_r;
	    imagpart_ = imagpart[ind] + i*step_r;
            Z_[aux] = complex<double>(realpart_, imagpart_);
          }
          if (a==0)a=1;

	}
   }
return Z_;

}

// Finds the closest vector component to a given value
unsigned int closestIndex(std::vector<double> vec, double value)
{
 vector<double>::iterator it;
 it = std::lower_bound(vec.begin(), vec.end(), value);
 
 int index = it-vec.begin();
 if (index < 0) index = 0;
 if (index > vec.size()-1) index = vec.size()-1;
 return index;
}

// Returns the real part of a complex vector
vector<double> getRealPart(vector<complex<double> > Z)
{
  vector<double> realpart(Z.size());
  vector<double>::iterator itr = realpart.begin();
  for (vector<complex<double> >::iterator it = Z.begin(); it != Z.end(); ++it, ++itr) *itr=it->real();
  return realpart;
}

// Returns the imaginary part of a complex vector
vector<double> getImagPart(vector<complex<double> > Z)
{
  vector<double> imagpart(Z.size());
  vector<double>::iterator itr = imagpart.begin();
  for (vector<complex<double> >::iterator it = Z.begin(); it != Z.end(); ++it, ++itr) *itr=it->imag();
  return imagpart;
}

// Conjugates a complex vector
complex<double> conj(complex<double> Z)
{
  return complex<double>(real(Z), -imag(Z));
}


// Creates a N-dim ones vector
vector<double> ones(unsigned int N)
{
  vector<double> V(N);
  for (unsigned int i = 0; i < N; i++) V[i] = 1;
  return V;
}

// double * vector operation
vector<double> operator * (double d, vector<double> Z)
{
  vector<double> V(Z.size());
  for (unsigned int i=0; i< Z.size(); i++) V[i] = d*Z[i];
  return V;
}

// double + vector operation
vector<double> operator + (double u, vector<double> V)
{
  vector<double> V_(V.size());
  for (unsigned int i=0; i< V.size(); i++) V_[i] = u+V[i];
  return V_;
}

// double - vector operation
vector<double> operator - (double u, vector<double> V)
{
  vector<double> V_(V.size());
  for (unsigned int i=0; i< V.size(); i++) V_[i] = u-V[i];
  return V_;
}

// vector + vector operation
vector<double> operator + (vector<double> U, vector<double> V)
{
  vector<double> V_(U.size());
  for (unsigned int i=0; i< U.size(); i++) V_[i] = U[i]+V[i];
  return V_;
}

// vector - vector operation
vector<double> operator - (vector<double> U, vector<double> V)
{
  vector<double> V_(U.size());
  for (unsigned int i=0; i< U.size(); i++) V_[i] = U[i]-V[i];
  return V_;
}

// vector * vector dot operation
vector<double> operator * (vector<double> U, vector<double> V)
{
  vector<double> V_(U.size());
  for (unsigned int i=0; i< U.size(); i++) V_[i] = U[i]*V[i];
  return V_;
}

// Prints the content of a real vector in the standard output
void print(vector<double> V)
{
  for (unsigned int i = 0; i < V.size(); i++) cout << V[i] << " ";
  cout << endl;
}

// Prints the content of a complex vector in the standard output
void print(vector<complex<double>> V)
{
  for (unsigned int i = 0; i < V.size(); i++) cout << real(V[i]) << "+j" << imag(V[i]) << endl;
}

// Creates a real vector by concatenating the input vector, V, N times 
vector<double> repvec(vector<double> V, unsigned int N)
{
  unsigned int aux=0;
  vector<double> V_(V.size()*N);
  for (unsigned int i = 0; i < N; i++)
     for (unsigned int c = 0; c < V.size(); c++, aux++)V_[aux] = V[c];
  return V_;
}

// Replace the content of the r-th row of C with V
void setRow(vector<vector<double>> & C, unsigned int r, vector<double> V)
{
   for (unsigned int i = 0; i < C[0].size(); i++) C[r][i] = V[i];
}

// Calculates the magnitude of a complex vector
vector<double> abs(vector<complex<double>> Z)
{
  vector<double> A(Z.size());
  for (unsigned int u=0; u< Z.size(); u++) A[u] = abs(Z[u]);
  return A;
}

// Calculates the mean value of a vector
double mean(vector<double> V)
{
  double acc=0;
  for (unsigned int u=0; u< V.size(); u++) acc += V[u];
  return acc/V.size();
}

// complex vector + complex vector operation
vector<complex<double>> operator + (vector<complex<double>> U, vector<complex<double>> V)
{
  vector<complex<double>> Z(U.size());
  for (unsigned int i=0; i<U.size();i++) Z[i] = U[i] + V[i];
  return Z;
}

// Gets the real part of a complex vector
vector<double> real (vector<complex<double>> Z)
{
  vector<double> V(Z.size());
  for (unsigned int i = 0; i < Z.size(); i++) V[i] = real(Z[i]);
  return V;
}

// Gets the c-th column from C
vector<double> getCol(vector<vector<double>> C, unsigned int c)
{
  unsigned int nrows = C.size();
  vector<double> V(nrows);
  for (unsigned int i = 0; i < nrows; i++)V[i] = C[i][c];
  return V;
}

// Gets the c-th row from C
vector<double> getRow(vector<vector<double>> C, unsigned int c)
{
  unsigned int ncols = C[0].size();
  vector<double> V(ncols);
  for (unsigned int i = 0; i < ncols; i++)V[i] = C[c][i];
  return V;
}

// Find the minumu value in V
unsigned int findMin(std::vector<double> V)
{
    unsigned int index=0;
    double min_=1e20;
    for (unsigned int i = 0; i < V.size(); i++)
    {
        if (V[i] < min_) 
        {
          min_=V[i];
          index = i;
        }
    }
  return index;
}

vector<complex<double>> SubVector(vector<complex<double>> Z, unsigned int i1, unsigned int i2)
{
  vector<complex<double>> Z_(i2-i1+1);
  unsigned int aux=0;
  for (unsigned int i = i1; i<=i2;i++, aux++)Z_[aux] = Z[i]; 
  return Z_;
}

vector<double> SubVector(vector<double> Z, unsigned int i1, unsigned int i2)
{
  vector<double> Z_ (i2-i1+1);
  unsigned int aux=0;
  for (unsigned int i = i1; i<=i2;i++, aux++)Z_[aux] = Z[i]; 
  return Z_;
}

// Calculates the centroid of an array of real vectors
vector<double> centroid(vector<vector<double>> X)
{
 vector<double> c(X[0].size());
 unsigned int N = X.size();//Number of vectors;
  for (unsigned int i = 0; i < X[0].size(); i++)//Calculates the mean of each component
  {
     c[i] = 0;
     for (unsigned int j = 0; j < N; j++)c[i] += X[j][i];
     c[i]/=N;
  }
 return c;
}

// Calculates the logarithm of a vector
vector<double> log(vector<double> V)
{
   for (unsigned int i = 0; i < V.size(); i++) V[i] = log10(V[i]);
   return V;
}



