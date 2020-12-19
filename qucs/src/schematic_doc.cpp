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

#include "schematic_doc.h"
#include "misc.h"
#include "globals.h"
#include "qucs.h" // BUG. QucsSettings?
#include <QUndoStack>
#include "simmessage.h"
#include "docfmt.h" // copy&paste

// tmp hack.
QucsDoc* newSchematicDoc(QucsApp* a, QString const& b, QWidget* o)
{
	return new SchematicDoc(a, b, o);
}
/*--------------------------------------------------------------------------*/
SchematicDoc::SchematicDoc(QucsApp* App_/*BUG?*/, const QString& Name_, QWidget* owner)
   : QGraphicsView(),
     QucsDoc(App_, Name_, owner),
     _root(nullptr),
     _model(nullptr),
     _main(nullptr),
     _undoStack(nullptr)
{itested();
  qDebug() << "SchematicDoc::SchematicDoc" << Name_;

  // ...........................................................
  // create empty schematic... (clear?)
  auto root = symbol_dispatcher.clone("schematic_root");
  _root = dynamic_cast<SchematicSymbol*>(root);
  assert(_root);

  { // hack?
	  _model = _root->subckt();
	  auto e = (*_model->find_("main"));
	  _main = prechecked_cast<SubcktBase*>(e);
	  assert(_main);
	  _model = _main->scope();
  }
  // ...........................................................

  _mouseActions = new SchematicActions(*this);
  _undoStack = new QUndoStack();

//  what is the viewport, and where is it initialised?
  assert(viewport());
  viewport()->installEventFilter(_mouseActions);

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

  // HUH?
  setSceneRect(-2000, -2000, 4000, 4000);
  SchematicScene* Scene = new SchematicScene(this);
  QBrush b(Qt::Dense7Pattern);
  Scene->setBackgroundBrush(b);
  // Scene->setBackgroundBrush(Qt::blue);
  // setBackgroundBrush(Qt::blue);

  setDragMode(QGraphicsView::RubberBandDrag); // why?

  setScene(Scene);

  ShowFrame = 0;  // don't show
  _frameText[0] = tr("Title");
  _frameText[1] = tr("Drawn By:");
  _frameText[2] = tr("Date:");
  _frameText[3] = tr("Revision:");

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  misc::setWidgetBackgroundColor(viewport(), QucsSettings.BGColor);
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
	  assert(_app);
    connect(this, SIGNAL(signalCursorPosChanged(int, int)), 
        this, SLOT(printCursorPosition(int, int)));
    /** \todo
    connect(this, SIGNAL(horizontalSliderPressed()), 
        App_, SLOT(slotHideEdit()));
    connect(this, SIGNAL(verticalSliderPressed()),
        App_, SLOT(slotHideEdit()));
    */
    connect(this, SIGNAL(signalUndoState(bool)),
        _app, SLOT(slotUpdateUndo(bool)));
    connect(this, SIGNAL(signalRedoState(bool)),
        _app, SLOT(slotUpdateRedo(bool)));
    connect(this, SIGNAL(signalFileChanged(bool)),
        this, SLOT(slotFileChanged(bool)));
  }
  
  // really?
    emit signalUndoState(true);
    emit signalRedoState(false);
} // ::SchematicDocument
/*--------------------------------------------------------------------------*/
SchematicDoc::~SchematicDoc()
{itested();
	delete _root;
	delete _undoStack;
	delete _mouseActions;
//	delete Scene; ???
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::showEvent(QShowEvent*e)
{itested();
	QGraphicsView::showEvent(e);
}
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
/*--------------------------------------------------------------------------*/
bool SchematicDoc::loadDocument(QFile& /*BUG*/ file)
{itested();
	incomplete();
  QString Line;
  istream_t stream(&file);

  // read header **************************
  do {itested();
    if(stream.atEnd()) {itested();
      file.close(); // BUG
      return true;
    }

    Line = QString::fromStdString(stream.read_line());
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    throw "incomplete_exception";
  }

  Line = Line.mid(16, Line.length()-17);

  parse(stream);
  file.close();
  for(auto i : *_model){
	 scene()->addElement(i);
  }
  return true;
}

// why both?!
bool SchematicDoc::load()
{itested();
  delete _root;
  _root = nullptr;

  auto root = symbol_dispatcher.clone("schematic_root");
  _root = dynamic_cast<SchematicSymbol*>(root);
  assert(_root);
  _root->setOwner(this); //needed?

  _root->setParameter("$filename", docName().toStdString());

  {
	  _model = _root->subckt();
	  auto e = (*_model->find_("main"));
	  _main = prechecked_cast<SubcktBase*>(e);
	  assert(_main);
	  _model = _main->scope();
  }

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
  becomeCurrent(false);

  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  if(ViewX1 > UsedX1)  ViewX1 = UsedX1;
  if(ViewY1 > UsedY1)  ViewY1 = UsedY1;
  if(ViewX2 < UsedX2)  ViewX2 = UsedX2;
  if(ViewY2 < UsedY2)  ViewY2 = UsedY2;
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
		auto D = doclang_dispatcher["leg_sch"]; // use command instead.
		L = dynamic_cast<SchematicLanguage const*>(D);
	}else{ untested();
	}
	assert(L);
	assert(_root);

	s.read_line();
	while(!s.atEnd()){itested();
		assert(_root->subckt());
		trace1("SchematicDoc::parse", s.fullstring());
		L->new__instance(s, _root, _model);
		s.read_line();
	}
}

