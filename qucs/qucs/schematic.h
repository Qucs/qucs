/***************************************************************************
                               schematic.h
                              -------------
    copyright            : (C) 2006 by Michael Margraf
                               2018, 2019 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_SCHEMATIC_H
#define QUCS_SCHEMATIC_H

#include "actions.h"
#include "wire.h"
#include "node.h"
#include "qucsdoc.h"
#include "viewpainter.h"
#include "diagram.h"
#include "paintings/painting.h"
#include "components/component.h"
#include "sim/sim.h"
#include "schematic_scene.h"
#include "schematic_model.h"
#include "schematic_lang.h"

#ifdef USE_SCROLLVIEW
#include <Q3ScrollView>
#else
#include <QGraphicsView>
#endif

#include "qt_compat.h"
#include <QVector>
#include <QStringList>
#include <QFileInfo>

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

// TODO: rename to SchematicDocument
class Schematic;
class MouseActions;
typedef bool (Schematic::*pToggleFunc) ();
typedef void (MouseActions::*pMouseFunc) (Schematic*, QMouseEvent*);
typedef void (MouseActions::*pMouseFunc2) (Schematic*, QMouseEvent*);

// digital signal data
struct DigSignal {
  DigSignal() { Name=""; Type=""; }
  DigSignal(const QString& _Name, const QString& _Type = "")
    : Name(_Name), Type(_Type) {}
  QString Name; // name
  QString Type; // type of signal
};
typedef QMap<QString, DigSignal> DigMap;
typedef enum {_NotRop, _Rect, _Line, _Ellipse, _Arc, _DotLine, _Translate, _Scale}PE;
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

#if QT_MAJOR_VERSION < 5
typedef Element ElementGraphics;
#define SchematicBase Q3ScrollView
#else
// strictly, this should also work with qt4.
class ElementGraphics;
#define SchematicBase QGraphicsView
#endif

// TODO: rename to SchematicDocument
class Schematic : public SchematicBase, public QucsDoc {
  Q_OBJECT
private:
  Schematic(Schematic const&x): SchematicBase(), QucsDoc(x), DocModel(this){ unreachable(); }
public:
  typedef Q3PtrList<ElementGraphics> EGPList;
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
private: //temporary/obsolete
  void sizeOfAll(int&a, int&b, int&c, int&d){
	  return DocModel.sizeOfAll(a, b, c, d, textCorr());
  }
public:
  void pushBack(Element* what);
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  void  setOnGrid(int&, int&);
  bool  elementsOnGrid();

  float zoom(float);
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
  bool    paste(QTextStream*, EGPList*);
  bool    load();
  int     save();
  int     saveSymbolCpp (void);
  int     saveSymbolJSON (void);
  void    becomeCurrent(bool);

  bool scrollUp(int);
  bool scrollDown(int);
  bool scrollLeft(int);
  bool scrollRight(int);

#ifndef USE_SCROLLVIEW
private:
  // schematic Scene for this View
  SchematicScene *Scene;
  SchematicScene *scene() { return Scene; }
  // schematic frame item
  // Frame *SchematicFrame;
#endif

// TODO: const access ONLY
  ComponentList& components(){
	  return DocModel.components();
  }
  NodeList& nodes(){
	  return DocModel.nodes();
  }
  DiagramList& diagrams(){
	  return DocModel.diagrams();
  }
  WireList& wires(){
	  return DocModel.wires();
  }
  ComponentList const& components() const{
	  return DocModel.components();
  }
  NodeList const& nodes() const{
	  return DocModel.nodes();
  }
  WireList const& wires() const{
	  return DocModel.wires();
  }
  DiagramList const& diagrams() const{
	  return DocModel.diagrams();
  }
  PaintingList& paintings(){
	  return DocModel.paintings();
  }
  PaintingList const& paintings() const{
	  return DocModel.paintings();
  }
  unsigned numberOfNets() const{
	  return DocModel.numberOfNets();
  }
  
	// transition
  PaintingList  SymbolPaints;  // symbol definition for subcircuit
  PaintingList& symbolPaints(){
	  return SymbolPaints; // -> schematicSymbol
  }
  PaintingList const& symbolPaints() const{
	  return SymbolPaints; // -> schematicSymbol
  }

  QList<PostedPaintEvent>   PostedPaintEvents;
public:
  void setSymbolMode(bool x){
	  SymbolMode = x;
  }
  bool isSymbolMode() const{
	  return SymbolMode;
  }


  int GridX, GridY;
  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area
  int UsedX1, UsedY1, UsedX2, UsedY2;  // document area used by elements

  int ShowFrame; // BUG// it's the frame type.
  int showFrame() const{ return ShowFrame; }
  void setFrameType(int t){
	  if(t != ShowFrame){
		  setChanged(true);
		  ShowFrame = t;
	  }else{
	  }
  }

  // BUG: use Frame::setParameter
  void setFrameText(int idx, QString s){
	  if(s != FrameText[idx]){
		  setChanged(true);
		  FrameText[idx] = s;
	  }else{
	  }
  }

private:
  QString FrameText[4];
public:
  QString frameText0() const { return FrameText[0]; }
  QString frameText1() const { return FrameText[1]; }
  QString frameText2() const { return FrameText[2]; }
  QString frameText3() const { return FrameText[3]; }

  // Two of those data sets are needed for Schematic and for symbol.
  // Which one is in "tmp..." depends on "symbolMode".
  float tmpScale;
  int tmpViewX1, tmpViewY1, tmpViewX2, tmpViewY2;
  int tmpUsedX1, tmpUsedY1, tmpUsedX2, tmpUsedY2;

  int undoActionIdx;
  QVector<QString *> undoAction;
  int undoSymbolIdx;
  QVector<QString *> undoSymbol;    // undo stack for circuit symbol

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
#ifdef USE_SCROLLVIEW
  void drawContents(QPainter*, int, int, int, int);
#endif
  void contentsMouseMoveEvent(QMouseEvent*);
  void contentsMousePressEvent(QMouseEvent*);
  void contentsMouseDoubleClickEvent(QMouseEvent*);
  void contentsMouseReleaseEvent(QMouseEvent*);
  void contentsWheelEvent(QWheelEvent*);
  void contentsDropEvent(QDropEvent*);
  void contentsDragEnterEvent(QDragEnterEvent*);
  void contentsDragLeaveEvent(QDragLeaveEvent*);
  void contentsDragMoveEvent(QDragMoveEvent*);

public:
#ifdef USE_SCROLLVIEW
  QPointF mapToScene(QPoint const& p) const;
#endif

protected slots:
  void slotScrollUp();
  void slotScrollDown();
  void slotScrollLeft();
  void slotScrollRight();
  void printCursorPosition(int x_, int y_);

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
  Node* insertNode(int x, int y, Element* e){
	  return DocModel.insertNode(x, y, e);
  }
  Node* selectedNode(int, int);

  int   insertWireNode1(Wire*);
  bool  connectHWires1(Wire*);
  bool  connectVWires1(Wire*);
  int   insertWireNode2(Wire*);
  bool  connectHWires2(Wire*);
  bool  connectVWires2(Wire*);
  int   insertWire(Wire*);
  void  selectWireLine(ElementGraphics*, Node*, bool);
  Wire* selectedWire(int, int);
  Wire* splitWire(Wire* w, Node* n){
	  return DocModel.splitWire(w, n);
  }
  bool  oneTwoWires(Node* n){
	  return DocModel.oneTwoWires(n);
  }
  void  deleteWire(Wire*);

  Marker* setMarker(int, int);

private: // FIXME: remove
  void    markerLeftRight(bool, Q3PtrList<Element>*);
  void    markerUpDown(bool, Q3PtrList<Element>*);
public:
  void    markerMove(arrow_dir_t d, Q3PtrList<Element>* l){
	  switch(d){
		  case arr_up:
		  case arr_down:
			  return   markerUpDown(d==arr_up, l);
		  case arr_left:
		  case arr_right:
			  return   markerLeftRight(d==arr_left, l);
	  }
  }

  // now in mouseactions
  // Element* selectElement(QPoint const&, bool, int *index=0);
  ElementGraphics* itemAt(float, float);
  ElementGraphics* itemAt(QPointF x) { return itemAt(x.x(), x.y());}
  int      selectElements(int, int, int, int, bool);
  void     selectMarkers();
  void     newMovingWires(Q3PtrList<Element>*, Node*, int);
  Q3PtrList<ElementGraphics> cropSelectedElements();
  bool     deleteElements();
  bool     aligning(int);
  bool     distributeHorizontal();
  bool     distributeVertical();

  void       setComponentNumber(Component*);
  void       insertRawComponent(Component*, bool noOptimize=true);
  void       recreateSymbol(Symbol* s){
	  DocModel.recreateSymbol(s);
  }
  void       insertElement(Element*);
private: // old legacy stuff.
  void       insertComponent(Component*);
public:
  void       activateCompsWithinRect(int, int, int, int);
  bool       activateSpecifiedComponent(int, int);
  bool       activateSelectedComponents();
  void       setCompPorts(Component*);
  Component* searchSelSubcircuit();
  Component* selectedComponent(int, int);
  void     oneLabel(Node*);
  int      placeNodeLabel(WireLabel*);
  Element* getWireLabel(Node*);
  void     insertNodeLabel(WireLabel*);
  void     copyLabels(int&, int&, int&, int&, QList<Element *> *);

  Painting* selectedPainting(float, float);
  void      copyPaintings(int&, int&, int&, int&, QList<Element *> *);


private:
  void insertComponentNodes(Component*, bool);
  int  copyWires(int&, int&, int&, int&, QList<Element *> *);
  int  copyComponents(int&, int&, int&, int&, QList<Element *> *);
  void copyComponents2(int&, int&, int&, int&, QList<Element *> *);
  bool copyComps2WiresPaints(int&, int&, int&, int&, QList<Element *> *);
  int  copyElements(int&, int&, int&, int&, QList<Element *> *);


/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_file.cpp". They only access the QPtrLists    *****
   *****  and their pointers. ("DocComps", "Components" etc.)     *****
   ******************************************************************** */

