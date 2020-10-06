/***************************************************************************
    copyright            : (C) 2006 by Michael Margraf
                               2018, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_SCHEMATIC_DOC_H
#define QUCS_SCHEMATIC_DOC_H

#include "actions.h"
#include "node.h"
#include "qucsdoc.h"
#include "viewpainter.h"
#include "diagram.h"
#include "paintings/painting.h"
#include "components/component.h"
#include "sim/sim.h"
#include "sim/tasks.h"
#include "schematic_scene.h"
#include "schematic_model.h"
#include "schematic_lang.h"
#include "schematic_action.h"
#include "qt_compat.h"

#include <QGraphicsView>

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
class QUndoCommand;
class MouseAction;

class SchematicDoc;
class MouseActions;
typedef bool (SchematicDoc::*pToggleFunc) ();
typedef void (MouseActions::*pMouseFunc) (SchematicDoc*, QMouseEvent*);
typedef void (MouseActions::*pMouseFunc2) (SchematicDoc*, QMouseEvent*);

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
// BUG: schematic_symbol
struct SubFile {
  SubFile() { Type=""; File=""; PortTypes.clear(); }
  SubFile(const QString& _Type, const QString& _File)
    : Type(_Type), File(_File) { PortTypes.clear(); }
  QString Type;          // type of file
  QString File;          // file name identifier
  QStringList PortTypes; // data types of in/out signals
};
typedef QMap<QString, SubFile> SubMap;

class ElementGraphics;

/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// TODO: add SymbolDocument (some other place)
class SchematicDoc : public QGraphicsView, public QucsDoc {
  Q_OBJECT
private:
  friend class SchematicActions;
private:
  SchematicDoc(SchematicDoc const&x) = delete;
public:
  typedef QList<ElementGraphics*> EGPList;
public:
  SchematicDoc(QucsApp&, const QString&);
 ~SchematicDoc();

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
	  assert(_model);
	  return _model->sizeOfAll(a, b, c, d, textCorr());
  }
public:
  void pushBack(Element* what);
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  QPoint  setOnGrid(int, int) const;
  QPoint  snapToGrid(QPointF const&) const;
  QMouseEvent snapToGrid(QMouseEvent* e) const;
  bool  elementsOnGrid();

  float zoom(float);
  float zoomBy(float) override;
  void  showAll();
  void  showNoZoom();
  void  enlargeView(int, int, int, int);
  void  switchPaintMode();
  int   adjustPortNumbers();
  void  reloadGraphs();
  bool  createSubcircuitSymbol();

  void    cut();
  void    copy();
  template<class SOME_LIST>
  bool    paste(DocumentStream*, SOME_LIST*);
  bool    load();
  int     save();
  int     saveSymbolCpp (void);
  int     saveSymbolJSON (void);
  void    becomeCurrent(bool);

  bool scrollUp(int);
  bool scrollDown(int);
  bool scrollLeft(int);
  bool scrollRight(int);

public: // scene
  void sceneAddItem(ElementGraphics*);
  void sceneRemoveItem(ElementGraphics*);

private:
	void deleteItem(ElementGraphics*);

public:
  SchematicScene const* scene() const{itested();
	  return prechecked_cast<SchematicScene const*>(QGraphicsView::scene()); }
private:
  // schematic Scene for this View
//  SchematicScene *Scene;
  SchematicScene *scene() {itested(); return prechecked_cast<SchematicScene*>(QGraphicsView::scene()); }
  // schematic frame item
  // Frame *SchematicFrame;
  void drawBackground(QPainter *painter, const QRectF& rect);
public:
  SchematicScene *sceneHACK() {untested(); return scene(); }

  // TODO: take iterator?
  Element* detachFromModel(Element* e){
	  assert(_model);
	  return _model->detach(e);
  }

  void takeOwnership(Element* e){
	  // bit of a hack.
	  e->setOwner(_root);
  }

  void deselectElements();

  // find_symbol
public:
  bool isNode(int x, int y) const{
	  assert(_model);
	  return _model->isNode(x,y);
  }
  Component* find_component(QString const&);

  // really?
  // CommandList const& commands() const{
  //    return _commands;
  // }
  CmdEltList& commands(){
	  return _commands;
  }
  CmdEltList const& commands() const{
	  return _commands;
  }
  ComponentList const& components() const{
	  assert(_model);
	  return _model->components();
  }
  NodeMap const& nodes() const{
	  assert(_model);
	  return _model->nodes();
  }
  WireList const& wires() const{
	  assert(_model);
	  return _model->wires();
  }
  DiagramList const& diagrams() const{
	  assert(_model);
	  return _model->diagrams();
  }
  PaintingList& paintings(){
	  assert(_model);
	  return _model->paintings();
  }
  PaintingList const& paintings() const{
	  assert(_model);
	  return _model->paintings();
  }
  unsigned numberOfNets() const{
	  assert(_model);
	  return _model->numberOfNets();
  }
  
	// transition
  PaintingList  SymbolPaints;  // symbol definition for subcircuit
  PaintingList& symbolPaints(){
	  return SymbolPaints; // -> schematicSymbol
  }
  PaintingList const& symbolPaints() const{
	  return SymbolPaints; // -> schematicSymbol
  }
  PaintingList& symbolPaintings(){
	  return SymbolPaints; // -> schematicSymbol
  }
  PaintingList const& symbolPaintings() const{
	  return SymbolPaints; // -> schematicSymbol
  }

  QList<PostedPaintEvent>   PostedPaintEvents;
public:
  bool isSymbolMode() const{
	  return false;
  }

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
  void setFrameText(int idx, QString s);
private:
  QString _frameText[4];
  int GridX, GridY;

public:
  QPoint gridSize() const{ return QPoint(GridX, GridY); }
  QString frameText0() const { return _frameText[0]; }
  QString frameText1() const { return _frameText[1]; }
  QString frameText2() const { return _frameText[2]; }
  QString frameText3() const { return _frameText[3]; }

  // Two of those data sets are needed for Schematic and for symbol.
  // Which one is in "tmp..." depends on "symbolMode".
  float tmpScale;
  int tmpViewX1, tmpViewY1, tmpViewX2, tmpViewY2;
  int tmpUsedX1, tmpUsedY1, tmpUsedX2, tmpUsedY2;

  /*! \brief Get (schematic) file reference */
  QFileInfo getFileInfo (void) {
	  assert(_model);
	  return _model->getFileInfo();
  }
  /*! \brief Set reference to file (schematic) */

