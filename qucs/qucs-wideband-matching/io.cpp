/*
 * io.cpp - Input/output operations class implementation
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




#include "io.h"

IO::IO()
{
    Nsamples = 30;
    fmatching_min = -1;
    fmatching_max = -1;
    CopyToClipboard=true;
}

IO::~IO()
{}

// Converts std::string to double
double string_to_double( const std::string& s )
{
    std::istringstream i(s);
    double x;
    if (!(i >> x))
        return 0;
    return x;
}


//Export data according to the GNUplot data file format
int IO::exportGNUplot(GRABIM_Result Res, string filepath)
{
    ofstream GNUplotExport;
    GNUplotExport.open (filepath, std::ofstream::out);
    if(!GNUplotExport.is_open()) return -1;
    GNUplotExport << "#GRABIM data" << endl;
    GNUplotExport << "#freq S11(Grid search) S11(Local optimiser) S21(Grid search) S21(Local optimiser) " << endl;
    for (unsigned int i = 0; i < Res.freq.size(); i++)
    {//Writes frequency S11_grid[db] S11_optimiser[db] and S21[dB]
        GNUplotExport << Res.freq.at(i)*1e-9 << " " << 20*log10(abs(Res.S11_gridsearch.at(i))) << " "
                      << 20*log10(abs(Res.S21_gridsearch.at(i))) << " " << 20*log10(abs(Res.S11_nlopt.at(i)))
                      << " " << 20*log10(abs(Res.S21_nlopt.at(i))) << " " << real(Res.S11_nlopt.at(i))
                      << " " << imag(Res.S11_nlopt.at(i)) <<endl;
    }
    GNUplotExport.close();

    return 0;
}


// Extends std::tolower(int c) capability to st::string arguments
string tolower(string str)
{
    char c;
    for (unsigned int i =0; i < str.length();i++)
    {
        c=str.at(i);
        str.at(i) = tolower(c);
    }
    return str;
}

//Removes consecutive blank spaces in a std::string
string RemoveBlankSpaces(string line)
{
    //Remove consecutive repeated blank spaces and space at the beginning
    //Sometimes, the fields may be separated by \t...
    for (unsigned int i = 0; i< line.length(); i++)
    {
        if (i == 0)//Remove first space
        {
            if((!line.substr(0,1).compare(" "))||(!line.substr(0,1).compare("\t")))
            {
                line.erase(0, 1);
                i--;
            }
            continue;
        }
        if (((!line.substr(i-1, 1).compare(" "))||(!line.substr(i-1,1).compare("\t")))&&((!line.substr(i, 1).compare(" "))||(!line.substr(i,1).compare("\t"))))
        {
            line.erase(i, 1);
            i--;
        }
    }
    return line;
}



// Loads impedance data from a s1p file
int IO::loadS1Pdata(std::string filepath, terminal Port)
{
    std::ifstream s2pfile(filepath.c_str());//Tries to open the data file.
    if(!s2pfile.is_open())//The data file cannot be opened => error
    {
        return -1;
    }

    std::string line;
    double freq_scale = 1;
    double Zref = 50;

    std::getline(s2pfile, line);
    while(line.compare(0, 1, "#"))//Looking for # field
    {
        std::getline(s2pfile, line);
    }

    line = tolower(line);
    freq_scale = getS2PfreqScale(line);


    //Get the impedance at which the S params were measured

    int Rindex = line.find_last_of("r");
    Rindex = line.find_first_not_of(" ", Rindex);
    Zref = atof(line.substr(Rindex+1).c_str());

    bool is_indB = (line.find("db") != string::npos);
    bool RI = (line.find("ma") == string::npos);
    bool isS_par = (line.find(" s ") != string::npos);
    bool isZ_par = (line.find(" z ") != string::npos);


    while( getline(s2pfile, line) )
    {//Looking for the start of the raw data

        line = RemoveBlankSpaces(line);

        if ((!line.compare(0,1, "!"))|| (line.length() == 1)) continue;
        else break;


    }

    //DATA beginning.
    //At this point, the number of frequency samples is not known, so it's better to
    //push data into queues and then arrange it into armadillo structures
    std::queue <double> frequency, S11M, S11A;
    unsigned int qsize=0;

    do
    {
        line = RemoveBlankSpaces(line);
        if (line.empty()|| (line.length()==1))break;
        if (line.at(0) == '!') break;//Comment

        //Frequency
        int index = line.find_first_of(" ");
        if (index == -1)index = line.find_first_of("\t");
        frequency.push(string_to_double(line.substr(0,index)));
        line.erase(0, index+1);


        index = line.find_first_of(" ");
        if (index == -1)index = line.find_first_of("\t");
        S11M.push(string_to_double(line.substr(0,index)));
        line.erase(0, index+1);

        index = line.find_first_of(" ");
        if (index == -1)index = line.find_first_of("\t");
        S11A.push(string_to_double(line.substr(0,index)));

        qsize++;
    }while (std::getline(s2pfile, line));
    s2pfile.close();
    vector<double> freq(qsize);
    vector<complex<double>> S(qsize);
    vector<complex<double>> Z(qsize);

    double phi, S11m, S11a;
    for (unsigned int i = 0; i < qsize; i++)
    {
        freq[i] = freq_scale*frequency.front();
        frequency.pop();

        S11m = S11M.front();
        S11a = S11A.front();
        S11M.pop();
        S11A.pop();
        if (is_indB) S11m = pow(10, .05*S11m);
        phi = (pi/180)*S11a;

        if (RI)
        {
            S[i] = complex<double>(S11m, S11a);
        }
        else
        {
            S[i] = complex<double>(S11m,0)*complex<double>(cos(phi), sin(phi));
        }

        if (isZ_par)
        {//The data file contains impedance data
            Z[i] =  S[i];
        }
        if (isS_par)
        {//The data file contains s1p data
            Z[i] = Zref*((1.+S[i])/(1.-S[i]));//Z
        }

    }
    vector<string> candidates;
    if (Port == SOURCE)//Set source port properties
    {
        fS = freq;
        ZS = Z;
        if (!ZL.empty())ResampleImpedances();
    }

    if (Port == LOAD)//Set load port properties
    {
        fL = freq;
        ZL = Z;
        if (!ZS.empty())ResampleImpedances();
    }
    return 0;
}


// Load and source impedances may be sampled at different frequencies. It is essential to resample them
// using the same frequency basis. This requires interpolation of complex data. It would be desirable to use
// spline or cubic interpolation, but it seems that they are not implemented in Armadillo
int IO::ResampleImpedances()
{
    bool ready = false;
    if (fmatching_min == -1) return 0;

    //Check whether the vectors are already resampled or not
    if ((ZS.size() == ZL.size()) && (ZS.size() == freq.size()))
    {
        return 0;
    }

    double fmin, fmax;
    /*We have to discern the following cases
    1: ZS constant, ZL constant versus frequency
    2: ZS s2p/s1p, ZL constant vs freq
    3: ZS constant vs freq, ZS s2p/s1p
    4: ZS s2p/s1p vs ZL s2p/s1p
    */

    fmin = fmatching_min;
    fmax = fmatching_max;

    if ((ZS.size() == 1) && (ZL.size() == 1)) //ZS constant, ZL constant vs frequency
    {
        freq = linspace(fmatching_min, fmatching_max, Nsamples);//Available freqs
        complex<double> zs_temp = ZS[0];
        complex<double> zl_temp = ZL[0];
        //Create ZS and ZL vectors with the same length than freq
        ZS.resize(freq.size());
        ZS = ones(ZS);
        ZS = Product(ZS,zs_temp);

        ZL.resize(freq.size());
        ZL = ones(ZL);
        ZL = Product(ZL,zl_temp);

        return 0;
    }

    if ((ZS.size() == 1) && (ZL.size() != 1)) //ZS constant vs ZL s2p/s1p
    {
        if (min(fL) > fmin ) fmin = min(fL);
        if (max(fL) < fmax ) fmax = max(fL);
        fS = fL;
        complex<double> zs_temp = ZS[0];
        ZS.resize(ZL.size());
        ZS = ones(ZL);
        ZS = Product(ZS,zs_temp);
        ready = true;
    }
    if ((ZS.size() != 1) && (ZL.size() == 1) && (!ready)) //ZS s2p/s1p vs ZL constant
    {
        if (min(fS) > fmin ) fmin = min(fS);
        if (max(fS) < fmax ) fmax = max(fS);
        fL = fS;
        complex<double> zl_temp = ZL[0];
        ZL.resize(ZS.size());
        ZL = ones(ZL);
        ZL = Product(ZL,zl_temp);
        ready = true;
    }
    if ((ZS.size() != 1) && (ZL.size() != 1) && (!ready)) //ZS s2p/s1p vs ZL s2p/s1p
    {
          double fmin_aux, fmax_aux;
          //Define vector of available freqs
          (min(fS) > min(fL)) ? fmin_aux = min(fS) : fmin_aux = min(fL);
          (max(fS) > max(fL)) ? fmax_aux = max(fL) : fmax_aux = max(fS);

          if (fmin_aux < fmin ) fmin = fmin_aux;
          if (fmax_aux > fmax ) fmax = fmax_aux;

          //int N;//Number of points for linear interpolation
          //(ZS.n_elem > ZL.n_elem) ? N = 2*ZS.n_elem : N = 2*ZL.n_elem;
    }

        freq = linspace(fmin, fmax, Nsamples);//Available freqs
        unsigned int i1s = closestIndex(fS, fmin)-1;
        unsigned int i2s = closestIndex(fS, fmax)+1;
        unsigned int i1l = closestIndex(fL, fmin)-1;
        unsigned int i2l = closestIndex(fL, fmax)+1;  

        vector<complex<double>> zs = SubVector(ZS, i1s, i2s);
        vector<complex<double>> zl = SubVector(ZL, i1l, i2l);
        vector<double> fs = SubVector(fS, i1s, i2s);
        vector<double> fl = SubVector(fL, i1l, i2l);

        vector<complex<double>> ZS_ = interp(fs, zs, freq);
        vector<complex<double>> ZL_ = interp(fl, zl, freq);  
          
        ZS.resize(ZS_.size());
        ZL.resize(ZL_.size());

        ZS = ZS_;
        ZL = ZL_;
        return 0;

}

