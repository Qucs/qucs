/***************************************************************************
                           schematic_model.cpp
                             ---------------
    begin                : 2018, 2020
    copyright            : Felix
    email                : felix@salfelder.org
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
#include "schematic_lang.h"
#include "globals.h"
#include "nodemap.h"
#include "net.h"
#include "trace.h"


// getting here in CLI mode
SchematicModel::SchematicModel()
  	: Nodes(Nets), _doc_(nullptr)
{
	trace2("::SchematicModel", this, _doc_);
}

SchematicModel::~SchematicModel()
{
}

// getting here in GUI mode
SchematicModel::SchematicModel(SchematicDoc* s)
	: _doc_(s),
	  Nodes(Nets)
{
	trace2("::SchematicModel s", this, _doc_);
	if(s){ untested();
	}else{
	}
	// presumably Q3PTRlist without this is just a QList<*> (check)
//  _symbol=new SchematicSymbol();
}

void SchematicModel::clear()
{
	incomplete(); // disconnect components
	components().clear();

	diagrams().clear();
	{ // clearWires
		// (in legacy code) deleting a wire may create another wire.
		while(wires().size()){
			trace2("clear wire", wires().size(), nodes().size());
			erase(wires().first());
		}
	}

	nodes().clear();
	paintings().clear();
	//SymbolPaints.clear(); ??
}

QString const& SchematicModel::devType() const
{
	return DevType;
}

void SchematicModel::setDevType(QString const& s)
{
	DevType = s;
}

namespace{
  static const QString QSTRING_ERROR("ERROR");
// TODO: use Object
class ins : public SchematicSymbol{
public:
	ins(SchematicModel* m) : _m(m) {
		assert(m);
	}
private: // ModelInserter
	void pushBack(Element* e){
		if(auto s=dynamic_cast<SchematicSymbol*>(e)){ untested();
			(void) s;
			incomplete();
		//delete _symbol;
		//_symbol = s;
		}else{
			_m->pushBack(e);
		}
	}
	void setParameter(std::string const&, std::string){
		incomplete();
	}
	PaintingList& symbolPaintings(){
		return _dummy;
	}

private: // internal. Portstuff
  unsigned portCount() const {unreachable(); return 0;}
  QString const& portValue(unsigned) const{ unreachable(); return QSTRING_ERROR; } // BUG
  void setPort(unsigned i, Node* n){ unreachable(); }
  Port& port(unsigned){unreachable(); return *new Port(0,0);}

private: // SchematicSymbol
	SchematicModel const& schematicModel() const{
		assert(_m);
		return *_m;
	}
	SchematicModel* schematicModel() {
		assert(_m);
		return _m;
	}
	std::string getParameter(std::string const&) const{
		return "incomplete getParameter";
	}

private:
	SchematicModel* _m;
	PaintingList _dummy; // ignore those paintings.
};
}

void SchematicModel::parse(DocumentStream& s, SchematicLanguage const* L)
{ untested();
	if(!L){ untested();
		auto D=doclang_dispatcher["leg_sch"];
		L = dynamic_cast<SchematicLanguage const*>(D);
	}else{ untested();
	}
	assert(L);
	ins i(this);
	while(!s.atEnd()){ untested();
		qDebug() << "entering parse";
		L->parse(s, i);
		assert(s.atEnd()); // happens with legacy lang
	}
}

/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ComponentList& SchematicModel::components()
{
	return Components;
}

void SchematicModel::pushBack(Element* what)
{
	trace2("SchematicModel::pushBack", what->label(), this);
	if(auto c=component(what)){ untested();
		trace1("SchematicModel::pushBack", c->type());
      simpleInsertComponent(c);
	}else if(auto d=diagram(what)){ untested();
		diagrams().append(d);
	}else if(auto c=command(what)){ untested();
		incomplete();
	}else if(auto w=wire(what)){
		trace4("pushback Wire", w->x1__(), w->y1__(), w->x2__(), w->y2__());
		simpleInsertWire(w);
//		insertWire(w);?? wtf?
	}else if(auto s=dynamic_cast<SchematicSymbol*>(what)){ untested();
		(void)s;
		assert(false);
		//delete _symbol;
		//_symbol = s;
	}else{
		incomplete();
	}

#ifndef USE_SCROLLVIEW
  if(doc()){ untested();
	  doc()->addToScene(what);
  }else{
  }
#endif
} // pushBack

// was Schematic::insertComponentNodes.
void SchematicModel::insertSymbolNodes(Symbol *c, bool noOptimize)
{
	// connect every node of the component to corresponding schematic node
	for(unsigned i=0; i<c->portCount(); ++i){

		c->connectNode(i, nodes());
	}

	if(noOptimize)  return;
#if 0
	// replace wires. later.

	Node    *pn;
	Element *pe, *pe1;
	Q3PtrList<Element> *pL;
	// if component over wire then delete this wire
	QListIterator<Port *> iport(c->Ports);
	// omit the first element
	Port *pp = iport.next();
	while (iport.hasNext())
	{
		pp = iport.next();
		pn = pp->Connection;
		for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next()){
			if(pe->Type == isWire)
			{
				if(((Wire*)pe)->portValue(0) == pn)  pL = &(((Wire*)pe)->Port2->Connections);
				else  pL = &(((Wire*)pe)->portValue(0)->Connections);

				for(pe1 = pL->first(); pe1!=0; pe1 = pL->next())
					if(pe1 == c)
					{
						deleteWire((Wire*)pe);
						break;
					}
			}
		}
	}
#endif
}

// called from schematic::erase only
// // possibly not needed. all actions must be undoable anyway
// -> use detach, store reference in UndoAction.
void SchematicModel::erase(Element* what)
{
	Element* e = detach(what);
	delete(e);
}

// TODO: take iterator.
Element* SchematicModel::detach(Element* what)
{
	if(auto c=component(what)){
		disconnect(c);
		components().removeRef(c);
	}else if(auto d=diagram(what)){
		diagrams().removeRef(d);
	}else if(auto w=wire(what)){
		disconnect(w);
		wires().removeRef(w);
	}else{
		unreachable();
	}
	return what;
}


// should be a QucsDoc*, probably
SchematicDoc* SchematicModel::doc()
{
	trace2("doc", _doc_, this);
	return _doc_;
}

QFileInfo const& SchematicModel::getFileInfo ()const
{
	return FileInfo;
}

WireList& SchematicModel::wires()
{
	return _wires;
}

NodeMap& SchematicModel::nodes()
{
	return Nodes;
}

//PaintingList const& SchematicModel::symbolPaints() const
//{
//	return SymbolPaints;
//}

PaintingList& SchematicModel::paintings()
{
	return Paintings;
}
//
//PaintingList& SchematicModel::symbolPaintings()
//{
//	assert(_symbol);
//	// temporary. move stuff here....
//	return _symbol->symbolPaintings();
//}
//
DiagramList& SchematicModel::diagrams()
{
	return Diagrams;
}

// same, but const.
ComponentList const& SchematicModel::components() const
{
	return Components;
}

WireList const& SchematicModel::wires() const
{
	return _wires;
}

NodeMap const& SchematicModel::nodes() const
{
	return Nodes;
}

PaintingList const& SchematicModel::paintings() const
{
	return Paintings;
}

DiagramList const& SchematicModel::diagrams() const
{
	return Diagrams;
}

//PaintingList const& SchematicModel::symbolPaints() const
//{
//	return SymbolPaintings;
//}

// TODO: what is this? (perhaps DocumentFormat?)
static void createNodeSet(QStringList& Collect, int& countInit,
		Conductor *pw, Node *p1)
{
	if(pw->Label)
		if(!pw->Label->initValue.isEmpty())
			Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
					p1->name() + " U=\"" + pw->Label->initValue + "\"");
}

#if 0
bool SchematicModel::throughAllComps(DocumentStream& stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		   bool creatingLib, NetLang const& nl)
{
	bool r;
	QString s;
	bool isAnalog=true;
	bool isVerilog=false;

	// give the ground nodes the name "gnd", and insert subcircuits etc.
	for(auto pc : components()) {
	}
}
#endif

// find connected components (slow)
// obsolete.
void SchematicModel::throughAllNodes(unsigned& z) const
{ untested();
#if 0
  z = 0; // number cc.

  for(auto pn : nodes()){
    pn->resetNetNumber();
  }

  for(auto pn : nodes()){
    if(pn->hasNetNumber()){
    }else{
      pn->setNetNumber(z++);
      propagateNode(pn);
    }
  }

  qDebug() << "got" << nodes().size() << "nodes and" << z << "cc";
  nc = z;
#endif
} // throughAllNodes

#if 0
void SchematicModel::propagateNode(QStringList& Collect,
		int& countInit, Node *pn)
{
	bool isAnalog=true;
	bool setName=false;
	Q3PtrList<Node> Cons;
	Node *p2;
	Wire *pw;
	Element *pe;

	Cons.append(pn);
	for(p2 = Cons.first(); p2 != 0; p2 = Cons.next()){
		for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
			if(wire(pe)){
				pw = (Wire*)pe;
				if(p2 != pw->portValue(0)) {
					if(pw->portValue(0)->name().isEmpty()) {
						pw->portValue(0)->setName(pn->name());
						pw->portValue(0)->State = 1;
						Cons.append(pw->portValue(0));
						setName = true;
					}
				}else{
					if(pw->Port2->name().isEmpty()) {
						assert(pn);
						pw->Port2->setName(pn->name());
						pw->Port2->State = 1;
						Cons.append(pw->Port2);
						setName = true;
					}
				}
				if(setName) {
					Cons.findRef(p2);   // back to current Connection
					if (isAnalog) createNodeSet(Collect, countInit, pw, pn);
					setName = false;
				}
			}
	}
	Cons.clear();
}
#endif

// really?
bool SchematicModel::giveNodeNames(DocumentStream& stream, int& countInit,
		QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		bool creatingLib, NetLang const& nl)
{
	incomplete(); // BUG. called from Sckt:tAC
	(void) nl;
	(void) Collect;
	(void) ErrText;
	(void) countInit;
	(void) creatingLib;
	(void) NumPorts;
	(void) stream;
#if 0
	bool isAnalog=true;
	// delete the node names
	for(auto pn : nodes()) {
		pn->State = 0;
		if(pn->Label) {
			if(isAnalog)
				pn->setName(pn->Label->name());
			else
				pn->setName("net" + pn->Label->name());
		}
		else pn->setName("");
	}

	// set the wire names to the connected node
	for(auto pw : wires()){
		if(pw->Label != 0) {
			if(isAnalog)
				pw->portValue(0)->setName(pw->Label->name());
			else  // avoid to use reserved VHDL words
				pw->portValue(0)->setName("net" + pw->Label->name());
		}
	}

	// go through components
	// BUG: ejects declarations
	if(!throughAllComps(stream, countInit, Collect, ErrText, NumPorts, creatingLib, nl)){
		fprintf(stderr, "Error: Could not go throughAllComps\n");
		return false;
	}

	// work on named nodes first in order to preserve the user given names
	throughAllNodes(true, Collect, countInit);

	// give names to the remaining (unnamed) nodes
	throughAllNodes(false, Collect, countInit);

	if(!isAnalog) // collect all node names for VHDL signal declaration
		collectDigitalSignals();

#endif
	return true;
}

bool SchematicModel::loadDocument(QFile& /*BUG*/ file)
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
    throw "incomplete_exception";
  }

  Line = Line.mid(16, Line.length()-17);

  parse(stream);
  file.close();
  return true;
}

