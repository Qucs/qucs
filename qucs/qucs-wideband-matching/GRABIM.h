#ifndef GRABIM_H
#define GRABIM_H

#include "sparengine.h"
#include <fstream>
#include <queue>
#include <list>

using namespace std;

enum ObjectiveFunction {NINF_S11dB, NINF_POWERTRANS};

typedef struct GRABIM_Result {
    vector<double> x_grid_search;
    double grid_val;
    vector<double> x_nlopt;
    double nlopt_val;
    vector<complex<double>> ZS, ZL;
    vector<double> freq;
    vector<complex<double>> S11_gridsearch, S21_gridsearch, S12_gridsearch, S22_gridsearch;
    vector<complex<double>> S11_nlopt, S21_nlopt, S12_nlopt, S22_nlopt;
    std::string topology;
    std::string source_path;
    std::string load_path;
    std::string QucsVersion;

} GRABIM_Result;


//Reference:
// [1] Broadband direct-coupled and RF matching networks. Thomas R. Cuthbert, 1999
// [2] Broadband impedance matching - Fast and simple. Thomas R. Cuthbert. RF design. 1994
class GRABIM
{
    const double c0 = 299792458;//Speed of light (m/s)

public:
    GRABIM();

    int SetSourceImpedance(vector<complex<double>>);
    int SetLoadImpedance(vector<complex<double>>);
    int SetFrequency(vector<double>);

    int SetInitialPivot(vector<double>);
    vector<double> GetInitialPivot();
    int SetMatchingBand(double, double);
    int SetTopology(std::string);
    GRABIM_Result RunGRABIM();
    int SetMaxIterGridSearch(int);
    int SetThreshold(double);
    double GetThreshold();
    double CandidateEval(vector<double>);
    //int SetNLoptAlg(nlopt::algorithm);
   // nlopt::algorithm GetNLoptAlg();
    int SetObjectiveFunction(ObjectiveFunction);
    ObjectiveFunction GetObjectiveFunction();

    void setTopoScript(std::string);
    void setSearchMode(int);
    void SimplifyNetwork(bool);

private:
    vector<double> GridSearch();
    int ResampleImpedances();
    vector<vector<double>> GeneratingMatrix(unsigned int);
    double CalcInvPowerTransfer(Mat, complex<double>, complex<double>);
    vector<double> InspectCandidate(vector<double>);
    vector<double> x_ini;

    vector<double> freq;
    vector<complex<double>> ZS, ZL;

    std::string topology;
    unsigned int Grid_MaxIter;
    double MatchingThreshold;
   // nlopt::algorithm NLoptAlgo;
    ObjectiveFunction ObjFun;
    int search_mode;

    string tolower(string str);
    string RemoveBlankSpaces(string line);

    std::string TopoScript_path;

    int GridSearch_DifferentTopologies(vector<vector<double>> &, vector<std::string> &);
    void AutoSetInitialPivot();
    void removeElement(vector<double> & xk, unsigned int pos, unsigned int postopo);
    void CheckDim(vector<vector<double>> &, vector<vector<double>> &, vector<vector<double>> &, unsigned int);
    void CheckNetwork(vector<double> &, unsigned int, unsigned int);
    
    //Optimisation code
    vector<double> NelderMead(vector<double>);
    void NM_initializeSimplex(vector<vector<double>> &, vector<double> &, vector<double>);
    void EvaluateSimplex(vector<vector<double>>, vector<double> &);

    list<string> TopoList;
    list<string> TagList;

    bool simplify_network;
};

#endif // GRABIM_H
