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

// maybe in another place...
#ifdef NDEBUG
// cast without overhead
#  define prechecked_cast static_cast
#else
// cast safely, for debugging purposes
#  define prechecked_cast dynamic_cast
#endif

#include "wire.h"
#include "node.h"
#include "qucsdoc.h"
#include "viewpainter.h"
#include "sharedObjectList.h"
#include "diagrams/diagram.h"
#include "paintings/painting.h"
#include "components/component.h"

#include <QVector>
#include <QStringList>
#include <QFileInfo>
#include <QAbstractScrollArea>
#include <QPoint>

class QTextStream;
class QTextEdit;
class QPlainTextEdit;
class QDragMoveEvent;
class QDropEvent;
class QDragLeaveEvent;
class QWheelEvent;
class QMouseEvent;
class QDragEnterEvent;
class QPainter;

// digital signal data
struct DigSignal {
  DigSignal() { Name=""; Type=""; }
  DigSignal(const QString& _Name, const QString& _Type = "")
    : Name(_Name), Type(_Type) {}
  QString Name; // name
  QString Type; // type of signal
};
typedef QMap<QString, DigSignal> DigMap;
typedef enum {_NotRop, _Rect, _Line, _Ellipse, _Arc, _DotLine, _Translate, _Scale} PE;
typedef struct {PE pe; int x1; int y1;int x2;int y2;int a; int b; bool PaintOnViewport;}PostedPaintEvent;

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

typedef SharedObjectList<Wire> WireList;
typedef SharedObjectList<Node> NodeList;
typedef SharedObjectList<Diagram> DiagramList;
typedef SharedObjectList<Component> ComponentList;
typedef SharedObjectList<Painting> PaintingList;

class Schematic : public QAbstractScrollArea, public QucsDoc {
  Q_OBJECT
public:
  Schematic(QucsApp*, const QString&);
 ~Schematic();

  void setName(const QString&);
  void setChanged(bool, bool fillStack=false, char Op='*');
  void paintGrid(ViewPainter*, int, int, int, int);
  void print(QPrinter*, QPainter*, bool, bool);

  void paintSchToViewpainter(ViewPainter* p, bool printAll, bool toImage, int screenDpiX=96, int printerDpiX=300);

  void PostPaintEvent(PE pe, int x1=0, int y1=0, int x2=0, int y2=0, int a=0, int b=0,bool PaintOnViewport=false);

  float textCorr();
  bool sizeOfFrame(int&, int&);
  void  sizeOfAll(int&, int&, int&, int&);
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  void  setOnGrid(int&, int&);
  bool  elementsOnGrid();

  //  NOTE: "around" is given in window coordinates - (0, 0) is upper left corner of viewport
  float zoom(float, QPoint around);
  float zoomBy(float);
  void  showAll();
  void  showNoZoom();
  void  enlargeView(int, int, int, int);
  void  switchPaintMode();
  int   adjustPortNumbers();
  void  reloadGraphs();
  bool  createSubcircuitSymbol();

  void    cut();
  void    copy();
  bool    paste(QTextStream*, SharedObjectList<Element> &);
  bool    load();
  int     save();
  int     saveAs(QString OutputFileName, QString OutputTypeName);
  int     saveSymbolCpp (void);
  int     saveSymbolJSON (void);
  void    becomeCurrent(bool);
  bool    undo();
  bool    redo();

  void scrollUp(int);
  void scrollDown(int);
  void scrollLeft(int);
  void scrollRight(int);

  void resizeContents(int w, int h);
  void scrollBy(int dx, int dy);
  void setContentsPos(int x, int y);
  int visibleWidth();
  int visibleHeight();
  int contentsWidth();
  int contentsHeight();
  int contentsX();
  int contentsY();
  void contentsToViewport(int x, int y, int &vx, int &vy);

  // The pointers points to the current lists, either to the schematic
  // elements "Doc..." or to the symbol elements "SymbolPaints".
// private: //TODO. one at a time.
  WireList      *Wires, DocWires;
  NodeList      *Nodes, DocNodes;
  DiagramList   *Diagrams, DocDiags;
  PaintingList  *Paintings, DocPaints;
  ComponentList *Components, DocComps;

  // TODO: const access
  ComponentList& components(){
	  assert(Components);
	  return *Components;
  }

  PaintingList  SymbolPaints;  // symbol definition for subcircuit

  QList<PostedPaintEvent>   PostedPaintEvents;
  bool symbolMode;  // true if in symbol painting mode