signals:
  void signalCursorPosChanged(int, int);
  void signalUndoState(bool);
  void signalRedoState(bool);
  void signalFileChanged(bool);

protected:
  void paintFrame(ViewPainter*);

protected:
  void showEvent(QShowEvent*) override;
//  void hideEvent(QShowEvent*) override;

protected: // these are the overrides that collect mouse actions
           // forward to mouseAction instances to produce UndoActions
   bool event(QEvent*) override;
   void mouseMoveEvent(QMouseEvent*) override;
//   void mousePressEvent(QMouseEvent*) override;
//   void mouseDoubleClickEvent(QMouseEvent*) override;
//   void mouseReleaseEvent(QMouseEvent*) override;
//   void wheelEvent(QWheelEvent*) override;
   void dropEvent(QDropEvent*e) override { untested();
		QGraphicsView::dropEvent(e);
	}
   void dragEnterEvent(QDragEnterEvent*e) override { untested();
		QGraphicsView::dragEnterEvent(e);
	}
//   void dragEnterEvent(QDragEnterEvent*) override;
//   void dragLeaveEvent(QDragLeaveEvent*) override;
//   void dragMoveEvent(QDragMoveEvent*) override;

public: // qt4 debris
//   QPointF mapToScene(QPoint const& p) const;

private:
public:
  // possible rule: return pointer:   transfer ownership
  //                return reference: don't
  // (same for arguments?)
  Element* eraseFromScene(ElementGraphics*);
  QGraphicsItem& addToScene(Element*); // called from SM::pushBack.

