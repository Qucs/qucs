/*
 * e_trsolver.cpp - external transient solver interface class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "compat.h"
#include "object.h"
#include "logging.h"
#include "complex.h"
#include "circuit.h"
#include "sweep.h"
#include "net.h"
#include "netdefs.h"
#include "analysis.h"
#include "nasolver.h"
#include "history.h"
#include "transient.h"
#include "exception.h"
#include "exceptionstack.h"
#include "environment.h"
#include "e_trsolver.h"

#define STEPDEBUG   0 // set to zero for release
#define BREAKPOINTS 0 // exact breakpoint calculation

#ifndef dState
#define dState 0 // delta T state
#endif

#ifndef sState
#define sState 1 // solution state
#endif

using namespace transient;

// Constructor creates an unnamed instance of the trsolver class.
e_trsolver::e_trsolver ()
    : trsolver ()
{
    //swp = NULL;
    type = ANALYSIS_E_TRANSIENT;
    //setDescription ("m-transient");
    //for (int i = 0; i < 8; i++) solution[i] = NULL;
    //tHistory = NULL;
    //relaxTSR = false;
    //initialDC = true;

    // initialise the message function pointer to
    // point to the PrintWarningMsg function
    messagefcn = &logprint;
#if DEBUG
    //loginit ();
    // produce an actual log file
//    file_error = file_status = fopen("e_trsolver.log", "w+");
//    precinit ();
//    ::srand (::time (NULL));
#endif // DEBUG
}

// Constructor creates a named instance of the e_trsolver class.
e_trsolver::e_trsolver (char * n)
    : trsolver (n)
{
    //swp = NULL;
    type = ANALYSIS_E_TRANSIENT;
    //setDescription ("m-transient");
    //for (int i = 0; i < 8; i++) solution[i] = NULL;
    //tHistory = NULL;
    //relaxTSR = false;
    //initialDC = true;

    // initialise the message function pointer to
    // point to the PrintWarningMsg function
    messagefcn = &logprint;
}

// Destructor deletes the e_trsolver class object.
e_trsolver::~e_trsolver ()
{
    deinitTR ();

    if (swp) delete swp;

    for (int i = 0; i < 8; i++)
    {
        if (solution[i] != NULL)
        {
            delete solution[i];
        }
    }
    if (tHistory) delete tHistory;

}

/* The copy constructor creates a new instance of the e_trsolver class
   based on the given e_trsolver object. */
e_trsolver::e_trsolver (e_trsolver & o)
    : trsolver (o)
{
    swp = o.swp ? new sweep (*o.swp) : NULL;
    for (int i = 0; i < 8; i++) solution[i] = NULL;
    tHistory = o.tHistory ? new history (*o.tHistory) : NULL;
    relaxTSR = o.relaxTSR;
    initialDC = o.initialDC;
}

// This function creates the time sweep if necessary.
void e_trsolver::initSteps (void)
{
    if (swp != NULL) delete swp;
    swp = createSweep ("time");
}

// Macro for the n-th state of the solution vector history.
#define SOL(state) (solution[(int) getState (sState, (state))])

//int e_trsolver::prepare_net (char * infile)
//{
//
//    // create static modules
//    module::registerModules ();
//
//    // create root environment
//    root = new environment ("root");
//
//    // create netlist object and input
//    subnet = new net ("subnet");
//    in = infile ? new input (infile) : new input ();
//
//    // pass root environment to netlist object and input
//    subnet->setEnv (root);
//    in->setEnv (root);
//
//    // get input netlist
//    if (in->netlist (subnet) != 0)
//    {
//        if (netlist_check)
//        {
//            // replace with mex error message
//            messagefcn (LOG_STATUS, "checker notice, netlist check FAILED\n");
//        }
//        return -1;
//    }
//    if (netlist_check)
//    {
//        messagefcn (LOG_STATUS, "checker notice, netlist OK\n");
//        return -2;
//    }
//
//    // attach a ground to the netlist
//    gnd = new ground ();
//    gnd->setNode (0, "gnd");
//    gnd->setName ("GND");
//    subnet->insertCircuit (gnd);
//
//    // apply the netlist to this solver
//    //this->setNet (subnet);
//
//    return 0;
//}