public: // TODO: move out of the way, perhaps to SchematicNetlist,
        // SchematicModel or so.
  static int testFile(const QString &);
  bool createLibNetlist(QTextStream*, QPlainTextEdit*, int, NetLang const& nl);
  void createSubNetlistPlain(QTextStream*, QPlainTextEdit*, int);
  int  prepareNetlist(QTextStream&, QStringList&, QPlainTextEdit*, const NetLang&);
  QString createNetlist(QTextStream&, int, NetLang const&);
  bool loadDocument();
  void highlightWireLabels (void);

private: // tmp hack. use SchematicModel.
  bool do_loadDocument(QFile&);
  void parse(DocumentStream& stream, SchematicLanguage const*l=nullptr);

private: // legacy, don't use
  void simpleInsertComponent(Component*);
  void simpleInsertCommand(Command*);
private:
  void simpleInsertElement(Element*);

  void saveDocument() const;

  bool loadProperties(QTextStream*);
  bool loadComponents(QTextStream*, ComponentList *List=0);
  void simpleInsertWire(Wire*);
  bool loadWires(QTextStream*, WireList*List=0);
  bool loadDiagrams(QTextStream*, DiagramList*);
  bool loadPaintings(QTextStream*, PaintingList*);
  bool loadIntoNothing(QTextStream*);

  bool    pasteFromClipboard(QTextStream *, EGPList*);

  QString createUndoString(char);
  bool    rebuild(QString *);
  QString createSymbolUndoString(char);
  bool    rebuildSymbol(QString *);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);

