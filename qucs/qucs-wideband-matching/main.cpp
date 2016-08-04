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
#include "ui.h"
#include <QApplication>

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
    if (argc < 5)//User interface
    {
    QApplication app(argc, argv);
    ui *WB_MatchingTool = new ui();
    WB_MatchingTool->raise();
    WB_MatchingTool->resize(350, 350);
    WB_MatchingTool->show();
    return app.exec();
    }
    else //User interface command line
    {
          string topo = "-1";
          unsigned int search_mode = 0;
          bool no_simplify = false;
          srand (time(NULL));//Random seed

          //Parse arguments
          for (unsigned int i = 1; i < argc; i++)
          {
              if (!strcmp(argv[i], "--set-topo") && (argc -1 >= (i+1)))
              {
                    topo = argv[i+1];
              }
              if (!strcmp(argv[i], "--set-seed") && (argc -1 >= (i+1)))
              {
                    srand(atof(argv[i+1]));//Set user-defined seed
              }

              if (!strcmp(argv[i], "--set-search-mode") && (argc -1 >= (i+1)))
              {
                   search_mode = atoi(argv[i+1]);
              }

              if (!strcmp(argv[i], "--no-simplify"))
              {
                   no_simplify = true;
              }
              if (!strcmp(argv[i], "--help"))
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
                  cout << "   0: Script" << endl;
                  cout << "   1: Predefined set" << endl;
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

        string SourceFile = argv[1];
        string LoadFile = argv[2];

        bool ZSisConstant = SourceFile.find(".s1p") == std::string::npos;
        bool ZLisConstant = LoadFile.find(".s1p") == std::string::npos;

        //Impedance data paths were already specified, let's proceed to bring the S-par data into memory
        IO inout_operations;
        inout_operations.UseClipboard(false);
        int ret;
        if (!ZSisConstant)//Read source impedance from file
        {
            ret = inout_operations.loadS1Pdata(SourceFile, SOURCE);//s1p
            if (ret == -1)
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

        if (!ZLisConstant)
        {
           ret = inout_operations.loadS1Pdata(LoadFile, LOAD);//s1p
           if (ret == -1)
           {
               cout << "ERROR: File not found" << endl;
               return -1;
           }
        }
        else
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

            //Check if the specified frequencies lie with the s1p/s2p data
            inout_operations.ResampleImpedances();//Force data update
            if (fmatching_min*1.05 < min(inout_operations.getFrequency()))//The lower freq is not present at s1p/s2p
            {
                cerr <<"One of the impedance data files does not contain the specified lower frequency" << endl;
                return 0;
            }
            if (fmatching_max*0.95 > max(inout_operations.getFrequency()))//The maximum freq is not present at s1p/s2p
            {
                cerr <<"One of the impedance data files does not contain the specified upper frequency" << endl;
                return 0;
            }
        }


        string TopoScript_path = "predefined_topologies";
        string QucsSchPath = "GRABIM_result.sch";

        GRABIM MatchingObject;
        // Impedance and frequency settings
        MatchingObject.SetSourceImpedance(inout_operations.getSourceImpedance());
        MatchingObject.SetLoadImpedance(inout_operations.getLoadImpedance());
        MatchingObject.SetFrequency(inout_operations.getFrequency());
        MatchingObject.setTopoScript(TopoScript_path);
        MatchingObject.SetTopology(topo);
        MatchingObject.setSearchMode(search_mode);
        MatchingObject.SimplifyNetwork(!no_simplify);


        GRABIM_Result R = MatchingObject.RunGRABIM();//Runs GRABIM. Well, this is not exactly the algorithm
        // detailed at [1] but from my point of view is functional and easier to code...
        //Notes:
        // 1) The candidate vector is not in log units. I do not see a good reason for doing so. Maybe I am missing something important
        // 2) Frequency is not in rad/s.
        // 3) The objective function is the magnitude of S11 expressed in dB. log(x) functions usually have strong
        // gradients so it seem to suggest that this is good for derivative free opt algorithms
        // 4) This code takes advantage from NLopt derivative-free local optimisers. NLopt is easy to link and it works
        // fine. Despite the fact that the Nelder-Mead algorithm does not guarantee convergence (among other problems), it leads to achieve a good local
        // (probably, global) optimum. This is caused by the fact that the matching network should be as simple as possible => few elements => xk \in R^N, where
        // N is typically < 6. Even N=6 is a big number, please consider that matching networks are tight to physical constraints in practice, so, the larger the
        // network, the harder the 'tuning'.

         inout_operations.PrintNetwork_StandardOutput(R);

        (ZSisConstant) ? R.source_path = "" : R.source_path = SourceFile;
        (ZLisConstant) ? R.load_path = "": R.load_path = LoadFile;


        string GNUplot_path = "GRABIM.dat";

        cout << "Finished: GRABIM has successfully finished." << endl;
        cout << "Please execute: 'gnuplot plotscript' to take a look to the results." << endl;
        cout << "A new Qucs schematic has been copied into the clipboard." << endl;

        inout_operations.exportGNUplot(R, GNUplot_path);
        inout_operations.ExportQucsSchematic(R);
    }
}