void e_trsolver::debug()
{
    circuit * root = subnet->getRoot ();

    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        messagefcn (0, c->getName() );
    }

    //netlist_list();
}


/* This is the initialisation function for the slaved transient
   netlist solver. It prepares the circuit for simulation. */
int e_trsolver::init (nr_double_t start, nr_double_t firstdelta, int mode)
{
    // prepare the netlist
    //prepare_net (infile);

    // run the equation solver for the netlist
    this->getEnv()->runSolver();

    int error = 0;
    char * solver = getPropertyString ("Solver");
    relaxTSR = !strcmp (getPropertyString ("relaxTSR"), "yes") ? true : false;
    initialDC = !strcmp (getPropertyString ("initialDC"), "yes") ? true : false;
    // fetch simulation properties
//    MaxIterations = getPropertyInteger ("MaxIter");
//    reltol = getPropertyDouble ("reltol");
//    abstol = getPropertyDouble ("abstol");
//    vntol = getPropertyDouble ("vntol");

    runs++;
    saveCurrent = current = 0;
    stepDelta = -1;
    converged = 0;
    fixpoint = 0;
    lastsynctime = 0.0;
    statRejected = statSteps = statIterations = statConvergence = 0;

    // Choose a solver.
    if (!strcmp (solver, "CroutLU"))
        eqnAlgo = ALGO_LU_DECOMPOSITION;
    else if (!strcmp (solver, "DoolittleLU"))
        eqnAlgo = ALGO_LU_DECOMPOSITION_DOOLITTLE;
    else if (!strcmp (solver, "HouseholderQR"))
        eqnAlgo = ALGO_QR_DECOMPOSITION;
    else if (!strcmp (solver, "HouseholderLQ"))
        eqnAlgo = ALGO_QR_DECOMPOSITION_LS;
    else if (!strcmp (solver, "GolubSVD"))
        eqnAlgo = ALGO_SV_DECOMPOSITION;

    // Perform initial DC analysis.
    if (initialDC)
    {
        error = dcAnalysis ();
        if (error)
            return -1;
    }

    // Initialize transient analysis.
    setDescription ("transient");
    initETR (start, firstdelta, mode);
    setCalculation ((calculate_func_t) &calcTR);
    solve_pre ();

    // Create time sweep if necessary.
    //initSteps ();
    //swp->reset ();

    // Recall the DC solution.
    recallSolution ();

    // Apply the nodesets and adjust previous solutions.
    applyNodeset (false);
    fillSolution (x);

    // Tell integrators to be initialized.
    setMode (MODE_INIT);

    // reset the circuit status to not running so the histories
    // etc will be reinitialised on the first time step
    running = 0;
    rejected = 0;
    if (mode == ETR_MODE_ASYNC)
    {
        delta /= 10;
    }
    else if (mode == ETR_MODE_SYNC)
    {
        //lastsynctime = start - delta;
    }
    fillState (dState, delta);
    adjustOrder (1);

    return 0;

}

/* Goes through the list of circuit objects and runs its initTR()
   function. */