// called from PushBack...
void SchematicModel::simpleInsertComponent(Component *c)
{
#if 0
	Node *pn;
	// connect every node of component
	for(auto pp : c->Ports){
		int x=pp->x+c->cx_();
		int y=pp->y+c->cy_();

		// check if new node lies upon existing node
		// creates a new node, if needed
		pn = &nodes().at(x, y);
		pn->appendConnection(c);  // connect schematic node to component node

		if (!pp->Type.isEmpty()) {
			//      pn->DType = pp->Type;
		}

		pp->connect(pn);  // connect component node to schematic node
	}

#endif
	assert(c);

	connect(c);
	components().append(c);
}

// screw this.
void SchematicModel::simpleInsertWire(Wire *pw)
{
  Node *pn=nullptr;
  // pn = &nodes().at(pw->x1_(), pw->y1_());

  if(pw->x1_() == pw->x2_() && pw->y1_() == pw->y2_()) {
#if 0
    pn->Label = pw->Label;   // wire with length zero are just node labels
    if (pn->Label) { untested();
      pn->Label->Type = isNodeLabel;
      pn->Label->pOwner = pn;
    }else{ untested();
	 }
#endif

	 incomplete();
#if 0 // gaaah
    delete pw;           // delete wire because this is not a wire
    return;
#endif
  }else{ untested();
  }

#if 1

  // possibly do all ports in single call?
  connect(pw);
//  Conductor* n0 = pw->connectNode(0, nodes());
//  Conductor* n1 = pw->connectNode(1, nodes());


#else
  Node* p2 = &nodes().at(pw->x2_(), pw->y2_());

  _cc.preAddEdge(_w, c1);

  pn->connectionsAppend(pw);
  pw->setPortByIndex(0, pn);

  _cc.preAddEdge(_w, c2);

  p2->connectionsAppend(pw);  // connect schematic node to component node
  pw->setPortByIndex(1, p2);
#endif

  wires().append(pw);
}

