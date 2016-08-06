/*
 * GRABIM.cpp - Grid search class implementation
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


﻿#include "GRABIM.h"

/* References:
  [1] Broadband direct-coupled and RF matching networks. Thomas R. Cuthbert, 1999 
  [2] Broadband impedance matching - Fast and simple. Thomas R. Cuthbert. RF design. 1994
*/

// Constructor
GRABIM::GRABIM()
{
    Grid_MaxIter = 1000;//Maximum number of grid iterations
    MatchingThreshold = -30;//It specifies the mininum S11 [dB] required. In general, S11 < 10 dB is considered a good matching condition for common applications
    simplify_network = true;//Enables circuit simplification when possible

    ObjFun = ObjectiveFunction::NINF_S11dB;//Sets the kind of objective function. Everything seems to
    //suggest that NINF_S11dB gives the best results
    topology = "-1";//By default, the engine will search the optimum topology from the predefined set of circuit
}

// It sets the source impedance vs frequency
int GRABIM::SetSourceImpedance(vector<complex<double>> zs)
{
    ZS = zs;
    return 0;
}

// It loads a s2p file and calculates the input impedance
int GRABIM::SetLoadImpedance(vector<complex<double>> zl)
{
    ZL = zl;
    return 0;
}

// It sets the load impedance vs frequency
int GRABIM::SetFrequency(vector<double> f)
{
    freq = f;
    return 0;
}

// Sets the first point of the grid search
int GRABIM::SetInitialPivot(vector<double> x)
{
    x_ini = x;
    return 0;
}

// Returns the first point of the grid search
vector<double> GRABIM::GetInitialPivot()
{
    return x_ini;
}



// This function sets the ladder arrangement of the matching network
int GRABIM::SetTopology(std::string s)
{
    topology = s;
    return 0;
}


// Sets the maximum number of iterations for the grid search engine
int GRABIM::SetMaxIterGridSearch(int max_iter)
{
    Grid_MaxIter = max_iter;
    return 0;
}

// Sets the threshold of what the program consideres a good matching
int GRABIM::SetThreshold(double th)
{
    MatchingThreshold = th;
    return 0;
}

// Returns the matching threshold
double GRABIM::GetThreshold()
{
    return MatchingThreshold;
}

// Starts the engine
GRABIM_Result GRABIM::RunGRABIM()
{
    GRABIM_Result Res;

    vector<vector<double>> Vopt;//This data structure which contains the optimum vector for each circuit topology.
    vector<string> candidates;//List of topologies. The grid search will be done for each one of them
    GridSearch_DifferentTopologies(Vopt, candidates);//Runs the grid search for each topology
    vector<double> aux_grid(Vopt[0]), aux_local(Vopt[0]);
    double  aux_flocal, best=1e8;
    unsigned int best_index=0;


    // The local optimizer is used for those topologies with good matching condition. Observation seems to suggest that acting so improves the chance of finding a good result
    cout << "Local candidates:" << endl;
    for (unsigned int i = 0; i < candidates.size(); i++)
    {
        topology = candidates[i];
        aux_grid = Vopt[i];
        aux_local = NelderMead(aux_grid);
        aux_flocal = CandidateEval(aux_local);
        if (aux_flocal < best)
        {
            best_index = i;
            best = aux_flocal;
            Res.x_grid_search = Vopt[i];
            Res.x_local_opt = aux_local;
            Res.grid_val = CandidateEval(Vopt[i]);
            Res.nlopt_val = aux_flocal;
        }
    }

    //At this point, it was found the best candidate so the code below arranges data so as to produce the output data
    topology = candidates[best_index];

    Res.ZS = ZS;
    Res.ZL = ZL;
    Res.freq = freq;

    //Initialize S param vectors
    Res.S11_gridsearch = vector<complex<double>>(freq.size());
    Res.S21_gridsearch = vector<complex<double>>(freq.size());
    Res.S12_gridsearch = vector<complex<double>>(freq.size());
    Res.S22_gridsearch = vector<complex<double>>(freq.size());

    Res.S11_nlopt = vector<complex<double>>(freq.size());
    Res.S21_nlopt = vector<complex<double>>(freq.size());
    Res.S12_nlopt = vector<complex<double>>(freq.size());
    Res.S22_nlopt = vector<complex<double>>(freq.size());


    //Generate S parameter results
    Mat S_gridsearch(2,2), S_nlopt(2,2);
    S_gridsearch.eye();
    S_nlopt.eye();

    SparEngine S2PEngine;

    for (unsigned int i = 0; i < freq.size(); i++)
    {
        // Grid search
        S_gridsearch = S2PEngine.getSparams(Res.x_grid_search, ZS[i], ZL[i], freq[i], topology);
        Res.S11_gridsearch[i] = S_gridsearch(0,0);
        Res.S21_gridsearch[i] = S_gridsearch(1,0);
        Res.S12_gridsearch[i] = S_gridsearch(0,1);
        Res.S22_gridsearch[i] = S_gridsearch(1,1);

        // NLopt
        S_nlopt = S2PEngine.getSparams(Res.x_local_opt, ZS[i], ZL[i], freq[i], topology);
        Res.S11_nlopt[i] = S_nlopt(0,0);
        Res.S21_nlopt[i] = S_nlopt(1,0);
        Res.S12_nlopt[i] = S_nlopt(0,1);
        Res.S22_nlopt[i] = S_nlopt(1,1);
    }

    Res.topology = topology.c_str();//Save network topology to create a Qucs schematic

    return Res;
}


