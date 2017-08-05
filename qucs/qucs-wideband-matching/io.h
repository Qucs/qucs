/*
 * io.h - Input/output operations class definition
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
    ~IO();
    int exportGNUplot(GRABIM_Result, string, bool);
    int loadS1Pdata(std::string, terminal);
    int loadS2Pdata(std::string);
    int ResampleImpedances();
    vector<double> getFrequency();
    void set_constant_ZS_vs_freq(complex<double>);
    void set_constant_ZL_vs_freq(complex<double>);
    void set_matching_band(double, double);
    int ExportQucsSchematic(TwoPortCircuit, string);
    void PrintNetwork_StandardOutput(GRABIM_Result);
    void UseClipboard(bool);
    bool UseGNUplot;
    string tmp_path;//Path to a temporary directory for data dumping
    bool Two_Port_Matching;

    // ZS and ZL are the source and load impedances, respectively whereas fS and fL indicates the frequencies where
    // ZS and ZL were sampled
    vector<complex<double>> ZS, ZL;
    vector<complex<double>> Zin_maxg, Zout_maxg;//Optimal Zin and Zout for achieving maximum gain on a two-port device
    vector<double> freq;//More often than not, ZS and ZL are sampled at different frecuencies, so it is necessary to have
    // common frequency vector for pairing ZS and ZL.
    MatchingData getMatchingData();
    AmpData AmpS2P;

private:
    vector<double>  fS, fL, fAMP;//Frequencies at which the input source, load and amplifier data are sampled

    double fmatching_min, fmatching_max;
    int getFreqIndex(double);

    vector<double> f_matching;
    double getS2PfreqScale(string line);

    int SchematicParser(TwoPortCircuit, int &, string &, string &, string &);
    bool CreateSchematic(string, string, string, string, string);
    void CreateLadder(GRABIM_Result, string &, string &, int &);

    int Nsamples;//Impedance samples within matching band

    bool CopyToClipboard;
    string Num2String(int x);
    string Num2String(double x);
    void generateGNUplotScript(string, string, bool);
    void generateConstant_s1p(string, complex<double>);

};

#endif // IO_H
