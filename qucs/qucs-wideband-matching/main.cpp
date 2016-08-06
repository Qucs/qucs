/*
 * main.cpp - Wideband matching network synthesis tool
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

#ifdef QT_NO_DEBUG //Qt libraries
     #include "ui.h"
     #include <QApplication>
#else //Only command line operation
    #include "GRABIM.h"
    #include "io.h"
#endif

#include "mat.h"
#include "MathOperations.h"
#include <time.h>//Get random seed using time
#include <stdlib.h> //srad()



/*
  Parses a complex impedance from text to complex<double>
*/
complex<double> getComplexImpedanceFromText(char *arg)
{
    string arg_str = arg;
    int index = arg_str.find_first_of("j");
    int sign = 1;
    double zreal, zimag;
    if (index != -1)//Then it is a single impedance
    {
        zreal = atof(arg_str.substr(0, index-1).c_str());
        if (!arg_str.substr(index-1, 1).compare("-")) sign = -1;
        zimag = sign*atof(arg_str.substr(index+1).c_str());
        return complex<double>(zreal, zimag);
    }
    else
    {
        zreal = atof(arg);
        if (zreal > 0)return complex<double>(zreal, 0);
        else
            return complex<double>(-1, -1);
    }
}








int main(int argc, char *argv[])
{
  bool help=false;
  if (argc == 2)//Need help?
  {
     if (!strcmp(argv[1], "--help")) help = true;
  }


    if ((argc < 5) && (help == false))//User interface
    {
     #ifdef QT_NO_DEBUG
    QApplication app(argc, argv);
    ui *WB_MatchingTool = new ui();
    WB_MatchingTool->raise();
    WB_MatchingTool->resize(350, 350);
    WB_MatchingTool->show();
    return app.exec();
     #endif
    }
    else //Command line
    {
          string GNUplot_path = "GRABIM.dat";//Path to the GNUplot output data
          string TopoScript_path = "predefined_topologies";//Topology list
          string QucsSchPath = "./Schematic.sch";//Qucs schematic
          string topo = "-1";//Circuit topology
          unsigned int search_mode = 0;//Search mode.
          bool no_simplify = false;//This option allows simplifying the circuit topology when possible

          srand (time(NULL));//Random seed. If the user provides a specific seed, this property is modified below

          //Parse arguments
          for (unsigned int i = 1; i < argc; i++)
          {
              if (!strcmp(argv[i], "--set-topo") && (argc -1 >= (i+1)))//User-defined topology
              {
                    topo = argv[i+1];
                    search_mode = -1;
              }
              if (!strcmp(argv[i], "--set-seed") && (argc -1 >= (i+1)))//Custom seed
              {
                    srand(atof(argv[i+1]));//Set user-defined seed
              }

              if (!strcmp(argv[i], "--set-search-mode") && (argc -1 >= (i+1)))//Search mode. The same as in the UI
              {
                   search_mode = atoi(argv[i+1]);
              }

              if (!strcmp(argv[i], "--set-topology-script") && (argc -1 >= (i+1)))//Path to the topology list
              {
                   TopoScript_path = argv[i+1];
              }

              if (!strcmp(argv[i], "--set-Qucs-Schematic-Path") && (argc -1 >= (i+1)))//Path to export the Qucs schematic
              {
                   QucsSchPath = argv[i+1];
              }

              if (!strcmp(argv[i], "--set-GNUplot-data") && (argc -1 >= (i+1)))//Path to output GNUplot data
              {
                   GNUplot_path = argv[i+1];
              }

              if (!strcmp(argv[i], "--no-simplify"))//This option avoids simplifying the circuit topology
              {
                   no_simplify = true;
              }
              if (!strcmp(argv[i], "--help"))//Display command line help
              {
                  cout << "This program implements GRABIM, a grid-search based" << endl;
                  cout << "wideband matching network synthesis algorithm." << endl;
                  cout << "References:" << endl;
                  cout << "[1] Broadband direct-coupled and RF matching networks. Thomas R. Cuthbert, 1999" << endl;
                  cout << "[2] Broadband impedance matching - Fast and simple. Thomas R. Cuthbert. RF design. 1994" << endl << endl;
                  cout << "Options:" << endl;
                  cout << "* ./GRABIM-CPP ZS ZL f1 f2" << endl;
                  cout << "   General call. Searches the best solution among a predefined set of network topologies" << endl;
                  cout << "* --set-seed xxx" <<endl;
                  cout << "   Sets a seed for random number generation. This implementation tries to" << endl;
                  cout << "   get closer to the optimum than the original method, so at the final" << endl;
                  cout << "   grid cycle, it generates a set of random vectors in the neighboorhood" << endl;
                  cout << "   of the current pivot." << endl;
                  cout << "* --set-search-mode  xxx" << endl;
                  cout << "   The topology search is accomplished using the following code:" << endl;
                  cout << "   0: Predefined set" << endl;
                  cout << "   1: Script" << endl;
                  cout << "   2: Combinations of LC (4 elements)" << endl;
                  cout << "   3: Combinations of LC and TL (6 elements)" << endl;
                  cout << "   4: Combinations of LC, TL and open/shorted stubs (6 elements). Not recommended" << endl;
                  cout << "* --no-simplify" << endl;
                  cout << "   By default, this program takes advantage from removing high/low impendance components" << endl;
                  cout << "   to avoid this, please use this option" << endl;
                  cout << "* --set-topo xxx" << endl;
                  cout << "   The program tries to find the optimum of an user defined topology. xxx can be" << endl;
                  cout << "   an arbitrary combination of the following items" << endl;
                  cout << "   0: Series inductance"<< endl;
                  cout << "   1: Series capacitance" << endl;
                  cout << "   2: Parallel inductance"<< endl;
                  cout << "   3: Parallel capacitance" << endl;
                  cout << "   4: Transmission line"<< endl;
                  cout << "   5: Open stub" << endl;
                  cout << "   6: Shorted stub" << endl;
                  return 0;
              }

          }

        string SourceFile = argv[1];//Path to the s1p which contains the source impedance data
        string LoadFile = argv[2];//Path to the s1p which contains the load impedance data

        //Detects if the user entered a constant impedance vs freq or, otherwise, he/she entered the path of a s1p file
        bool ZSisConstant = SourceFile.find(".s1p") == std::string::npos;
        bool ZLisConstant = LoadFile.find(".s1p") == std::string::npos;

        //Impedance data paths were already specified, let's proceed to bring the S-par data into memory
        IO inout_operations;
        inout_operations.UseClipboard(false);//Command line mode does not allow using the clipboard, so the Qucs schematic will be dumped into a file

        if (!ZSisConstant)//Read source impedance from file
        {
            if (inout_operations.loadS1Pdata(SourceFile, SOURCE))
            {
                cout << "ERROR: File not found" << endl;
                return -1;
            }
        }
        else//Set constant source impedance
        {
           complex<double> zs_temp;
           char * text = argv[1];
           zs_temp = getComplexImpedanceFromText(text);
           if (zs_temp.real() == -1)//Check if the input value is correct
           {
               cerr << "The input given for the source impedance is not valid" << endl;
               return 0;
           }
           inout_operations.set_constant_ZS_vs_freq(zs_temp);
        }

        if (!ZLisConstant)//Read load impedance from file
        {
           if (inout_operations.loadS1Pdata(LoadFile, LOAD))
           {
               cout << "ERROR: File not found" << endl;
               return -1;
           }
        }
        else//Set constant load impedance
        {
            complex<double> zl_temp;
            char * text = argv[2];
            zl_temp = getComplexImpedanceFromText(text);

            if (zl_temp.real() == -1)//Check if the input value is correct
            {
                cerr << "The input given for the load impedance is not valid" << endl;
                return 0;
            }
            inout_operations.set_constant_ZL_vs_freq(zl_temp);

        }

        //Check frequency specifications
        double fmatching_min = atof(argv[3]);
        double fmatching_max = atof(argv[4]);

 
        if (fmatching_max <= fmatching_min)
        {
            cout << "The lowest frequency must be lower than the highest" << endl;
            return 0;
        }

        if ((fmatching_min == -1) || (fmatching_max == -1))
        {
            cerr << "Incorrect frequency settings" << endl;
            return 0;
        }
        else//Everything correct... lets set frequency
        {
            inout_operations.set_matching_band(fmatching_min, fmatching_max);

            // In general, the load and source impedance data may be taken at different frequencies. For this reason, it is performed
            // a linear interpolation before running GRABIM
            inout_operations.ResampleImpedances();//Force data update
            if (fmatching_min*1.05 < min(inout_operations.getFrequency()))//The s1p does not contains the lowest freq
            {
                cerr <<"One of the impedance data files does not contain the specified lower frequency" << endl;
                return 0;
            }
            if (fmatching_max*0.95 > max(inout_operations.getFrequency()))//The s1p does not contains the highest freq
            {
                cerr <<"One of the impedance data files does not contain the specified upper frequency" << endl;
                return 0;
            }
        }

        GRABIM MatchingObject;
        // Impedance and frequency settings
        MatchingObject.SetSourceImpedance(inout_operations.getSourceImpedance());
        MatchingObject.SetLoadImpedance(inout_operations.getLoadImpedance());
        MatchingObject.SetFrequency(inout_operations.getFrequency());
        MatchingObject.setTopoScript(TopoScript_path);
        MatchingObject.SetTopology(topo);
        MatchingObject.setSearchMode(search_mode);
        MatchingObject.SimplifyNetwork(!no_simplify);


        GRABIM_Result R = MatchingObject.RunGRABIM();//Runs GRABIM. Please bear in mind that this is not a rigorous implementation of [1] 
        // Biggest differences between this code and [1]:
        // 1) The candidate vector is always defined in natural units rather than logarithmic units.
        // 2) Frequency is not normalized.
        // 3) The objective function is the magnitude of S11 expressed in dB. log(x) functions have bigger
        // gradients than x so using S11 in dB may help to find the optimum faster
        // 4) The local optimizer is the standard Nelder-Mead algorithm [2]. Observation seems to suggest that NM is enough for dealing
        // with the usual wideband matching network topologies.

        // References:
        // [1] Broadband direct-coupled and RF matching networks. Thomas R. Cuthbert, 1999
        // [2] Convergence properties of the Nelder-Mead simplex method in low dimensions. J.F. Lagarias, J.A. Reeds. SIAM J. OPTIM, Vol 9, No. 1, pp. 112-147

        inout_operations.PrintNetwork_StandardOutput(R);//Prints the matching network in the terminal

        (ZSisConstant) ? R.source_path = "" : R.source_path = SourceFile;
        (ZLisConstant) ? R.load_path = "": R.load_path = LoadFile;


        cout << "Finished: GRABIM has successfully finished." << endl;
        cout << "The matching network was written using the Qucs schematic format at " << QucsSchPath << endl << endl;
        cout << "Alternatively, you can check the performance of the network using gnuplot" << endl;
        cout << "The GNUplot data was written at " << GNUplot_path << endl;
        cout << "You can find a sample GNUplot script to plot S11 in a Smith Chart as well as rectangular plots" << endl;

        inout_operations.exportGNUplot(R, GNUplot_path);
        inout_operations.ExportQucsSchematic(R, QucsSchPath);
    }
}