// This function is the core of the grid search. It follows the guidelines given in [1]
vector<double> GRABIM::GridSearch()
{
    unsigned int dim = x_ini.size();
    vector<double> delta_k(4), best(4), step(dim);
    vector<double> f_xkq(pow(2,dim)+1);
    vector<double> xk(x_ini);
    vector<vector<double>> xkq(pow(2, dim)+1, vector<double>(dim));
    vector<vector<double>> best_pivot(4, vector<double>(dim));

    //Set grid cycle steps
    delta_k[0] = 1.; delta_k[1] =  1./4; delta_k[2] =  1./16; delta_k[3] =  1./64;
    best[0] = 100; best[1] = 100; best[2] =100; best[3] = 100;
 
    double fxk = CandidateEval(xk);
    double best_candidate;
    best[0] = fxk;
    vector<vector<double>> C = GeneratingMatrix(dim);


    unsigned int niter = 0;
    f_xkq = 1e6*ones(f_xkq.size());
    //Initial guess. Sometimes, the initial point may be far away from the optimum, so it
    // seems to be reasonable to pick a handful of random vectors nearby the inital point and
    // choose the best one.

    int N_initial_guess  = 100;
    vector<vector<double>> XKQ(N_initial_guess, vector<double>(xk.size()));
    vector<double> FXK(N_initial_guess);
    for (unsigned int r = 0; r < N_initial_guess; r++)
    {
       for (unsigned int c = 0; c < dim; c++)
       {
        XKQ[r][c] = xk[c]* (rand() % 3 + 1e-3);
       }
        FXK[r] = CandidateEval(XKQ[r]);
    }

    if (min(FXK) < fxk)
    {
        unsigned int imin = findMin(FXK);
        xk = XKQ[imin];
        fxk = CandidateEval(xk);
    }

    // Factorial search

    for (int i = 0; i<4; i++, niter++)
    {
        if ((min(best) < MatchingThreshold)||(niter > Grid_MaxIter))//Stop condition
        {
            return xk;
        }

        dim = xk.size();
        CheckDim(C, xkq, best_pivot, dim);//InspectCandidate() may have shrunk the network, so it is needed to update the settings

        for (int q = 0; q<=pow(2, dim);q++)//Calculates the vertices of the searching hypercube
        {
            step = delta_k[i]*getCol(C,q);
            xkq[q] = xk * (1. + .99*step);
            f_xkq[q] = CandidateEval(xkq[q]);
        }
        best_candidate = min(f_xkq);


        if (best_candidate < min(best) - 0.1)
        {
            best[0] = best_candidate;
            best[1] = 0;
            best[4] = 0;
            best[3] = 0;

            xk = xkq[findMin(f_xkq)];//Set new pivot
            xk = InspectCandidate(xk);
            i = -1;//New pivot
            continue;
        }
        else
        {
            best[i] = best_candidate;
            best_pivot[i] = xk;
        }

        if (i > 0)//Checks if the i-th cycle improved the result with regard to the (i-1)-th cycle
        {
            if ((best[i]-best[i-1]<-0.01))//The current cycle did not improve the result
            {
                //When GRABIM gets stuck, this seems to help finding better results. The distribution was chosen to be U(-0.5, 0.5)
                unsigned int Ncatchup  = 200;
                unsigned int dim = best_pivot[i].size();
                vector<vector<double>> XKQ(Ncatchup, vector<double>(dim));
                vector<double> FXK (Ncatchup);
                
                for (unsigned int r = 0; r < Ncatchup; r++)
                {
                    for (unsigned int c = 0; c < dim; c++)
                    {
                       XKQ[r][c] = ((rand() % 3000)/2000.)*best_pivot[i][c];
                    }
                    FXK[r] =CandidateEval(XKQ[r]);
                }
                if (min(FXK) < min(best))
                {
                    xk = XKQ[findMin(FXK)];
                    xk = InspectCandidate(xk);
                    i=-1;
                }
                
            }

        }
    }
    //Destroy variables
    xkq.clear();
    f_xkq.clear();
    XKQ.clear();
    FXK.clear();
    return xk;
}



