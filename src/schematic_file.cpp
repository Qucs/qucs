/***************************************************************************
    begin                : Sat Mar 27 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <deque>
#include <QtCore>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QPlainTextEdit>
#include "qt_compat.h"
#include <QTextStream>
#include <QList>
#include <QProcess>
#include <QDebug>

#include "qucs_app.h"
#include "node.h"
#include "module.h"
#include "misc.h"
#include "io_trace.h"
#include "task_element.h"
#include "qucs_globals.h"
#include "docfmt.h"
#include "schematic_lang.h"
#include "sckt_base.h"
// -------------------------------------------------------------
// // TODO: move to frame::setParameters
#if 0
{
  if(s != FrameText[idx]){
    setChanged(true);
    FrameText[idx] = s;
    misc::convert2Unicode(FrameText[idx]);
  }else{
  }
}
#endif

// -------------------------------------------------------------
// // what is this?
#if 0
bool ElementList::createLibNetlist(ostream_t& stream,
     QPlainTextEdit*, int, NetLang const&)
{ untested();
  incomplete(); // wrong place.
  qDebug() << "createLibNetlist";
  bool isAnalog=true;
  bool isVerilog=false;
  DigMap Signals; //?!
//  int countInit = 0;
  QStringList Collect;
  Collect.clear();
  Signals.clear();
  // Apply node names and collect subcircuits and file include
//  bool creatingLib = true;
#if 0
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts, creatingLib, nl)) {
    creatingLib = false;
    return false;
  }else{
  }
#endif
//  creatingLib = false;

  // Marking start of actual top-level subcircuit
  QString c;
  if(!isAnalog) {
    if (isVerilog)
      c = "///";
    else
      c = "---";
  }
  else c = "###";
  stream << "\n" << c << " TOP LEVEL MARK " << c << "\n";

  // Emit subcircuit components
  incomplete();
//  createSubNetlistPlain(stream, ErrText, NumPorts, creatingLib);

  Signals.clear();  // was filled in "giveNodeNames()"
  return true;
} // createLibNetlist
#endif


// vim:ts=8:sw=2:noet