protected slots:
  void slotScrollUp();
  void slotScrollDown();
  void slotScrollLeft();
  void slotScrollRight();
  void printCursorPosition(int x_, int y_);

private:
  bool dragIsOkay;
  /*! \brief hold system-independent information about a schematic file */

//  void removeWire(Wire const* w);

private:
/* ********************************************************************
   *****  The following methods are in the file                   *****
   *****  "schematic_element.cpp". They only access the QPtrList  *****
   *****  pointers "Wires", "Nodes", "Diagrams", "Paintings" and  *****
   *****  "Components".                                           *****
   ******************************************************************** */

public: // mostly not here
  Node* insertNode(int x, int y, Element* e){
	  assert(_model);
	  return _model->insertNode(x, y, e);
  }
  Node* selectedNode(int, int);

#if 0
  int   insertWireNode1(Wire* w){
	  assert(_model);
	  return _model->insertWireNode1(w);
  }
  bool  connectHWires1(Wire* w){
	  assert(_model);
	  return _model->connectHWires1(w);
  }
  bool  connectVWires1(Wire* w){
	  assert(_model);
	  return _model->connectVWires1(w);
  }
  int   insertWireNode2(Wire* w){
	  assert(_model);
	  return _model->insertWireNode2(w);
  }
  bool  connectHWires2(Wire* w){
	  assert(_model);
	  return _model->connectHWires2(w);
  }
  bool  connectVWires2(Wire* w){
	  assert(_model);
	  return _model->connectVWires2(w);
  }
  Wire* splitWire(Wire* w, Node* n){
	  assert(_model);
	  return _model->splitWire(w, n);
  }
  bool  oneTwoWires(Node* n){
	  assert(_model);
	  return _model->oneTwoWires(n);
  }
  void  deleteWire(Wire* w){
	  //called from mouse actions...?
	  assert(_model);
	  return _model->deleteWire(w);
  }
#endif

  Marker* setMarker(int, int);

private: // FIXME: remove
  void    markerLeftRight(bool, Q3PtrList<ElementGraphics>*);
  void    markerUpDown(bool, Q3PtrList<ElementGraphics>*);