//This function searches the optimum over a predefined circuit topologies
int GRABIM::GridSearch_DifferentTopologies(vector<vector<double>> & Vopt, vector<std::string> & candidates)
{
    double gridtest, opttopo = 1e12;
    vector<double> Vaux;
    string tag, best;

    unsigned int n_topo = TopoList.size();
    for (unsigned int i = 0; i < n_topo; i++)
    {
        topology = TopoList.front();
        tag = TagList.front();
        TopoList.pop_front();
        TagList.pop_front();
        AutoSetInitialPivot();

        Vaux = GridSearch();
        gridtest = CandidateEval(Vaux);
        cout << 100.*(i+1)/n_topo<< "% completed. " << tag << ": S11_min = " <<  gridtest << " dB" << endl;

        if (gridtest < -8.)
        {
            candidates.push_back(topology);
            opttopo = gridtest;
            Vopt.push_back(Vaux);
        }
        else
        {
            if(gridtest < opttopo)
            {
                best = topology;
                candidates.push_back(topology);
                opttopo = gridtest;
                Vopt.push_back(Vaux);
            }
        }
        Vaux.clear();
    }
    if (candidates.empty())
    {
        candidates.push_back(best);
    }
    return 0;
}

// Whenever the impedance of a component is not significant (e.g. low series impedance or high shunt impedance) its
// better to remove this component from the optimisation problem. This will lead to an improvement in terms of better convergence
void GRABIM::removeElement(vector<double> & xk, unsigned int pos, unsigned int postopo)
{
    vector<double> aux(xk.size()-1);
    int index = 0;
    int element = atoi(topology.substr(postopo,1).c_str());
    for (unsigned int i = 0; i < xk.size(); i++)//Removes component values from the working vector
    {
        if (element == 4)
        {
            if ((i == pos) || (i == pos+1)) continue;
        }
        if (i == pos) continue;
        aux.at(index) = xk.at(i);
        index++;
    }
    topology.erase(postopo, 1);//Removes the component from the topology indicator

    //Reshape xk
    xk.resize(aux.size());
    xk=aux;
}

//This function is aimed to remove reduncancies in case some network element
//is removed because of the irrelevance of its impedance
void GRABIM::CheckNetwork(vector<double> & xk, unsigned int x_index, unsigned int topo_index)
{
    //The following cases must be treated separatedly
    // * Two series inductors
    // * Two series capacitors
    // * Two parallel inductors
    // * Two parallel capacitors

    if ((topo_index == 0) || (topo_index == topology.length()-1)) return;//It was removed the first or the last element. Any case below applies
    if (!strcmp(topology.substr(topo_index-1, 2).c_str(), "00"))//Two series inductors
    {
        xk.at(x_index-1) += xk.at(x_index);
        removeElement(xk, x_index, topo_index);
        return;
    }

    if (!strcmp(topology.substr(topo_index-1, 2).c_str(), "11"))//Two series capacitors
    {
        xk.at(x_index-1) = (xk.at(x_index)*xk.at(x_index-1))/(xk.at(x_index)+xk.at(x_index-1));
        removeElement(xk, x_index, topo_index);
        return;
    }

    if (!strcmp(topology.substr(topo_index-1, 2).c_str(), "22"))//Two parallel inductors
    {
        xk.at(x_index-1) = (xk.at(x_index)*xk.at(x_index-1))/(xk.at(x_index)+xk.at(x_index-1));
        removeElement(xk, x_index, topo_index);
        return;
    }

    if (!strcmp(topology.substr(topo_index-1, 2).c_str(), "33"))//Two parallel capacitors
    {
        xk.at(x_index-1) += xk.at(x_index);
        removeElement(xk, x_index, topo_index);
        return;
    }

}


