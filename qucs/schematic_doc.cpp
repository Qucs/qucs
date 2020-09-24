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

namespace{
class sda : public SchematicSymbol{
public:
  sda(SchematicDoc& s) : SchematicSymbol(), _s(s) {itested();
	  new_subckt();
	  subckt()->attachDoc(&s);
  }

private:
  Port& port(unsigned){ unreachable(); return *new Port(0,0); }
  unsigned numPorts() const{ return 0; }
private: // SchematicSymbol
  SchematicModel const& schematicModel() const{ untested();
    incomplete();
    return *subckt();
  }
  SchematicModel* schematicModel() { untested();
    return nullptr;
  }
  std::string getParameter(std::string const&) const{itested();
    incomplete();
    return "incomplete";
  }
private:
  SchematicDoc const& _s;
};
}

SchematicDoc::SchematicDoc(QucsApp& App_, const QString& Name_)
   : QGraphicsView(),
     QucsDoc(App_, Name_),
     _root(nullptr),
     _model(nullptr),
     SymbolMode(false),
     _undoStack(nullptr)
{itested();
  qDebug() << "SchematicDoc::SchematicDoc" << Name_;

  _root = new sda(*this);
  _model = _root->subckt();

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

  // The 'i' means state for being unchanged.
  undoActionIdx = 0;
  undoAction.append(new QString(" i\n</>\n</>\n</>\n</>\n"));
  undoSymbolIdx = 0;
  undoSymbol.append(new QString(" i\n</>\n</>\n</>\n</>\n"));

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

  //if (App_)
  {itested();
    connect(this, SIGNAL(signalCursorPosChanged(int, int)), 
        this, SLOT(printCursorPosition(int, int)));
    /** \todo
    connect(this, SIGNAL(horizontalSliderPressed()), 
        App_, SLOT(slotHideEdit()));
    connect(this, SIGNAL(verticalSliderPressed()),
        App_, SLOT(slotHideEdit()));
    */
    connect(this, SIGNAL(signalUndoState(bool)),
        &App_, SLOT(slotUpdateUndo(bool)));
    connect(this, SIGNAL(signalRedoState(bool)),
        &App_, SLOT(slotUpdateRedo(bool)));
    connect(this, SIGNAL(signalFileChanged(bool)),
        this, SLOT(slotFileChanged(bool)));
  }
  
  // really?
    emit signalUndoState(true);
    emit signalRedoState(false);
} // ::SchematicDocument

SchematicDoc::~SchematicDoc()
{itested();
	delete _root;
	delete _undoStack;
	delete _mouseActions;
//	delete Scene; ???
}

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

bool SchematicDoc::loadDocument(QFile& /*BUG*/ file)
{itested();
	incomplete();
  QString Line;
  DocumentStream stream(&file);

  // read header **************************
  do {itested();
    if(stream.atEnd()) {itested();
      file.close(); // BUG
      return true;
    }

    Line = stream.readLine();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    throw "incomplete_exception";
  }

  Line = Line.mid(16, Line.length()-17);

  parse(stream);
  file.close();
  return true;
}

void SchematicDoc::parse(DocumentStream& s, SchematicLanguage const* L)
{itested();
	if(!L){itested();
		auto D=doclang_dispatcher["leg_sch"];
		L = dynamic_cast<SchematicLanguage const*>(D);
	}else{ untested();
	}
	assert(L);
	assert(_root);
	// ins i(this); // BUG
	while(!s.atEnd()){itested();
		qDebug() << "entering parse";
		L->parse(s, *_root);
		assert(s.atEnd()); // happens with legacy lang
	}
}

// obsolete?
void SchematicDoc::insertComponent(Component *c)
{ untested();
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
#if 0 // moved to Ground::insertComponentNodes
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
#endif
    }else{ untested();
        // determines the name by looking for names with the same
        // prefix and increment the number
        for(auto pc : components()){ untested();
            if(pc->name().left(len) == c->name())
            { untested();
                s = pc->name().right(pc->name().length()-len);
                z = s.toInt(&ok);
                if(ok) if(z >= max) max = z + 1;
            }
	}
//        c->obsolete_name_override_hack(
//	    c->name() + QString::number(max));  // create name with new number
    }

    setComponentNumber(c); // important for power sources and subcircuit ports

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
}
void SchematicDoc::sceneRemoveItem(ElementGraphics* x)
{
	assert(scene());
	scene()->removeItem(x);
}

// questionable.
ElementGraphics& SchematicDoc::addToScene(Element* x)
{itested();
  auto i=new ElementGraphics(x);
  scene()->addItem(i);
  return *i;
}

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

