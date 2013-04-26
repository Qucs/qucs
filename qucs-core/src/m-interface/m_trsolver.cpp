/*
 * m_trsolver.cpp - transient solver class implementation
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
#include "m_trsolver.h"
#include "transient.h"
#include "exception.h"
#include "exceptionstack.h"

#define STEPDEBUG   0 // set to zero for release
#define BREAKPOINTS 0 // exact breakpoint calculation

#define dState 0 // delta T state
#define sState 1 // solution state

using namespace transient;

// Constructor creates an unnamed instance of the trsolver class.
m_trsolver::m_trsolver ()
    : nasolver<nr_double_t> (), states<nr_double_t> ()
{
    swp = NULL;
    type = ANALYSIS_TRANSIENT;
    setDescription ("transient");
    for (int i = 0; i < 8; i++) solution[i] = NULL;
    tHistory = NULL;
    relaxTSR = false;
    initialDC = true;
}

// Constructor creates a named instance of the m_trsolver class.
m_trsolver::m_trsolver (char * n)
    : nasolver<nr_double_t> (n), states<nr_double_t> ()
{
    swp = NULL;
    type = ANALYSIS_TRANSIENT;
    setDescription ("transient");
    for (int i = 0; i < 8; i++) solution[i] = NULL;
    tHistory = NULL;
    relaxTSR = false;
    initialDC = true;
}

// Destructor deletes the m_trsolver class object.
m_trsolver::~m_trsolver ()
{
    if (swp) delete swp;
    for (int i = 0; i < 8; i++) if (solution[i] != NULL) delete solution[i];
    if (tHistory) delete tHistory;
}

/* The copy constructor creates a new instance of the m_trsolver class
   based on the given m_trsolver object. */
m_trsolver::m_trsolver (m_trsolver & o)
    : nasolver<nr_double_t> (o), states<nr_double_t> (o)
{
    swp = o.swp ? new sweep (*o.swp) : NULL;
    for (int i = 0; i < 8; i++) solution[i] = NULL;
    tHistory = o.tHistory ? new history (*o.tHistory) : NULL;
    relaxTSR = o.relaxTSR;
    initialDC = o.initialDC;
}

// This function creates the time sweep if necessary.
void m_trsolver::initSteps (void)
{
    if (swp != NULL) delete swp;
    swp = createSweep ("time");
}

// Macro for the n-th state of the solution vector history.
#define SOL(state) (solution[(int) getState (sState, (state))])

/* This is the initialisation function for the slaved transient
   netlist solver. It prepares the circuit for simulation. */
int m_trsolver::init (void)
{

    int error = 0, convError = 0;
    char * solver = getPropertyString ("Solver");
    relaxTSR = !strcmp (getPropertyString ("relaxTSR"), "yes") ? true : false;
    initialDC = !strcmp (getPropertyString ("initialDC"), "yes") ? true : false;
    // fetch simulation properties
    MaxIterations = getPropertyInteger ("MaxIter");
    reltol = getPropertyDouble ("reltol");
    abstol = getPropertyDouble ("abstol");
    vntol = getPropertyDouble ("vntol");

//    runs++;
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
    initTR ();
    setCalculation ((calculate_func_t) &calcTR);
    solve_pre ();

    // Create time sweep if necessary.
    initSteps ();
    swp->reset ();

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
    delta /= 10;
    fillState (dState, delta);
    adjustOrder (1);

}

/* synchronous step solver for external ode routine
 *
 * This function solves the circuit for a single time delta provided
 * by an external source. Convergence issues etc. are expected to
 * be handled by the external solver, as it is in full control of the
 * time stepping.
 */