// This function checks whether the candidate vector contains some
// irrelevant value or not. Unsignificant values may prevent the algorithm
// to find a better or more realistic solution. In this sense, shunt elements
// whose impedance exceeds 4kOhm are interpreted as open circuits, and conversely,
// series components whose impedance is below 1 Ohm are treated as short circuits
vector<double> GRABIM::InspectCandidate(vector<double> xk)
{
    double impedance, fmax = max(freq), fmin = min(freq);
    unsigned int element;
    double wmax =2*pi*fmax, wmin =2*pi*fmin;
    int index = 0;
    for (unsigned int i = 0; i < topology.length(); i++)//Inspects all elements. aux is defined so as to take into account
    {                                                   // those elements which depend on more than one variable (e.g.) TL
        element = atoi(topology.substr(i,1).c_str());
        switch(element)
        {
        case 0://Series inductor
            impedance = wmax*xk[index];
            if ((impedance < 1) && (simplify_network))
            {
                xk[index] = 1e-100;
                removeElement(xk, index, i);//The element seems not to have an important effect on the network, so it can
                // be removed
                CheckNetwork(xk, index, i);//Avoid network redundancies (parallel/series capacitors/inductances)
            }
            if (xk[index] < 0)//Inductance must be > 0, so it seems that a capacitor would do a better job
            {
                xk[index] = 1./(2e3*wmin);//High impedance capacitor
                topology[i] = '1';//Series capacitor
                cout << "Warning: The selected topology leads to L < 0" <<endl;
                cout << "Warning: Topology changed. The new topology is: " << topology << endl;
            }
            index++;
            continue;
        case 2://Shunt inductor
            impedance = wmin*xk[index];
            if ((impedance > 4e3) && (simplify_network))
            {
                xk.at(index) = 1e10;
                removeElement(xk, index, i);
                CheckNetwork(xk, index, i);//Avoid network redundancies (parallel/series capacitors/inductances)
            }
            if (xk[index] < 0)//Inductance must be > 0, so it seems that a capacitor would do a better job
            {
                xk[index] = 1./(2e3*wmin);//High impedance capacitor
                topology[i]='3';//Shunt capacitor
                cout << "Warning: The selected topology leads to L < 0" <<endl;
                cout << "Warging: Topology changed. The new topology is: " << topology << endl;
            }
            index++;
            continue;
        case 1://Series capacitor
            impedance = 1./(wmin*xk[index]);
            if ((impedance < 1) && (simplify_network))
            {
                xk[index] = 100;
                removeElement(xk, index, i);
                CheckNetwork(xk, index, i);//Avoid network redundancies (parallel/series capacitors/inductances)
            }
            if (xk[index] < 0)//C must be > 0, so it seems that an inductor would do a better job
            {
                xk[index] = 5./(wmax);//Low impedance inductance
                topology[i] = '0';//Series inductance
                cout << "Warning: The selected topology leads to C < 0" <<endl;
                cout << "Warging: Topology changed. The new topology is: " << topology << endl;
            }
            index++;
            continue;
        case 3://Shunt capacitor
            impedance = 1./(wmax*xk[index]);
            if ((impedance > 4e3) && (simplify_network))
            {
                xk[index] = 1e-30;
                removeElement(xk, index, i);
                CheckNetwork(xk, index, i);//Avoid network redundancies (parallel/series capacitors/inductances)
            }
            if (xk[index] < 0)//C must be > 0, so it seems that an inductor would do a better job
            {
                xk[index] = 5./(wmax);//Low impedance inductance
                topology[i] = '2';//Shunt inductance
                cout << "Warning: The selected topology leads to C < 0" <<endl;
                cout << "Warging: Topology changed. The new topology is: " << topology << endl;
            }
            index++;
            continue;
        default://It is a transmission line or a stub
            if (xk[index] < 0)//Something is wrong...
            {
                xk[index] = .5*(mean(abs(ZS))+mean(abs(ZL)));
            }
            if (xk[index+1] < 0)
            {
                double lambda = c0/mean(freq);
                do{xk[index+1] += lambda/2;} while(xk[index+1]<=0);
            }
            index += 2;//Two parameters: Z0 and length


        }
    }
    return xk;
}