// obsolete?
void SchematicDoc::insertComponent(Component *)
{ untested();
#if 0
    assert(_model);
	 assert(false); // obsolete?
    _model->pushBack(c);
    // connect every node of component to corresponding schematic node
    _model->insertSymbolNodes(c, false);
    return;

    bool ok;
    QString s;
    int  max=1, len = c->name().length(), z;
    if(c->name().isEmpty()) { // BUG
        // a ground symbol erases an existing label on the wire line
        if(c->obsolete_model_hack() == "GND") { // BUG
            c->gnd_obsolete_model_override_hack("x");
            Element *pe = getWireLabel(c->Ports.first()->Connection);
            if(pe) if((pe->Type & isComponent) == 0)
                { untested();
                    delete ((Conductor*)pe)->Label;
                    ((Conductor*)pe)->Label = 0;
                }
            c->gnd_obsolete_model_override_hack("GND");
        }
    }else{ untested();
        // determines the name by looking for names with the same
        // prefix and increment the number
        for(auto pc : components()){ untested();
            if(pc->label().left(len) == c->name())
            { untested();
                s = pc->label().right(pc->label().length()-len);
                z = s.toInt(&ok);
                if(ok) if(z >= max) max = z + 1;
            }
	}
//        c->obsolete_name_override_hack(
//	    c->name() + QString::number(max));  // create name with new number
    }

    setComponentNumber(c); // important for power sources and subcircuit ports
#endif

} // insertComponent


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
{
	return setOnGrid(getX(e), getY(e));
}
QMouseEvent SchematicDoc::snapToGrid(QMouseEvent* e)const
{
	  auto type = e->type();
	  auto localPos = e->localPos();
	  auto& windowPos = e->windowPos();
	  auto& screenPos = e->screenPos();
	  auto button = e->button();
	  auto buttons = e->buttons();
	  auto modifiers = e->modifiers();
	  auto source = e->source();

	  // if snapToGrid?
	  localPos = setOnGrid(localPos.x(), localPos.y());
	  trace1("SchematicDoc::snapToGrid", localPos);
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
  }else{
  }
  if(y1 > y2) { untested();
    y1 = 0;
    y2 = 800;
  }else{
  }

  if(x2==0 && y2==0 && x1==0 && y1==0){
    x2 = y2 = 800;
  }else{
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
{
	QGraphicsView::drawBackground(painter, rect);

	double n = 10;

	// Draw origin when visible
	if(rect.contains(QPointF(0, 0))) {
		painter->drawLine(QLine(-n, 0.0, n, 0.0));
		painter->drawLine(QLine(0.0, -n, 0.0, n));
	}else{
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
  if(e->isAccepted()){ itested();
  }else{itested();
	  auto ee = snapToGrid(e);

	  // move actions go through here.
	  signalCursorPosChanged(ee.localPos().x(), ee.localPos().y());
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
  mouseActions()->handle(e);
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
bool SchematicDoc::event(QEvent* e)
{itested();
	assert(mouseActions());
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
{
	assert(mouseActions());
	return mouseActions()->handle(e);
}

#if 0
QPointF SchematicDoc::mapToScene(QPoint const& p) const
{
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
  if(App->MouseReleaseAction == &MouseActions::MReleasePaste){ untested();
    // wtf?
    return;
  }else{ untested();
  }

  // map screen coordinates do scene coordinates.
  // FIXME there must be a sane way.
  float x = float(e->pos().x())/Scale + float(ViewX1);
  float y = float(e->pos().y())/Scale + float(ViewY1);
  QPoint p(x, y);

  qDebug() << "nestedEvent?"; // this does not work right. need both...
  QMouseEvent nestedEvent(e->type(), e->pos(), e->globalPos(),
      e->button(), e->buttons(), e->modifiers());

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
    QucsDoc *d = QucsMain->getDoc(0);
    bool changed = d->DocChanged;
    d->DocChanged = true;

    // URI:  file:/home/linuxuser/Desktop/example.sch
    foreach(QUrl url, urls) { untested();
      App->gotoPage(QDir::toNativeSeparators(url.toLocalFile()));
    }

    d->DocChanged = changed;
    return;
  }else{
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
    }else{
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
    }else{
	 }

  Event->ignore();
}
#endif

void SchematicDoc::contentsDragLeaveEvent(QDragLeaveEvent*)
{ untested();
#if 0
  if(!App->view->selElem){
  }else if(App->view->selElem->Type & isComponent){
    if(App->view->drawn) { untested();

      QPainter painter(viewport());
      //App->view->setPainter(this);
      ((Component*)App->view->selElem)->paintScheme(this);
      App->view->drawn = false;
    }else{
    }
  }else{
  }
#endif

  if(formerAction){
    formerAction->setChecked(true);  // restore old action
  }else{
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
  }else{
    Event->accept();
  }
}
#endif

// take ownership.
void SchematicDoc::sceneAddItem(ElementGraphics* x)
{
	assert(scene());
	scene()->addItem(x);
	QGraphicsItem* g = x;
	g->show();
}
void SchematicDoc::sceneRemoveItem(ElementGraphics* x)
{
	assert(scene());
	scene()->removeItem(x);
}

void SchematicDoc::addElement(Element* x)
{
	assert(!x->mutable_owner());
	x->setOwner(_main);
	_model->pushBack(x);
}

#if 0
QGraphicsItem& SchematicDoc::addToScene(Element* x)
{itested();
	unreachable(); // why?
	assert(scene());
	return scene()->addElement(x);
}
#endif

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
{
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
Node const* SchematicDoc::nodeAt(pos_t const& p) const
{
	assert(_model);
	return _model->nodeAt(p);
}
/*--------------------------------------------------------------------------*/
#include <QMessageBox> // really??
void SchematicDoc::slotSimulate()
{
  auto w = prechecked_cast<QWidget*>(this);
  assert(w);


#if 0 //not sure
  if(isTextDocument (w)) { untested();
    Doc = (QucsDoc*)((TextDoc*)w);
    if(Doc->SimTime.isEmpty() && ((TextDoc*)Doc)->simulation) { untested();
      DigiSettingsDialog *d = new DigiSettingsDialog((TextDoc*)Doc);
      if(d->exec() == QDialog::Rejected)
	return;
    }
  } else {
    Doc = (QucsDoc*)((SchematicDoc*)w);
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
      if(No == 0){
        Doc->load();
		}else{
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

  try{
	  ctrl->startProcess(cs);
  }catch(Exception& e){
	  incomplete();
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), ctrl, SLOT(slotClose()));
} // SchematicDoc::slotSimulate
/*--------------------------------------------------------------------------*/
// TODO: deduplicate
void SchematicDoc::slotDCbias()
{
//  slotHideEdit(); // disable text edit of component property (why?)

  if(!docName().isEmpty()){
	  // ok.
  }else if(saveAs()){
		// perhaps because the output filename is determined by the schematic
		// filename!?
  }else{
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
      if(No == 0){
			Doc->load();
		}else{
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

  try{
	  ctrl->startProcess(cs);
  }catch(Exception& e){
	  incomplete();
  }

  connect(this, SIGNAL(signalKillEmAll()), this, SLOT(killSimulator()));
} // slotDCbias
/*--------------------------------------------------------------------------*/
void SchematicDoc::slotRefreshData(std::string const& what)
{
	incomplete();
}
/*--------------------------------------------------------------------------*/
// Copy function, 
#include <QApplication>
#include <QClipboard>
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
	explicit cnpsymbol() : SubcktBase(){
		new_subckt();
	}

private:
	virtual Port& port(unsigned){ assert(false); return *new Port(); }
};
/*--------------------------------------------------------------------------*/
// BUG: it does not create a file.
QString SchematicDoc::createClipboardFile() const
{
	cnpsymbol sym;
	assert(sym.subckt());
	for (auto i : scene()->selectedItems()){
		auto cl = i->cloneElement();
		sym.subckt()->pushBack(cl);
	}

	auto lang = doclang_dispatcher["leg_sch"];
	assert(lang);
	auto fmt = prechecked_cast<DocumentLanguage const*>(lang);
	assert(fmt);

	QString buf;
	ostream_t s(&buf);
	for(auto i : *sym.subckt()){
		fmt->printItem(i, s);
	}
	s.flush();
	return buf;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
