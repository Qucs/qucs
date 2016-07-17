#ifndef SPARENGINE_H
#define SPARENGINE_H

#include <string.h>
#include <set>
#include "MathOperations.h"

using namespace std;



class SparEngine
{
    const double c0 = 299792458;//Speed of light (m/s)
    std::set<string> preTopoList;
public:
    SparEngine();
    Mat getSparams(vector<double>, complex<double>, complex<double>, double, std::string);
    Mat getABCDmatrix(vector<double>, double, std::string);
    Mat PreComputedABCD(vector<double>, double, std::string);
};

#endif // SPARENGINE_H