int m_trsolver::stepsolve_sync(nr_double_t synctime)
{

    // copy the externally chosen time step to delta
    delta = synctime - lastsynctime;

    // get the current solution time
    //current += delta;

    // update the integrator coefficients
    updateCoefficients (delta);

    // Run predictor to get a start value for the solution vector for
    // the successive iterative corrector process
    error += predictor ();

    // restart Newton iteration
    restart (); // restart non-linear devices

    // Attempt to solve the circuit with the given delta
    try_running () // #defined as:    do {
    {
        error += solve_nonlinear_step ();
    }
    catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
    {
    case EXCEPTION_NO_CONVERGENCE:
        pop_exception ();

        // Retry using damped Newton-Raphson.
        convHelper = CONV_SteepestDescent;
        convError = 2;
#if DEBUG
        logprint (LOG_ERROR, "WARNING: delta rejected at t = %.3e, h = %.3e "
                  "(no convergence)\n", (double) saveCurrent, (double) delta);
#endif

        try_running () // #defined as:    do {
        {
            error += solve_nonlinear_step ();
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
        statIterations += iterations;
        if (--convError < 0) convHelper = 0;


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
//        logprint (LOG_ERROR, "ERROR: %s: Jacobian singular at t = %.3e, "
//                  "aborting %s analysis\n", getName (), (double) current,
//                  getDescription ());
        return -1;
    }

}

// accept a time step into the solution history
void m_trsolver::acceptstep_sync()
{
    // Now advance in time or not...
    if (running > 1)
    {
//        adjustDelta (time);
        adjustOrder ();
    }
    else
    {
        fillStates ();
        nextStates ();
        rejected = 0;
    }

    running++;
    converged++;

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

    // add the delta value to the current simulation time
    current += delta;

}

//// asynchronous step solver
//int m_trsolver::stepsolve_async()
//{
//    // Start to sweep through time.
//    for (int i = 0; i < swp->getSize (); i++)
//    {
//
//        time = swp->next ();
//        if (progress) logprogressbar (i, swp->getSize (), 40);
//
//#if DEBUG && 0
//        logprint (LOG_STATUS, "NOTIFY: %s: solving netlist for t = %e\n",
//                  getName (), (double) time);
//#endif
//
//        do
//        {
//#if STEPDEBUG
//            if (delta == deltaMin)
//            {
//                logprint (LOG_ERROR,
//                          "WARNING: %s: minimum delta h = %.3e at t = %.3e\n",
//                          getName (), (double) delta, (double) current);
//            }
//#endif
//            updateCoefficients (delta);
//
//            // Run predictor to get a start value for the solution vector for
//            // the successive iterative corrector process
//            error += predictor ();
//
//            // restart Newton iteration
//            if (rejected)
//            {
//                restart ();      // restart non-linear devices
//                rejected = 0;
//            }
//
//            // Run corrector process with appropriate exception handling.
//            // The corrector iterates through the solutions of the integration
//            // process until a certain error tolerance has been reached.
//            try_running () // #defined as:    do {
//            {
//                error += corrector ();
//            }
//            catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
//            {
//            case EXCEPTION_NO_CONVERGENCE:
//                pop_exception ();
//
//                // Reduce step-size (by half) if failed to converge.
//                if (current > 0) current -= delta;
//                delta /= 2;
//                if (delta <= deltaMin)
//                {
//                    delta = deltaMin;
//                    adjustOrder (1);
//                }
//                if (current > 0) current += delta;
//
//                // Update statistics.
//                statRejected++;
//                statConvergence++;
//                rejected++;
//                converged = 0;
//                error = 0;
//
//                // Start using damped Newton-Raphson.
//                convHelper = CONV_SteepestDescent;
//                convError = 2;
//#if DEBUG
//                logprint (LOG_ERROR, "WARNING: delta rejected at t = %.3e, h = %.3e "
//                          "(no convergence)\n", (double) saveCurrent, (double) delta);
//#endif
//                break;
//            default:
//                // Otherwise return.
//                estack.print ();
//                error++;
//                break;
//            }
//            if (error) return -1;
//            if (rejected) continue;
//
//            // check whether Jacobian matrix is still non-singular
//            if (!A->isFinite ())
//            {
//                logprint (LOG_ERROR, "ERROR: %s: Jacobian singular at t = %.3e, "
//                          "aborting %s analysis\n", getName (), (double) current,
//                          getDescription ());
//                return -1;
//            }
//
//            // Update statistics and no more damped Newton-Raphson.
//            statIterations += iterations;
//            if (--convError < 0) convHelper = 0;
//
//            // Now advance in time or not...
//            if (running > 1)
//            {
//                adjustDelta (time);
//                adjustOrder ();
//            }
//            else
//            {
//                fillStates ();
//                nextStates ();
//                rejected = 0;
//            }
//
//            saveCurrent = current;
//            current += delta;
//            running++;
//            converged++;
//
//            // Tell integrators to be running.
//            setMode (MODE_NONE);
//
//            // Initialize or update history.
//            if (running > 1)
//            {
//                updateHistory (saveCurrent);
//            }
//            else
//            {
//                initHistory (saveCurrent);
//            }
//        }
//        while (saveCurrent < time); // Hit a requested time point?
//
//        // Save results.
//#if STEPDEBUG
//        logprint (LOG_STATUS, "DEBUG: save point at t = %.3e, h = %.3e\n",
//                  (double) saveCurrent, (double) delta);
//#endif
//
//#if BREAKPOINTS
//        saveAllResults (saveCurrent);
//#else
//        saveAllResults (time);
//#endif
//    }
//}

int m_trsolver::finish()
{
    solve_post ();

    if (progress) logprogressclear (40);

    logprint (LOG_STATUS, "NOTIFY: %s: average time-step %g, %d rejections\n",
              getName (), (double) (saveCurrent / statSteps), statRejected);
    logprint (LOG_STATUS, "NOTIFY: %s: average NR-iterations %g, "
              "%d non-convergences\n", getName (),
              (double) statIterations / statSteps, statConvergence);

    // cleanup
    deinitTR ();
    return 0;
}

/* The single step non-linear iterative nodal analysis netlist solver. */
int m_trsolver::solve_nonlinear_step (void)
{
    int convergence, run = 0, error = 0;

    updateMatrix = 1;

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

    // run solving loop until convergence is reached
    do
    {
        error = solve_once ();
        if (!error)
        {
            // convergence check
            convergence = (run > 0) ? checkConvergence () : 0;
            savePreviousIteration ();
            run++;
            // control fixpoint iterations
            if (fixpoint)
            {
                if (convergence && !updateMatrix)
                {
                    updateMatrix = 1;
                    convergence = 0;
                }
                else
                {
                    updateMatrix = 0;
                }
            }
        }
        else
        {
            break;
        }
    }
    while (!convergence &&
            run < MaxIterations * (1 + convHelper ? 1 : 0));

    if (run >= MaxIterations || error)
    {
        e = new qucs::exception (EXCEPTION_NO_CONVERGENCE);
        e->setText ("no convergence in %s analysis after %d iterations",
                    desc, run);
        throw_exception (e);
        error++;
    }

    iterations = run;
    return error;
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
struct define_t m_trsolver::anadef =
    { "MTR", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };



