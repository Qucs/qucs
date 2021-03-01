/***************************************************************************
    copyright            : (C) 2006 by Michael Margraf
                               2018, 2019, 2020, 2021 Felix Salfelder
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
#include "simulator.h"
#include "tasks.h"
#include "schematic_scene.h"
#include "schematic_model.h"
#include "schematic_lang.h"
#include "schematic_action.h"
#include "qt_compat.h"

#include <QGraphicsView>

#include <QVector>
#include <QStringList>
#include <QFileInfo>
#include <QAction>

#include <QApplication>
#include <QClipboard>
#include <QShortcut>

// #include "schematic_doc.h"
#include "misc.h"
#include "qucs_globals.h"
#include "qucs_app.h" // BUG. QucsSettings?
#include <QUndoStack>
#include "simmessage.h"
#include "docfmt.h" // copy&paste
#include "sckt_base.h"
#include <QMessageBox> // really??

// /// workaround. deduplicate event forwarding.
// this could be done using Qt. but not yet.
// Toolbar::?
// #define ASSIGN_STUFF
//   auto s = prechecked_cast<QAction*>(sender());
//   assert(s);

namespace{

using namespace qucs;

class SchematicDoc;

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
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
class SchematicDoc : public QGraphicsView /* bug?! */, public Doc {
	Q_OBJECT
	// Q_INTERFACES(Doc) makes no difference.
public:
	typedef QList<ElementGraphics*> EGPList;

public:
	SchematicDoc(SchematicDoc const&x) = delete;
	explicit SchematicDoc(); // (QucsApp* /*BUG?*/, const QString&, QWidget* owner);
	~SchematicDoc();

	Widget* clone() const{ untested();
		return new SchematicDoc( /*this?*/ );
	}
	void setParent(QWidget*) override;
	void expand();

	void setName(const QString&); // what is this?
	void setChanged(bool, bool fillStack=false, char Op='*');
	void paintGrid(ViewPainter*, int, int, int, int);
	void print(QPrinter*, QPainter*, bool, bool) const{incomplete();}
	
	void paintSchToViewpainter(ViewPainter* p, bool printAll, bool toImage, int screenDpiX=96, int printerDpiX=300);
	
	void PostPaintEvent(PE pe, int x1=0, int y1=0, int x2=0, int y2=0, int a=0, int b=0,bool PaintOnViewport=false);
	
	float textCorr(){return 1;}
	bool sizeOfFrame(int&, int&);

	// BUG: wrong place.
	void setVisible(bool x) override { itested();
		if(!x){ itested();
			cleanup();
		}else{ itested();
		}
		QWidget::setVisible(x);
	}

private: // Doc.
	void new_root();
//	QAction* createUndoAction() override;
//	QAction* createRedoAction() override;

private: //temporary/obsolete
#if 0
  void sizeOfAll(int&a, int&b, int&c, int&d){ untested();
	  assert(_model);
	  return _model->sizeOfAll(a, b, c, d, textCorr());
  }
#endif
public:
//  void pushBack(Element* what);
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
	bool  createSubcircuitSymbol();
	
	void    cut();
	void    copy();
	template<class SOME_LIST>
	bool    paste(ostream_t*, SOME_LIST*);
	bool    load();
	int     save();
	//int     saveSymbolCpp (void);
	//int     saveSymbolJSON (void);
	void becomeCurrent();
	void cleanup();

	// remove.
//	void showEvent(QShowEvent*) override;
//	void hideEvent(QHideEvent*) override;
	
	bool scrollUp(int);
	bool scrollDown(int);
	bool scrollLeft(int);
	bool scrollRight(int);

public: // scene
  void sceneAddItem(ElementGraphics*);
  void sceneRemoveItem(ElementGraphics*);

private:
	void deleteItem(ElementGraphics*);
	QString createClipboardFile() const;

public:
  SchematicScene const* scene() const{itested();
	  return prechecked_cast<SchematicScene const*>(QGraphicsView::scene());
  }

private:
  SchematicScene *scene() {itested(); return prechecked_cast<SchematicScene*>(QGraphicsView::scene()); }
  void drawBackground(QPainter *painter, const QRectF& rect);
public:
  SchematicScene *sceneHACK() {itested(); return scene(); }

  // TODO: take iterator?
  Element* detachFromModel(Element* e){ untested();
	  assert(_model);
	  return _model->detach(e);
  }

  void deselectElements();

  // find_symbol
public:
  // Component* find_component(QString const&);
  CmdEltList& commands(){ untested();
	  return _commands;
  }
  CmdEltList const& commands() const{ untested();
	  return _commands;
  }
  unsigned numberOfNets() const{ untested();
	  assert(_model);
	  return _model->numberOfNets();
  }

  QList<PostedPaintEvent>   PostedPaintEvents;
public:
  bool isSymbolMode() const{ untested();
	  return false;
  }

  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area
  int UsedX1, UsedY1, UsedX2, UsedY2;  // document area used by elements

