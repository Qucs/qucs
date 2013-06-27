
#include "qucs_interface.h"
#include "e_trsolver.h"
#include "mex.h"

#ifndef MEXTRSOLVER_H
#define MEXTRSOLVER_H

// mex interface class
class mextrsolver
{
    public:
        mextrsolver();
        virtual ~mextrsolver();

        // the one and only trsolver object to which the class
        // will interface
        qucsint thequcsint;
        e_trsolver * thetrsolver;

        // test method
        void debug(void);
        void printx(void);

        // the interface methods
        int prepare_netlist(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void init_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void init_async(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int stepsolve_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        void acceptstep_sync(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int getsolution(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int getN(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
        int getM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

    protected:

    private:

};

// function to display messages
void mextrsolvermessage(int, const char*, ...);

#endif // MEXTRSOLVER_H