vector<complex<double>> IO::getSourceImpedance()
{
    ResampleImpedances();
    return ZS;
}
vector<complex<double>> IO::getLoadImpedance()
{
    ResampleImpedances();
    return ZL;
}

vector<double> IO::getFrequency()
{
    ResampleImpedances();
    return freq;
}


void IO::set_constant_ZS_vs_freq(complex<double> zs)
{
    ZS = vector<complex<double>>(1);
    ZS[0] = zs;
}


void IO::set_constant_ZL_vs_freq(complex<double> zl)
{
    ZL = vector<complex<double>>(1);
    ZL[0] = zl;
}


void IO::set_matching_band(double fmin, double fmax)
{
    fmatching_min = fmin;
    fmatching_max = fmax;
    freq = linspace(fmatching_min, fmatching_max, Nsamples);//Available freqs
}

// Set load/source matching impedace
int IO::setMatchingImpedances()
{
    //Select impedance values at the matching band
    int index1 = getFreqIndex(fmatching_min);
    int index2 = getFreqIndex(fmatching_max);

    vector<complex<double>> ZS_(&ZS[index1], &ZS[index2+1]);
    vector<complex<double>> ZL_(&ZL[index1], &ZL[index2+1]);

    ZS_matching = ZS_;
    ZL_matching = ZL_;

    //Now, it's needed to update the matching band according to f_analysis values
    vector<double>freq_(&freq[index1], &freq[index2]);
    f_matching = freq_;

    return 0;
}


