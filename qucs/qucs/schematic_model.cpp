/***************************************************************************
                           schematic_model.cpp
                             ---------------
    begin                : 2018
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

#include "schematic.h"
#include "schematic_lang.h"
#include "globals.h"

SchematicModel::SchematicModel(Schematic* s)
  :_doc(s)
{
	// presumably Q3PTRlist without this is just a QList<*> (check)
//  _symbol=new SchematicSymbol();
}

void SchematicModel::clear()
{
	// memory leak?!
	components().clear();
	nodes().clear();
	diagrams().clear();
	wires().clear();
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
	if(auto c=component(what)){
      simpleInsertComponent(c);
	}else if(auto d=diagram(what)){
		diagrams().append(d);
	}else if(auto w=wire(what)){
		simpleInsertWire(w);
	}else if(auto s=dynamic_cast<SchematicSymbol*>(what)){ untested();
		assert(false);
		//delete _symbol;
		//_symbol = s;
	}else{
		incomplete();
	}

	if(doc()){
		// only necessary when gui is running.
#ifndef USE_SCROLLVIEW
		// doc()->addToScene(what);
#endif
	}else{
	}

}

// called from schematic::erase only
void SchematicModel::erase(Element* what)
{
	if(auto c=component(what)){
		components().removeRef(c);
	}else if(auto d=diagram(what)){
		diagrams().removeRef(d);
	}else if(auto w=wire(what)){
		wires().removeRef(w);
	}else{
		unreachable();
	}
	delete(what);
}

// BUG: use schematic_symbol
Schematic* SchematicModel::doc()
{
	return _doc;
}

QFileInfo const& SchematicModel::getFileInfo ()const
{
	return FileInfo;
}

WireList& SchematicModel::wires()
{
	return Wires;
}

NodeList& SchematicModel::nodes()
{
	return Nodes;
}

PaintingList const& SchematicModel::symbolPaints() const
{
	return SymbolPaints;
}

PaintingList& SchematicModel::paintings()
{
	return Paintings;
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
	return Wires;
}

NodeList const& SchematicModel::nodes() const
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

static void createNodeSet(QStringList& Collect, int& countInit,
			      Conductor *pw, Node *p1)
{
  if(pw->Label)
    if(!pw->Label->initValue.isEmpty())
      Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
                     p1->name() + " U=\"" + pw->Label->initValue + "\"");
}

#if 0
// not here.
void SchematicModel::throughAllNodes(bool User, QStringList& Collect,
				int& countInit)
{
	bool isAnalog=true; //?!
	Node *pn=nullptr;
	int z=0;

	for(pn = nodes().first(); pn != 0; pn = nodes().next()) {
		if(pn->name().isEmpty() == User) {
			continue;  // already named ?
		}
		if(!User) {
			if(isAnalog)
				pn->setName("_net" + QString::number(z++));
			else
				pn->setName("net_net" + QString::number(z++));
		}
		else if(pn->State) {
			continue;  // already worked on
		}

		if(isAnalog){
			createNodeSet(Collect, countInit, pn, pn);
		}else{
		}

		pn->State = 1;
		propagateNode(Collect, countInit, pn);
	}
}
#endif

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
  for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
    for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
      if(pe->Type == isWire) {
	pw = (Wire*)pe;
	if(p2 != pw->Port1) {
	  if(pw->Port1->name().isEmpty()) {
	    pw->Port1->setName(pn->name());
	    pw->Port1->State = 1;
	    Cons.append(pw->Port1);
	    setName = true;
	  }
	}
	else {
	  if(pw->Port2->name().isEmpty()) {
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
  Cons.clear();
}
#endif

// really?
bool SchematicModel::giveNodeNames(DocumentStream& stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		  bool creatingLib, NetLang const& nl)
{
	incomplete(); // BUG. called from Sckt:tAC
	(void) Collect;
	(void) countInit;
	(void) creatingLib;
	(void) NumPorts;
#if 0
	bool isAnalog=true;
	// delete the node names
	for(auto i : nodes()) {
		Node* pn=i;
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
				pw->Port1->setName(pw->Label->name());
			else  // avoid to use reserved VHDL words
				pw->Port1->setName("net" + pw->Label->name());
		}
	}

	// go through components
	// BUG: ejects declarations
	incomplete();
#if 0
	if(!throughAllComps(stream, countInit, Collect, ErrText, NumPorts, nl)){
		fprintf(stderr, "Error: Could not go throughAllComps\n");
		return false;
	}
#endif

	// work on named nodes first in order to preserve the user given names
	throughAllNodes(true, Collect, countInit);

	// give names to the remaining (unnamed) nodes
	throughAllNodes(false, Collect, countInit);

	if(!isAnalog){
		incomplete();
		// collectDigitalSignals();
	}else{
	}

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

void SchematicModel::simpleInsertComponent(Component *c)
{
	Node *pn;
	// connect every node of component
	for(auto pp : c->Ports){
		int x=pp->x+c->cx_();
		int y=pp->y+c->cy_();
		qDebug() << c->label() << "port" << x << y;

		// check if new node lies upon existing node
		for(pn = nodes().first(); pn != 0; pn = nodes().next()){
			if(pn->cx_() == x) if(pn->cy_() == y) {
				// 	if (!pn->DType.isEmpty()) {
				// 	  pp->Type = pn->DType;
				// 	}
				// 	if (!pp->Type.isEmpty()) {
				// 	  pn->DType = pp->Type;
				// 	}
				break;
			}
		}

		if(pn == 0) { // create new node, if no existing one lies at this position
			pn = new Node(x, y);
			nodes().append(pn);
		}
		pn->Connections.append(c);  // connect schematic node to component node
		if (!pp->Type.isEmpty()) {
			//      pn->DType = pp->Type;
		}

		pp->Connection = pn;  // connect component node to schematic node
	}

	components().append(c);
}

void SchematicModel::simpleInsertWire(Wire *pw)
{
  Node *pn=nullptr;

  // find_node_at
  for(auto pn_ : nodes()){
    if(pn_->cx_() == pw->x1_()) {
      if(pn_->cy_() == pw->y1_()) {
	pn = pn_;
	break;
      }
    }
  }

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x1_(), pw->y1_());
    nodes().append(pn);
  }

  if(pw->x1_() == pw->x2_()) if(pw->y1_() == pw->y2_()) {
    pn->Label = pw->Label;   // wire with length zero are just node labels
    if (pn->Label) {
      pn->Label->Type = isNodeLabel;
      pn->Label->pOwner = pn;
    }
#if 0 // gaaah
    delete pw;           // delete wire because this is not a wire
    return;
#endif
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port1 = pn;

  // find_node_at
  pn=nullptr;
  for(auto pn_ : nodes()){
    if(pn_->cx_() == pw->x2_()) {
      if(pn_->cy_() == pw->y2_()) {
	pn = pn_;
	break;
      }
    }
  }

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x2_(), pw->y2_());
    nodes().append(pn);
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port2 = pn;

  wires().append(pw);
}

ModelAccess::ModelAccess(){}
