/***************************************************************************
    copyright            : (C) 2016, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// base object for qucs.

#ifndef QUCS_OBJECT_H
#define QUCS_OBJECT_H
#include "io_trace.h"
#include "e_base.h"
#include <string>
/*--------------------------------------------------------------------------*/
typedef unsigned index_t;
/*--------------------------------------------------------------------------*/
namespace qucs{
enum MsgType{
	MsgTrace=0,
	MsgLog=1,
	MsgDebug=2,
	MsgWarning=3,
	MsgCritical=4,
	MsgFatal=5
};
/*--------------------------------------------------------------------------*/
using Object = ::CKT_BASE;
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
using qucs::Object; // transition.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