// This function creates the generating matrix
vector<vector<double>> GRABIM::GeneratingMatrix(unsigned int dim)
{
    vector<vector<double>> C(dim, vector<double>(pow(2, dim)+1));
    for (unsigned int i = 0; i <dim; i++)
    {
        vector<double> V(pow(2, (i+1))), aux(pow(2, dim));
        for (unsigned int u = 0; u < pow(2, i); u++) V[u] = 1;
        for (unsigned int u = pow(2,i); u < pow(2, i+1); u++) V[u] = -1;
        setRow(C, i, repvec(V, pow(2, dim-i-1)));
    }
    return C;
}


// Evaluates the objective function
double GRABIM::CandidateEval(vector<double> x)
{
    double fobj = -1e3;
    Mat S(2,2), ABCD(2,2);
    SparEngine S2PEngine;
    vector<complex<double>> s11 (freq.size());
    for (unsigned int i = 0; i < freq.size(); i++)
    {
        if (ObjFun == ObjectiveFunction::NINF_S11dB)
        {
            S = S2PEngine.getSparams(x, ZS.at(i), ZL.at(i), freq.at(i), topology);
            if (abs(S(0,0)) > fobj) fobj = abs(S(0,0));//N inf
        }
        if (ObjFun == ObjectiveFunction::NINF_POWERTRANS)
        {
            ABCD = S2PEngine.getABCDmatrix(x, freq.at(i), topology);
            fobj = CalcInvPowerTransfer(ABCD, ZS.at(i), ZL.at(i));
        }

    }
    if (ObjFun == ObjectiveFunction::NINF_S11dB)fobj = 20*log10(fobj);//|grad{log(x)}| > |grad{x}| when x < 1;
    return fobj;
}





// Sets the objective function. The problem can be thought in terms of either S11 or the inverse power transfer
int GRABIM::SetObjectiveFunction(ObjectiveFunction of)
{
    ObjFun = of;
    return 0;
}

//Returns the objective function
ObjectiveFunction GRABIM::GetObjectiveFunction()
{
    return ObjFun;
}

// Inverse power transfer from ABCD matrix
// [1] Eq (6.2.1), (6.2.2)
double GRABIM::CalcInvPowerTransfer(Mat ABCD, complex<double> ZS, complex<double> ZL)
{
    complex<double> p = real(ZS)*real(ZL) - imag(ZS)*imag(ZL);
    complex<double> q = imag(ZS)*real(ZL) + imag(ZL)*real(ZS);
    complex<double> a = ABCD(0,0)*real(ZL) - ABCD(1,0)*q + ABCD(1,1)*real(ZS);
    complex<double> b = ABCD(0,1) + ABCD(1,0)*p + ABCD(1,1)*imag(ZS) + ABCD(0,0)*imag(ZL);
    return abs((a*a + b*b)/(4*real(ZS)*real(ZL)));
}


// This function tries to give a good initial point for the grid search depending on the frequency.
void GRABIM::AutoSetInitialPivot()
{
    double meanf = .5*(min(freq)+max(freq));
    double meanw = 2*pi*meanf;
    double lambda4 = c0/(4.*meanf);
    double meanZ = 0.75*(mean(abs(ZL))+mean(abs(ZS)));
    queue <double> XINI;
    for (unsigned int i = 0; i< topology.size();i++)
    {
        if (!topology.substr(i,1).compare("0")) XINI.push(meanZ/meanw);
        if (!topology.substr(i,1).compare("2")) XINI.push(meanZ/meanw);
        if (!topology.substr(i,1).compare("1")) XINI.push(1/(meanZ*meanw));
        if (!topology.substr(i,1).compare("3")) XINI.push(1/(meanZ*meanw));
        if((!topology.substr(i,1).compare("5"))||(!topology.substr(i,1).compare("6")))XINI.push(mean(real(ZS)+real(ZL))),XINI.push(lambda4);

        if (!topology.substr(i,1).compare("4"))//Transmission line
        {
            // Here it is defined a linear impedance profile. In case the user selected 444, this tries
            // works real-to-real impedance transformer. Luckily, GRABIM will find a better result
            double Zi;
            double m = (mean(real(ZL))-mean(real(ZS)))/topology.length();

            Zi = m*i+mean(real(ZS));
            XINI.push(Zi);
            XINI.push(lambda4);
        }
    }
    x_ini = ones(XINI.size());
    for (unsigned int i = 0; i < x_ini.size();i++)
    {
        x_ini[i] = XINI.front();
        XINI.pop();
    }

    for (unsigned int i = 0; i < x_ini.size(); i++) x_ini[i] *= (.2*(rand() % 1000)/1e3)+0.9;//Randomizes the initial point

}