// Gets the index of a given frequency
int IO::getFreqIndex(double f1)
{
    return closestIndex(freq, f1) ;
}

/*
//Sets local optimisation algorithm
void IO::setLocalOptimiser(nlopt::algorithm NL)
{
    LocalOptAlgo = NL;
}


nlopt::algorithm IO::getLocalOptimiser()
{
    return LocalOptAlgo;
}*/


//Get freq scale from a string line
double IO::getS2PfreqScale(string line)
{
    if (line.find("ghz") != std::string::npos)
    {
        return 1e9;
    }
    else
    {
        if (line.find("mhz") != std::string::npos)
        {
            return 1e6;
        }
        else
        {
            if ((line.find("khz") != std::string::npos))
            {
                return 1e3;
            }
            else
            {
                return 1;//Hz
            }
        }
    }
}


// This function exports the best topology found to a Qucs schematic
int IO::ExportQucsSchematic(GRABIM_Result R, string QucsFile)
{
    std::string wirestr = "";
    std::string componentstr = "";
    std::string paintingstr = "";
    int x_pos = 0;
    SchematicParser(R, x_pos, componentstr, wirestr, paintingstr);
    if (R.QucsVersion.empty()) R.QucsVersion = "0.0.19";
    CreateSchematic(componentstr, wirestr, paintingstr, R.QucsVersion, QucsFile);
    return 0;
}