void e_trsolver::initETR (nr_double_t start, nr_double_t firstdelta, int mode)
{
    char * IMethod = getPropertyString ("IntegrationMethod");
    //nr_double_t start = getPropertyDouble ("Start");
    //nr_double_t stop = start + firstdelta;
    //nr_double_t points = 1.0;

    // fetch corrector integration method and determine predicor method
    corrMaxOrder = getPropertyInteger ("Order");
    corrType = CMethod = correctorType (IMethod, corrMaxOrder);
    predType = PMethod = predictorType (CMethod, corrMaxOrder, predMaxOrder);
    corrOrder = corrMaxOrder;
    predOrder = predMaxOrder;

    // initialize step values
    if (mode == ETR_MODE_ASYNC){
        delta = getPropertyDouble ("InitialStep");
        deltaMin = getPropertyDouble ("MinStep");
        deltaMax = getPropertyDouble ("MaxStep");
        if (deltaMax == 0.0)
            deltaMax = firstdelta; // MIN ((stop - start) / (points - 1), stop / 200);
        if (deltaMin == 0.0)
            deltaMin = NR_TINY * 10 * deltaMax;
        if (delta == 0.0)
            delta = firstdelta; // MIN (stop / 200, deltaMax) / 10;
        if (delta < deltaMin) delta = deltaMin;
        if (delta > deltaMax) delta = deltaMax;
    }
    else if (mode == ETR_MODE_SYNC)
    {
        delta = firstdelta;
        deltaMin = NR_TINY * 10;
        deltaMax = NR_MAX / 10;
    }

    // initialize step history
    setStates (2);
    initStates ();
    // initialise the history of states, setting them all to 'delta'
    fillState (dState, delta);

    // copy the initialised states to the 'deltas' array
    saveState (dState, deltas);
    // copy the deltas to all the circuits
    setDelta ();
    // set the initial corrector and predictor coefficients
    calcCorrectorCoeff (corrType, corrOrder, corrCoeff, deltas);
    calcPredictorCoeff (predType, predOrder, predCoeff, deltas);

    // initialize history of solution vectors (solutions)
    for (int i = 0; i < 8; i++)
    {
        // solution contains the last sets of node voltages and branch
        // currents at each of the last 8 'deltas'.
        // Note for convenience the definition:
        //   #define SOL(state) (solution[(int) getState (sState, (state))])
        // is provided and used elsewhere to update the solutions
        solution[i] = new tvector<nr_double_t>;
        setState (sState, (nr_double_t) i, i);
    }

    // tell circuits about the transient analysis
    circuit *c, * root = subnet->getRoot ();
    for (c = root; c != NULL; c = (circuit *) c->getNext ())
        initCircuitTR (c);
    // also initialize created circuits
    for (c = root; c != NULL; c = (circuit *) c->getPrev ())
        initCircuitTR (c);
}

void e_trsolver::printx()
{
    char buf [1024];

    for (int r = 0; r < x->getSize(); r++) {
        buf[0] = '\0';
        //sprintf (buf, "%+.2e%+.2ei", (double) real (x->get (r)), (double) imag (x->get (r)));

        if (r == 2)
        {
            // print just the currents
//            SOL (0)->get->(r)
//            solution[0]->get(r);
            sprintf (buf, "%f\t%18.17f\t%6.5f %6.5f %6.5f %6.5f %6.5f %6.5f %6.5f %6.5f",
                     current,
                     (double) real (x->get (r)),
                     solution[0]->get(r) ,
                     solution[1]->get(r) ,
                     solution[2]->get(r) ,
                     solution[3]->get(r) ,
                     solution[4]->get(r) ,
                     solution[5]->get(r) ,
                     solution[6]->get(r) ,
                     solution[7]->get(r) );

            messagefcn(0, buf);
        }
    }
}

/* synchronous step solver for external ode routine
 *
 * This function solves the circuit for a single time delta provided
 * by an external source. Convergence issues etc. are expected to
 * be handled by the external solver, as it is in full control of the
 * time stepping.
 */
