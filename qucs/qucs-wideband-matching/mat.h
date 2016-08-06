/*
 * mat.h - Matrix class definition
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


#ifndef MAT_H
#define MAT_H

#include <complex>
#include <vector>
#include <iostream>
using namespace std;

typedef vector<complex<double>> vec;

class Mat
{
private:
    complex<double> * data;

public:
    Mat(unsigned int, unsigned int);

    int rows;
    int cols;

    complex<double>  operator () (unsigned int r, unsigned int c) const { return data[r * cols + c]; }
    complex<double> & operator () (unsigned int r, unsigned int c) { return data[r * cols + c]; }

    void print();
    friend Mat operator * (complex<double>, Mat);
    friend Mat operator * (Mat, Mat);

    vec getRow(unsigned int);
    vec getCol(unsigned int);
    void eye();
    void ones();
    vector<double> getRowReal(unsigned int);
    vector<double> getRowImag(unsigned int);
    vector<double> getColReal(unsigned int);
    vector<double> getColImag(unsigned int);
    
};

#endif // SPARENGINE_H
