/*
 * schematicfile.h - implement schematic file handling
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2020, Guilherme Brondani Torri, guitorri@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
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
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SCHEMATICFILE_H
#define SCHEMATICFILE_H

#include "diagrams/diagram.h"
#include "paintings/painting.h"
#include "components/component.h"

#include <QString>
#include <QStringList>

class Wire;

class QTextStream;
class QPlainTextEdit;

// digital signal data
struct DigSignal {
  DigSignal() { Name=""; Type=""; }
  DigSignal(const QString& _Name, const QString& _Type = "")
    : Name(_Name), Type(_Type) {}
  QString Name; // name
  QString Type; // type of signal
};
typedef QMap<QString, DigSignal> DigMap;

// subcircuit, vhdl, etc. file structure
struct SubFile {
  SubFile() { Type=""; File=""; PortTypes.clear(); }
  SubFile(const QString& _Type, const QString& _File)
    : Type(_Type), File(_File) { PortTypes.clear(); }
  QString Type;          // type of file
  QString File;          // file name identifier
  QStringList PortTypes; // data types of in/out signals
};
typedef QMap<QString, SubFile> SubMap;


/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_file.cpp". They only access the QPtrLists    *****
   *****  and their pointers. ("DocComps", "Components" etc.)     *****
   ******************************************************************** */
class SchematicFile {

public:
  static int testFile(const QString &);
  bool createLibNetlist(QTextStream*, QPlainTextEdit*, int);
  bool createSubNetlist(QTextStream *, int&, QStringList&, QPlainTextEdit*, int);
  void createSubNetlistPlain(QTextStream*, QPlainTextEdit*, int);
  int  prepareNetlist(QTextStream&, QStringList&, QPlainTextEdit*);
  QString createNetlist(QTextStream&, int);
  bool loadDocument();

private:
  int  saveDocument();

  bool loadProperties(QTextStream*);
  void simpleInsertComponent(Component*);
  bool loadComponents(QTextStream*, Q3PtrList<Component> *List=0);
  void simpleInsertWire(Wire*);
  bool loadWires(QTextStream*, Q3PtrList<Element> *List=0);
  bool loadDiagrams(QTextStream*, Q3PtrList<Diagram>*);
  bool loadPaintings(QTextStream*, Q3PtrList<Painting>*);
  bool loadIntoNothing(QTextStream*);

  QString createClipboardFile();
  bool    pasteFromClipboard(QTextStream *, Q3PtrList<Element>*);

  QString createUndoString(char);
  bool    rebuild(QString *);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(QString *);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);
  void throughAllNodes(bool, QStringList&, int&);
  void propagateNode(QStringList&, int&, Node*);
  void collectDigitalSignals(void);
  bool giveNodeNames(QTextStream *, int&, QStringList&, QPlainTextEdit*, int);
  void beginNetlistDigital(QTextStream &);
  void endNetlistDigital(QTextStream &);
  bool throughAllComps(QTextStream *, int&, QStringList&, QPlainTextEdit *, int);

  DigMap Signals; // collecting node names for VHDL signal declarations
  QStringList PortTypes;

public: // for now. move to parser asap
	Component* loadComponent(const QString& _s, Component* c) const;

public:
  bool isAnalog;
  bool isVerilog;
  bool creatingLib;

public: // serializer
  void saveComponent(QTextStream& s, Component /* FIXME const */* c) const;

  // from schematicview
  void    cut();
  void    copy();
  bool    paste(QTextStream*, Q3PtrList<Element>*);
  bool    load();
  int     save();
  int     saveSymbolCpp (void);
  int     saveSymbolJSON (void);
  int   adjustPortNumbers();
};

#endif
