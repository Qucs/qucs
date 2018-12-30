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

SchematicModel::SchematicModel(Schematic* s)
  :_doc(s)
{
	// presumably Q3PTRlist without this is just a QList<*> (check)
  _symbol=new SchematicSymbol();
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

void SchematicModel::parse(DocumentStream& s, SchematicLanguage const* L)
{
	assert(L);
	while(!s.atEnd()){ untested();
		qDebug() << "entering parse";
		L->parse(s, this);
		assert(s.atEnd()); // happens with legacy lang
	}
}



/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ComponentList& SchematicModel::components()
{
	// temporary. move stuff here....
	return _doc->components();
}

WireList& SchematicModel::wires()
{
	// temporary. move stuff here....
	return _doc->wires();
}

NodeList& SchematicModel::nodes()
{
	// temporary. move stuff here....
	return _doc->nodes();
}

PaintingList& SchematicModel::paintings()
{
	// temporary. move stuff here....
	return _doc->paintings();
}

DiagramList& SchematicModel::diagrams()
{
	// temporary. move stuff here....
	return _doc->diagrams();
}

// same, but const.
ComponentList const& SchematicModel::components() const
{
	// temporary. move stuff here....
	return _doc->components();
}

WireList const& SchematicModel::wires() const
{
	// temporary. move stuff here....
	return _doc->wires();
}

NodeList const& SchematicModel::nodes() const
{
	// temporary. move stuff here....
	return _doc->nodes();
}

PaintingList const& SchematicModel::paintings() const
{
	// temporary. move stuff here....
	return _doc->paintings();
}

DiagramList const& SchematicModel::diagrams() const
{
	// temporary. move stuff here....
	return _doc->diagrams();
}

static void createNodeSet(QStringList& Collect, int& countInit,
			      Conductor *pw, Node *p1)
{
  if(pw->Label)
    if(!pw->Label->initValue.isEmpty())
      Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
                     p1->name() + " U=\"" + pw->Label->initValue + "\"");
}

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

// really?
bool SchematicModel::giveNodeNames(DocumentStream& stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		  bool creatingLib, NetLang const& nl)
{
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
				pw->Port1->setName(pw->Label->Name);
			else  // avoid to use reserved VHDL words
				pw->Port1->setName("net" + pw->Label->Name);
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

	return true;
}