//Sets the path to the user-defined topology list
void GRABIM::setTopoScript(std::string path)
{
    TopoScript_path = path;
}


//This function checks whether a network has been modified or not, and, if it was, it
//reshapes the generating matrix and xkq
void GRABIM::CheckDim(vector<vector<double>> & C, vector<vector<double>> & xkq, vector<vector<double>> & best_pivot, unsigned int dim)
{
    if (xkq.size() != dim)//The current network has changed
    {
        C.resize(dim);
        for (unsigned int r = 0; r < dim; r++) C[r].resize(pow(2, dim)+1);
        C = GeneratingMatrix(dim);

        xkq.resize(pow(2, dim)+1);
        for (unsigned int r = 0; r < dim; r++) xkq[r].resize(dim);        

        for (unsigned int r = 0; r < 4; r++) best_pivot[r].resize(dim);    
    }
}


// This function sets the search mode:
// 0: Predefined circuit topologies. The ABCD response of these circuit topologies was calculated analitically, so the evaluation of the objective function is faster
// 1: User-defined topology list (plain text file).
// 2: LC networks up to 4 elements
// 3: LC + TL networks up to 6 elements
// 4: LC + TL + stubs networks up to 6 elements
void GRABIM::setSearchMode(int mode)
{
    string str;
    TopoList.clear();
    TagList.clear();
    list<string>::iterator it_topo, it_tag;
    it_topo = TopoList.begin();
    it_tag = TagList.begin();
    switch (mode)
    {
    case -1://Unique network (Only quick command line checks)
    {
      TopoList.insert(it_topo, topology), TagList.insert(it_tag, "User-defined network");
      return;
    }
    case 0://Preprogrammed networks
    {
        TopoList.insert(it_topo, "444"), TagList.insert(it_tag, "Cascaded transmission lines");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"020313"), TagList.insert(it_tag,"020313");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"313020"), TagList.insert(it_tag,"313020");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"03030"), TagList.insert(it_tag,"Lowpass LC 3 sections");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"313202"), TagList.insert(it_tag,"313202");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"202313"), TagList.insert(it_tag,"202313");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"03030"), TagList.insert(it_tag,"03030");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"43434"), TagList.insert(it_tag,"43434");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"01230123"), TagList.insert(it_tag,"01230123");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"23012301"), TagList.insert(it_tag,"23012301");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"12031203"), TagList.insert(it_tag,"12031203");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"21302130"), TagList.insert(it_tag,"21302130");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"03120312"), TagList.insert(it_tag,"03120312");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"30213021"), TagList.insert(it_tag,"30213021");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"30303030"), TagList.insert(it_tag,"30303030");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"03030303"), TagList.insert(it_tag,"Lowpass LC 4 sections");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"12121212"), TagList.insert(it_tag,"12121212");
        it_topo++, it_tag++;
        TopoList.insert(it_topo,"21212121"), TagList.insert(it_tag,"21212121");
        it_topo++, it_tag++;
        return;
    }
    case 1://User-defined list
    {
        if (topology.compare("-1"))
        {
            TopoList.insert(it_topo, topology);
            TagList.insert(it_tag, "");
            return;//Command input
        }
        std::ifstream TopologiesFile(TopoScript_path);//Tries to open the file.
        std::string tag, aux;
        if(TopologiesFile.is_open())//The data file cannot be opened => error
        {
            while (std::getline(TopologiesFile, aux))
            {
                if (aux.empty())continue;
                if (aux.substr(0,1).compare("#")==0)
                {
                    tag = aux;
                    continue;
                }
                TopoList.insert(it_topo, aux);
                TagList.insert(it_tag, tag);
                it_topo++;
                it_tag++;
            }
            return;
        }//If either topology == -1 or the input file cannot be opened, it tries the prefined networks
    }
    case 2://LC networks up to 4 elements
    {
        for (unsigned int i1 = 0; i1 < 4; i1++)
        {
            for (unsigned int i2 = 0; i2 < 4; i2++)
            {
                for (unsigned int i3 = 0; i3 < 4; i3++)
                {
                    for (unsigned int i4 = 0; i4 < 4; i4++)
                    {
                        str.clear();
                        str = to_string(i1) + to_string(i2) + to_string(i3) + to_string(i4);
                        TopoList.insert(it_topo, str);
                        TagList.insert(it_tag, str);
                        it_topo++, it_tag++;
                    }
                }
            }
        }
        return;
    }
    case 3: //LC + TL networks up to 6 elements
    {
        for (unsigned int i1 = 0; i1 < 5; i1++)
        {
            for (unsigned int i2 = 0; i2 < 5; i2++)
            {
                for (unsigned int i3 = 0; i3 < 5; i3++)
                {
                    for (unsigned int i4 = 0; i4 < 5; i4++)
                    {
                        for (unsigned int i5 = 0; i5 < 5; i5++)
                        {
                            for (unsigned int i6 = 0; i6 < 5; i6++)
                            {
                                str.clear();
                                str = to_string(i1) + to_string(i2) + to_string(i3) + to_string(i4) + to_string(i5)+ to_string(i6);
                                TopoList.insert(it_topo, str);
                                TagList.insert(it_tag, str);
                                it_topo++, it_tag++;
                            }
                        }
                    }
                }
            }
        }
        return;
    }
    case 4: //LC + TL + stubs networks up to 6 elements
    {
        for (unsigned int i1 = 0; i1 < 7; i1++)
        {
            for (unsigned int i2 = 0; i2 < 7; i2++)
            {
                for (unsigned int i3 = 0; i3 < 7; i3++)
                {
                    for (unsigned int i4 = 0; i4 < 7; i4++)
                    {
                        for (unsigned int i5 = 0; i5 < 7; i5++)
                        {
                            for (unsigned int i6 = 0; i6 < 7; i6++)
                            {
                                str.clear();
                                str = to_string(i1) + to_string(i2) + to_string(i3) + to_string(i4) + to_string(i5)+ to_string(i6);
                                TopoList.insert(it_topo, str);
                                TagList.insert(it_tag, str);
                                it_topo++, it_tag++;
                            }
                        }
                    }
                }
            }
        }
        return;
    }

    }
}

