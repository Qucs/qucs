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
