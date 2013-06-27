/*
 * qucs_interface.h - m-code interface class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: analysis.h 1869 2013-03-06 12:50:21Z crobarcro $
 *
 */

/*! \file qucs_interface.h
 * \brief The m-code interface class header file.
 *
 * Contains the m-code interface class definition.
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
#include "e_trsolver.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef __QUCS_INTERFACE_H__
#define __QUCS_INTERFACE_H__

/*! \class qucsint
 * \brief class for performing circuit analyses.
 *
 * This class is used for interfacing qucs with octave and matlab
 *
 */
class qucsint
{

public:
    qucsint();
    ~qucsint();

    int prepare_netlist (char * infile);
    int evaluate();
    int output (char * outfile);
    analysis * getETR(void);


private:

  net * subnet;
  input * in;
  circuit * gnd;
  dataset * out;
  environment * root;
  int err;
  int ret;

};

#endif /* __ANALYSIS_H__ */
