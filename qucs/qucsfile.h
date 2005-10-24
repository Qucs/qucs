/***************************************************************************
                               qucsfile.h
                              ------------
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

#ifndef QUCSFILE_H
#define QUCSFILE_H

#include "wire.h"
#include "diagrams/diagram.h"
#include "paintings/painting.h"
#include "components/component.h"

class QTextEdit;
class QProcess;
class QucsDoc;


class QucsFile {
public:
  QucsFile(QucsDoc*);
 ~QucsFile();

  int  save();
  bool load();

  QString createClipboardFile();
  bool    pasteFromClipboard(QTextStream*, QPtrList<Element>*);
  QString createUndoString(char);
  bool    rebuild(QString *);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(QString *);

  bool createSubNetlist(QTextStream*, int&, QStringList&, QTextEdit*, bool);
  int  prepareNetlist(QTextStream&, QStringList&, QTextEdit*);
  QString createNetlist(QTextStream&, bool);


private:
  bool loadProperties(QTextStream*);
  void simpleInsertComponent(Component*);
  bool loadComponents(QTextStream*, QPtrList<Component> *List=0);
  void simpleInsertWire(Wire*);
  bool loadWires(QTextStream*, QPtrList<Element> *List=0);
  bool loadDiagrams(QTextStream*, QPtrList<Diagram>*);
  bool loadPaintings(QTextStream*, QPtrList<Painting>*);
  bool loadIntoNothing(QTextStream*);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);
  void throughAllNodes(bool, QStringList&, int&, bool);
  bool giveNodeNames(QTextStream*, int&, QStringList&, QTextEdit*, bool);

  QucsDoc  *Doc;
  QPtrList<Wire>      *Wires;
  QPtrList<Node>      *Nodes;
  QPtrList<Component> *Comps;
  QPtrList<Diagram>   *Diags;
  QPtrList<Painting>  *Paints, *SymbolPaints;

  QStringList Signals; // collecting node names for VHDL signal declarations
};

#endif