// obsolete?
void SchematicModel::detachFromNode(Element* what, Node* from)
{
	unreachable();
	if(from->connectionsCount()==1){
#if 0
		if(from->Label){
			incomplete();
			delete from->Label;
		}else{
		}
#endif

		incomplete(); // need a proper nodemap
		// nodes().removeRef(from);  // delete open nodes
	}else if(from->connectionsCount()==3){
		from->connectionsRemove(what);// delete connection
		//			pn->disconnect(c);
		//
		//			// BUG //
		//			must be undoable
		oneTwoWires(from);  // two wires -> one wire
	}else{
		from->connectionsRemove(what);// remove connection
		//			pn->disconnect(c);
	}
}


void SchematicModel::disconnect(Symbol* c)
{
	// drop port connections
	for(unsigned i=0; i<c->portCount(); ++i) {
		trace1("sm:ds", i);
		Node* nn = c->disconnectNode(i, nodes());
		assert(nn);

		if(!nn){
			unreachable();
		}else if(nn->connectionsCount()==0){
			nodes().erase(nn); // possibly garbage collect only.
		}else if(nn->connectionsCount()==2){ untested();
			oneTwoWires(nn);  // two wires -> one wire
		}else{
		}
	}
}

void SchematicModel::connect(Symbol* c)
{
// 	if(Conductor* cw = dynamic_cast<Wire*>(c)){
// 		Nodes.registerVertex(cw);
// 		assert(2==c->portCount());
// 	}else{
// 	}

	for(unsigned i=0; i<c->portCount(); ++i){
		c->connectNode(i, nodes()); // use scope.
		assert(dynamic_cast<Symbol const*>(c)->port(i).connected());
	}
}

// obsolete. probably.
void SchematicModel::updateNetLabels() const
{ untested();
	incomplete();
#if 0

	for(auto pc : components()){
		if(pc->type() == "GND") { // BUG, use rails with net names.
			Port* n = pc->Ports.first();
			assert(n);
			assert(n->connected());
			Net* net = n->value()->getNet();
			assert(net);

			if (net->label().size()){
				qDebug() << "GND: warning: overriding label" << net->label();
			}else{
			}
			net->setLabel("0");
		}
	}
#endif
}

ModelAccess::ModelAccess(){}
//
// needed?
void SchematicModel::merge(SchematicModel& src)
{
  for(auto i : src.components()){ itested();
	  components().append(i);
  }
  src.components().clear();
}

#if 0
Net* SchematicModel::newNet()
{
	return Nets.newNet();
}
void SchematicModel::delNet(Net* n)
{
	Nets.delNet(n);
}
#endif

