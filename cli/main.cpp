/* Copyright (C) 2001 Albert Davis
 *               2020 Felix Salfelder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * top level module
 * it all starts here
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include <stdlib.h>
#include <ctype.h>
#include <locale.h>

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QtSvg>
#include <QDebug>

//#include "qucs.h"
#include "settings.h"
#include "docfmt.h"
#include "printerwriter.h"
#include "imagewriter.h"
#include "schematic_lang.h"
#include "simulator.h"

#include "module.h"
#include "misc.h"
#include "exception.h"
#include "exception.h"
#include "qucs_globals.h"
#include "library.h"
#include "message.h"
#include "qio.h"
/*--------------------------------------------------------------------------*/
// qucs hacks and wraps
#define CS istream_t
#define CARD_LIST SchematicModel
#define OPT tQucsSettings
#define CMD Command
static std::string I_PROMPT = "qucs> ";
SchematicModel static_model;
/*--------------------------------------------------------------------------*/
struct JMP_BUF{
  sigjmp_buf p;
} env;
/*--------------------------------------------------------------------------*/
static void sign_on(void)
{
  std::cout <<
    "Qucs : experimental CLI\n"
    "used for testing only\n"
    "This is free software, and you are welcome\n"
    "to redistribute it under the terms of \n"
    "the GNU General Public License, version 3 or later.\n"
    "See the file \"COPYING\" for details.\n";
  std::cout.flush();
}
/*--------------------------------------------------------------------------*/
static void prepare_env()
{
  static const char* plugpath="PLUGPATH=" QUCS_PLUGPATH
                              "\0         (reserved space)                 ";

  std::string ldlpath = OS::getenv("LD_LIBRARY_PATH");
  if (ldlpath != "") {
    ldlpath += ":";
  }else{
  }
  assert(strlen("PLUGPATH=") == 9);
  OS::setenv("QUCS_PLUGPATH", ldlpath + (plugpath+9), false);
}
/*--------------------------------------------------------------------------*/
static void read_startup_files(void)
{
#if 1
  CMD::command(std::string("load plugins/misc"), &static_model);
#else
  {
    std::string name = findfile(SYSTEMSTARTFILE, SYSTEMSTARTPATH, R_OK);
    if (name != "") {untested();
      CMD::command("include " + name, &static_model);
    }else{
      CMD::command(std::string("load " DEFAULT_PLUGINS), &static_model);
    }
  }
  {
    std::string name = findfile(USERSTARTFILE, USERSTARTPATH, R_OK);
    if (name != "") {untested();
      CMD::command("include " + name, &static_model);
    }else{
    }
  }
  //CMD::command("clear", &static_model);
  if (!OPT::language) {
    OPT::language = language_dispatcher[DEFAULT_LANGUAGE];
    
    for(DISPATCHER<LANGUAGE>::const_iterator
	  i=language_dispatcher.begin(); !OPT::language && i!=language_dispatcher.end(); ++i) {untested();
      OPT::language = prechecked_cast<LANGUAGE*>(i->second);
    }
  }else{untested();
    // already have a language specified in a startup file
  }
  if (OPT::language) {
    OPT::case_insensitive = OPT::language->case_insensitive();
    OPT::units            = OPT::language->units();
  }else{
    OPT::case_insensitive = false;
    OPT::units            = uSI;
  }
#endif
}
/*--------------------------------------------------------------------------*/
/* sig_abrt: trap asserts
 */
#if 0
extern "C" {
  static void sig_abrt(SIGNALARGS)
  {untested();
    signal(SIGABRT,sig_abrt);
    static int count = 100;
    if (--count > 0 && ENV::run_mode != rBATCH) {untested();
      IO::error << '\n';
      siglongjmp(env.p,1);
    }else{untested();
      exit(1);
    }
  }
}
#endif
/*--------------------------------------------------------------------------*/
/* sig_int: what to do on receipt of interrupt signal (SIGINT)
 * cancel batch files, then back to command mode.
 * (actually, control-c trap)
 */
