/*
 * trsolver.h - transient solver class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include "trsolver.h"

#ifndef __E_TRSOLVER_H__
#define __E_TRSOLVER_H__

#define ETR_MODE_ASYNC 0
#define ETR_MODE_SYNC 1

class e_trsolver : public trsolver
{
public:
    ACREATOR (e_trsolver);
    //e_trsolver ();
    e_trsolver (char *);
    e_trsolver (e_trsolver &);
    ~e_trsolver ();

    int init (nr_double_t, nr_double_t, int);
//    int prepare_net (char * infile);
    void initSteps (void);
    void initETR (nr_double_t start, nr_double_t, int);
    int stepsolve_sync(nr_double_t synctime);
    int stepsolve_async(nr_double_t steptime);
    void acceptstep_sync();
    void getsolution(double *);
    int getN();
    int getM();
    int finish();

    // debugging functions
    void debug(void);
    void printx(void);

    void (*messagefcn)(int, const char *, ...);

private:

    int MaxIterations;
    nr_double_t reltol;
    nr_double_t abstol;
    nr_double_t vntol;
    nr_double_t time;
    nr_double_t saveCurrent;
    nr_double_t lastsynctime;
    int running;
    int rejected;
    int convError;

    int solve_nonlinear_step (void);
    void adjustDelta_sync (nr_double_t);

};

#endif /* __e_trsolver_H__ */

