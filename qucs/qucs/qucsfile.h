/***************************************************************************
                          qucsfile.h  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
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


class QucsDoc;

/**
  *@author Michael Margraf
  */

class QucsFile {
public:
  QucsFile(QucsDoc *p);
  ~QucsFile();

  int   save();
  bool  load();

  QString createClipboardFile();
  bool    pasteFromClipboard(QTextStream *stream, QPtrList<Element> *pe);

private:
  bool  loadProperties(QTextStream *stream);
  void  simpleInsertComponent(Component *c);
  bool  loadComponents(QTextStream *stream, QPtrList<Component> *List=0);
  void  simpleInsertWire(Wire *pw);
  bool  loadWires(QTextStream *stream, QPtrList<Wire> *List=0);
  bool  loadDiagrams(QTextStream *stream, QPtrList<Diagram> *List);
  bool  loadPaintings(QTextStream *stream, QPtrList<Painting> *List);

  QucsDoc  *Doc;
  QPtrList<Wire>      *Wires;
  QPtrList<Node>      *Nodes;
  QPtrList<Component> *Comps;
  QPtrList<Diagram>   *Diags;
  QPtrList<Painting>  *Paints;

};

#endif
