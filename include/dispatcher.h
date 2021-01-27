/*$Id: l_dispatcher.h 2016/03/29 al -*- C++ -*-
 * Copyright (C) 2018 Felix Salfelder
 * dispatcher -- for dynamically loaded modules
 */
//testing=script 2015.01.21
#ifndef QUCS_DISPATCHER_H
#define QUCS_DISPATCHER_H
#include "platform.h"
/*--------------------------------------------------------------------------*/
// TODO: connect with qucs error facilities.
#define INTERFACE // ??
// INTERFACE void error(int,const char*,...);
// INTERFACE void error(int,const std::string&);
/*--------------------------------------------------------------------------*/
class Object;
/*--------------------------------------------------------------------------*/
#include <string>
std::string to_string(int);
#include <map>
#include <assert.h>
#include "platform.h"
#include "io_trace.h"
#define CKT_BASE Object
#define DISPATCHER_BASE DispatcherBase
#define DISPATCHER Dispatcher
#define OPT tQucsSettings
#ifndef L_STLEXTRA_H
namespace notstd{
	static void to_lower(std::string*){}
}
#endif
#define error message
#include "l_dispatcher_.h"
#undef CKT_BASE
#undef error
#endif