int e_trsolver::stepsolve_sync(nr_double_t synctime)
{

    int error = 0;
    convError = 0;

    time = synctime;

    // copy the externally chosen time step to delta
    delta = time - lastsynctime;

    // get the current solution time
    //current += delta;

    // updates the integrator coefficients, and updates the array of prev
    // 8 deltas with the new delta for this step
    updateCoefficients (delta);

    // Run predictor to get a start value for the solution vector for
    // the successive iterative corrector process
    error += predictor ();

    // restart Newton iteration
    restart (); // restart non-linear devices

    // Attempt to solve the circuit with the given delta
    try_running () // #defined as:    do {
    {
        //error += solve_nonlinear_step ();
        error += corrector ();
    }
    catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
    {
    case EXCEPTION_NO_CONVERGENCE:
        pop_exception ();

        // Retry using damped Newton-Raphson.
        this->convHelper = CONV_SteepestDescent;
        convError = 2;
#if DEBUG
        messagefcn (LOG_ERROR, "WARNING: delta rejected at t = %.3e, h = %.3e "
                  "(no convergence)\n", (double) saveCurrent, (double) delta);
#endif

        try_running () // #defined as:    do {
        {
//            error += solve_nonlinear_step ();
            error += solve_nonlinear ();
        }
        catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
        {
        case EXCEPTION_NO_CONVERGENCE:
            pop_exception ();

            // Update statistics.
            statRejected++;
            statConvergence++;
            rejected++;
            converged = 0;
            error = 0;

            break;
        default:
            // Otherwise return.
            estack.print ();
            error++;
            break;
        }

        // Update statistics and no more damped Newton-Raphson.
//        statIterations += iterations;
//        if (--convError < 0) this->convHelper = 0;


        break;
    default:
        // Otherwise return.
        estack.print ();
        error++;
        break;
    }
    // if there was an error other than non-convergence, return -1
    if (error) return -1;

    // check whether Jacobian matrix is still non-singular
    if (!A->isFinite ())
    {
//        messagefcn (LOG_ERROR, "ERROR: %s: Jacobian singular at t = %.3e, "
//                  "aborting %s analysis\n", getName (), (double) current,
//                  getDescription ());
        return -1;
    }

    return 0;
}

// accept a time step into the solution history
void e_trsolver::acceptstep_sync()
{
    statIterations += iterations;
    if (--convError < 0) convHelper = 0;

    // Now advance in time or not...
    if (running > 1)
    {
        adjustDelta_sync (current);
//        deltaOld = delta;
//        stepDelta = deltaOld;
//        nextStates ();
//        rejected = 0;

        adjustOrder ();
    }
    else
    {
        fillStates ();
        nextStates ();
        rejected = 0;
    }

    saveCurrent = current;
    current += delta;
    running++;
    converged++;

    // Tell integrators to be running.
    setMode (MODE_NONE);

    // Initialize or update history.
    if (running > 1)
    {
        // make the circuit histories no longer than the total of
        // all the stored deltas in the solution history
        nr_double_t deltasum = 0.0;
        for (int i = 0; i<8; i++) deltasum += deltas[i];
        tHistory->setAge (deltasum);
        // update the solution history with the new results
        updateHistory (current);
    }
    else
    {
        // we have just solved the first transient state
        initHistory (current);
    }

    // store the current time
    lastsynctime = current;
}

/* This function tries to adapt the current time-step according to the
   global truncation error. */
void e_trsolver::adjustDelta_sync (nr_double_t t)
{
    deltaOld = delta;

    // makes a new delta based on truncation error
//    delta = checkDelta ();

    if (delta > deltaMax)
    {
        delta = deltaMax;
    }

    if (delta < deltaMin)
    {
        delta = deltaMin;
    }

    // delta correction in order to hit exact breakpoint
    int good = 0;
//    if (!relaxTSR)   // relaxed step raster?
//    {
//        if (!statConvergence || converged > 64)   /* Is this a good guess? */
//        {
//            // check next breakpoint
//            if (stepDelta > 0.0)
//            {
//                // restore last valid delta
//                delta = stepDelta;
//                stepDelta = -1.0;
//            }
//            else
//            {
//                if (delta > (t - current) && t > current)
//                {
//                    // save last valid delta and set exact step
//                    stepDelta = deltaOld;
//                    delta = t - current;
//                    good = 1;
//                }
//                else
//                {
//                    stepDelta = -1.0;
//                }
//            }
//            if (delta > deltaMax) delta = deltaMax;
//            if (delta < deltaMin) delta = deltaMin;
//        }
//    }

    stepDelta = -1;
    good = 1;

    // usual delta correction
    if (delta > 0.9 * deltaOld || good)   // accept current delta
    {
        nextStates ();
        rejected = 0;
#if STEPDEBUG
        logprint (LOG_STATUS,
                  "DEBUG: delta accepted at t = %.3e, h = %.3e\n",
                  (double) current, (double) delta);
#endif
    }
    else if (deltaOld > delta)   // reject current delta
    {
        rejected++;
        statRejected++;
#if STEPDEBUG
        logprint (LOG_STATUS,
                  "DEBUG: delta rejected at t = %.3e, h = %.3e\n",
                  (double) current, (double) delta);
#endif
        if (current > 0) current -= deltaOld;
    }
    else
    {
        nextStates ();
        rejected = 0;
    }
}