  int GridX, GridY;
  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area
  int UsedX1, UsedY1, UsedX2, UsedY2;  // document area used by elements

  int showFrame;
  QString Frame_Text0, Frame_Text1, Frame_Text2, Frame_Text3;

  // Two of those data sets are needed for Schematic and for symbol.
  // Which one is in "tmp..." depends on "symbolMode".
  float tmpScale;
  int tmpViewX1, tmpViewY1, tmpViewX2, tmpViewY2;
  int tmpUsedX1, tmpUsedY1, tmpUsedX2, tmpUsedY2;

  int undoActionIdx;
  QVector<QString> undoAction;
  int undoSymbolIdx;
  QVector<QString> undoSymbol;    // undo stack for circuit symbol

  /*! \brief Get (schematic) file reference */
  QFileInfo getFileInfo (void) { return FileInfo; }
  /*! \brief Set reference to file (schematic) */
  void setFileInfo(QString FileName) { FileInfo = QFileInfo(FileName); }

signals:
  void signalCursorPosChanged(int, int);
  void signalUndoState(bool);
  void signalRedoState(bool);
  void signalFileChanged(bool);

protected:
  void paintFrame(ViewPainter*);

  // overloaded function to get actions of user
  void paintEvent(QPaintEvent * /*event*/);