public:
  void    markerMove(arrow_dir_t d, Q3PtrList<ElementGraphics>* l){
	  switch(d){
		  case arr_up:
		  case arr_down:
			    markerUpDown(d==arr_up, l);
				 break;
		  case arr_left:
		  case arr_right:
				 markerLeftRight(d==arr_left, l);
				 break;
	  }
  }

  // now in mouseactions
  // Element* selectElement(QPoint const&, bool, int *index=0);
  ElementGraphics* itemAt(float, float);
  ElementGraphics* itemAt(QPointF x) { return itemAt(x.x(), x.y());}
  int      selectElements(int, int, int, int, bool);
  void     selectMarkers();
  void     newMovingWires(QList<Element*>*, Node*, int);
  QList<ElementGraphics*> cropSelectedElements();
  QList<ElementGraphics*> selectedItems();
  QUndoCommand* deleteElements();
  bool     aligning(int);
  bool     distributeHorizontal();
  bool     distributeVertical();

  void       setComponentNumber(Component*);
  void       insertRawComponent(Component*, bool noOptimize=true);
  void       recreateSymbol(Symbol* s){
	  assert(_model);
	  _model->recreateSymbol(s);
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

public: // not here.
  static int testFile(const QString &);
  bool loadDocument();
  void highlightWireLabels (void);

public:
	bool loadDocument(QFile& /*BUG*/ file);
	bool load(QTextStream& stream);

private:
	void parse(DocumentStream& stream, SchematicLanguage const*l=nullptr);

private:
  void simpleInsertElement(Element*);

  void saveDocument() const;

  bool loadWires(QTextStream*, EGPList *List=0);
  bool loadIntoNothing(DocumentStream*);

  bool    pasteFromClipboard(DocumentStream *, EGPList*);

  bool    rebuild(QString *);
  bool    rebuildSymbol(QString *);

  static void createNodeSet(QStringList&, int&, Conductor*, Node*);

  void updateViewport() { viewport()->update(); }

public:
  void throughAllNodes(unsigned& count) const{
	  assert(_model);
	  return _model->throughAllNodes(count);
  }

private:
  void propagateNode(Node*) const;
  void collectDigitalSignals(void);
  bool giveNodeNames(QTextStream *, int&, QStringList&, QPlainTextEdit*,
		  int, NetLang const&);

  DigMap Signals; // collecting node names for VHDL signal declarations

public: // for now
	CmdElement* loadCommand(const QString& _s, CmdElement* c) const{
		assert(_model);
		return _model->loadCommand(_s, c);
	}
	Component* loadComponent(const QString& _s, Component* c) const{
		assert(_model);
		return _model->loadComponent(_s, c);
	}
   int  prepareNetlist(DocumentStream& a, QStringList& b, QPlainTextEdit* c,
			bool creatingLib, NetLang const& nl){
		assert(!creatingLib); // ?!
 	  return _model->prepareNetlist(a,b,c, creatingLib, nl);
   }
  bool createLibNetlist(DocumentStream& a, QPlainTextEdit* b, int c, NetLang const& nl){
	  assert(_model);
		return _model->createLibNetlist(a,b,c, nl);
  }

	QString getParameter(std::string const& key) const;

public:
  bool isAnalog;
  bool isVerilog;
  bool creatingLib;

private: // QucsDoc overrides, schematic_action.cpp
  void actionCopy(QAction*) override{
	  copy();
  }
  void actionCut(QAction*) override{
	  cut();
  }
  void actionEditUndo(QAction*) override;
  void actionEditRedo(QAction*) override;
  void actionAlign(int what) override;
  void actionDistrib(int dir) override; // 0=horiz, 1=vert

  void actionApplyCompText() override;
  void actionSelect(QAction*) override;
  void actionSelectMarker() override;
  void actionSelectAll(QAction*) override;
  void actionChangeProps(QAction*) override;
  void actionCursor(arrow_dir_t) override;

  void actionOnGrid(QAction*) override;
  void actionEditRotate(QAction*) override;
  void actionEditMirrorX(QAction*) override;
  void actionEditMirrorY(QAction*) override;
  void actionEditActivate(QAction*) override;
  void actionEditDelete(QAction*) override;
  void actionEditPaste(QAction*) override;
  void actionSetWire(QAction*) override;
  void actionInsertLabel(QAction*) override;
  void actionInsertEquation(QAction*) override;
  void actionInsertPort(QAction*) override;
  void actionInsertGround(QAction*) override;
  void actionSetMarker(QAction*) override;
  void actionMoveText(QAction*) override;
  void actionZoomIn(QAction*) override;
  void actionExportGraphAsCsv(); // BUG
  void actionSelectElement(QObject*) override;

public:
	bool handleMouseActions(QEvent* e);
private:
  bool performToggleAction(bool, QAction*, pToggleFunc, pMouseFunc, pMouseFunc2); // this is nuts.

  SchematicActions* _mouseActions; //needed? no. FIXME
  MouseActions* mouseActions() override { assert(_mouseActions); return _mouseActions; }
  SchematicActions& schematicActions() { assert(_mouseActions); return *_mouseActions; }
//  MouseAction* mouseAction(); QucsDoc

  void setDrawn(bool b=true){mouseActions()->setDrawn(b);}
  QUndoStack* undoStack() override{ return _undoStack; }
public: // serializer
  void saveComponent(QTextStream& s, Component const* c) const;
  SchematicSymbol const* root() const { return _root; }

public: // need access to SchematicModel. grr
  friend class MouseActions;
  friend class ImageWriter;

private:
  SchematicSymbol* _root;
  SchematicModel* _model;
  CmdEltList _commands;
  QUndoStack* _undoStack;
}; // SchematicDocument
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// Peeforms paste function from clipboard
template<class SOME_LIST>
inline bool SchematicDoc::paste(DocumentStream *stream, SOME_LIST *pe)
{ untested();
  return pasteFromClipboard(stream, pe);
}

#endif
