/***************************************************************************
                              schematic_file.cpp
                             --------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include "qucs.h"
#include "node.h"
#include "schematic_doc.h"
#include "module.h"
#include "misc.h"
#include "io_trace.h"
#include "task_element.h"
#include "globals.h"
#include "docfmt.h"
#include "schematic_lang.h"
#include "sckt_base.h"
// -------------------------------------------------------------

// -------------------------------------------------------------
#if 0// moved
bool SchematicDoc::loadProperties(QTextStream *stream)
#endif
// // TODO: move to frame::setParameters
void SchematicDoc::setFrameText(int, QString)
{
  incomplete();
}
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

// ***************************************************************
// *****                                                     *****
// *****             Functions to create netlist             *****
// *****                                                     *****
// ***************************************************************

// ----------------------------------------------------------
// Check whether this file is a qucs file and whether it is an subcircuit.
// It returns the number of subcircuit ports.
int SchematicDoc::testFile(const QString& DocName)
{
  incomplete(); // not SchematicDoc.
  QFile file(DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    return -1;
  }else{
  }

  QString Line;
  // .........................................
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.readAll();
  file.close();
  qDebug() << "ReadWhole opening" << FileString;
  QTextStream stream(&FileString, QIODevice::ReadOnly);


  // read header ........................
  do {
    if(stream.atEnd()) {
      file.close();
      return -2;
    }
    Line = stream.readLine();
    Line = Line.trimmed();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    return -3;
  }

  Line = Line.mid(16, Line.length()-17); // get version out from <Qucs Schematic 0.0.20
  VersionTriplet DocVersion = VersionTriplet(Line);
  if (DocVersion > QucsVersion) { // wrong version number ?
      if (!QucsSettings.IgnoreFutureVersion) {
          file.close();
          return -4;
      }
    //file.close();
    //return -4;
  }

  // read content ....................
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Components>") break;
  }

  int z=0;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "</Components>") {
      file.close();
      return z;       // return number of ports
    }

    Line = Line.trimmed();
    QString s = Line.section(' ',0,0);    // component type
    if(s == "<Port") z++;
  }
  return -5;  // component field not closed
}

// ---------------------------------------------------
// Collects the signal names for digital simulations.
void SchematicModel::collectDigitalSignals(void)
{
  incomplete();
// Node *pn=nullptr;
//
//  for(pn = nodes().first(); pn != 0; pn = nodes().next()) {
//    DigMap::Iterator it = Signals.find(pn->name());
//    if(it == Signals.end()) { // avoid redeclaration of signal
//      Signals.insert(pn->name(), DigSignal(pn->name(), pn->DType));
//    } else if (!pn->DType.isEmpty()) {
//      it.value().Type = pn->DType;
//    }
//  }
}

//#include <iostream>


// ---------------------------------------------------
// // what is this?
#if 0
bool SchematicModel::createLibNetlist(ostream_t& stream,
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

//#define VHDL_SIGNAL_TYPE "bit"
//#define VHDL_LIBRARIES   ""
static const std::string VHDL_SIGNAL_TYPE("std_logic");
static const std::string VHDL_LIBRARIES("\nlibrary ieee;\nuse ieee.std_logic_1164.all;\n");

// ---------------------------------------------------
// void SchematicModel::createSubNetlistPlain(ostream_t& str, QPlainTextEdit *ErrText,
// int NumPorts, bool creatingLib, NetLang const& )
// {
//   incomplete(); // obsolete.
// }
// ---------------------------------------------------

// vim:ts=8:sw=2:noet