// Given a string code of inductors, capacitors and transmission lines, it generates the Qucs network. Notice that the schematic is split into
// three part: components, wires and paintings, all of them are passed by reference.
int IO::SchematicParser(GRABIM_Result R, int & x_pos, string & componentstr, string & wirestr, string & paintingstr)
{
    string component;
    int x_series = 120, x_shunt = 20;//x-axis spacing depending on whether the component is placed in a series or shunt configuration
    //Clear input strings (just in case)
    componentstr = "";
    wirestr = "";
    paintingstr = "";

    // Schematic code
    // 0: Port 1 + S parameter simulation
    // 1: Port 1
    // 2: Port 2
    // 3: Port 1, Port 2 and S parameter simulation


    if ((R.source_path.empty()) && (abs(ZS.at(0).imag()) < 1e-3) && (ZS.at(0).real() > 1e-3))
    {//Conventional term
        componentstr += "<Pac P1 1 " + Num2String(x_pos) + " -30 18 -26 0 1 \"1\" 1 \"" + Num2String(ZS.at(0).real()) + " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n";
        componentstr += "<GND * 1 " + Num2String(x_pos) + " 0 0 0 0 0>\n";

        wirestr += "<" + Num2String(x_pos) + " -60 " + Num2String(x_pos) + " -120>\n";
        wirestr += "<" + Num2String(x_pos) +" -120 "+ Num2String(x_pos+60) +" -120>\n";
        x_pos +=60;
    }
    else
    {//Place a S-param file
        componentstr += "<SPfile X1 1 " + Num2String(x_pos) + " -120 -26 -67 1 2 \"" + R.source_path + "\" 1 \"rectangular\" 0 \"linear\" 0 \"open\" 0 \"1\" 0>\n";
        componentstr += "<GND * 1 " + Num2String(x_pos) +" -90 0 0 0 0>\n";
        x_pos = 30;
        wirestr += "<" + Num2String(x_pos) + " -120 " + Num2String(x_pos+60) + " -120>\n";
        x_pos +=60;
    }


    // The string format is as follows: "XX<value>;XX<value2>;...XX<valueN>;"
    // where XX, YY, ZZ define the type of component and its configuration.
    //    0: Series inductance
    //    1: Series capacitor
    //    2: Shunt inductance
    //    3: Shunt capacitor
    //    4: Series transmission line
    //    5: Open stub
    //    6: Short circuited stub

    int value_index = 0;
    for (int i = 0; i < R.topology.length(); i++)
    {
        component = R.topology.at(i);

        if (!component.compare("0"))//Series inductor
        {
            componentstr += "<L L1 1 " + Num2String(x_pos+60) + " -120 -26 10 0 0 \"" + Num2String(R.x_local_opt.at(value_index)) + "\" 1 \"\" 0>\n";
            wirestr +=  "<" + Num2String(x_pos) + " -120 " +  Num2String(x_pos+30) + " -120 \"\" 0 0 0 \"\">\n";
            wirestr += "<" + Num2String(x_pos+90) + " -120 " + Num2String(x_pos+x_series) + " -120 \"\" 0 0 0 \"\">\n";
            x_pos += x_series;
            value_index++;
        }
        else if (!component.compare("1"))//Series capacitor
        {
            componentstr += "<C C1 1 " + Num2String(x_pos+60) + " -120 -26 17 0 0 \"" + Num2String(R.x_local_opt.at(value_index)) + "\" 1 "" 0>\n";
            wirestr += "<" + Num2String(x_pos) + " -120 " + Num2String(x_pos+30) + " -120 \"\" 0 0 0 \"\">\n";
            wirestr += "<"+ Num2String(x_pos+90) +" -120 " + Num2String(x_pos+x_series) + " -120 \"\" 0 0 0 \"\">\n";
            x_pos += x_series;
            value_index++;
        }
        else if (!component.compare("2"))//Shunt inductor
        {
            componentstr += "<GND * 1 " + Num2String(x_pos) + " 0 0 0 0 0>\n";
            componentstr += "<L L1 1 " + Num2String(x_pos) + " -30 -26 2 0 1 \"" + Num2String(R.x_local_opt.at(value_index)) + "\" 1 \"\" 0>\n";
            wirestr += "<" + Num2String(x_pos) + " -60 " + Num2String(x_pos) +" -120 \"\" 0 0 0 \"\">\n";
            wirestr += "<" + Num2String(x_pos) + " -120 " + Num2String(x_pos+x_shunt) + " -120 \"\" 0 0 0 "">\n";
            x_pos += x_shunt;
            value_index++;
        }
        else if (!component.compare("3"))//Shunt capacitor
        {
            componentstr += "<GND * 1 " + Num2String(x_pos) + " 0 0 0 0 0>\n";
            componentstr += "<C C1 1 " + Num2String(x_pos) + " -30 -26 17 0 1 \"" + Num2String(R.x_local_opt.at(value_index)) + "\" 1 \"\" 0>\n";
            wirestr += "<" + Num2String(x_pos) +" -60 " + Num2String(x_pos) + " -120 \"\" 0 0 0 \"\">\n";
            wirestr += "<" + Num2String(x_pos) + " -120 " + Num2String(x_pos+x_shunt) + " -120 \"\" 0 0 0 "">\n";
            x_pos += x_shunt;
            value_index++;
        }
        else if (!component.compare("4"))//Transmission line
        {
            componentstr += "<TLIN Line1 1 " + Num2String(x_pos+60) + " -120 -26 20 0 0 \"" + Num2String(R.x_local_opt.at(value_index)) + "\" 1 \"" + Num2String(R.x_local_opt.at(value_index+1)) + "\" 1 \"0 dB\" 0 \"26.85\" 0>\n";
            wirestr += "<" + Num2String(x_pos) + " -120 " + Num2String(x_pos+30) + " -120 \"\" 0 0 0 \"\">\n";
            wirestr += "<" + Num2String(x_pos+90) + " -120 " + Num2String(x_pos+x_series) + " -120 \"\" 0 0 0 \"\">\n";
            x_pos += x_series;
            value_index+=2;
        }
        else if (!component.compare("5"))//Open stub
        {
            componentstr += "<TLIN Line1 1 " + Num2String(x_pos) + " -60 -26 20 0 1 \"" + Num2String(R.x_local_opt.at(value_index)) + "\" 1 \"" + Num2String(R.x_local_opt.at(value_index+1)) + "\" 1 \"0 dB\" 0 \"26.85\" 0>\n";
            wirestr += "<" + Num2String(x_pos) + "-90 " + Num2String(x_pos) + " -120 \"\" 0 0 0 \"\">\n";
            wirestr += "<" + Num2String(x_pos) + " -120 "+ Num2String(x_pos+x_shunt) + " -120 \"\" 0 0 0 \"\">\n";
            x_pos += x_shunt;
            value_index+=2;
        }
        else if (!component.compare("6"))//Short circuited stub
        {
            componentstr += "<TLIN Line1 1 " + Num2String(x_pos) + " -60 -26 20 0 1 \"" + Num2String(R.x_local_opt.at(value_index)) + "\" 1 \""+Num2String(R.x_local_opt.at(value_index+1))+"\" 1 \"0 dB\" 0 \"26.85\" 0>\n";
            componentstr += "<GND * 1 " + Num2String(x_pos) + " -30 0 0 0 0>\n";
            wirestr += "<" + Num2String(x_pos) +" -90 " + Num2String(x_pos) + " -120 \"\" 0 0 0 \"\">\n";
            wirestr += "<" + Num2String(x_pos) + "-120 " + Num2String(x_pos+x_shunt) + "-120 \"\" 0 0 0 \"\">\n";
            x_pos += x_shunt;
            value_index+=2;
        }

    }

    double spacing = 30;
    x_pos += spacing;

    if ((R.load_path.empty()) && (abs(ZL.at(0).imag()) < 1e-3) && (ZL.at(0).real() > 1e-3))
    {//Conventional term
        componentstr += "<Pac P1 1 " + Num2String(x_pos) + " -30 18 -26 0 1 \"1\" 1 \"" + Num2String(ZL.at(0).real()) + " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n";
        componentstr += "<GND * 1 "+Num2String(x_pos)+" 0 0 0 0 0>\n";

        wirestr += "<" + Num2String(x_pos) + " -60 " + Num2String(x_pos) + " -120>\n";
        wirestr += "<" + Num2String(x_pos-spacing) + " -120 " + Num2String(x_pos) + " -120>\n";
    }
    else
    {//Place a S-param file
        componentstr += "<SPfile X1 1 " + Num2String(x_pos) + " -120 -26 -67 0 0 \"" + R.load_path + "\" 1 \"rectangular\" 0 \"linear\" 0 \"open\" 0 \"1\" 0>\n";
        componentstr += "<GND * 1 " + Num2String(x_pos) + " -90 0 0 0 0>\n";
    }
    return 0;
}


