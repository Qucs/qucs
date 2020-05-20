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

namespace{
class sda : public SchematicSymbol{
public:
  sda(SchematicDoc& s) : SchematicSymbol(), _s(s) {
	  new_subckt();
	  subckt()->attachDoc(&s);
  }

private:
  Port& port(unsigned){ unreachable(); return *new Port(0,0); }
  unsigned numPorts() const{ return 0; }
private: // SchematicSymbol
  SchematicModel const& schematicModel() const{
    incomplete();
    return *subckt();
  }
  SchematicModel* schematicModel() {
    return nullptr;
  }
  std::string getParameter(std::string const&) const{
    incomplete();
    return "incomplete";
  }
private:
  SchematicDoc const& _s;
};
}

SchematicDoc::SchematicDoc(QucsApp& App_, const QString& Name_)
    : QucsDoc(App_, Name_), _model(nullptr), _root(nullptr),
	 mouseAction(nullptr),
  SymbolMode(false)
{ untested();
  qDebug() << "SchematicDoc::SchematicDoc" << Name_;

  _root = new sda(*this);
  _model = _root->subckt();

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

#ifndef USE_SCROLLVIEW
  // HUH?
  setSceneRect(-2000, -2000, 4000, 4000);
  Scene = new SchematicScene(this);
  QBrush b(Qt::Dense7Pattern);
  Scene->setBackgroundBrush(b);
  //Scene->setBackgroundBrush(Qt::blue);

  setDragMode(QGraphicsView::RubberBandDrag); // why?

  this->setScene(Scene);
#endif

  ShowFrame = 0;  // don't show
  FrameText[0] = tr("Title");
  FrameText[1] = tr("Drawn By:");
  FrameText[2] = tr("Date:");
  FrameText[3] = tr("Revision:");

#ifdef USE_SCROLLVIEW
  setVScrollBarMode(Q3ScrollView::AlwaysOn);
  setHScrollBarMode(Q3ScrollView::AlwaysOn);
#else
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
#endif

  misc::setWidgetBackgroundColor(viewport(), QucsSettings.BGColor);
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
  { untested();
    connect(this, SIGNAL(signalCursorPosChanged(int, int)), 
        this, SLOT(printCursorPosition(int, int)));
    /** \todo
    connect(this, SIGNAL(horizontalSliderPressed()), 
        App_, SLOT(slotHideEdit()));
    connect(this, SIGNAL(verticalSliderPressed()),
        App_, SLOT(slotHideEdit()));
    */
    connect(this, SIGNAL(signalUndoState(bool)),
        this, SLOT(slotUpdateUndo(bool)));
    connect(this, SIGNAL(signalRedoState(bool)),
        this, SLOT(slotUpdateRedo(bool)));
    connect(this, SIGNAL(signalFileChanged(bool)),
        this, SLOT(slotFileChanged(bool)));
  }
} // ::SchematicDocument

SchematicDoc::~SchematicDoc()
{ untested();
	delete _root;
}

bool SchematicDoc::loadDocument(QFile& /*BUG*/ file)
{ untested();
	incomplete();
  QString Line;
  DocumentStream stream(&file);

  // read header **************************
  do { untested();
    if(stream.atEnd()) { untested();
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
{ untested();
	if(!L){ untested();
		auto D=doclang_dispatcher["leg_sch"];
		L = dynamic_cast<SchematicLanguage const*>(D);
	}else{ untested();
	}
	assert(L);
	assert(_root);
	// ins i(this); // BUG
	while(!s.atEnd()){ untested();
		qDebug() << "entering parse";
		L->parse(s, *_root);
		assert(s.atEnd()); // happens with legacy lang
	}
}

void SchematicDoc::insertComponent(Component *c)
{ untested();
    assert(_model);
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
        c->obsolete_name_override_hack(
	    c->name() + QString::number(max));  // create name with new number
    }

    setComponentNumber(c); // important for power sources and subcircuit ports

} // insertComponent
