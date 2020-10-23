/***************************************************************************
    copyright            : 2018, 2020 Felix Salfelder
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
{ untested();
	trace2("::SchematicModel", this, _doc_);
}

SchematicModel::~SchematicModel()
{
}

// getting here in GUI mode
SchematicModel::SchematicModel(SchematicDoc* s)
	: Nodes(Nets),
	  _doc_(s)
{
	trace2("::SchematicModel s", this, _doc_);
	if(s){ untested();
	}else{
	}
	// presumably Q3PTRlist without this is just a QList<*> (check)
//  _symbol=new SchematicSymbol();
}

void SchematicModel::clear()
{itested();
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
{ untested();
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
	ins(SchematicModel* m) : _m(m) { untested();
		assert(false); // still used?
		_subckt=m;
		assert(m);
	}
	~ins(){ untested();
	}
private: // ModelInserter
	void pushBack(Element* e){ untested();
		if(auto s=dynamic_cast<SchematicSymbol*>(e)){ untested();
			(void) s;
			incomplete();
		//delete _symbol;
		//_symbol = s;
		}else{ untested();
			_m->pushBack(e);
		}
	}
	void setParameter(std::string const&, std::string){ untested();
		incomplete();
	}
	PaintingList& symbolPaintings(){ untested();
		return _dummy;
	}

private: // internal. Portstuff
  unsigned numPorts() const {unreachable(); return 0;}
  Net const* portValue(unsigned) const{ unreachable(); return nullptr; } // BUG
  void setPort(unsigned, Node*){ unreachable(); }
  Port& port(unsigned){ throw Exception(); }

private: // SchematicSymbol
	SchematicModel const& schematicModel() const{ untested();
		assert(_m);
		return *_m;
	}
	SchematicModel* schematicModel() { untested();
		assert(_m);
		return _m;
	}
	std::string getParameter(std::string const&) const{ untested();
		incomplete();
		return "incomplete getParameter";
	}

private:
	SchematicModel* _m;
	PaintingList _dummy; // ignore those paintings.
};
}

// BUG: not here.

/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ComponentList& SchematicModel::components()
{
	return Components;
}

// same as attach??
void SchematicModel::pushBack(Element* what)
{
	trace2("SchematicModel::pushBack", what->label(), this);
	if(auto c=component(what)){
		connect(c);
		components().push_back(c);
	}else if(prechecked_cast<Conductor*>(element(what))){
		auto s=dynamic_cast<Symbol*>(what);
	  connect(s);
	  // why not components??
	  wires().append(s);
	}else if(auto d=diagram(what)){
		diagrams().append(d);
	}else if(auto c=command(what)){
		//trace1("SchematicModel::pushBack", owner());
		if(doc()){
			doc()->commands().push_back(c);
		}else{
			// possibly a subcircuit model? ignore commands.
		}
	}else if(auto s=dynamic_cast<Symbol*>(what)){
		connect(s);
		components().push_back(s);
	}else if(auto s=dynamic_cast<SchematicSymbol*>(what)){
		trace1("hmm", s->label());
		assert(false);
		//delete _symbol;
		//_symbol = s;
	}else if(auto d=painting(what)){
		/// BUG BUG BUG. some are "symbolpaints", some are just paintings. the
		//Object should decide.
		paintings().append(d);
	}else{
		incomplete();
	}

  if(doc()){itested();
	  doc()->addToScene(what);
  }else{
  }
} // pushBack

// was Schematic::insertComponentNodes.
void SchematicModel::insertSymbolNodes(Symbol *c, bool noOptimize)
{ untested();
	// connect every node of the component to corresponding schematic node
	for(unsigned i=0; i<c->numPorts(); ++i){ untested();
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
	{ untested();
		pp = iport.next();
		pn = pp->Connection;
		for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next()){ untested();
			if(pe->Type == isWire)
			{ untested();
				if(((Wire*)pe)->portValue(0) == pn)  pL = &(((Wire*)pe)->Port2->Connections);
				else  pL = &(((Wire*)pe)->portValue(0)->Connections);

				for(pe1 = pL->first(); pe1!=0; pe1 = pL->next())
					if(pe1 == c)
					{ untested();
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
{itested();
	if(auto c=component(what)){itested();
		disconnect(c);
		components().removeRef(c);
	}else if(auto d=diagram(what)){ untested();
		diagrams().removeRef(d);
	}else if(prechecked_cast<Conductor*>(element(what))){
		auto s=dynamic_cast<Symbol*>(what);
		disconnect(s);
		wires().removeRef(s);
	}else if(auto w=dynamic_cast<Symbol*>(what)){ untested();
		incomplete();
	}else{ untested();
		unreachable();
	}
	return what;
}
// TODO: take iterator.
Element* SchematicModel::attach(Element* what)
{itested();
	trace2("SchematicModel::attach", what->label(), this);
	if(auto c=dynamic_cast<TaskElement*>(what)){ untested();
		commands().push_back(c);
	}else if(auto c=component(what)){itested();
		// really?
		connect(c);
		components().append(c);
	}else if(auto d=diagram(what)){ untested();
		diagrams().append(d);
	}else if(prechecked_cast<Conductor*>(element(what))){
		auto s=dynamic_cast<Symbol*>(what);
		connect(s);
		wires().append(s);
	}else if(auto w=dynamic_cast<Symbol*>(what)){ untested();
		incomplete();
		//connect(c);
		//components().append(c);
	}else{ untested();
		incomplete();
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
//{ untested();
//	return SymbolPaints;
//}

PaintingList& SchematicModel::paintings()
{itested();
	return Paintings;
}
//
//PaintingList& SchematicModel::symbolPaintings()
//{ untested();
//	assert(_symbol);
//	// temporary. move stuff here....
//	return _symbol->symbolPaintings();
//}
//
CmdEltList& SchematicModel::commands()
{ untested();
	assert(doc());
	return doc()->commands();
}
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
//{ untested();
//	return SymbolPaintings;
//}

#if 0 // TODO: what is this? (perhaps DocumentFormat?)
static void createNodeSet(QStringList& Collect, int& countInit,
		Conductor *pw, Node *p1)
{ untested();
	if(pw->Label)
		if(!pw->Label->initValue.isEmpty())
			Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
					p1->name() + " U=\"" + pw->Label->initValue + "\"");
}
#endif

#if 0
bool SchematicModel::throughAllComps(DocumentStream& stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		   bool creatingLib, NetLang const& nl)
{ untested();
	bool r;
	QString s;
	bool isAnalog=true;
	bool isVerilog=false;

	// give the ground nodes the name "gnd", and insert subcircuits etc.
	for(auto pc : components()) { untested();
	}
}
#endif

// find connected components (slow)
// obsolete.
void SchematicModel::throughAllNodes(unsigned&) const
{
#if 0
  z = 0; // number cc.

  for(auto pn : nodes()){ untested();
    pn->resetNetNumber();
  }

  for(auto pn : nodes()){ untested();
    if(pn->hasNetNumber()){ untested();
    }else{ untested();
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
{ untested();
	bool isAnalog=true;
	bool setName=false;
	Q3PtrList<Node> Cons;
	Node *p2;
	Wire *pw;
	Element *pe;

	Cons.append(pn);
	for(p2 = Cons.first(); p2 != 0; p2 = Cons.next()){ untested();
		for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
			if(wire(pe)){ untested();
				pw = (Wire*)pe;
				if(p2 != pw->portValue(0)) { untested();
					if(pw->portValue(0)->name().isEmpty()) { untested();
						pw->portValue(0)->setName(pn->name());
						pw->portValue(0)->State = 1;
						Cons.append(pw->portValue(0));
						setName = true;
					}
				}else{ untested();
					if(pw->Port2->name().isEmpty()) { untested();
						assert(pn);
						pw->Port2->setName(pn->name());
						pw->Port2->State = 1;
						Cons.append(pw->Port2);
						setName = true;
					}
				}
				if(setName) { untested();
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
{ untested();
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
	for(auto pn : nodes()) { untested();
		pn->State = 0;
		if(pn->Label) { untested();
			if(isAnalog)
				pn->setName(pn->Label->name());
			else
				pn->setName("net" + pn->Label->name());
		}
		else pn->setName("");
	}

	// set the wire names to the connected node
	for(auto pw : wires()){ untested();
		if(pw->Label != 0) { untested();
			if(isAnalog)
				pw->portValue(0)->setName(pw->Label->name());
			else  // avoid to use reserved VHDL words
				pw->portValue(0)->setName("net" + pw->Label->name());
		}
	}

	// go through components
	// BUG: ejects declarations
	if(!throughAllComps(stream, countInit, Collect, ErrText, NumPorts, creatingLib, nl)){ untested();
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


// called from PushBack...
#if 0
void SchematicModel::simpleInsertComponent(Component *c)
{
	assert(c);

	connect(c);
	components().append(c);
}
#endif

#if 0 // screw this.
void SchematicModel::simpleInsertWire(Wire *)
{
//  Node *pn=nullptr;
  // pn = &nodes().at(pw->x1_(), pw->y1_());
  //
  incomplete(); // but not here.
  some label crazyness. propagate labels to nets in wire::connect.
    pn->Label = pw->Label;   // wire with length zero are just node labels
    if (pn->Label) { untested();
      pn->Label->Type = isNodeLabel;
      pn->Label->pOwner = pn;
    }else{ untested();
	 }
}
#endif

// obsolete?
void SchematicModel::detachFromNode(Element* what, Node* from)
{ untested();
	unreachable();
#if 0
	if(from->degree()==1){ untested();
#if 0
		if(from->Label){ untested();
			incomplete();
			delete from->Label;
		}else{ untested();
		}
#endif

		incomplete(); // need a proper nodemap
		// nodes().removeRef(from);  // delete open nodes
	}else if(from->degree()==3){ untested();
		from->connectionsRemove(what);// delete connection
		//			pn->disconnect(c);
		//
		//			// BUG //
		//			must be undoable
		oneTwoWires(from);  // two wires -> one wire
	}else{ untested();
		from->connectionsRemove(what);// remove connection
		//			pn->disconnect(c);
	}
#endif
}


void SchematicModel::disconnect(Symbol* c)
{itested();
	// drop port connections
	for(unsigned i=0; i<c->numPorts(); ++i) {itested();
		trace2("sm:ds", i, c->portPosition(i));
		Node* nn = c->disconnectNode(i, nodes());
		assert(nn);

		if(!nn){ untested();
			unreachable();
		}else if(!nn->hasPorts()){
			nodes().erase(nn); // possibly garbage collect only.
		}else{
		}
	}
}

void SchematicModel::connect(Symbol* c)
{
	for(unsigned i=0; i<c->numPorts(); ++i){
		c->connectNode(i, nodes()); // use scope.
//		assert(dynamic_cast<Symbol const*>(c)->port(i).connected());
	}
}

unsigned SchematicModel::numPorts() const
{
	assert(this);
	trace1("SchematicModel::numPorts", this);
	// incomplete
	return _ports.size();
}

void SchematicModel::setPort(unsigned i, Node* n)
{
	trace2("setPort", i, _ports.size());
	_ports.resize(std::max(_ports.size(), size_t(i)+1));
	_ports[i] = n;
}

Net const* SchematicModel::portValue(unsigned i) const
{
	assert(i<numPorts());
	if(_ports[i]){
		return _ports[i]->net();
	}else{ untested();
		return nullptr;
	}
}

void SchematicModel::setOwner(Element* o)
{ untested();
	for(auto pc : components()){ untested();
		assert(pc);
		trace3("set_owner", pc->label(), pc, o);
		pc->setOwner(o);
		const Symbol* sym = pc;
		assert(sym->owner() == o);
	}
}

#if 0
// needed?
void SchematicModel::merge(SchematicModel& src)
{ untested();
	assert(false);
  for(auto i : src.components()){ itested();
	  assert(i);
	  components().append(i);
  }
  src.components().clear();
}
#endif

Symbol const* SchematicModel::findProto(QString const& what) const
{
	return _protos[what];
}

PrototypeMap const& SchematicModel::declarations() const
{
	return _protos;
}

void SchematicModel::cacheProto(Symbol const* what) const
{
	auto key = what->label();
	trace1("pushProto", key);
	assert(!_protos[key]);
	_protos.push(key, what);
}

Symbol const* PrototypeMap::operator[](QString const& s) const
{ untested();
	auto i=_map.find(s);

	if(i!=_map.end()){ untested();
		return (*i).second;
	}else{ untested();
		return nullptr;
	}
}

void PrototypeMap::clear()
{
	for(auto i: _map){
		// no, we don't own them
		// delete i.second;
	}
	_map.clear();
}
/*--------------------------------------------------------------------------*/
PrototypeMap::PrototypeMap()
{
}
/*--------------------------------------------------------------------------*/
// Inserts a port into the schematic and connects it to another node if
// the coordinates are identical. The node is returned.
Node* SchematicModel::insertNode(int , int , Element *)
{   untested();
    unreachable();

#if 0
    Node *pn;
    pn = &nodes().at(x,y);
    pn->connectionsAppend(e);

    // check if the new node lies within an existing wire
    //
    if(pn->degree()==1){
	// BUG. the wire is connected. just use it.
	for(auto pw : wires()) {
	    if(pw->x1_() == x) {
		if(pw->y1_() > y) continue;
		if(pw->y2_() < y) continue;
	    } else if(pw->y1_() == y) {
		if(pw->x1_() > x) continue;
		if(pw->x2_() < x) continue;
	    }else{
		continue;
	    }

	    // split the wire into two wires
	    splitWire(pw, pn);
	    break;
	}
    }else{
    }
    return pn;
#endif
    return nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