// Allows simplifying circuit topology
void GRABIM::SimplifyNetwork(bool simplify_mode)
{
    simplify_network = simplify_mode;
}

// This function is intended for debugging purposes. It prints the simplex of the Nelder-Mead algorithm
void printSimplex(vector<vector<double>>X, vector<double>fx)
{
cout << "SIMPLEX" << endl;
for (int i = 0; i < X.size(); i++)
{
  for (int j = 0; j < X[0].size(); j++) cout << X[i][j]<<" ";
  cout << endl;
}

cout << "FX: " <<endl;
  for (int j = 0; j < fx.size(); j++) cout << fx[j]<<" ";
  cout << endl;

}
//Fills the simplex with vectors in the near of x
void GRABIM::NM_initializeSimplex(vector<vector<double>> & X, vector<double> & fx, vector<double> x)
{
 //The first vector is the initial point
 for (unsigned int c = 0; c < x.size(); c++) X[0][c] = x[c];
 fx[0] = CandidateEval(X[0]);

 for (unsigned int r = 1; r < x.size()+1; r++)
    {
       for (unsigned int c = 0; c < x.size(); c++)
       {
        X[r][c] = x[c] * (0.9+(rand() % 200)/1000.);
       }
       fx[r] = CandidateEval(X[r]);
    }
}


void GRABIM::EvaluateSimplex(vector<vector<double>> X, vector<double> & fx)
{
  unsigned int N_ = fx.size();
  for (unsigned int i = 0; i < N_; i++)
  {
    fx[i] = CandidateEval(getRow(X, i));
  }
}

void NM_sortSimplex(vector<vector<double>> & X, vector<double> & fx)
{
 unsigned int N_ = fx.size();
 vector<unsigned int> aux_pos(N_);//Contains the ordered indices of fx
 vector<double> f_aux(N_);
 vector<vector<double>> X_aux(N_, vector<double>(N_-1));
 
 for (unsigned int i = 0; i < N_; i++)
 { 
    aux_pos[i] = findMin(fx);
    f_aux[i] = fx[aux_pos[i]];
    fx[aux_pos[i]] = 1e30;
    for (unsigned int c = 0; c < N_-1; c++) X_aux[i][c] = X[aux_pos[i]][c];
 }
 
 for (unsigned int i = 0; i < N_; i++)//Copy the auxiliar variables into the original ones
 {
    for (unsigned int c = 0; c < N_-1; c++) X[i][c] = X_aux[i][c];
    fx[i] = f_aux[i];
 } 

}


