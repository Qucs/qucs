/*
 * schematicscene.h - define schematic scene
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 * Copyright (C) 2019-2020, Guilherme Brondani Torri, guitorri@gmail.com
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

#ifndef SCHEMATICSCENE_H_
#define SCHEMATICSCENE_H_

#include "qt_compat.h"

#include <QGraphicsScene>
#include <QFileInfo>

class QUndoStack;

class Wire;
class Node;
class Diagram;
class Component;
class Painting;
class Marker;
class Element;
class WireLabel;
class Frame;

// TODO: refactor here
class WireList : public Q3PtrList<Wire> {
};
// TODO: refactor here
class NodeList : public Q3PtrList<Node> {
};
// TODO: refactor here
class DiagramList : public Q3PtrList<Diagram> {
};
// TODO: refactor here
class ComponentList : public Q3PtrList<Component> {
};
// TODO: refactor here
class PaintingList : public Q3PtrList<Painting> {
};

/*!
 * Return list of objects of type T from provided list of items.
 * For instance (Element): Wire, Node, Diagram, Component,
 * Painting, Marker, WireLabel;
 */
template<typename T>
QList<T*> filterItems(QList<QGraphicsItem*> &items)  {
    QList<T*> castItems;
    foreach(QGraphicsItem *item, items) {
        T *cast = dynamic_cast<T*>(item);
        if(cast) {
            castItems << cast;
        }
    }
    return castItems;
}


class SchematicScene : public QGraphicsScene
{
Q_OBJECT
public:
  SchematicScene (QObject *);
  virtual ~SchematicScene ();

public:
  QUndoStack *undoStack;

  // schematic frame item
  Frame *schematicFrame;

protected:
  void drawBackground(QPainter *painter, const QRectF& rect);

  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public: ///\todo set public for now
  // from schematicview
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  void  setOnGrid(int&, int&);
  bool  elementsOnGrid();
  void highlightWireLabels (void);

  int GridX, GridY;

  QString DocName; // from QucsDoc
  QString DataSet; // from QucsDoc

  int  showBias;     // -1=no, 0=calculation running, >0=show DC bias points

private:
  void drawBias(QPainter *painter, const QRectF& rect);

public:
  /// \todo make then public while we fix stuff
  // The pointers points to the current lists, either to the schematic
  // elements "Doc..." or to the symbol elements "SymbolPaints".
// private: //TODO. one at a time.
  WireList      *Wires, DocWires;
  NodeList      *Nodes, DocNodes;
  DiagramList   *Diagrams, DocDiags;
  PaintingList  *Paintings, DocPaints;
  ComponentList *Components, DocComps;

  PaintingList  SymbolPaints;  // symbol definition for subcircuit
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

  Marker* setMarker(int, int);
  void    markerLeftRight(bool, Q3PtrList<Element>*);
  void    markerUpDown(bool, Q3PtrList<Element>*);

  void     deselectElements(Element*);
  int      selectElements(int, int, int, int, bool);
  void     selectMarkers();
  void     newMovingWires(Q3PtrList<Element>*, Node*, int);
  int      copySelectedElements(Q3PtrList<Element>*);
  bool     deleteElements();
  bool     aligning(int);
  bool     distributeHorizontal();
  bool     distributeVertical();

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
  void       deleteComp(Component*);

  void     oneLabel(Node*);
  int      placeNodeLabel(WireLabel*);
  Element* getWireLabel(Node*);
  void     insertNodeLabel(WireLabel*);
  void     copyLabels(int&, int&, int&, int&, QList<Element *> *);

  void      copyPaintings(int&, int&, int&, int&, QList<Element *> *);

  void  reloadGraphs();

  void simpleInsertComponent(Component*);
  void simpleInsertWire(Wire*);

private:
  void insertComponentNodes(Component*, bool);
  int  copyWires(int&, int&, int&, int&, QList<Element *> *);
  int  copyComponents(int&, int&, int&, int&, QList<Element *> *);
  void copyComponents2(int&, int&, int&, int&, QList<Element *> *);
  bool copyComps2WiresPaints(int&, int&, int&, int&, QList<Element *> *);
  int  copyElements(int&, int&, int&, int&, QList<Element *> *);

public:
  /*! \brief Get (schematic) file reference */
  QFileInfo getFileInfo (void) { return FileInfo; }
  /*! \brief Set reference to file (schematic) */
  void setFileInfo(QString FileName) { FileInfo = QFileInfo(FileName); }

private:
  /*! \brief hold system-independent information about a schematic file */
  QFileInfo FileInfo;

};

/*!
 * Given a scene and a type, return items from scene.
 */
template<typename T>
QList<T*> filterItems(SchematicScene *sc)  {
    QList<QGraphicsItem*> items;
    items = sc->items();
    QList<T*> castItems;
    foreach(QGraphicsItem *item, items) {
        T *cast = dynamic_cast<T*>(item);
        if(cast) {
            castItems << cast;
        }
    }
    return castItems;
}

#endif /* SCHEMATICSCENE_H_ */
