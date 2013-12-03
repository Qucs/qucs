/*
 * qucs_interface.cpp - m-code interface implementation
 *
 * Copyright (C) 2013 Richard Crozier <richard.crozier@yahoo.co.uk>
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
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "logging.h"
#include "precision.h"
#include "component.h"
#include "components.h"
#include "net.h"
#include "input.h"
#include "dataset.h"
#include "equation.h"
#include "environment.h"
#include "exceptionstack.h"
#include "check_netlist.h"
#include "module.h"
#include "qucs_interface.h"
#include "analysis.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

using namespace qucs;

// constructor
qucsint::qucsint ()
{

    //int listing = 0;
    ret = 0;
    err = 0;

    loginit ();
    precinit ();
    ::srand (::time (NULL));

}

// destructor
qucsint::~qucsint ()
{
    delete subnet;
    delete in;
//    delete out;
    delete root;

    // delete modules
    module::unregisterModules ();

    netlist_destroy_env ();
}

int qucsint::prepare_netlist (char * infile)
{

    // create static modules
    module::registerModules ();

    // create root environment
    root = new qucs::environment ("root");

    // create netlist object and input
    subnet = new net ("subnet");
    in = infile ? new input (infile) : new input ();

    // pass root environment to netlist object and input
    subnet->setEnv (root);
    in->setEnv (root);

    // get input netlist
    if (in->netlist (subnet) != 0)
    {
        if (netlist_check)
        {
            // replace with mex error message
            logprint (LOG_STATUS, "checker notice, netlist check FAILED\n");
        }
        return -1;
    }
    if (netlist_check)
    {
        logprint (LOG_STATUS, "checker notice, netlist OK\n");
        return -2;
    }

    // attach a ground to the netlist
    gnd = new ground ();
    gnd->setNode (0, "gnd");
    gnd->setName ("GND");
    subnet->insertCircuit (gnd);

    // apply some data to all analyses
    for (int i = 0; i < subnet->getNActions(); i++)
    {
        subnet->setActionNet(i, subnet);
        //a->setData (out);
    }

    return 0;
}

int qucsint::evaluate ()
{
    // analyse the netlist
    err = 0;
    ret = 0;
    out = subnet->runAnalysis (err);
    ret |= err;

    return ret;
}

// Returns a pointer to the e_trsolver if it is present
analysis * qucsint::getETR()
{
    // get a pointer to the external transient solver interface
    return subnet->findAnalysis (ANALYSIS_E_TRANSIENT);
}

int qucsint::output (char * outfile)
{
    // evaluate output dataset
    ret |= root->equationSolver (out);

    if (outfile != NULL)
    {
        out->setFile (outfile);
        out->print ();
    }

    return ret;
}