//-----------------------------------------------------------------------------
// Given the components, wires and paintings, it creates the schematic and copies on the clipboard
bool IO::CreateSchematic(string components, string wires, string paintings, string QucsVersion, string QucsFilePath)
{
    //Header
    std::string Schematic = "<Qucs Schematic " + QucsVersion + ">\n";

    //Add components
    Schematic += "<Components>\n";
    Schematic += components;
    Schematic += "</Components>\n";

    //Add wires
    Schematic+= "<Wires>\n";
    Schematic += wires;
    Schematic+= "</Wires>\n";

    //Add paintings
    Schematic += "<Paintings>\n";
    Schematic += paintings;
    Schematic += "</Paintings>\n";

    if (CopyToClipboard)
    {  
       #ifdef QT_NO_DEBUG
       QApplication::clipboard()->setText(QString(Schematic.c_str()), QClipboard::Clipboard);//Copy into clipboard
       #endif
    }
    else//Dump content into a file
    {
       std::ofstream QucsFile(QucsFilePath.c_str(), ios_base::out);
       QucsFile << Schematic;
       QucsFile.close();
    }

    return true;
}

string IO::Num2String(double x)
{
 std::ostringstream s;
s << x;
return s.str();
}


string IO::Num2String(int x)
{
    std::ostringstream s;
    s << x;
    return s.str();
}