vector<vector<double>> getBestVertices(vector<vector<double>> X)
{
   unsigned int N = X.size();
  vector<vector<double>> AUX_X(N, vector<double>(N));
   for (unsigned int i = 0; i < N; i++)
   {
      for (unsigned int j=0; j < N; j++)
      {
        AUX_X[i][j] = X[i][j];
      }
   }
return AUX_X;
}


//Reference:
// [1] Convergence properties of the Nelder-Mead simplex method in low dimensions. J.F. Lagarias, J.A. Reeds. SIAM J. OPTIM, Vol 9, No. 1, pp. 112-147
vector<double> GRABIM::NelderMead(vector<double> x)
{
  unsigned int n = x.size();
  unsigned int iter = 0, max_iter = 500;
  double grad = 1e-8;//Step gradient threshold
  double grad_aux;
  double alpha = 1., beta = 2., gamma = 1/2., delta = 1/2.;//Classic coefficients
  double fr, fe, foc, fic, f1, fn, fn_1;
  double f_pre, f_curr;
  bool shrink = false, stopcondition=false;
  vector<vector<double>> X(n+1, vector<double>(n));//Simplex
  vector<double> fx(n+1);
  vector<double> xr(n), xe(n), xoc(n), xic(n), xi(n), x1(n);
  vector<double> c_(n);

  NM_initializeSimplex(X, fx, x);//Initializes & evaluates the simplex

  while ((fx[0] > -40) && (iter < max_iter))
  {iter++;
  (iter != 1) ? f_pre = fx[0] : f_pre=1e30;
  EvaluateSimplex(X, fx);
  NM_sortSimplex(X, fx);
  f_curr = fx[0];
  grad_aux=abs(f_curr - f_pre);
 // if (grad_aux<grad && iter>50) break; //Convergence achieved

  cout << "Best value ["<< iter<< "]: " << f_curr << "dB" <<endl;
  shrink=false;
  //Reflection
  c_=centroid(getBestVertices(X));
  xr = c_ + alpha*(c_ - getRow(X, n));
  fr = CandidateEval(xr);
  f1 = CandidateEval(getRow(X, 0));
  fn = CandidateEval(getRow(X, n-1));
  fn_1 = CandidateEval(getRow(X, n));

  if ((f1 <= fr)&&(fr<fn)) 
  { //cout << "ACCEPT REFLECTION: " << endl;
   setRow(X, n, xr);
   continue;//Accept the point and terminate the iteration
  }

  if (fr < f1)//Expansion
  {
     xe = c_ + beta*(xr - c_);
     fe = CandidateEval(xe);
     if (fe < fr)
     { //cout << "ACCEPT EXPANSION" << endl;
        setRow(X, n, xe);
        continue;
     }
     else
     { //cout << "ACCEPT REFLECTION 2" << endl;
       setRow(X, n, xr);
       continue;
     } 
  }
  
  if ((fn <= fr) && (fr < fn_1))//Outside contraction
  {
     xoc = c_ + gamma*(xr - c_);
     foc = CandidateEval(xoc);
     if (foc <= fr)
     {//cout << "ACCEPT OC" << endl;
        setRow(X, n, xoc);
        continue;
     }
     else
     {
        shrink = true;
     }
  }
  if ((!shrink) && (fr >= fn_1))//Inner contraction
  {
    xic = c_ - gamma*(c_ - xr);
    fic = CandidateEval(xic);
    if (fic < fn_1)
    {//cout << "ACCEPT IC" << endl;
        setRow(X, n, xic);
        continue;
    }
    else
    {
       shrink = true;
    }
  }

  if (shrink)
  {//cout << "SHRINK" << endl;
    x1 = getRow(X, 0);
    for (unsigned int i = 1; i <= n; i++)
    {
        xi = getRow(X, i);
        xi = x1 + delta*(xi - x1);
        setRow(X, i, xi);
    }
  }
 }
 cout << "STOP CONDITION: fmin=" << fx[0] << " gradient=" << grad_aux << " Niter=" << iter << endl;
 return getRow(X, 0);
}