extern "C" {
  static void sig_int(SIGNALARGS)
  {itested();
    signal(SIGINT,sig_int);
#if 0
    if (ENV::run_mode == rBATCH) {untested();
      exit(1);
    }else{itested();
      IO::error << '\n';
      siglongjmp(env.p,1);
    }
#endif
  }
}
/*--------------------------------------------------------------------------*/
extern "C" {
  static void sig_fpe(SIGNALARGS)
  {untested();
    signal(SIGFPE,sig_fpe);
    message(bDANGER, "floating point error\n");
  }
}
/*--------------------------------------------------------------------------*/
static void setup_traps(void)
{
  signal(SIGFPE,sig_fpe);
  signal(SIGINT,sig_int);
  //signal(SIGABRT,sig_abrt);
}
/*--------------------------------------------------------------------------*/
/* finish: clean up after a command
 * deallocates space, closes plot windows, resets i/o redirection, etc.
 * This is done separately for exception handling.
 * If a command aborts, clean-up is still done, leaving a consistent state.
 * //BUG// It is a function to call as a remnant of old C code.
 * Should be in a destructor, so it doesn't need to be explicitly called.
 */
static void finish(void)
{
  incomplete();
  // plclose();
  // outreset();
}
/*--------------------------------------------------------------------------*/
static void process_cmd_line(int argc, const char *argv[])
{
  for (int ii = 1;  ii < argc;  /*inside*/) {
    try {
      if (strncmp(argv[ii], "--", 2) == 0) {
	if (ii < argc) {
	  CS cmd(CS::_STRING, argv[ii++]+2); // command line
	  CMD::cmdproc(cmd, &static_model); 
	}else{untested();
	}
      }else{
	try {
	  CMD::command(std::string("include ") + argv[ii++], &static_model);
	}catch (qucs::Exception& e) {itested();
	  message(bDANGER, e.message() + '\n');
	  finish();
	}
	if (ii >= argc) {itested();
	  //CMD::command("end", &static_model);
	  throw qucs::Exception_Quit("");
	}else{untested();
	}	
      }
    }catch (qucs::Exception_Quit& e) {
      throw;
    }catch (qucs::Exception& e) {itested();
      // abort command, continue loop
      message(bDANGER, e.message() + '\n');
      finish();
    }
  }
}
/*--------------------------------------------------------------------------*/
int main(int argc, const char *argv[])
{
  prepare_env();
  //CKT_BASE::_sim = new SIM_DATA;
  //CKT_BASE::_probe_lists = new PROBE_LISTS;
  try {
  {
    // SET_RUN_MODE xx(rBATCH);
    sign_on();
    if (!sigsetjmp(env.p, true)) {
#if 0
      try {untested();
#endif
	read_startup_files();
	setup_traps();
	process_cmd_line(argc,argv);
#if 0
      }catch (qucs::Exception& e) {untested();
	error(bDANGER, e.message() + '\n');
	finish();		/* error clean up (from longjmp()) */
	//CMD::command("quit", &static_model);
	unreachable();
	exit(0);
      }
#endif
    }else{untested();
      finish();		/* error clean up (from longjmp()) */
      //CMD::command("quit", &static_model);
      exit(0);
    }
  }
  {
    // SET_RUN_MODE xx(rINTERACTIVE);
    CS cmd(CS::_STDIN);
    for (;;) {
      if (!sigsetjmp(env.p, true)) {
	try {
	  if (OPT::language) {
	    trace1("parse", cmd.fullstring());
	    OPT::language->parse_top_item(cmd, &static_model);
	  }else{
	    CMD::cmdproc(cmd.get_line(I_PROMPT), &static_model);
	  }
	}catch (qucs::Exception_End_Of_Input& e) {
	  message(bDANGER, e.message() + '\n');
	  finish();
	  //CMD::command("quit", &static_model);
	  //exit(0);
	  break;
	}catch (qucs::Exception& e) {
	  message(bDANGER, e.message() + '\n');
	  finish();
	}
      }else{itested();
	finish();		/* error clean up (from longjmp()) */
      }
    }
  }
  }catch (qucs::Exception_Quit&) {
  }catch (qucs::Exception& e) {untested();
    message(bDANGER, e.message() + '\n');
  }
  
  //static_model.erase_all();
  CMD::command("clear", &static_model);
  assert(static_model.size()==0);
  CMD::command("detach_all", &static_model);
//  delete CKT_BASE::_probe_lists;
//  CKT_BASE::_probe_lists = NULL;
//  delete CKT_BASE::_sim;
//  CKT_BASE::_sim = NULL;
  
  return 0;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