  int ShowFrame; // BUG// it's the frame type.
  int showFrame() const{ return ShowFrame; }
  void setFrameType(int t){ untested();
	  if(t != ShowFrame){ untested();
		  setChanged(true);
		  ShowFrame = t;
	  }else{ untested();
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
  QFileInfo getFileInfo (void) { untested();
	  assert(false);
	  assert(_model);
	  return QFileInfo();
//	  return _model->getFileInfo();
  }
  /*! \brief Set reference to file (schematic) */

signals:
  void signalCursorPosChanged(int, int);
  void signalUndoState(bool);
  void signalRedoState(bool);
  void signalFileChanged(bool) override;

protected:
  void paintFrame(ViewPainter*);

protected:
//  void showDocEvent(QShowEvent*) override;
//  void hideDocEvent(QShowEvent*) override;

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
	void slotDCbias() override; // not a slot yet..
	void slotSimulate() override; // not a slot yet?
	void slotRefreshData(std::string const& what);

public:
  // possible rule: return pointer:   transfer ownership
  //                return reference: don't
  // (same for arguments?)
  Element* eraseFromScene(ElementGraphics*);
  QGraphicsItem& addToScene(Element*); // called from SM::pushBack.


protected slots:
  // void slotScrollUp();
  // void slotScrollDown();
  // void slotScrollLeft();
  // void slotScrollRight();
//  void printCursorPosition(int x_, int y_);

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
  Node* insertNode(int x, int y, Element* e){ untested();
	  assert(_model);
	  return _model->insertNode(x, y, e);
  }
  Node* selectedNode(int, int);
  Marker* setMarker(int, int);

private: // FIXME: remove
  void    markerLeftRight(bool, Q3PtrList<ElementGraphics>*);
  void    markerUpDown(bool, Q3PtrList<ElementGraphics>*);
public:
  void    markerMove(arrow_dir_t d, Q3PtrList<ElementGraphics>* l){ untested();
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
//  int      selectElements(int, int, int, int, bool);
  void     selectMarkers();
//  void     newMovingWires(QList<Element*>*, Node*, int);
//  QList<ElementGraphics*> cropSelectedElements();
  QList<ElementGraphics*> selectedItems() const;
//  QUndoCommand* deleteElements();
//  bool     aligning(int);
//  bool     distributeHorizontal();
//  bool     distributeVertical();

//  void       setComponentNumber(Component*);
//  void       insertRawComponent(Component*, bool noOptimize=true);
//  void       recreateSymbol(Symbol* s){ untested();
//	  assert(_model);
//	  _model->recreateSymbol(s);
//  }
  void       insertElement(Element*);
//private: // old legacy stuff.
//  void       insertComponent(Component*);
public:
  void       activateCompsWithinRect(int, int, int, int);
//  bool       activateSpecifiedComponent(int, int);
  bool       activateSelectedComponents();
//  void       setCompPorts(Component*);
  Component* searchSelSubcircuit();
  Component* selectedComponent(int, int);
  void     oneLabel(Node*);
  int      placeNodeLabel(WireLabel*);
  Element* getWireLabel(Node*);
  void     insertNodeLabel(WireLabel*);
//  void     copyLabels(int&, int&, int&, int&, QList<Element *> *);

//  Painting* selectedPainting(float, float);
//  void      copyPaintings(int&, int&, int&, int&, QList<Element *> *);


private: // obsolete
  //int  copyWires(int&, int&, int&, int&, QList<Element *> *);
  //int  copyComponents(int&, int&, int&, int&, QList<Element *> *);
  //bool copyComps2WiresPaints(int&, int&, int&, int&, QList<Element *> *);
  //int  copyElements(int&, int&, int&, int&, QList<Element *> *);

public: // not here.
  static int testFile(const QString &);
  bool loadDocument();
  void highlightWireLabels (void);

public:
	bool loadDocument(QString& filename);
	bool load(QTextStream& stream);

private: // probably wrong.
	void parse(istream_t& stream, SchematicLanguage const*l=nullptr);

private:
  bool    pasteFromClipboard(istream_t*, EGPList*);

//  bool    rebuild(QString *);
//  bool    rebuildSymbol(QString *);

  void updateViewport() override { untested();
	  viewport()->update();
  }

public: // for now
	QString getParameter(std::string const& key) const;

public:
  bool isAnalog;
  bool isVerilog;
  bool creatingLib;

private: // Doc overrides, schematic_action.cpp
  void reloadGraphs() override; // fix later
  void actionCopy(QAction*) override{ untested();
	  copy();
  }
  void actionCut(QAction*) override{ untested();
	  cut();
  }

//  void actionEditMirrorX(QAction*) override;
//  void actionEditMirrorY(QAction*) override;
public slots:
//  void slotToolbar(){ incomplete(); } // really?
#if 0
  void actionEditUndo(QAction*) override;
  void actionEditRedo(QAction*) override;
  void actionAlign(int what) override;
  void actionDistrib(int dir) override; // 0=horiz, 1=vert

  void actionApplyCompText() override;
  void actionSelectMarker() override;
  void actionSelectAll(QAction*) override;
  void actionChangeProps(QAction*) override;
  void actionCursor(arrow_dir_t) override;

  void actionOnGrid(QAction*) override;
  void actionEditRotate(QAction*) override;
#endif

	void slotSelect();  // select mode
	void slotEditRotate();  // rotate the selected items
	void slotEditMirrorX(); // mirror the selected items about X axis
	void slotEditMirrorY(); // mirror the selected items about Y axis
	void slotEditPaste();
	void slotInsertLabel();
	void slotSetMarker();
	void slotChangeProps(); // ?
#if 0
  void actionMoveText(QAction*) override;
  void actionZoomIn(QAction*) override;
  void actionSelectElement(QObject*) override;
  void actionInsertEquation(QAction*) override;
  void actionInsertPort(QAction*) override;
#endif
	void slotInsertEquation();
	void slotInsertPort();
	void slotInsertGround();
	void slotInsertWire();

private: // Doc overrides.
	      // actions that are not specific to SchematicDoc..
			//
			// move to mouseActions (DocActions?)
	void slotEditDelete(QAction*) override;
	void slotEditActivate(QAction*) override;
	void slotEditUndo(QAction*) override;
	void slotEditRedo(QAction*) override;

public:
	bool handleMouseActions(QEvent* e) override;

private:
//  bool performToggleAction(bool, QAction*, pToggleFunc, pMouseFunc, pMouseFunc2); // this is nuts.

	MouseActions* mouseActions() {return eventHandler(); }
	SchematicActions* schematicActions() {
	  	assert(eventHandler());
	  	return dynamic_cast<SchematicActions*>(eventHandler());
	}
//  MouseAction* mouseAction(); Doc

	void setDrawn(bool b=true){mouseActions()->setDrawn(b);}

	// ??
	SubcktBase* root() override {return _root;}

public:
  SubcktBase const* root() const { return _root; }
  ElementList const* model() const { return _model; }

  void addElement(Element*);

protected:
  // HACK
  ElementList* model() { return _model; }

public: // need access to ElementList...
  friend class MouseActions;
  friend class ImageWriter;
  friend class SchematicScene;

private:
	SubcktBase* _root;
	ElementList* _model;
	SubcktBase* _main;
	CmdEltList _commands;
	std::map<std::string, SimProcess*> _simProcess; // Doc?
	
	// Shortcuts for scolling schematic / TextEdit
	// This is rather cumbersome -> Make this with a QScrollView instead??
	QShortcut* _cursorUp{nullptr};
	QShortcut* _cursorLeft{nullptr};
	QShortcut* _cursorRight{nullptr};
	QShortcut* _cursorDown{nullptr};
}; // SchematicDoc
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
int SchematicDoc::adjustPortNumbers()
{ untested();
	incomplete();
	// not sure what this means. possibly flush the ports into the symbol
	// definition?
	return 17;
}
/* -------------------------------------------------------------------------------- */
// Perform paste function from clipboard
template<class SOME_LIST>
inline bool SchematicDoc::paste(ostream_t *stream, SOME_LIST *pe)
{ untested();
  return pasteFromClipboard(stream, pe);
}


/* -------------------------------------------------------------------------------- */
#if 0 // TODO
  editActivate = new QAction(QIcon((":/bitmaps/deactiv.png")),	tr("Deactivate/Activate"), this);
  editActivate->setShortcut(Qt::CTRL+Qt::Key_D);
  editActivate->setStatusTip(tr("Deactivate/Activate selected components"));
  editActivate->setWhatsThis(
	tr("Deactivate/Activate\n\nDeactivate/Activate the selected components"));
  editActivate->setCheckable(true);
  connect(editActivate, &QAction::toggled, this, &QucsApp::slotEditActivate);

  insEquation = new QAction(QIcon((":/bitmaps/equation.png")),	tr("Insert Equation"), this);
  insEquation->setShortcut(Qt::CTRL+Qt::Key_Less);
  insEquation->setStatusTip(tr("Insert an equation"));
  insEquation->setWhatsThis(
	tr("Insert Equation\n\nInserts a user defined equation"));
  insEquation->setCheckable(true);
  connect(insEquation, &QAction::toggled, this, &QucsApp::slotInsertEquation);

  insLabel = new QAction(QIcon((":/bitmaps/nodename.png")), tr("Wire Label"), this);
  insLabel->setShortcut(Qt::CTRL+Qt::Key_L);
  insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  insLabel->setWhatsThis(tr("Wire Label\n\nInsert a wire or pin label"));
  insLabel->setCheckable(true);
  connect(insLabel, &QAction::toggled, this, &QucsApp::slotInsertLabel);

  setMarker = new QAction(QIcon((":/bitmaps/marker.png")),	tr("Set Marker on Graph"), this);
  setMarker->setShortcut(Qt::CTRL+Qt::Key_B);
  setMarker->setStatusTip(tr("Sets a marker on a diagram's graph"));
  setMarker->setWhatsThis(
	tr("Set Marker\n\nSets a marker on a diagram's graph"));
  setMarker->setCheckable(true);
  connect(setMarker, &QAction::toggled, this, &QucsApp::slotSetMarker);

  onGrid = new QAction(tr("Set on Grid"), this);
  onGrid->setShortcut(Qt::CTRL+Qt::Key_U);
  onGrid->setStatusTip(tr("Set selected elements on grid"));
  onGrid->setWhatsThis(
	tr("Set on Grid\n\nSet selected elements on grid"));
  onGrid->setCheckable(true);
  connect(onGrid, &QAction::toggled, this, &QucsApp::slotOnGrid);

  moveText = new QAction(tr("Move Component Text"), this);
  moveText->setShortcut(Qt::CTRL+Qt::Key_K);
  moveText->setStatusTip(tr("Move the property text of components"));
  moveText->setWhatsThis(
	tr("Move Component Text\n\nMove the property text of components"));
  moveText->setCheckable(true);
  connect(moveText, &QAction::toggled, this, &QucsApp::slotMoveText);

  changeProps = new QAction(tr("Replace..."), this);
  changeProps->setShortcut(Qt::Key_F7);
  changeProps->setStatusTip(tr("Replace component properties or VHDL code"));
  changeProps->setWhatsThis(
	tr("Replace\n\nChange component properties\nor\ntext in VHDL code"));
  connect(changeProps, SIGNAL(triggered()), SLOT(slotChangeProps()));

  distrHor = new QAction(tr("Distribute horizontally"), this);
  distrHor->setStatusTip(tr("Distribute equally horizontally"));
  distrHor->setWhatsThis(
	tr("Distribute horizontally\n\n"
	   "Distribute horizontally selected elements"));
  connect(distrHor, SIGNAL(triggered()), SLOT(slotDistribHoriz()));

  distrVert = new QAction(tr("Distribute vertically"), this);
  distrVert->setStatusTip(tr("Distribute equally vertically"));
  distrVert->setWhatsThis(
	tr("Distribute vertically\n\n"
	   "Distribute vertically selected elements"));
  connect(distrVert, SIGNAL(triggered()), SLOT(slotDistribVert()));

  centerHor = new QAction(tr("Center horizontally"), this);
  centerHor->setStatusTip(tr("Center horizontally selected elements"));
  centerHor->setWhatsThis(
	tr("Center horizontally\n\nCenter horizontally selected elements"));
  connect(centerHor, SIGNAL(triggered()), SLOT(slotCenterHorizontal()));

  centerVert = new QAction(tr("Center vertically"), this);
  centerVert->setStatusTip(tr("Center vertically selected elements"));
  centerVert->setWhatsThis(
	tr("Center vertically\n\nCenter vertically selected elements"));
  connect(centerVert, SIGNAL(triggered()), SLOT(slotCenterVertical()));

  // why not TextDoc?
  selectAll = new QAction(tr("Select All"), this);
  selectAll->setShortcut(Qt::CTRL+Qt::Key_A);
  selectAll->setStatusTip(tr("Selects all elements"));
  selectAll->setWhatsThis(
	tr("Select All\n\nSelects all elements of the document"));
  connect(selectAll, SIGNAL(triggered()), SLOT(slotSelectAll()));

#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SchematicDoc::SchematicDoc()
   : QGraphicsView(),
     Doc(),
     _root(nullptr),
     _model(nullptr),
     _main(nullptr)
{ untested();
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::expand()
{ untested();
	new_root();
	assert(!scene());
	// ...........................................................
	// HUH?
	setSceneRect(-2000, -2000, 4000, 4000);
	SchematicScene* Scene = new SchematicScene(this);
	QBrush b(Qt::Dense7Pattern);
	Scene->setBackgroundBrush(b);
	setScene(Scene);
	
	assert(scene());

	{ untested();
	}
	auto h = new SchematicActions(this); // needs scene (bug?)
	setEventHandler(h); // ActionHandler?
	return;

	// TODO: deduplicate??
//	connect(h->_actionSelect, &QAction::toggled, this, &SchematicDoc::slotSelect);
//	connect(h->_actionRotate, &QAction::toggled, this, &SchematicDoc::slotEditRotate);
//	connect(h->_actionMX, &QAction::toggled, this, &SchematicDoc::slotEditMirrorX);
//	connect(h->_actionMY, &QAction::toggled, this, &SchematicDoc::slotEditMirrorY);
//	connect(h->_actionInsertGround, &QAction::toggled, this, &SchematicDoc::slotInsertGround);
//	connect(h->_actionInsertWire, &QAction::toggled, this, &SchematicDoc::slotInsertWire);
//	connect(h->_actionInsertPort, &QAction::toggled, this, &SchematicDoc::slotInsertPort);

  // cursor left/right/up/down to move marker on a graph
  // BUG: why can't we move Elements?!
  // BUG: why is tnis not in SchematicDocument::init (or so?)
  _cursorLeft = new QShortcut(QKeySequence(Qt::Key_Left), this);
  connect(_cursorLeft, SIGNAL(activated()), SLOT(slotCursorLeft()));

  _cursorRight = new QShortcut(QKeySequence(Qt::Key_Right), this);
  connect(_cursorRight, SIGNAL(activated()), SLOT(slotCursorRight()));

  _cursorUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
  connect(_cursorUp, SIGNAL(activated()), SLOT(slotCursorUp()));

  _cursorDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
  connect(_cursorDown, SIGNAL(activated()), SLOT(slotCursorDown()));
}
/*--------------------------------------------------------------------------*/
#if 0 // these won't work
QAction* SchematicDoc::createUndoAction()
{
	if(_undoStack){
		return _undoStack->createUndoAction(this);
	}else{ untested();
		return nullptr; 
	}
}
/*--------------------------------------------------------------------------*/
QAction* SchematicDoc::createRedoAction()
{
	if(_undoStack){
		return _undoStack->createRedoAction(this);
	}else{ untested();
		return nullptr; 
	}
}
#endif
/*--------------------------------------------------------------------------*/
void SchematicDoc::setParent(QWidget* owner)
{itested();
	assert(!parentWidget());
	/// Doc::setParent(owner); no. pure.
	QGraphicsView::setParent(owner);

	expand();

//  what is the viewport, and where is it initialised?
  assert(viewport());
  viewport()->installEventFilter(eventHandler());

  // ...........................................................
  GridX  = GridY  = 10;
  ViewX1=ViewY1=0;
  ViewX2=ViewY2=800;
  UsedX1 = UsedY1 = INT_MAX;
  UsedX2 = UsedY2 = INT_MIN;

  tmpPosX = tmpPosY = -100;
  tmpUsedX1 = tmpUsedY1 = tmpViewX1 = tmpViewY1 = -200;
  tmpUsedX2 = tmpUsedY2 = tmpViewX2 = tmpViewY2 =  200;
  tmpScale = 1.0;

  isVerilog = false;
  creatingLib = false;

  setDragMode(QGraphicsView::RubberBandDrag); // why?

  ShowFrame = 0;  // don't show
  _frameText[0] = tr("Title");
  _frameText[1] = tr("Drawn By:");
  _frameText[2] = tr("Date:");
  _frameText[3] = tr("Revision:");

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

//  misc::setWidgetBackgroundColor(viewport(), QucsSettings.BGColor);
  assert(viewport());
  viewport()->setMouseTracking(true);
  viewport()->setAcceptDrops(true);  // enable drag'n drop

  TODO("Repair scroll connect");
  // to repair some strange  scrolling artefacts
  /** \todo
  connect(this, SIGNAL(horizontalSliderReleased()),
      viewport(), SLOT(update()));
  connect(this, SIGNAL(verticalSliderReleased()),
      viewport(), SLOT(update()));
  */

  {itested();
//    connect(this, SIGNAL(signalCursorPosChanged(int, int)), 
//        STATUSBAR, SLOT(printCursorPosition(int, int)));
    /** \todo
    connect(this, SIGNAL(horizontalSliderPressed()), 
        App_, SLOT(slotHideEdit()));
    connect(this, SIGNAL(verticalSliderPressed()),
        App_, SLOT(slotHideEdit()));
    */
    connect(this, SIGNAL(signalFileChanged(bool)),
        this, SLOT(slotFileChanged(bool)));
  }

  // toolbar? commands? not here maybe
  if(_app){ untested();
	  assert(_app);
    connect(this, SIGNAL(signalUndoState(bool)),
        _app, SLOT(slotUpdateUndo(bool)));
    connect(this, SIGNAL(signalRedoState(bool)),
        _app, SLOT(slotUpdateRedo(bool)));

	 // something like this?
	 // connect(_undoStack, &QUndoStack::canRedoChanged, _app, [=](bool canRedo){App->setRedoEnabled(canRedo);});
	 // connect(_undoStack, &QUndoStack::canUndoChanged, _app, [=](bool canUndo){App->setUndoEnabled(canUndo);});

  // really?
    emit signalUndoState(true);
    emit signalRedoState(false);
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// bool SchematicDoc::event(QEvent* e);
// { untested();
// 	Doc::event(e);
// 	return QWidget::event(pEvent);
// }
/*--------------------------------------------------------------------------*/
SchematicDoc::~SchematicDoc()
{itested();
	delete _root;
//	delete _mouseActions;
//	delete Scene; ???
}
/*--------------------------------------------------------------------------*/
//void SchematicDoc::showEvent(QShowEvent*e)
//{untested();
//	{ // merge?
//	becomeCurrent();
//	Doc::showEvent(e);
//	}
//
//	QGraphicsView::showEvent(e);
//}
/*--------------------------------------------------------------------------*/
//void SchematicDoc::hideEvent(QHideEvent*e)
//{untested();
//	Doc::hideEvent(e);
//	QGraphicsView::hideEvent(e);
//}
#if 0 // not yet
}
  incomplete();
  app().connectToolbar(*this);
}

// void SchematicDoc::hideEvent(QEvent*)
// { untested();
//   incomplete();
// }
#endif

// another one?!
bool SchematicDoc::loadDocument()
{itested();
  QFile file(docName());
  qDebug() << "opening" << docName();
  if(!file.open(QIODevice::ReadOnly)) { untested();
    /// \todo implement unified error/warning handling GUI and CLI
#if 0
    if (QucsMain)
      QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load document: ")+docName());
    else
      qCritical() << "SchematicDoc::loadDocument:"
                  << QObject::tr("Cannot load document: ")+docName();
#endif
	 incomplete();
    return false;
  }else{ untested();
//    assert(_model);
//    _model->setFileInfo(docName());
    QString d = docName();
    loadDocument(d);

//    QGraphicsScene& s = *scene();
    return true;
  }
}
/*--------------------------------------------------------------------------*/
bool SchematicDoc::loadDocument(QString& /*BUG*/ file)
{itested();
	assert(_model);
	QString Line;
	istream_t stream(istream_t::_WHOLE_FILE, file.toStdString());

	parse(stream);
	_model->prepare();
	for(auto i : *_model){ untested();
		trace1("postload addElement", i->label());
		scene()->addElement(i);
	}
	return true;
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::new_root()
{ untested();
	delete _root;
  // create empty schematic... (clear?)
  auto root = symbol_dispatcher.clone("schematic_root");
  _root = dynamic_cast<SubcktBase*>(root);
  assert(_root);
  _root->set_label("SchematicDoc");

  { // hack?
	  _model = _root->subckt();
	  auto e = (*_model->find_("main"));
	  _main = dynamic_cast<SubcktBase*>(e);
	  assert(_main);
	  assert(_main->makes_own_scope());
	  assert(_main->owner()==_root);
	  _model = e->scope();
	  assert(_model);
  }
}
/*--------------------------------------------------------------------------*/
// why both?!
bool SchematicDoc::load()
{itested();
  delete _root;
  _root = nullptr;

  new_root();
#if 0
  auto root = symbol_dispatcher.clone("schematic_root");
  _root = dynamic_cast<SubcktBase*>(root);
  assert(_root);
//  _root->set_owner(this); //needed?


  { untested();
	  _model = _root->subckt();
	  auto e = (*_model->find_("main"));
	  _main = dynamic_cast<SubcktBase*>(e);
	  assert(_main);
	  assert(_main->makes_own_scope()); // for now.
	  _model = e->scope();
	  assert(_model);
  }
#endif
  _root->set_param_by_name("$filename", docName().toStdString());

  if(!loadDocument()){ untested();
    return false;
  }else{itested();
    // Keep reference to source file (the schematic file)
    // setFileInfo(DocName);
  }
  lastSaved = QDateTime::currentDateTime();


#if 0 // obsolete
  while(!undoAction.isEmpty()) {itested();
    delete undoAction.last();
    undoAction.pop_back();
  }
  undoActionIdx = 0;
  while(!undoSymbol.isEmpty()) {itested();
    delete undoSymbol.last();
    undoSymbol.pop_back();
  }
  setSymbolMode(true);
  setChanged(false, true); // "not changed" state, but put on undo stack
  undoSymbolIdx = 0;
  undoSymbol.at(undoSymbolIdx)->replace(1, 1, 'i');
  setSymbolMode(false);
  setChanged(false, true); // "not changed" state, but put on undo stack
  undoActionIdx = 0;
  undoAction.at(undoActionIdx)->replace(1, 1, 'i');
#endif

  // The undo stack of the circuit symbol is initialized when first
  // entering its edit mode.
  
  // have to call this to avoid crash at sizeOfAll
  // what??
//  becomeCurrent(false);

  incomplete();
  /// sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  /// if(ViewX1 > UsedX1)  ViewX1 = UsedX1;
  /// if(ViewY1 > UsedY1)  ViewY1 = UsedY1;
  /// if(ViewX2 < UsedX2)  ViewX2 = UsedX2;
  /// if(ViewY2 < UsedY2)  ViewY2 = UsedY2;
  zoom(1.0f);
  TODO("Fix setContentsPos");
  /// \todo setContentsPos(tmpViewX1, tmpViewY1);
  tmpViewX1 = tmpViewY1 = -200;   // was used as temporary cache
  return true;
}
/*--------------------------------------------------------------------------*/
// here?? CMD::get? but need owner in new__instance...
void SchematicDoc::parse(istream_t& s, SchematicLanguage const* L)
{itested();

	incomplete(); // but still used in qucs -i $file
	if(!L){itested();
		auto D = language_dispatcher["leg_sch"]; // use command instead.
		L = dynamic_cast<SchematicLanguage const*>(D);
	}else{ untested();
	}
	assert(L);
	assert(_root);
	assert(_model);

	// L->new__instance(getcmd, _root, _root->subckt());

	s.read_line();
	while(!s.atEnd()){itested();
		assert(_root->subckt());
		trace2("SchematicDoc::parse", s.fullstring(), _root->label());
		L->new__instance(s, _root, _root->subckt());
		s.read_line();
	}
}

// -----------------------------------------------------------
QPoint SchematicDoc::setOnGrid(int x, int y) const
{itested();
  if(x<0) x -= (GridX >> 1) - 1;
  else x += GridX >> 1;
  x -= x % GridX;

  if(y<0) y -= (GridY >> 1) - 1;
  else y += GridY >> 1;
  y -= y % GridY;

  return QPoint(x, y);
}
//
QPoint SchematicDoc::snapToGrid(QPointF const& e)const
{ untested();
	return setOnGrid(getX(e), getY(e));
}

QMouseEvent SchematicDoc::snapToGrid(QMouseEvent* e)const
{ untested();
	  auto type = e->type();
	  auto localPos = e->localPos();
	  auto& windowPos = e->windowPos();
	  auto& screenPos = e->screenPos();
	  auto button = e->button();
	  auto buttons = e->buttons();
	  auto modifiers = e->modifiers();
	  auto source = e->source();

	  // this snaps to the wrong grid, anyway
	  trace1("SchematicDoc::snapToGrid1", localPos);
	  localPos = setOnGrid(localPos.x(), localPos.y());
	  trace1("SchematicDoc::snapToGrid2", localPos);
	  auto ee = QMouseEvent(type, localPos, windowPos, screenPos, button, buttons, modifiers, source);
	  return ee;
}

#if 0
void SchematicDoc::mouseReleaseEvent(QMouseEvent *e)
{ untested();
  if(e->isAccepted()){ itested();
  }else{ itested();
	  // not necessary.
	  // does not mattter where QGraphics* sees the release
	  //auto ee = snapToGrid(e);
	  QGraphicsView::mouseReleaseEvent(e);
  }
}
#endif
void SchematicDoc::showAll()
{ untested();
  fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}

void SchematicDoc::showNoZoom()
{ untested();

	QTransform matrix = transform();
	double d = matrix.m11();
	assert(d == matrix.m22());

	// yikes.
	double di = 1./d;
	scale(di,di);

#if 0
  Scale = 1.0;

  int x1 = UsedX1;
  int y1 = UsedY1;
  int x2 = UsedX2;
  int y2 = UsedY2;

  if(x1 > x2) {  // happens e.g. if untitled without changes
    x1 = 0;
    x2 = 800;
  }else{ untested();
  }
  if(y1 > y2) { untested();
    y1 = 0;
    y2 = 800;
  }else{ untested();
  }

  if(x2==0 && y2==0 && x1==0 && y1==0){ untested();
    x2 = y2 = 800;
  }else{ untested();
  }

  ViewX1 = x1-40;
  ViewY1 = y1-40;
  ViewX2 = x2+40;
  ViewY2 = y2+40;
  TODO("Fix resizeContents");
  ///\todo resizeContents(x2-x1+80, y2-y1+80);
  updateViewport();
  // App->view->drawn = false;
#endif
}

float SchematicDoc::zoomBy(float s)
{ untested();
	QTransform matrix = transform();
	double d = matrix.m11();
	assert(d == matrix.m22());
	double Scale = d * s;

	if(Scale > 10.0){ untested();
		Scale = 10.0f;
	}else if(Scale < 0.1){ untested();
		Scale = 0.1f;
	}else{ untested();
	}

	s = Scale / d;

	scale(s, s); // scale is relative...
	s -= 1.0;
	TODO("Fix contentsX");
	/// todo scrollBy( int(s * float(contentsX()+visibleWidth()/2)),
	///          int(s * float(contentsY()+visibleHeight()/2)) );
	return Scale;
}

void SchematicDoc::drawBackground(QPainter *painter, const QRectF &rect)
{ untested();
	QGraphicsView::drawBackground(painter, rect);

	double n = 10;

	// Draw origin when visible
	if(rect.contains(QPointF(0, 0))) { untested();
		painter->drawLine(QLine(-n, 0.0, n, 0.0));
		painter->drawLine(QLine(0.0, -n, 0.0, n));
	}else{ untested();
	}
}

// directly call scale??
float SchematicDoc::zoom(float)
{itested();
	incomplete();
#if 0


	// "resizeContents()" performs an immediate repaint. So, set widget
	// to hidden. This causes some flicker, but it is still nicer.
	assert(viewport());
	viewport()->setHidden(true);
	//  setHidden(true);
	TODO("Fix resizeContents");
	/// todo resizeContents(int(Scale*float(ViewX2 - ViewX1)),
	///               int(Scale*float(ViewY2 - ViewY1)));
	//  setHidden(false);
	viewport()->setHidden(false);

	viewport()->update();
	// App->view->drawn = false;

#endif
	return 0;
}

// why is this here and not in SchematicScene?
void SchematicDoc::mouseMoveEvent(QMouseEvent *e)
{itested();
  assert(e);
  if(e->isAccepted()){ untested();
  }else{untested();
	  auto xx = mapToScene(e->pos());
	  auto gg = snapToGrid(xx);

	  // move actions go through here.
	  // signalCursorPosChanged(xx.x(), xx.y());
	  printCursorPosition(gg.x(), gg.y());
	  QGraphicsView::mouseMoveEvent(e);
  }

  return;

  // mouse moveaction set by toggleaction
  // toggleaction should instead subscribe.
#if 0
  // what is view?
  if(App->MouseMoveAction){ untested();
    (App->view->*(App->MouseMoveAction))(this, e);
  }else{ itested();
  }
#else

  // just subscribe?
  assert(mouseActions());

  if(mouseActions()){ untested();
	  mouseActions()->handle(e);
  }else{ untested();
	  incomplete();
  }
#endif

  // how to do this for all callbacks at once?!
  if(e->isAccepted()){ untested();
	  // nice
  }else{ untested();
	  // is this a good way to propagate?
	  QGraphicsView::mouseMoveEvent(e);
  }
}

// "should return true if the event e was recognized and processed."
// (whatever that means)
//
// getting here *after* the event has passed through Scene
// possible fix: deal with it in scene. need to merge mouseActions into Scene
// anyway...
bool SchematicDoc::event(QEvent* e)
{itested();
	trace2("SchematicDoc::event", e->type(), e->isAccepted());
//	e->ignore();

	if(e->isAccepted()){itested();
		// move actions are highjacked.
		// (that's okay);
		// releaseEvent is also here.
	}else{itested();
		// need releaseEvent here.
		// but maybe not.
		//mouserelease?!
		// handleMouseActions(e); //
	}

	handleMouseActions(e);
	
	// TODO: what is this call?
	bool a = QGraphicsView::event(e);

//	mouseActions()->handle(e); // try scene
	return a;
}

bool SchematicDoc::handleMouseActions(QEvent* e)
{ untested();
	if(mouseActions()){ untested();
		return mouseActions()->handle(e);
	}else{ untested();
		// race?
		unreachable();
		return false;
	}
}

#if 0
QPointF SchematicDoc::mapToScene(QPoint const& p) const
{ untested();
	return QGraphicsView::mapToScene(p);
}
#endif

#ifdef INDIVIDUAL_MOUSE_CALLBACKS
// -----------------------------------------------------------
// override function. catch mouse presses.
//  why not just forward?!
void SchematicDoc::mousePressEvent(QMouseEvent *e)
{ untested();

	return;
  QGraphicsView::mousePressEvent(e);

//  App->editText->setHidden(true); // disable text edit of component property
  return;

#if 1
  assert(mouseActions());
  mouseActions()->handle(e);
#else
  //TODO: move cruft to actions.
  if(Event->button() != Qt::LeftButton){ untested();
    if(App->MousePressAction == &MouseActions::MPressElement){ untested();
      // hmm
    }else if(App->MousePressAction == &MouseActions::MPressWire2) { untested();
      // hmm
    }else{ untested();
	// some action needs to handle right button
        // show menu on right mouse button
        App->view->rightPressMenu(this, Event);
        if(App->MouseReleaseAction){ untested();
           // Is not called automatically because menu has focus.
          (App->view->*(App->MouseReleaseAction))(this, &nestedEvent);
        }
        return;
    }
  }else{ untested();
  }
#endif
}
void SchematicDoc::mouseDoubleClickEvent(QMouseEvent *Event)
{ untested();
  assert(mouseActions());
  mouseActions()->handle(Event);
}

// -----------------------------------------------------------
// possibly manufacture a GraphicsView::wheelEvent (or so) from this.
// the current qt implementation does not do the modifiers, but scrolling
// works, including multitouch
void SchematicDoc::wheelEvent(QWheelEvent * Event)
{ untested();
#ifndef USE_SCROLLVIEW
  (void) Event;
#else
  App->editText->setHidden(true);  // disable edit of component property
  // use smaller steps; typically the returned delta() is a multiple of 120

  int delta = Event->delta() >> 1;

  // ...................................................................
  if((Event->modifiers() & Qt::ShiftModifier) ||
     (Event->orientation() == Qt::Horizontal)) { // scroll horizontally ?
      if(delta > 0) { if(scrollLeft(delta)) scrollBy(-delta, 0); }
      else { if(scrollRight(delta)) scrollBy(-delta, 0); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }
  // ...................................................................
  else if(Event->modifiers() & Qt::ControlModifier) {  // use mouse wheel to zoom ?
      // zoom factor scaled according to the wheel delta, to accomodate
      //  values different from 60 (slower or faster zoom)
      float Scaling = pow(1.1, delta/60.0);
      zoom(Scaling);
      Scaling -= 1.0;
      scrollBy( int(Scaling * float(Event->pos().x())),
                int(Scaling * float(Event->pos().y())) );
  }
  // ...................................................................
  else {     // scroll vertically !
      if(delta > 0) { if(scrollUp(delta)) scrollBy(0, -delta); }
      else { if(scrollDown(delta)) scrollBy(0, -delta); }
      viewport()->update(); // because QScrollView thinks nothing has changed
      App->view->drawn = false;
  }

  Event->accept();   // QScrollView must not handle this event
#endif
}

void SchematicDoc::dropEvent(QDropEvent *Event)
{ untested();
  if(dragIsOkay) { untested();
    QList<QUrl> urls = Event->mimeData()->urls();
    if (urls.isEmpty()) { untested();
      return;
    }

    // do not close untitled document to avoid segfault
    Doc *d = QucsMain->getDoc(0);
    bool changed = d->DocChanged;
    d->DocChanged = true;

    // URI:  file:/home/linuxuser/Desktop/example.sch
    foreach(QUrl url, urls) { untested();
      App->gotoPage(QDir::toNativeSeparators(url.toLocalFile()));
    }

    d->DocChanged = changed;
    return;
  }else{ untested();
    // not okay


    int x = int(Event->pos().x()/Scale) + ViewX1;
    int y = int(Event->pos().y()/Scale) + ViewY1;
    QPoint p(x, y);

    qDebug() << "nestedEvent in contentsDropEvent? at" << p;
    QMouseEvent e(QEvent::MouseButtonPress, p,
		  Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

#if 0
    App->view->MPressElement(this, &e);

    if(App->view->selElem) delete App->view->selElem;
    App->view->selElem = 0;  // no component selected
#endif

    if(formerAction){ untested();
      formerAction->setChecked(true);  // restore old action
    }else{ untested();
    }
  }
}

#if 0
void SchematicDoc::dragLeaveEvent(QDragLeaveEvent *Event)
{ incomplete();
}

void SchematicDoc::dragEnterEvent(QDragEnterEvent *Event)
{ untested();
  formerAction = 0;
  dragIsOkay = false;

  // file dragged in ?
  if(Event->mimeData()->hasUrls()) { untested();
    dragIsOkay = true;
    Event->accept();
    return;
  }else if(Event->mimeData()->hasText()) { untested();
    QString s = Event->mimeData()->text();
    if(s.left(15) == "QucsComponent:<") { untested();
      s = s.mid(14);
      incomplete();
#if 0
      App->view->selElem = legacySchematicLanguage::getComponentFromName(s);
      if(App->view->selElem) { untested();
        Event->accept();
        return;
      }else{ untested();
      }
#endif
    }else{ untested();
	 }
    Event->ignore();
    return;
  }


//   if(Event->format(1) == 0) {  // only one MIME type ? }

    auto ll = ComponentWidget::elementRefMimeType();
    if(Event->mimeData()->hasFormat(ll)) { untested();
		 // drag component from listview
		 incomplete();

#if 0
	 }else if(Event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) { untested();
      QListWidgetItem *Item = App->CompComps->currentItem();
      if(Item) { untested();
        formerAction = App->activeAction;
        App->slotSelectComponent(Item);  // also sets drawn=false
        App->MouseMoveAction = 0;
        App->MousePressAction = 0;

        Event->accept();
        return;
      }
#endif
    }else{ untested();
	 }

  Event->ignore();
}
#endif

void SchematicDoc::contentsDragLeaveEvent(QDragLeaveEvent*)
{ untested();
#if 0
  if(!App->view->selElem){ untested();
  }else if(App->view->selElem->Type & isComponent){ untested();
    if(App->view->drawn) { untested();

      QPainter painter(viewport());
      //App->view->setPainter(this);
      ((Component*)App->view->selElem)->paintScheme(this);
      App->view->drawn = false;
    }else{ untested();
    }
  }else{ untested();
  }
#endif

  if(formerAction){ untested();
    formerAction->setChecked(true);  // restore old action
  }else{ untested();
  }
}

// ---------------------------------------------------
void SchematicDoc::contentsDragMoveEvent(QDragMoveEvent *Event)
{ untested();
  assert(Event);

  if(!dragIsOkay) { untested();
#if 0
    if(App->view->selElem == 0) { untested();
      Event->ignore();
      return;
    }
#endif

    QMouseEvent e(QEvent::MouseMove, Event->pos(), Qt::NoButton, 
		  Qt::NoButton, Qt::NoModifier);
    assert(mouseActions());
    mouseActions()->handle(&e);
  }else{ untested();
    Event->accept();
  }
}
#endif

/*--------------------------------------------------------------------------*/
// take ownership. this does not call show_.
// use scene?
void SchematicDoc::sceneAddItem(ElementGraphics* x)
{ untested();
	incomplete();
	assert(scene());
	scene()->addItem(x);
	QGraphicsItem* g = x;
	g->show();
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::sceneRemoveItem(ElementGraphics* x)
{ untested();
	assert(scene());
	scene()->removeItem(x);
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::addElement(Element* x)
{ untested();
	assert(!x->owner());
	x->set_owner(_main);
	_model->push_back(x);

	if(auto c=dynamic_cast<Symbol*>(x)){ untested();
		if(c->is_device()){ untested();
			trace1("connect?", c->label());
#if 1
//			_model->connect(c); // BUG. wrong place.
#else
			// something like this. now in scene::attachToModel.
			for(unsigned i=0; i<c->max_nodes(); ++i){ untested();
				assert(portValue()==""); // ?
				try{ untested();
					// probably here.
					// p = c->nodePosition(i);
					// q = _model->places()->at(p);
					// n = q->_n(0);
					c->setPort(i, n.label());
				}except{ untested();
					// pass.
				}
			}
#endif	
		}else{ untested();
			assert(!dynamic_cast<Conductor*>(x));
		}
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
// questionable.
Element* SchematicDoc::eraseFromScene(ElementGraphics* g)
{ untested();
  Element* e = element(g);
  scene()->removeItem(g);
  delete g;
  return e;
}

// undo action?
void SchematicDoc::deleteItem(ElementGraphics *g)
{ untested();
    Element* e=element(g);
    delete(g); // will it detach from scene?
	
    assert(_model);
    _model->erase(e); // also get rid of the payload.
}

// forward to graphicscene
// BUG: what if there are multiple items?
ElementGraphics* SchematicDoc::itemAt(float x, float y)
{ untested();
	QPointF p(x, y);
	QGraphicsItem* I=scene()->itemAt(p, QTransform());
	if(ElementGraphics* G=dynamic_cast<ElementGraphics*>(I)){ untested();
		return G;
	}else{ untested();
		qDebug() << "miss";
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
// SchematicScene??
QList<ElementGraphics*> SchematicDoc::selectedItems() const
{ itested();
	assert(scene());
	return scene()->selectedItems();
}
/*--------------------------------------------------------------------------*/
// Node const* SchematicDoc::nodeAt(pos_t const& p) const
// { untested();
// 	assert(_model);
// 	return _model->nodeAt(p);
// }
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotSelect()
{ untested();
	eventHandler()->possiblyToggleAction(schematicActions()->maSelect, sender());
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotEditRotate()
{ untested();
  eventHandler()->possiblyToggleAction(schematicActions()->maRotate, sender());
  //possiblyToggleAction(_actionRotate, sender); ?
}
// -----------------------------------------------------------------------
void SchematicDoc::slotEditMirrorX()
{ untested();
  eventHandler()->possiblyToggleAction(schematicActions()->maMirrorYaxis, sender());
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotEditMirrorY()
{ untested();
	eventHandler()->possiblyToggleAction(schematicActions()->maMirrorXaxis, sender());
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotSimulate()
{ untested();
  auto w = prechecked_cast<QWidget*>(this);
  assert(w);
/*--------------------------------------------------------------------------*/
#if 0 //not sure
  if(isTextDocument (w)) { untested();
    Doc = (Doc*)((TextDoc*)w);
    if(Doc->SimTime.isEmpty() && ((TextDoc*)Doc)->simulation) { untested();
      DigiSettingsDialog *d = new DigiSettingsDialog((TextDoc*)Doc);
      if(d->exec() == QDialog::Rejected)
	return;
    }
  } else { untested();
    Doc = (Doc*)((SchematicDoc*)w);
  }
#endif

  auto Doc = this;
  if(Doc->docName().isEmpty()) // if document 'untitled' ...
    if(!saveAs()) return;    // ... save schematic before

  // Perhaps the document was modified from another program ?
  QFileInfo Info(Doc->docName());
  if(Doc->lastSaved.isValid()) { untested();
    if(Doc->lastSaved < Info.lastModified()) { untested();
      int No = QMessageBox::warning(w, QObject::tr("Warning"),
               QObject::tr("The document was modified by another program !") + '\n' +
               QObject::tr("Do you want to reload or keep this version ?"),
               QObject::tr("Reload"), QObject::tr("Keep it"));
      if(No == 0){ untested();
        Doc->load();
		}else{ untested();
		}
    }
  }

  // qucsapp->slotResetWarnings(); // what does it do?

  Simulator *sim = Doc->simulatorInstance();
  assert(sim);
  SimMessage *ctrl = new SimMessage(sim, Doc);
  // disconnect is automatically performed, if one of the involved objects
  // is destroyed !
  

//  connect(ctrl, &SimMessage::SimulationEnded,
//          this, &SchematicDoc::slotRefreshData);
//  connect(sim, SIGNAL(displayDataPage(QString&, QString&)),
//		this, SLOT(slotChangePage(QString&, QString&)));

//  ctrl->show();
  ctrl->open(); // does not work

  istream_t cs(istream_t::_STRING, "run all");

  try{ untested();
	  ctrl->startProcess(cs);
  }catch(qucs::Exception& e){ untested();
	  incomplete();
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), ctrl, SLOT(slotClose()));
} // SchematicDoc::slotSimulate
/*--------------------------------------------------------------------------*/
// TODO: deduplicate
void SchematicDoc::slotDCbias()
{ untested();
//  slotHideEdit(); // disable text edit of component property (why?)

  if(!docName().isEmpty()){ untested();
	  // ok.
  }else if(saveAs()){ untested();
		// perhaps because the output filename is determined by the schematic
		// filename!?
  }else{ untested();
	  incomplete();
	  // could still use temp files...
	  return;
  }

#if 0 // TODO
  //checkReload();
  // Perhaps the document was modified from another program ?
  QFileInfo Info(Doc->docName());
  if(Doc->lastSaved.isValid()) { untested();
    if(Doc->lastSaved < Info.lastModified()) { untested();
      int No = QMessageBox::warning(this, tr("Warning"),
               tr("The document was modified by another program !") + '\n' +
               tr("Do you want to reload or keep this version ?"),
               tr("Reload"), tr("Keep it"));
      if(No == 0){ untested();
			Doc->load();
		}else{ untested();
		}
    }
  }
#endif

  //app.slotResetWarnings(); // ?
  Simulator *sim = simulatorInstance();
  SimMessage *ctrl = new SimMessage(sim, this);

  // QWidget* w = this;
  // ctrl->setParent(w); embeds into scene...
//  sim->setMode("dcop");
  assert(sim);
  // disconnect is automatically performed, if one of the involved objects
  // is destroyed
  //
  connect(ctrl, &SimMessage::signalData, this, &SchematicDoc::slotRefreshData);
//  connect(sim, SIGNAL(displayDataPage(QString&, QString&)),
//		this, SLOT(slotChangePage(QString&, QString&)));
//
//  ctrl->show();
  ctrl->open(); // does not work

  istream_t cs(istream_t::_STRING, "run dcop");

  try{ untested();
	  ctrl->startProcess(cs);
  }catch(qucs::Exception& e){ untested();
	  incomplete();
  }

  connect(this, SIGNAL(signalKillEmAll()), this, SLOT(killSimulator()));
} // slotDCbias
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotRefreshData(std::string const& what)
{ untested();
	incomplete();
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::copy()
{itested();
  QString s = createClipboardFile();
  QClipboard *cb = QApplication::clipboard();  // get system clipboard
  if (!s.isEmpty()) { untested();
    cb->setText(s, QClipboard::Clipboard);
  }
}
/*--------------------------------------------------------------------------*/
class cnpsymbol : public SubcktBase {
public:
	explicit cnpsymbol() : SubcktBase(){ untested();
		new_subckt();
	}

private: // BUG/feature. a SubcktBase is a Painting...
	rect_t bounding_rect() const override{unreachable(); return rect_t();}
	void paint(ViewPainter*) const override{unreachable();}

private:
	virtual Port& port(unsigned){ assert(false); return *new Port(); }
};
/*--------------------------------------------------------------------------*/
// BUG: it does not create a file.
QString SchematicDoc::createClipboardFile() const
{ untested();
	cnpsymbol sym;
	assert(sym.subckt());
	for (auto i : scene()->selectedItems()){ untested();
		auto cl = i->cloneElement();
		sym.subckt()->push_back(cl);
	}

	auto lang = language_dispatcher["leg_sch"];
	assert(lang);
	auto fmt = prechecked_cast<Language const*>(lang);
	assert(fmt);

	QString buf;
	ostream_t s(&buf);
	for(auto i : *sym.subckt()){ untested();
		fmt->printItem(s, i);
	}
	s.flush();
	return buf;
}
/*--------------------------------------------------------------------------*/
// Save this Qucs document. Returns the number of subcircuit ports.
int SchematicDoc::save()
{ untested();
	int result = adjustPortNumbers();// same port number for schematic and symbol
	{ // saveDocument();
		// TODO: provide selection GUI
		auto d = command_dispatcher["leg_sch"];
		assert(d);
		assert(_root);

		std::string command = "save " + docName().toStdString();

		istream_t cs(istream_t::_STRING, command);
		d->do_it(cs, _root->subckt());
	}

	QFileInfo Info(docName());
	lastSaved = Info.lastModified();

	if(result >= 0) { untested();
		setChanged(false);

		//    QVector<QString *>::iterator it;
		//    for (it = undoAction.begin(); it != undoAction.end(); it++) { untested();
		//      (*it)->replace(1, 1, ' '); //at(1) = ' '; state of being changed
		//    }
		//(1) = 'i';   // state of being unchanged
		//    undoAction.at(undoActionIdx)->replace(1, 1, 'i');
		//
		//    for (it = undoSymbol.begin(); it != undoSymbol.end(); it++) { untested();
		//      (*it)->replace(1, 1, ' '); //at(1) = ' '; state of being changed
		//    }
		//at(1) = 'i';   // state of being unchanged
		//    undoSymbol.at(undoSymbolIdx)->replace(1, 1, 'i');
	}
	// update the subcircuit file lookup hashes
	incomplete();
#if 0
	QucsMain->updateSchNameHash();
	QucsMain->updateSpiceNameHash();
#endif

	return result;
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotEditUndo(QAction*)
{
	incomplete();
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotEditRedo(QAction*)
{
	incomplete();
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotInsertGround()
{ untested();
	eventHandler()->possiblyToggleAction(schematicActions()->maInsertGround, sender());
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotInsertEquation()
{ untested();
	eventHandler()->possiblyToggleAction(schematicActions()->maInsertEqn, sender());
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotInsertPort()
{ untested();
	eventHandler()->possiblyToggleAction(schematicActions()->maInsertPort, sender());
}
/*--------------------------------------------------------------------------*/
#if 0 // later turn into slots...
void SchematicDoc::actionOnGrid(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions()->maOnGrid, sender);
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::actionEditRotate(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions()->maRotate, sender);
}
/*--------------------------------------------------------------------------*/
// void SchematicDoc::actionEditMirrorX(QAction* sender)
// { untested();
// }
// /*--------------------------------------------------------------------------*/
// void SchematicDoc::actionEditMirrorY(QAction* sender)
// { untested();
// }
#endif

/*--------------------------------------------------------------------------*/
void SchematicDoc::slotEditActivate(QAction* sender)
{
	eventHandler()->possiblyToggleAction(schematicActions()->maActivate, sender);
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotEditDelete(QAction* sender)
{ untested();
	eventHandler()->possiblyToggleAction(schematicActions()->maDelete, sender);

//	updateViewport();??
//  assert(mouseActions());
//  mouseActions()->setDrawn(false);
}
/*--------------------------------------------------------------------------*/

// void QucsApp::slotOnGrid()
// {
//   ASSIGN_STUFF
//   qd->actionOnGrid(s);
// }
void SchematicDoc::slotInsertWire()
{itested();
//	auto s = prechecked_cast
	assert(false);

	// possiblyToggleAction(schematicActions()->maWire, sender());
}

void SchematicDoc::slotInsertLabel()
{ untested();
//  possiblyToggleAction(schematicActions()->maInsertLabel, sender);
  incomplete();
}

void SchematicDoc::slotSetMarker()
{ untested();
  incomplete();
//  performToggleAction(on, App->setMarker, 0,
//		&MouseActions::MMoveMarker, &MouseActions::MPressMarker);
  // mouseAction = mouseActions().maSetMarker;
}

#if 0 // later
void SchematicDoc::slotMoveText()
{ untested();
  incomplete();
//  performToggleAction(on, App->moveText, 0,
//		&MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
  // mouseAction = mouseActions().maMoveText;
}

void SchematicDoc::slotZoomIn()
{ untested();
	possiblyToggleAction(schematicActions()->maZoomIn, sender());
}
#endif

void SchematicDoc::slotEditPaste()
{ untested();
	eventHandler()->possiblyToggleAction(schematicActions()->maEditPaste, sender());
#if 0
	// if it's not a text doc, prevent the user from editing
	// while we perform the paste operation
	App->hideEdit();

	if(!on) { untested();
		App->MouseMoveAction = 0;
		App->MousePressAction = 0;
		App->MouseReleaseAction = 0;
		App->MouseDoubleClickAction = 0;
		App->activeAction = 0;   // no action active
		if(mouseActions().wasDrawn()) { untested();
			viewport()->update();
		}
		return;
	}else{ untested();
        }

	if(!mouseActions()->pasteElements(this)) { untested();
		App->editPaste->blockSignals(true); // do not call toggle slot
		App->editPaste->setChecked(false);       // set toolbar button off
		App->editPaste->blockSignals(false);
		return;   // if clipboard empty
	}else{ untested();
        }

	uncheckActive();
	App->activeAction = App->editPaste;

        mouseActions().setDrawn(false)
	App->MouseMoveAction = &MouseActions::MMovePaste;
	mouseActions()->movingRotated = 0;
	App->MousePressAction = 0;
	App->MouseReleaseAction = 0;
	App->MouseDoubleClickAction = 0;

void QucsApp::slotAlignTop()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionAlign(0); // BUG use enum
}
// --------------------------------------------------------------
void QucsApp::slotAlignBottom()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionAlign(1); // BUG use enum
}

// --------------------------------------------------------------
// Is called, when "Align left" action is triggered.
void QucsApp::slotAlignLeft()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionAlign(2);
}

// --------------------------------------------------------------
// Is called, when "Align right" action is triggered.
void QucsApp::slotAlignRight()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionAlign(3);
}

// --------------------------------------------------------------
// Is called, when "Distribute horizontally" action is triggered.
void QucsApp::slotDistribHoriz()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionDistrib(0);
}

// --------------------------------------------------------------
// Is called, when "Distribute vertically" action is triggered.
void QucsApp::slotDistribVert()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionDistrib(1);
}

// --------------------------------------------------------------
// Is called, when "Center horizontally" action is triggered.
void QucsApp::slotCenterHorizontal()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionAlign(4);
}

// --------------------------------------------------------------
// Is called, when "Center vertically" action is triggered.
void QucsApp::slotCenterVertical()
{
  Doc *qd = DocumentTab->current();
  assert(qd);

  qd->actionAlign(5);
}

// ---------------------------------------------------------------------
// Is called when the "select all" action is triggered.
void QucsApp::slotSelectAll()
{
  hideEdit();

  ASSIGN_STUFF
  qd->actionSelectAll(s);
}

#endif
}

// ??
#if 0
void SchematicDoc::slotSelectElement(QObject*e)
{ untested();
  schematicActions()->maInsertElement->activate(e);
  possiblyToggleAction(schematicActions()->maInsertElement, nullptr);
}

// is this still in use?
void SchematicDoc::actionEditUndo(QAction*)
{itested();
  // really?
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  undo();
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

// is this still in use?
void SchematicDoc::actionEditRedo(QAction*)
{ untested();
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  redo();
  updateViewport();
  setDrawn(false);
}
#endif

#if 0
void SchematicDoc::slotAlign(int what)
{ untested();
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  if(!aligning(what)){ untested();
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  }
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

void SchematicDoc::actionDistrib(int d)
{ untested();
#if 0 // wrong place
  assert(_app);
  _app->hideEdit(); // disable text edit of component property

  if (d==0){ untested();
	  distributeHorizontal();
  }else if(d==1){ untested();
	  distributeVertical();
  }else{ untested();
	  unreachable();
  }
  updateViewport();
  setDrawn(false);
#endif
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotSelectAll()
{ untested();
#if 0
	incomplete();
    // selectElements(INT_MIN, INT_MIN, INT_MAX, INT_MAX, true);
    updateViewport();
#endif
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::actionSelectMarker()
{ untested();
#if 0
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  selectMarkers();
  updateViewport();
  setDrawn(false);
#endif
}
#endif
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotChangeProps()
{ untested();
	Widget* w = widget_dispatcher.clone("ChangeDialog");
	auto d = dynamic_cast<QDialog*>(w); // prech?
	assert(d);
	d->setParent(this); // mode?
	if(d->exec() == QDialog::Accepted) { untested();
		setChanged(true, true);
		updateViewport();
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
#if 0
void SchematicDoc::actionCursor(arrow_dir_t)
{ untested();
	incomplete();
#ifdef USE_SCROLLVIEW
	int sign = 1;

	if(dir==arr_left){ untested();
		sign = -1;
	}

	if(App->editText->isHidden()) { untested();
		// for edit of component property ?
		auto movingElements = cropSelectedElements();
		int markerCount=0;
		for(auto const& i : movingElements){ untested();
			if(marker(i)){ untested();
				++markerCount;
			}
		}

		if((movingElements.count() - markerCount) < 1) { // all selections are markers
			incomplete();
#if 0
			if(markerCount > 0) {  // only move marker if nothing else selected
				markerMove(dir, &movingElements);
			} else if(dir==arr_up) { untested();
				// nothing selected at all
				if(scrollUp(verticalScrollBar()->singleStep()))
					scrollBy(0, -verticalScrollBar()->singleStep());
			} else if(dir==arr_down) { untested();
				if(scrollDown(-verticalScrollBar()->singleStep()))
					scrollBy(0, verticalScrollBar()->singleStep());
			} else if(dir==arr_left) { untested();
				if(scrollLeft(horizontalScrollBar()->singleStep()))
					scrollBy(-horizontalScrollBar()->singleStep(), 0);
			} else if(dir==arr_right) { untested();
				if(scrollRight(-horizontalScrollBar()->singleStep()))
					scrollBy(horizontalScrollBar()->singleStep(), 0);
			}else{ untested();
				// unreachable. TODO: switch
			}

			updateViewport();
			mouseActions()->drawn = false;
#endif
		}else if(dir==arr_up || dir==arr_down){ untested();
			// some random selection, put it back
			mouseActions()->moveElements(movingElements, 0, ((dir==arr_up)?-1:1) * GridY);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else if(dir==arr_left || dir==arr_right){ untested();
			mouseActions()->moveElements(movingElements, sign*GridX, 0);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else{ untested();
			//unreachable(); //TODO: switch.
		}

	}else if(dir==arr_up){ // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Begin = pp->Description.indexOf('[');
		if(Begin < 0) return;  // no selection list ?
		int End = pp->Description.indexOf(App->editText->text(), Begin); // current
		if(End < 0) return;  // should never happen
		End = pp->Description.lastIndexOf(',', End);
		if(End < Begin) return;  // was first item ?
		End--;
		int Pos = pp->Description.lastIndexOf(',', End);
		if(Pos < Begin) Pos = Begin;   // is first item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		App->editText->setText(pp->Description.mid(Pos, End-Pos+1));
		App->editText->selectAll();
	}else if(dir==arr_down) { // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Pos = pp->Description.indexOf('[');
		if(Pos < 0) return;  // no selection list ?
		Pos = pp->Description.indexOf(App->editText->text(), Pos); // current list item
		if(Pos < 0) return;  // should never happen
		Pos = pp->Description.indexOf(',', Pos);
		if(Pos < 0) return;  // was last item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		int End = pp->Description.indexOf(',', Pos);
		if(End < 0) {  // is last item ?
			End = pp->Description.indexOf(']', Pos);
			if(End < 0) return;  // should never happen
		}
		App->editText->setText(pp->Description.mid(Pos, End-Pos));
		App->editText->selectAll();
	}else{ untested();

	}
#endif
} // actionCursor
#endif

#if 0
// edit text in a graphicselement text?
// (obsolete)
void SchematicDoc::actionApplyCompText()
{ untested();
#if 0 // what is focusElement??
	auto Doc=this;
	auto editText=App->editText;

	QString s;
	QFont f = QucsSettings.font;
	f.setPointSizeF(Scale * float(f.pointSize()) );
	editText->setFont(f);

	Property  *pp = 0;
	Component *pc = component(mouseActions()->focusElement);
	if(!pc) return;  // should never happen
	mouseActions()->MAx1 = pc->cx_() + pc->tx;
	mouseActions()->MAy1 = pc->cy_() + pc->ty;

	int z, n=0;  // "n" is number of property on screen
	pp = pc->Props.first();
	for(z=mouseActions()->MAx3; z>0; z--) {  // calculate "n"
		if(!pp) {  // should never happen
			App->hideEdit();
			return;
		}
		if(pp->display) n++;   // is visible ?
		pp = pc->Props.next();
	}

	pp = 0;
	if(mouseActions()->MAx3 > 0)  pp = pc->Props.at(mouseActions()->MAx3-1); // current property
	else s = pc->name();

	if(!editText->isHidden()) {   // is called the first time ?
		// no -> apply value to current property
		if(mouseActions()->MAx3 == 0) {   // component name ?
			if(!editText->text().isEmpty())
				if(pc->name() != editText->text()) { untested();
					auto pc2=Doc->find_component( editText->text());
					if(!pc2) { untested();
						pc->obsolete_name_override_hack( editText->text() );
						setChanged(true, true);  // only one undo state
					}
				}
		}
		else if(pp) {  // property was applied
			if(pp->Value != editText->text()) { untested();
				pp->Value = editText->text();
				recreateSymbol(pc);  // because of "Num" and schematic symbol
				setChanged(true, true); // only one undo state
			}
		}

		n++;     // next row on screen
		(mouseActions()->MAx3)++;  // next property
		pp = pc->Props.at(mouseActions()->MAx3-1);  // search for next property

		viewport()->update();
                mouseActions().setDrawn(false);

		if(!pp) {     // was already last property ?
			App->hideEdit();
			return;
		}


		while(!pp->display) {  // search for next visible property
			(mouseActions()->MAx3)++;  // next property
			pp = pc->Props.next();
			if(!pp) {     // was already last property ?
				App->hideEdit();
				return;
			}
		}
	}

	// avoid seeing the property text behind the line edit
	if(pp)  // Is it first property or component name ?
		s = pp->Value;
	editText->setMinimumWidth(editText->fontMetrics().width(s)+4);

  incomplete();
  /// Doc->contentsToViewport(int(Doc->Scale * float(view->MAx1 - Doc->ViewX1)),
  ///			 int(Doc->Scale * float(view->MAy1 - Doc->ViewY1)),
  ///	 view->MAx2, view->MAy2);
	editText->setReadOnly(false);
	if(pp) {  // is it a property ?
		s = pp->Value;
		mouseActions()->MAx2 += editText->fontMetrics().width(pp->Name+"=");
		if(pp->Description.indexOf('[') >= 0)  // is selection list ?
			editText->setReadOnly(true);
		Expr_CompProp.setPattern("[^\"]*");
		if(!pc->showName) n--;
	}
	else   // it is the component name
		Expr_CompProp.setPattern("[\\w_]+");
	Val_CompProp.setRegExp(Expr_CompProp);
	editText->setValidator(&Val_CompProp);

	z = editText->fontMetrics().lineSpacing();
	mouseActions()->MAy2 += n*z;
	editText->setText(s);
	misc::setWidgetBackgroundColor(editText, QucsSettings.BGColor);
	editText->setFocus();
	editText->selectAll();
	// make QLineEdit editable on mouse click
	QPoint p = QPoint(mouseActions()->MAx2, mouseActions()->MAy2);
	editText->setParent(Doc->viewport());
	App->editText->setGeometry(p.x(), p.y(), App->editText->width(), App->editText->height());
	App->editText->show();
#endif
}
#endif
/*--------------------------------------------------------------------------*/
// void SchematicDoc::printCursorPosition(int x, int y)
// {itested();
// 	QPoint p(x,y);
// 	QPointF mp = mapToScene(p);
// 	Doc::printCursorPosition(mp.x(),mp.y());
// }

// ---------------------------------------------------
// why/what?
bool SchematicDoc::createSubcircuitSymbol()
{ untested();
  incomplete();
#if 0
  // If the number of ports is not equal, remove or add some.
  unsigned int countPort = adjustPortNumbers();

  // If a symbol does not yet exist, create one.
  if(symbolPaintings().count() != countPort)
    return false;

  int h = 30*((countPort-1)/2) + 10;

  Painting* txt=painting_dispatcher.clone("ID_Text");
  assert(txt);
  txt->setArgs2Int(-20, h+4); // fix later.
  symbolPaintings().prepend(txt);

  Painting* pp;

  pp = painting_dispatcher.clone("GraphicsLine");
  assert(pp);
  pp->setSomeStuff(-20, -h, 40,  0, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  pp = painting_dispatcher.clone("GraphicsLine");
  pp->setSomeStuff( 20, -h,  0,2*h, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  pp = painting_dispatcher.clone("GraphicsLine");
  pp->setSomeStuff(-20,  h, 40,  0, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  pp = painting_dispatcher.clone("GraphicsLine");
  pp->setSomeStuff(-20, -h,  0,2*h, QPen(Qt::darkBlue,2));
  symbolPaintings().append(pp);

  unsigned int i=0, y = 10-h;
  while(i<countPort) { untested();
    i++;
    pp = painting_dispatcher.clone("GraphicsLine");
    pp->setSomeStuff(-30, y, 10, 0, QPen(Qt::darkBlue,2));
    symbolPaintings().append(pp);
    symbolPaintings().at(i)->setCenter(-30,  y);

    if(i == countPort)  break;
    i++;
    pp = painting_dispatcher.clone("GraphicsLine");
    pp->setSomeStuff( 20, y, 10, 0, QPen(Qt::darkBlue,2));
    symbolPaintings().append(pp);
    symbolPaintings().at(i)->setCenter(30,  y);
    y += 60;
  }
#endif
  return true;
}
/*--------------------------------------------------------------------------*/
// needed?
void SchematicDoc::cleanup()
{ untested();
	Doc::cleanup();
}
/*--------------------------------------------------------------------------*/
// needed?
void SchematicDoc::becomeCurrent()
{itested();
	Doc::becomeCurrent();
  emit signalCursorPosChanged(0, 0);

  // update appropriate menu entry
  if (!_app) { untested();
	  incomplete(); // decouple
//  }else if(isSymbolMode()) { untested();
//    incomplete(); // SymbolDoc.
//    if (docName().right(4) == ".sym") { untested();
//      _app->symEdit->setText(tr("Edit Text"));
//      _app->symEdit->setStatusTip(tr("Edits the Text"));
//      _app->symEdit->setWhatsThis(tr("Edit Text\n\nEdits the text file"));
//    }else{ untested();
//      _app->symEdit->setText(tr("Edit Schematic"));
//      _app->symEdit->setStatusTip(tr("Edits the schematic"));
//      _app->symEdit->setWhatsThis(tr("Edit Schematic\n\nEdits the schematic"));
//    }
  }else{itested();
    _app->symEdit->setText(tr("Edit Circuit Symbol"));
    _app->symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
    _app->symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  }

  {itested();
    incomplete();
    /// if(update){itested();
    ///   incomplete();
    ///   reloadGraphs();   // load recent simulation data
    /// }else{itested();
    /// }
  }


//   TODO: also create menu and attach to main.
//  alignMenu->addAction(moveText);
//  alignMenu->addAction(onGrid);
//  alignMenu->addSeparator();
//  alignMenu->addAction(centerHor);
//  alignMenu->addAction(centerVert);
//  alignMenu->addSeparator();
//  alignMenu->addAction(alignTop);
//  alignMenu->addAction(alignBottom);
//  alignMenu->addAction(alignLeft);
//  alignMenu->addAction(alignRight);
//  alignMenu->addSeparator();
//  alignMenu->addAction(distrHor);
//  alignMenu->addAction(distrVert);
//
//  insMenu->addAction(insWire);
//  insMenu->addAction(insLabel);
//  insMenu->addAction(insEquation);
//  insMenu->addAction(insGround);
//  insMenu->addAction(insPort);
//  insMenu->addAction(setMarker);
//  insMenu->addAction(insEntity);
}

// ---------------------------------------------------
void SchematicDoc::setName (const QString& Name_)
{ untested();
  setFileName(Name_);
  QFileInfo Info (docName());
  QString base = Info.completeBaseName ();
  QString ext = Info.suffix();

  // BUG: properties?
  DataSet = base + ".dat";
  Script = base + ".m";
  if (ext != "dpl"){ untested();
    DataDisplay = base + ".dpl";
  }else{ untested();
    DataDisplay = base + ".sch";
  }
}

// ---------------------------------------------------
// Sets the document to be changed or not to be changed.
void SchematicDoc::setChanged(bool , bool , char)
{itested();
  unreachable(); // moved to doc::execute


  // ................................................
  #if 0 // -> SymbolDoc
  if(isSymbolMode()) {  // for symbol edit mode
    while(undoSymbol.size() > undoSymbolIdx + 1) { untested();
      delete undoSymbol.last();
      undoSymbol.pop_back();
    }

    undoSymbol.append(new QString(createSymbolUndoString(Op)));
    undoSymbolIdx++;

    emit signalUndoState(true);
    emit signalRedoState(false);

    while(static_cast<unsigned int>(undoSymbol.size()) > QucsSettings.maxUndo) { // "while..." because
      delete undoSymbol.first();
      undoSymbol.pop_front();
      undoSymbolIdx--;
    }
    return;
  }
#endif

  // ................................................
  // for schematic edit mode
//  while(undoAction.size() > undoActionIdx + 1) { untested();
//    delete undoAction.last();
//    undoAction.pop_back();
//  }

//  if(Op == 'm') {   // only one for move marker
//    if (undoAction.at(undoActionIdx)->at(0) == Op) { untested();
//      delete undoAction.last();
//      undoAction.pop_back();
//      undoActionIdx--;
//    }
//  }

#if 0 // obsolete?
  undoAction.append(new QString(createUndoString(Op)));
  undoActionIdx++;

  emit signalUndoState(true);
  emit signalRedoState(false);

  while(static_cast<unsigned int>(undoAction.size()) > QucsSettings.maxUndo) { // "while..." because
    delete undoAction.first(); // "maxUndo" could be decreased meanwhile
    undoAction.pop_front();
    undoActionIdx--;
  }
  return;
#endif
}

// -----------------------------------------------------------
//
  // Values exclude border of 1.5cm at each side.
    // DIN A5 landscape
    // DIN A5 portrait
    // DIN A4 landscape
    // DIN A4 portrait
    // DIN A3 landscape
    // DIN A3 portrait
    // letter landscape
    // letter portrait
static int frameX[10]= {0, 1020, 765, 1530, 1020, 2295, 1530, 1414, 1054 };
static int frameY[10]= {0, 765, 1020, 1020, 1530, 1530, 2295, 1054, 1414 };

bool SchematicDoc::sizeOfFrame(int& xall, int& yall)
{ untested();
  int i = showFrame();
  if(!i) { untested();
    // don't show? why does it not have a size?!
    return false;
  }else if(i<9) { untested();
    xall = frameX[i];
    yall = frameY[i];
    return true;
  }else{ untested();
    return false;
  }
}

// -----------------------------------------------------------
void SchematicDoc::paintFrame(ViewPainter *p)
{ untested();
  // dimensions:  X cm / 2.54 * 144
  int xall, yall;
  if(!sizeOfFrame(xall, yall))
    return;
  p->setPen(QPen(Qt::darkGray,1));
  //p->setPen(QPen(Qt::black,0));
  int d = p->LineSpacing + int(4.0 * p->Scale);
  int x1_, y1_, x2_, y2_;
  p->map(xall, yall, x1_, y1_);
  x2_ = int(xall * p->Scale) + 1;
  y2_ = int(yall * p->Scale) + 1;
  p->drawRect(x1_, y1_, -x2_, -y2_);
  p->drawRect(x1_-d, y1_-d, 2*d-x2_, 2*d-y2_);

  int z;
  int step = xall / ((xall+127) / 255);
  for(z=step; z<=xall-step; z+=step) { untested();
    p->map(z, 0, x2_, y2_);
    p->drawLine(x2_, y2_, x2_, y2_+d);
    p->drawLine(x2_, y1_-d, x2_, y1_);
  }
  char Letter[2] = "1";
  for(z=step/2+5; z<xall; z+=step) { untested();
    p->drawText(Letter, z, 3, 0);
    p->map(z, yall+3, x2_, y2_);
    p->drawText(x2_, y2_-d, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  step = yall / ((yall+127) / 255);
  for(z=step; z<=yall-step; z+=step) { untested();
    p->map(0, z, x2_, y2_);
    p->drawLine(x2_, y2_, x2_+d, y2_);
    p->drawLine(x1_-d, y2_, x1_, y2_);
  }
  Letter[0] = 'A';
  for(z=step/2+5; z<yall; z+=step) { untested();
    p->drawText(Letter, 5, z, 0);
    p->map(xall+5, z, x2_, y2_);
    p->drawText(x2_-d, y2_, 0, 0, Qt::TextDontClip, Letter);
    Letter[0]++;
  }

  // draw text box with text
  p->map(xall-340, yall-3, x1_, y1_);
  p->map(xall-3,   yall-3, x2_, y2_);
  x1_ -= d;  x2_ -= d;
  y1_ -= d;  y2_ -= d;
  d = int(6.0 * p->Scale);
  z = int(200.0 * p->Scale);
  y1_ -= p->LineSpacing + d;
  p->drawLine(x1_, y1_, x2_, y1_);
  p->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[2]);
  p->drawLine(x1_+z, y1_, x1_+z, y1_ + p->LineSpacing+d);
  p->drawText(x1_+d+z, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[3]);
  y1_ -= p->LineSpacing + d;
  p->drawLine(x1_, y1_, x2_, y1_);
  p->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[1]);
  y1_ -= (_frameText[0].count('\n')+1) * p->LineSpacing + d;
  p->drawRect(x2_, y2_, x1_-x2_-1, y1_-y2_-1);
  p->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, _frameText[0]);
}
/*--------------------------------------------------------------------------*/
#if 0 // Doc?
void SchematicDoc::print(QPrinter*, QPainter *Painter, bool printAll, bool fitToPage)
{ untested();
#ifndef USE_SCROLLVIEW
  (void) Painter;
  (void) printAll;
  (void) fitToPage;
  incomplete(); // does not work with qt5
#else
  QPaintDevice *pdevice = device();
  float printerDpiX = (float)pdevice->logicalDpiX();
  float printerDpiY = (float)pdevice->logicalDpiY();
  float printerW = (float)pdevice->width();
  float printerH = (float)pdevice->height();
  QPainter pa(viewport());
  float screenDpiX = (float)pa.device()->logicalDpiX();
  float screenDpiY = (float)pa.device()->logicalDpiY();
  float PrintScale = 0.5;
  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  int marginX = (int)(40 * printerDpiX / screenDpiX);
  int marginY = (int)(40 * printerDpiY / screenDpiY);

  if(fitToPage) { untested();

    float ScaleX = float((printerW - 2*marginX) /
                   float((UsedX2-UsedX1) * printerDpiX)) * screenDpiX;
    float ScaleY = float((printerH - 2*marginY) /
                   float((UsedY2-UsedY1) * printerDpiY)) * screenDpiY;

    if(showFrame()){ untested();
        int xall, yall;
        sizeOfFrame(xall, yall);
        ScaleX = ((float)(printerW - 2*marginX) /
                       (float)(xall * printerDpiX)) * screenDpiX;
        ScaleY = ((float)(printerH - 2*marginY) /
                       (float)(yall * printerDpiY)) * screenDpiY;
    }

    if(ScaleX > ScaleY)
      PrintScale = ScaleY;
    else
      PrintScale = ScaleX;
  }


  //bool selected;
  ViewPainter p;
  int StartX = UsedX1;
  int StartY = UsedY1;
  if(showFrame()) { untested();
    if(UsedX1 > 0)  StartX = 0;
    if(UsedY1 > 0)  StartY = 0;
  }

  float PrintRatio = printerDpiX / screenDpiX;
  QFont oldFont = Painter->font();
  QFont printFont = Painter->font();
#ifdef __MINGW32__
  printFont.setPointSizeF(printFont.pointSizeF()/PrintRatio);
  Painter->setFont(printFont);
#endif
  p.init(Painter, PrintScale * PrintRatio,
         -StartX, -StartY, -marginX, -marginY, PrintScale, PrintRatio);

  if(!isSymbolMode())
    paintFrame(&p);

  paintSchToViewpainter(&p,printAll,false,screenDpiX,printerDpiX);

  Painter->setFont(oldFont);
#endif
}
#endif
/*--------------------------------------------------------------------------*/
// Enlarge the viewport area if the coordinates x1-x2/y1-y2 exceed the
// visible area.
//
// same as center on union(current, rectangle(xy))??
// // implemented in QGraphicsView::ensureVisible, it seems
void SchematicDoc::enlargeView(int , int, int, int )
{ untested();
  incomplete();
#if 0
  int dx=0, dy=0;
  (void) (dx+dy);
  if(x1 < UsedX1) UsedX1 = x1;
  if(y1 < UsedY1) UsedY1 = y1;
  if(x2 > UsedX2) UsedX2 = x2;
  if(y2 > UsedY2) UsedY2 = y2;

  if(x1 < ViewX1) { untested();
    dx = int(Scale * float(ViewX1-x1+40));
    ViewX1 = x1-40;
  }
  if(y1 < ViewY1) { untested();
    dy = int(Scale * float(ViewY1-y1+40));
    ViewY1 = y1-40;
  }
  if(x2 > ViewX2) ViewX2 = x2+40;
  if(y2 > ViewY2) ViewY2 = y2+40;

  TODO("Fix resizeContents");
  /// \todo resizeContents(int(Scale*float(ViewX2 - ViewX1)),
  /// 		int(Scale*float(ViewY2 - ViewY1)));
  ///scrollBy(dx,dy);
#endif
}
/*--------------------------------------------------------------------------*/
// Update the graph data of all diagrams (load from data files).
void SchematicDoc::reloadGraphs()
{itested();
  incomplete();
#if 0
  QFileInfo Info(docName());
  for(auto pd : diagrams()){itested();
    pd->loadGraphData(Info.path()+QDir::separator()+DataSet);
  }
#endif
}
/*--------------------------------------------------------------------------*/
// Cut function, copy followed by deletion
void SchematicDoc::cut()
{ untested();
	incomplete();

 //  copy();
 //  deleteElements(); //delete selected elements
 //  viewport()->update();
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::deselectElements()
{ untested();
	qDebug() << "deselectElements";
	assert(scene());
	while(scene()->selectedItems().size()){ untested();
		auto i=scene()->selectedItems().first();
		if(auto x=dynamic_cast<ElementGraphics*>(i)){ untested();
			// BUG: selected state is stored in element.
			x->setSelected(false);
			i->setSelected(false);
		}else{ unreachable();
		}
	}

}
/*--------------------------------------------------------------------------*/
void SchematicDoc::setFrameText(int, QString)
{ untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Factory : public Widget {
	Widget* clone() const override{ untested();
		Doc* wd = new SchematicDoc();
		return wd;
	}
}F;
static Dispatcher<Widget>::INSTALL p1(&widget_dispatcher, "SchematicDoc", &F);
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