public:
  void throughAllNodes(unsigned& count) const{
	  return DocModel.throughAllNodes(count);
  }

private:
  void propagateNode(Node*) const;
  void collectDigitalSignals(void);
  bool giveNodeNames(QTextStream *, int&, QStringList&, QPlainTextEdit*,
		  int, NetLang const&);
  void beginNetlistDigital(QTextStream &, NetLang const&);
  void endNetlistDigital(QTextStream &, NetLang const&);
//  bool throughAllComps(QTextStream *, int&, QStringList&, QPlainTextEdit *,
//		  int, NetLang const&);

  DigMap Signals; // collecting node names for VHDL signal declarations
public: // BUG
  QStringList PortTypes;

public: // for now. move to parser asap
	Element* loadElement(const QString& _s, Element* c) const;
	Component* loadComponent(const QString& _s, Component* c) const;
	Command* loadCommand(const QString& _s, Command* c) const;

	QString getParameter(std::string const& key) const;

public: // schematicModel
	QString const& portType(int i) const{
		return PortTypes[i];
	}
public:
  bool isAnalog;
  bool isVerilog;
  bool creatingLib;

private: // action overrides, schematic_action.cpp
  void actionCopy(){
	  copy();
  }
  void actionCut(){
	  cut();
  }
  void actionEditUndo();
  void actionEditRedo();
  void actionAlign(int what);
  void actionDistrib(int dir); // 0=horiz, 1=vert

  void actionApplyCompText();
  void actionSelect(bool);
  void actionSelectMarker();
  void actionSelectAll();
  void actionChangeProps();
  void actionCursor(arrow_dir_t);

  void actionOnGrid(bool);
  void actionEditRotate(bool);
  void actionEditMirrorX(bool);
  void actionEditMirrorY(bool);
  void actionEditActivate(bool);
  void actionEditDelete(bool);
  void actionEditPaste(bool);
  void actionSetWire(bool);
  void actionInsertLabel(bool);
  void actionInsertEquation(bool);
  void actionInsertPort(bool);
  void actionInsertGround(bool);
  void actionSetMarker(bool);
  void actionMoveText(bool);
  void actionZoomIn(bool);
  void actionExportGraphAsCsv(); // BUG

private:
  bool performToggleAction(bool, QAction*, pToggleFunc, pMouseFunc, pMouseFunc2);

public: // serializer
  void saveComponent(QTextStream& s, Component const* c) const;

public: // need access to SchematicModel. grr
  friend class MouseActions;
  friend class ImageWriter;

private:
public: // BUG. use symbol
  SchematicModel DocModel;
private:
  bool SymbolMode;


private: // obsolete.
  void       deleteComp(Component*c){
	  unreachable();
	  return DocModel.erase(c);
  }
};


//tmp hack
inline bool Schematic::do_loadDocument(QFile& /*BUG*/ file)
{ untested();

  QString Line;
  DocumentStream stream(&file);

  // read header **************************
  do {
    if(stream.atEnd()) {
      file.close(); // BUG
      return true;
    }

    Line = stream.readLine();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    // BUG: throw
 throw "Wrong document type: ";
    return false;
  }

  Line = Line.mid(16, Line.length()-17);

  parse(stream);
  file.close();
  return true;
}

#endif