  void mouseMoveEvent(QMouseEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseDoubleClickEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void wheelEvent(QWheelEvent*);
  void dropEvent(QDropEvent*);
  void dragEnterEvent(QDragEnterEvent*);
  void dragLeaveEvent(QDragLeaveEvent*);
  void dragMoveEvent(QDragMoveEvent*);
  void resizeEvent(QResizeEvent*);

  void contentMouseMoveEvent(QMouseEvent*);
  void contentMousePressEvent(QMouseEvent*);
  void contentMouseDoubleClickEvent(QMouseEvent*);
  void contentMouseReleaseEvent(QMouseEvent*);

private:
  bool dragIsOkay;
  /*! \brief hold system-independent information about a schematic file */
  QFileInfo FileInfo;

/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_element.cpp". They only access the QPtrList  *****
   *****  pointers "Wires", "Nodes", "Diagrams", "Paintings" and  *****
   *****  "Components".                                           *****
   ******************************************************************** */

public:
  std::shared_ptr<Node> insertNode(int, int, const std::shared_ptr<Element> &);
  Node* selectedNode(int, int);

  int   insertWireNode1(const std::shared_ptr<Wire> &);
  bool  connectHWires1(const std::shared_ptr<Wire> &);
  bool  connectVWires1(const std::shared_ptr<Wire> &);
  int   insertWireNode2(const std::shared_ptr<Wire> &);
  bool  connectHWires2(const std::shared_ptr<Wire> &);
  bool  connectVWires2(const std::shared_ptr<Wire> &);
  int   insertWire(const std::shared_ptr<Wire> &);
  void  selectWireLine(const std::shared_ptr<Element> &, Node *, bool);
  std::shared_ptr<Wire> selectedWire(int, int);
  std::shared_ptr<Wire> splitWire(const std::shared_ptr<Wire> &, const std::shared_ptr<Node> &);
  bool  oneTwoWires(const std::shared_ptr<Node> &);
  void  deleteWire(const WireList::iterator &);

  Marker* setMarker(int, int);
  void    markerLeftRight(bool, SharedObjectList<Element> &);
  void    markerUpDown(bool, SharedObjectList<Element> &);

  std::shared_ptr<Element> selectElement(float, float, bool, int *index=0);
  void     deselectElements(const std::shared_ptr<Element> &);
  int      selectElements(int, int, int, int, bool);
  void     selectMarkers();
  void     newMovingWires(SharedObjectList<Element> &, Node*, int);
  int      copySelectedElements(SharedObjectList<Element> &);
  bool     deleteElements();
  bool     aligning(int);
  bool     distributeHorizontal();
  bool     distributeVertical();

  void       setComponentNumber(const std::shared_ptr<Component> &);
  void       insertRawComponent(const ComponentList::holder &, bool noOptimize=true);
  void       recreateComponent(const std::shared_ptr<Component> &);
  void       insertComponent(const std::shared_ptr<Component> &);
  void       activateCompsWithinRect(int, int, int, int);
  bool       activateSpecifiedComponent(int, int);
  bool       activateSelectedComponents();
  void       setCompPorts(std::shared_ptr<Component> &);
  std::shared_ptr<Component> selectCompText(int, int, int&, int&);
  Component* searchSelSubcircuit();
  std::shared_ptr<Component> selectedComponent(int, int);
  void       deleteComp(const ComponentList::iterator &);

  void     oneLabel(Node*);
  int      placeNodeLabel(const std::shared_ptr<WireLabel> &);
  std::shared_ptr<Element> getWireLabel(Node*);
  void     insertNodeLabel(const std::shared_ptr<WireLabel> &);
  void     copyLabels(int&, int&, int&, int&, SharedObjectList<Element> &);

  Painting* selectedPainting(float, float);
  void      copyPaintings(int&, int&, int&, int&, SharedObjectList<Element> &);


private:
  void insertComponentNodes(const std::shared_ptr<Component> &, bool);
  int  copyWires(int&, int&, int&, int&, SharedObjectList<Element> &);
  int  copyComponents(int&, int&, int&, int&, SharedObjectList<Element> &);
  void copyComponents2(int&, int&, int&, int&, SharedObjectList<Element> &);
  bool copyComps2WiresPaints(int&, int&, int&, int&, SharedObjectList<Element> &);
  int  copyElements(int&, int&, int&, int&, SharedObjectList<Element> &);


/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_file.cpp". They only access the QPtrLists    *****
   *****  and their pointers. ("DocComps", "Components" etc.)     *****
   ******************************************************************** */

public:
  static int testFile(const QString &);
  bool createLibNetlist(QTextStream*, QPlainTextEdit*, int);
  bool createSubNetlist(QTextStream *, int&, QStringList&, QPlainTextEdit*, int);
  void createSubNetlistPlain(QTextStream*, QPlainTextEdit*, int);
  int  prepareNetlist(QTextStream&, QStringList&, QPlainTextEdit*);
  QString createNetlist(QTextStream&, int);
  bool loadDocument();
  bool loadVerilogDocument();
  void highlightWireLabels (void);

private:
  int  saveDocument(QString OutputFileName, QString OutputTypeName);
  int  saveSchematicDocument(QFile *file);
  QString getWireName(const QPoint *p)const; // BUG // names are key!

private: /// BUG // move to Verilog class, create if needed.
  void dumpDeclaration(QTextStream& stream, const Component *c, QString model, QString name, QList<QPoint> ports) const;
  void dumpVerilogComponent(QTextStream& stream, Component const* c) const;
  void dumpVerilogWire(QTextStream& stream, Wire const* w) const;
  void dumpVerilogQucsPreamble(QTextStream& stream) const;
  int  saveVerilogDocument(QFile *file);

  bool loadProperties(QTextStream*);
  void simpleInsertComponent(const std::shared_ptr<Component> &);
  bool loadComponents(QTextStream*, SharedObjectList<Element> *List=0);
  void simpleInsertWire(const std::shared_ptr<Wire> &);
  bool loadWires(QTextStream*, SharedObjectList<Element> *List=0);
  bool loadDiagrams(QTextStream*, SharedObjectList<Element> &);
  bool loadDiagrams(QTextStream*, DiagramList &);
  bool loadPaintings(QTextStream*, SharedObjectList<Element> &);
  bool loadPaintings(QTextStream*, PaintingList &);
  bool loadIntoNothing(QTextStream*);

  QString createClipboardFile();
  bool    pasteFromClipboard(QTextStream *, SharedObjectList<Element> &);

  QString createUndoString(char);
  bool    rebuild(const QString &);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(const QString &);

  static void createNodeSet(QStringList&, int&, const std::shared_ptr<Conductor> &, const std::shared_ptr<Node> &pn);
  void throughAllNodes(bool, QStringList&, int&);
  void propagateNode(QStringList&, int&, const std::shared_ptr<Node> &);
  void collectDigitalSignals(void);
  bool giveNodeNames(QTextStream *, int&, QStringList&, QPlainTextEdit*, int);
  void beginNetlistDigital(QTextStream &);
  void endNetlistDigital(QTextStream &);
  bool throughAllComps(QTextStream *, int&, QStringList&, QPlainTextEdit *, int);

  DigMap Signals; // collecting node names for VHDL signal declarations
  QStringList PortTypes;

public: // for now. move to parser asap
        bool loadComponent(const QString& _s, const std::shared_ptr<Component> &c) const;

public:
  bool isAnalog;
  bool isVerilog;
  bool creatingLib;

public: // serializer
  void saveComponent(QTextStream& s, Component /* FIXME const */* c) const;
};

#endif
