/***************************************************************************
    copyright            : (C) 2018-2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_SCHEMATIC_MODEL_H
#define QUCS_SCHEMATIC_MODEL_H

#ifndef UNTANGLE_QT // later.
//# include <QGraphicsScene>
//# include <QFileInfo> // BUG
#endif

#include "object.h"
//#include "node.h"
#include "io.h"
#include "schematic_scene.h"
#include "qucsdoc.h"
#include "nodemap.h"
#include "netlist.h"
#include "protomap.h"
#include "tasks.h"
#include "element.h"
#include "symbol.h"
/*--------------------------------------------------------------------------*/
class Node;
class SchematicDoc;
class QPlainTextEdit; //??!
class QFileInfo; // BUG
class TaskElement;
class PARAM_LIST;
class NetLang;
/*--------------------------------------------------------------------------*/
class WireList;
class NodeMap;
class CmdEltList;
class ElementList;
class Diagram;
class Painting;
/*--------------------------------------------------------------------------*/
class NodeMap;
#if 0
// TODO: refactor here
class ElementList : public std::list<Element*> {
public:
	typedef std::list<Element*> container;
	typedef std::list<Element*>::const_iterator const_iterator;
private:
	ElementList(ElementList const&) = delete;
public:
	explicit ElementList() {}
	~ElementList() {
		while(!empty()){
			delete(front());
			pop_front();
		}
	}

public:
	void pushBack(Element*s) {push_back(s);}
	void append(Element*s) {push_back(s);}
	bool isEmpty() const { return empty(); }
public:
	const_iterator begin() const{return container::begin();}
	const_iterator end() const{return container::end();}
};
#endif
/*--------------------------------------------------------------------------*/
// TODO: rename to ObjectList
class SchematicModel{
public: // stub
	typedef std::list<Element*> ElementList;
	typedef ElementList::iterator iterator;
	typedef ElementList::const_iterator const_iterator;
private:
	SchematicModel(SchematicModel const&) = delete;
public:
	explicit SchematicModel();
	~SchematicModel();
public: // stuff saved from Schematic
	void sizeOfAll(int&, int&, int&, int&, float) const;

private:
	void detachFromNode(Element* what, Node* from);
	void removeRef(Element* s) { erase(std::find(begin(), end(), s)); }
	void erase(const_iterator what){Components.erase(what);}

public:
	void collectDigitalSignals(void);
	void print(QPrinter*, QPainter*, bool, bool);
	void setDevType(QString const& type); // BUG. move to parent.
	QString const& devType() const;

public:
	void setOwner(Element* s);
	int  prepareNetlist(ostream_t&, QStringList&, QPlainTextEdit*,
			bool creatingLib, NetLang const&);
	TaskElement* loadtaskElement(const QString& _s, TaskElement* c) const;
	bool loadProperties(QTextStream*);

public: // not sure. leaves unconnected objects in the main container...
	void connect(Symbol* c);
	void disconnect(Symbol* c);

public:
	Element* detach(Element* c);

public: // container
	void clear();
	void push_back(Element* what);
	void pushBack(Element* what);
	void erase(Element* what);
	size_t size() const{ return Components.size(); }

public: // compat? test? debug?
	size_t nodeCount() const{ return nodes().size(); }
	unsigned numberOfNets() const{ return Nets.size(); }

public:
	size_t numNodes() const{ return nodes().size(); }
	unsigned numNets() const{ return Nets.size(); }

public: // why?
	Node* insertNode(int x, int y, Element* owner);

public: // scene interaction
	void toScene(QGraphicsScene& s, QList<ElementGraphics*>* l=nullptr) const;

private:
//	ElementList& components(); // possibly "devices". lets see.
	CmdEltList& commands();

public:
	NodeMap& nodes();
	bool isNode(pos_t p) const{
		return nodes().find_at(p);
	}
	Node const* nodeAt(pos_t p) const{
		return nodes().find_at(p);
	}
//	WireList const& wires() const;
	NodeMap const& nodes() const;
//	ElementList const& components() const;

	Symbol const* findProto(QString const& what) const;
	void cacheProto(Symbol const* what) const;

//	iterator find_(const std::string& short_name)
//					{return find_again(short_name, begin());}
//	iterator find_again(const std::string& short_name, iterator);
  // return a const_iterator
	SchematicModel const* parent() const;
	const_iterator begin()const {return Components.begin();}
	const_iterator end()const {return Components.end();}
	const_iterator find_again(const std::string& short_name, const_iterator)const;
	const_iterator find_(const std::string& short_name)const
					{return find_again(short_name, begin());}

	QString const& portType(int i) const{
		return PortTypes[i];
	}

	unsigned numPorts() const;

	// not so sure about these
	void setPort(unsigned i, Node* n);
	Node const* portValue(unsigned i) const;

private:
	ElementList Components;
	NetList Nets;
	NodeMap Nodes;
//	SchematicSymbol* _symbol;
	QStringList PortTypes; // obsolete.
	std::vector<Node*> _ports; // -> symbol?
//	QFileInfo FileInfo;
	QString DevType; // BUG move to parent

public:
	// HACK
	unsigned nextIdx(std::string const& s) const;
	PARAM_LIST* params();
	PARAM_LIST const* params() const;

private:
	const SchematicModel* _parent;
	std::multimap<std::string, Element*> _map;
	mutable PARAM_LIST* _params;

public:
	void precalc_first();

public: // for now.
//	friend class SchematicDoc;
//	friend class NodeMap;
//	friend class SchematicEdit;
//	friend class SchematicSymbol;
}; // schematicmodel
/*--------------------------------------------------------------------------*/
size_t numWires(SchematicModel const& m);
/*--------------------------------------------------------------------------*/
#endif
