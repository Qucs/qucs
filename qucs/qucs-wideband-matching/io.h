#ifndef IO_H
#define IO_H
#include "GRABIM.h"
#include <locale>
#include "MathOperations.h"
#include <complex>
#include <queue>
#include <fstream>
#include <QApplication>
#include <QClipboard>

using namespace std;
enum terminal {SOURCE, LOAD};

class IO
{
public:
    IO();
    int exportGNUplot(GRABIM_Result, string);
    int loadS1Pdata(std::string, terminal);
    int ResampleImpedances();
    vector<complex<double>> getSourceImpedance();
    vector<complex<double>> getLoadImpedance();
    vector<double> getFrequency();
    void set_constant_ZS_vs_freq(complex<double>);
    void set_constant_ZL_vs_freq(complex<double>);
    void set_matching_band(double, double);
  //  void setLocalOptimiser(nlopt::algorithm);
   // nlopt::algorithm getLocalOptimiser();
    int ExportQucsSchematic(GRABIM_Result);
    void PrintNetwork_StandardOutput(GRABIM_Result);
    void UseClipboard(bool);


private:
    // ZS and ZL are the source and load impedances, respectively whereas fS and fL indicates the frequencies where
    // ZS and ZL were sampled
    vector<complex<double>> ZS, ZL;
    vector<double>  fS, fL;

    vector<double> freq;//More often than not, ZS and ZL are sampled at different frecuencies, so it is necessary to have
    // common frequency vector for pairing ZS and ZL.


    double fmatching_min, fmatching_max;
    int getFreqIndex(double);

    vector<complex<double>> ZS_matching, ZL_matching;
    vector<double> f_matching;
    int setMatchingImpedances();
    double getS2PfreqScale(string line);
   // nlopt::algorithm LocalOptAlgo;

    int SchematicParser(GRABIM_Result, int &, string &, string &, string &);
    bool CreateSchematic(string, string, string, string);

    int Nsamples;//Impedance samples within matching band

    bool CopyToClipboard;
    string Num2String(int x);
    string Num2String(double x);

};

#endif // IO_H