// asynchronous step solver
int e_trsolver::stepsolve_async(nr_double_t steptime)
{
    // Start to sweep through time.
    int error = 0;
    convError = 0;

    time = steptime;


    //delta = (steptime - time) / 10;
    //if (progress) logprogressbar (i, swp->getSize (), 40);

#if DEBUG && 0
    messagefcn (LOG_STATUS, "NOTIFY: %s: solving netlist for t = %e\n",
              getName (), (double) time);
#endif

    do
    {
#if STEPDEBUG
        if (delta == deltaMin)
        {
            messagefcn (LOG_ERROR,
                      "WARNING: %s: minimum delta h = %.3e at t = %.3e\n",
                      getName (), (double) delta, (double) current);
        }
#endif
        updateCoefficients (delta);

        // Run predictor to get a start value for the solution vector for
        // the successive iterative corrector process
        error += predictor ();

        // restart Newton iteration
        if (rejected)
        {
            restart ();      // restart non-linear devices
            rejected = 0;
        }

        // Run corrector process with appropriate exception handling.
        // The corrector iterates through the solutions of the integration
        // process until a certain error tolerance has been reached.
        try_running () // #defined as:    do {
        {
            error += corrector ();
        }
        catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
        {
        case EXCEPTION_NO_CONVERGENCE:
            pop_exception ();

            // Reduce step-size (by half) if failed to converge.
            if (current > 0) current -= delta;
            delta /= 2;
            if (delta <= deltaMin)
            {
                delta = deltaMin;
                adjustOrder (1);
            }
            if (current > 0) current += delta;

            // Update statistics.
            statRejected++;
            statConvergence++;
            rejected++;
            converged = 0;
            error = 0;

            // Start using damped Newton-Raphson.
            convHelper = CONV_SteepestDescent;
            convError = 2;
#if DEBUG
            messagefcn (LOG_ERROR, "WARNING: delta rejected at t = %.3e, h = %.3e "
                      "(no convergence)\n", (double) saveCurrent, (double) delta);
#endif
            break;
        default:
            // Otherwise return.
            estack.print ();
            error++;
            break;
        }
        if (error) return -1;
        if (rejected) continue;

        // check whether Jacobian matrix is still non-singular
        if (!A->isFinite ())
        {
            messagefcn (LOG_ERROR, "ERROR: %s: Jacobian singular at t = %.3e, "
                      "aborting %s analysis\n", getName (), (double) current,
                      getDescription ());
            return -1;
        }

        // Update statistics and no more damped Newton-Raphson.
        statIterations += iterations;
        if (--convError < 0) convHelper = 0;

        // Now advance in time or not...
        if (running > 1)
        {
            adjustDelta (time);
            adjustOrder ();
        }
        else
        {
            fillStates ();
            nextStates ();
            rejected = 0;
        }

        saveCurrent = current;
        current += delta;
        running++;
        converged++;

        // Tell integrators to be running.
        setMode (MODE_NONE);

        // Initialize or update history.
        if (running > 1)
        {
            updateHistory (saveCurrent);
        }
        else
        {
            initHistory (saveCurrent);
        }
    }
    while (saveCurrent < time); // Hit a requested time point?

    return 0;
}

int e_trsolver::finish()
{
    solve_post ();

    if (progress) logprogressclear (40);

    messagefcn (LOG_STATUS, "NOTIFY: %s: average time-step %g, %d rejections\n",
              getName (), (double) (saveCurrent / statSteps), statRejected);
    messagefcn (LOG_STATUS, "NOTIFY: %s: average NR-iterations %g, "
              "%d non-convergences\n", getName (),
              (double) statIterations / statSteps, statConvergence);

    // cleanup
    return 0;
}

