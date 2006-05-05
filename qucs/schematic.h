/***************************************************************************
                               schematic.h
                              -------------
    begin                : Sat Mar 11 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include "wire.h"
#include "diagrams/diagram.h"
#include "paintings/painting.h"
#include "components/component.h"
#include "qucsdoc.h"
#include "viewpainter.h"
#include "node.h"

#include <qscrollview.h>
#include <qpainter.h>
#include <qptrlist.h>
#include <qfile.h>

class QTextEdit;


class Schematic : public QScrollView, public QucsDoc {
  Q_OBJECT
public: 
  Schematic(QucsApp*, const QString&);
 ~Schematic();

  void setName(const QString&);
  void setChanged(bool, bool fillStack=false, char Op='*');
  void paintGrid(ViewPainter*, int, int, int, int);
  void print(QPrinter*, bool);

  float textCorr();
  void  sizeOfAll(int&, int&, int&, int&);
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  void  setOnGrid(int&, int&);
  bool  elementsOnGrid();

  float zoom(float);
  void  showAll();
  void  showNoZoom();
  void  enlargeView(int, int, int, int);
  void  switchPaintMode();
  int   adjustPortNumbers();
  void  reloadGraphs();

  QString copySelected(bool);
  bool    paste(QTextStream*, QPtrList<Element>*);
  bool    load();
  int     save();
  void    becomeCurrent(bool);
  bool    undo();
  bool    redo();

  bool scrollUp(int);
  bool scrollDown(int);
  bool scrollLeft(int);
  bool scrollRight(int);

  // The pointers points to the current lists, either to the schematic
  // elements "Doc..." or to the symbol elements "SymbolPaints".
  QPtrList<Wire>      *Wires, DocWires;
  QPtrList<Node>      *Nodes, DocNodes;
  QPtrList<Diagram>   *Diagrams, DocDiags;
  QPtrList<Painting>  *Paintings, DocPaints;
  QPtrList<Component> *Components, DocComps;

  QPtrList<Painting>  SymbolPaints;  // symbol definition for subcircuit
  bool symbolMode;  // true if in symbol painting mode


  int GridX, GridY;
  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area
  int UsedX1, UsedY1, UsedX2, UsedY2;  // document area used by elements

  // Two of those data sets are needed for Schematic and for symbol.
  // Which one is in "tmp..." depends on "symbolMode".
  float tmpScale;
  int tmpViewX1, tmpViewY1, tmpViewX2, tmpViewY2;
  int tmpUsedX1, tmpUsedY1, tmpUsedX2, tmpUsedY2;

  QPtrList<QString> UndoStack;
  QPtrList<QString> UndoSymbol;    // undo stack for circuit symbol

protected:
  // overloaded function to get actions of user
  void drawContents(QPainter*, int, int, int, int);
  void contentsMouseMoveEvent(QMouseEvent*);
  void contentsMousePressEvent(QMouseEvent*);
  void contentsMouseDoubleClickEvent(QMouseEvent*);
  void contentsMouseReleaseEvent(QMouseEvent*);
  void contentsWheelEvent(QWheelEvent*);
  void contentsDropEvent(QDropEvent*);
  void contentsDragEnterEvent(QDragEnterEvent*);
  void contentsDragLeaveEvent(QDragLeaveEvent*);
  void contentsDragMoveEvent(QDragMoveEvent*);

protected slots:
  void slotScrollUp();
  void slotScrollDown();
  void slotScrollLeft();
  void slotScrollRight();

private:
  bool dragIsOkay;


/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_element.cpp". They only access the QPtrList  *****
   *****  pointers "Wires", "Nodes", "Diagrams", "Paintings" and  *****
   *****  "Components".                                           *****
   ******************************************************************** */

public:
  Node* insertNode(int, int, Element*);
  Node* selectedNode(int, int);

  int   insertWireNode1(Wire*);
  bool  connectHWires1(Wire*);
  bool  connectVWires1(Wire*);
  int   insertWireNode2(Wire*);
  bool  connectHWires2(Wire*);
  bool  connectVWires2(Wire*);
  int   insertWire(Wire*);
  void  selectWireLine(Element*, Node*, bool);
  Wire* selectedWire(int, int);
  Wire* splitWire(Wire*, Node*);
  bool  oneTwoWires(Node*);
  void  deleteWire(Wire*);
  void  copyWires(int&, int&, int&, int&, QPtrList<Element>*);

  Marker* setMarker(int, int);
  bool    markerLeftRight(bool);
  bool    markerUpDown(bool);

  Element* selectElement(int, int, bool, int *index=0);
  void     deselectElements(Element*);
  int      selectElements(int, int, int, int, bool);
  void     newMovingWires(QPtrList<Element>*, Node*);
  void     copySelectedElements(QPtrList<Element>*);
  bool     copyComps2WiresPaints(int&, int&, int&, int&, QPtrList<Element>*);
  int      copyElements(int&, int&, int&, int&, QPtrList<Element>*);
  bool     deleteElements();
  bool     aligning(int);
  bool     distribHoriz();
  bool     distribVert();

  void       setComponentNumber(Component*);
  void       insertRawComponent(Component*, bool noOptimize=true);
  void       recreateComponent(Component*);
  void       insertComponent(Component*);
  void       activateCompsWithinRect(int, int, int, int);
  bool       activateSpecifiedComponent(int, int);
  bool       activateSelectedComponents();
  void       setCompPorts(Component*);
  Component* selectCompText(int, int, int&, int&);
  Component* searchSelSubcircuit();
  Component* selectedComponent(int, int);
  void       deleteComp(Component*);
  void       copyComponents(int&, int&, int&, int&, QPtrList<Element>*);
  void       copyComponents2(int&, int&, int&, int&, QPtrList<Element>*);

  void     oneLabel(Node*);
  int      placeNodeLabel(WireLabel*);
  Element* getWireLabel(Node*);
  void     insertNodeLabel(WireLabel*);
  void     copyLabels(int&, int&, int&, int&, QPtrList<Element>*);

  Painting* selectedPainting(int, int);
  void      copyPaintings(int&, int&, int&, int&, QPtrList<Element>*);

public:
  void insertComponentNodes(Component*, bool);


/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_file.cpp". They only access the QPtrLists    *****
   *****  and their pointers. ("DocComps", "Components" etc.)     *****
   ******************************************************************** */

public:
  bool createSubNetlist(QTextStream*, int&, QStringList&, QTextEdit*, int);
  int  prepareNetlist(QTextStream&, QStringList&, QTextEdit*);
  QString createNetlist(QTextStream&, int);


private:
  int  saveDocument();
  bool loadDocument();

  bool loadProperties(QTextStream*);
  void simpleInsertComponent(Component*);
  bool loadComponents(QTextStream*, QPtrList<Component> *List=0);
  void simpleInsertWire(Wire*);
  bool loadWires(QTextStream*, QPtrList<Element> *List=0);
  bool loadDiagrams(QTextStream*, QPtrList<Diagram>*);
  bool loadPaintings(QTextStream*, QPtrList<Painting>*);
  bool loadIntoNothing(QTextStream*);

  QString createClipboardFile();
  bool    pasteFromClipboard(QTextStream*, QPtrList<Element>*);

  QString createUndoString(char);
  bool    rebuild(QString *);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(QString *);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);
  void throughAllNodes(bool, QStringList&, int&, bool);
  bool giveNodeNames(QTextStream*, int&, QStringList&, QTextEdit*, int);

  QStringList Signals; // collecting node names for VHDL signal declarations
};

#endif
