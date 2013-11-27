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
 * \brief The generic external interface class header file.
 *
 * Contains the m-code interface class definition.
 */

//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>
//#include <string.h>
//#include <time.h>

//#include <qucs-core/logging.h>"
//#include <qucs-core/precision.h"
//#include "component.h"
//#include "components.h"
//#include <qucs-core/net.h>
//#include <qucs-core/input.h>
//#include <qucs-core/dataset.h>
//#include <qucs-core/equation.h>
//#include <qucs-core/environment.h>
//#include <qucs-core/circuit.h>
//#include "exceptionstack.h"
//#include "check_netlist.h"
//#include "module.h"
//#include "e_trsolver.h"

#ifndef __QUCS_INTERFACE_H__
#define __QUCS_INTERFACE_H__

// forward declarations of some classes to avoid including
// header files and speed compilation a little
namespace qucs {
  class net;
  class input;
  class circuit;
  class dataset;
  class environment;
  class analysis;
}

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
    qucs::analysis * getETR(void);


private:

  qucs::net * subnet;
  qucs::input * in;
  qucs::circuit * gnd;
  qucs::dataset * out;
  qucs::environment * root;
  int err;
  int ret;

};

#endif /* __QUCS_INTERFACE_H__ */