void IO::PrintNetwork_StandardOutput(GRABIM_Result Res)
{
    printf("\n+----SRC-----+");
    unsigned int index = 0;
    for(unsigned int i=0;i<Res.topology.size();i++, index++)
        {
                                          cout<<"\n|            |  ";
    if(!Res.topology.substr(i,1).compare("0"))cout<<"\n|            L  "<<Res.x_local_opt[index]*1E9 << "nH";
    if(!Res.topology.substr(i,1).compare("1"))cout<<"\n|            C  "<<Res.x_local_opt[index]*1E12<< "pF";
    if(!Res.topology.substr(i,1).compare("2"))cout<<"\n+-----L------+  "<<Res.x_local_opt[index]*1E9 << "nH";
    if(!Res.topology.substr(i,1).compare("3"))cout<<"\n+-----C------+  "<<Res.x_local_opt[index]*1E12<< "pF";
    if(!Res.topology.substr(i,1).compare("4"))cout<<"\n|            T  "<<
                                                "\n|            l  "<<Res.x_local_opt[index] << " " << Res.x_local_opt[index+1], index++;
    if(!Res.topology.substr(i,1).compare("5"))cout<<"\n|      oc+stub  "<<Res.x_local_opt[index] << " " << Res.x_local_opt[index+1], index++;
    if(!Res.topology.substr(i,1).compare("6"))cout<<"\n|      sc+stub  "<<Res.x_local_opt[index] << " " << Res.x_local_opt[index+1], index++;
                                          cout<<"\n|            |  ";
        }
    printf("\n+----LOAD----+\n\n");

    cout << "min(S11) = " << Res.nlopt_val << "dB" << endl;


    //LOG
    std::ofstream LogFile("log", ios_base::app);
    LogFile << Res.ZS.at(0) << " " << Res.ZS.at(round(2*Nsamples/5)-1) << " " << Res.ZS.at(round(3*Nsamples/5)-1) << " " << Res.ZS.at(round(4*Nsamples/5)-1) << " " << Res.ZS.at(Nsamples-1) << endl;
    LogFile << Res.ZL.at(0) << " " << Res.ZL.at(round(2*Nsamples/5)-1) << " " << Res.ZL.at(round(3*Nsamples/5)-1) << " " << Res.ZL.at(round(4*Nsamples/5)-1) << " " << Res.ZL.at(Nsamples-1) << endl;
    LogFile << min(Res.freq) << endl;
    LogFile << max(Res.freq) << endl;
    LogFile << Res.topology << endl;
    for (unsigned int i=0; i < Res.S11_gridsearch.size(); i++) LogFile << "(" << Res.S11_gridsearch.at(i).real() << "," <<  Res.S11_gridsearch.at(i).imag() << ") "<< endl;
    LogFile << "###" << endl;
    LogFile.close();
}


void IO::UseClipboard(bool B)
{
  CopyToClipboard=B;
}