///* The single step non-linear iterative nodal analysis netlist solver. */
//int e_trsolver::solve_nonlinear_step (void)
//{
//    int convergence, run = 0, error = 0;
//    qucs::exception * e;
//
//    // fetch simulation properties
//    MaxIterations = getPropertyInteger ("MaxIter");
//    reltol = getPropertyDouble ("reltol");
//    abstol = getPropertyDouble ("abstol");
//    vntol = getPropertyDouble ("vntol");
//    updateMatrix = 1;
//
//    if (convHelper == CONV_GMinStepping)
//    {
//        // use the alternative non-linear solver solve_nonlinear_continuation_gMin
//        // instead of the basic solver provided by this function
//        iterations = 0;
//        error = solve_nonlinear_continuation_gMin ();
//        return error;
//    }
//    else if (convHelper == CONV_SourceStepping)
//    {
//        // use the alternative non-linear solver solve_nonlinear_continuation_Source
//        // instead of the basic solver provided by this function
//        iterations = 0;
//        error = solve_nonlinear_continuation_Source ();
//        return error;
//    }
//
//    // run solving loop until convergence is reached
//    do
//    {
//        error = solve_once ();
//        if (!error)
//        {
//            // convergence check
//            convergence = (run > 0) ? checkConvergence () : 0;
//            savePreviousIteration ();
//            run++;
//            // control fixpoint iterations
//            if (fixpoint)
//            {
//                if (convergence && !updateMatrix)
//                {
//                    updateMatrix = 1;
//                    convergence = 0;
//                }
//                else
//                {
//                    updateMatrix = 0;
//                }
//            }
//        }
//        else
//        {
//            break;
//        }
//    }
//    while (!convergence &&
//            run < MaxIterations * (1 + this->convHelper ? 1 : 0));
//
//    if (run >= MaxIterations || error)
//    {
//        e = new qucs::exception (EXCEPTION_NO_CONVERGENCE);
//        e->setText ("no convergence in %s analysis after %d iterations",
//                    desc, run);
//        throw_exception (e);
//        error++;
//    }
//
//    iterations = run;
//    return error;
//}

void e_trsolver::getsolution(double * lastsol)
{
    int N = countNodes ();
    int M = countVoltageSources ();

    // copy solution
    for (int r = 0; r < N + M; r++)
    {
        lastsol[r]  = real(x->get(r));
    }
}

int e_trsolver::getN()
{
    return countNodes ();
}

int e_trsolver::getM()
{
    return countVoltageSources ();
}

int e_trsolver::getJacRows()
{
    return A->getRows();
}

int e_trsolver::getJacCols()
{
    return A->getCols();
}

double e_trsolver::getJacData(int r, int c)
{
    return A->get(r,c);
}

// properties
PROP_REQ [] =
{
    {
        "Type", PROP_STR, { PROP_NO_VAL, "lin" },
        PROP_RNG_STR2 ("lin", "log")
    },
    PROP_NO_PROP
};
PROP_OPT [] =
{
    {
        "IntegrationMethod", PROP_STR, { PROP_NO_VAL, "Trapezoidal" },
        PROP_RNG_STR4 ("Euler", "Trapezoidal", "Gear", "AdamsMoulton")
    },
    { "Order", PROP_INT, { 2, PROP_NO_STR }, PROP_RNGII (1, 6) },
    { "InitialStep", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
    { "MinStep", PROP_REAL, { 1e-16, PROP_NO_STR }, PROP_POS_RANGE },
    { "MaxStep", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
    { "MaxIter", PROP_INT, { 150, PROP_NO_STR }, PROP_RNGII (2, 10000) },
    { "abstol", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_RNG_X01I },
    { "vntol", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_RNG_X01I },
    { "reltol", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_RNG_X01I },
    { "LTEabstol", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_RNG_X01I },
    { "LTEreltol", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_RNG_X01I },
    { "LTEfactor", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1, 16) },
    { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
    { "Solver", PROP_STR, { PROP_NO_VAL, "CroutLU" }, PROP_RNG_SOL },
    { "relaxTSR", PROP_STR, { PROP_NO_VAL, "no" }, PROP_RNG_YESNO },
    { "initialDC", PROP_STR, { PROP_NO_VAL, "yes" }, PROP_RNG_YESNO },
    PROP_NO_PROP
};
struct define_t e_trsolver::anadef =
    { "ETR", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };



