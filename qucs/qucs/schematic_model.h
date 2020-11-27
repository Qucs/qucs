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
# include <QGraphicsScene>
#endif
# include <QFileInfo> // BUG

#include "object.h"
//#include "node.h"
#include "io.h"
#include "schematic_scene.h"
#include "schematic_symbol.h"
#include "qucsdoc.h"
#include "nodemap.h"
#include "netlist.h"
#include "protomap.h"
#include "sim/tasks.h"
/*--------------------------------------------------------------------------*/
class Node;
class SchematicDoc;
class QPlainTextEdit; //??!
class TaskElement;
class PARAM_LIST;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// TODO: refactor here
class WireList : public Q3PtrList<Symbol> {
};
class NodeMap;
// TODO: refactor here
class DiagramList : public Q3PtrList<Diagram> {
};
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
	void removeRef(Symbol* s) { erase(std::find(begin(), end(), s)); }
public:
	const_iterator begin() const{return container::begin();}
	const_iterator end() const{return container::end();}
};
/*--------------------------------------------------------------------------*/
// TODO: use generic list.
class PaintingList : public Q3PtrList<Painting> {
public:
	void sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax) const;
};
/*--------------------------------------------------------------------------*/
// Base class for all schematic models.
// currently containging chunks/cruft from legacy Schematic implementation
// reminiscent of "subckt", "cardlist" ...
// TODO: rename to ElementList or CircuitModel, SymbolModel.
class SchematicModel{
public: // stub
	typedef ElementList::iterator iterator;
	typedef ElementList::const_iterator const_iterator;
private:
	SchematicModel(SchematicModel const&) = delete;
	SchematicModel();
#if 0
	SchematicModel() : _doc(nullptr),
	_symbol(new SchematicSymbol());
	{
		incomplete();
	}
#endif
public:
	SchematicModel(SchematicDoc* s);
	~SchematicModel();
public: // stuff saved from Schematic
	void sizeOfAll(int&, int&, int&, int&, float) const;
	//void simpleInsertComponent(Component* c);
	//void simpleInserttaskElement(TaskElement* c);
	//void simpleInsertWire(Wire*);
	void recreateSymbol(Symbol* s); // yikes.
	//private??
//	bool giveNodeNames(DocumentStream&, int&, QStringList&, QPlainTextEdit*, int,
//			bool creatingLib, NetLang const&);
	bool throughAllComps(DocumentStream&, int&, QStringList&, QPlainTextEdit *, int,
			bool creatingLib, NetLang const&);
	bool createLibNetlist(DocumentStream&, QPlainTextEdit*, int, NetLang const&);
	bool createSubNetlist(ostream_t&, int&, QStringList&, QPlainTextEdit*, int,
			bool creatingLib, NetLang const&);
	void throughAllNodes(unsigned& z) const;
//	void propagateNode(Node* z) const;

private:
	void detachFromNode(Element* what, Node* from);

public:
	void collectDigitalSignals(void);
//	QString createNetlist(DocumentStream&, int, NetLang const&);
//	void createSubNetlistPlain(ostream_t&, QPlainTextEdit*, int,
//			bool creatingLib, NetLang const&);
	QFileInfo const& getFileInfo ()const;
	void print(QPrinter*, QPainter*, bool, bool);
	void setFileInfo(QString FileName) { FileInfo = QFileInfo(FileName); }
	void setDevType(QString const& type); // BUG. move to parent.
	QString const& devType() const;

public:
	void setOwner(Element* s);
	int  prepareNetlist(DocumentStream&, QStringList&, QPlainTextEdit*,
			bool creatingLib, NetLang const&);
	// Component* loadComponent(const QString& _s, Component* c) const;
	TaskElement* loadtaskElement(const QString& _s, TaskElement* c) const;
//	bool loadPaintings(QTextStream*, PaintingList* p=NULL);
	bool loadProperties(QTextStream*);
	// bool loadComponents(QTextStream*);

public: // not sure. leaves unconnected objects in the main container...
	void connect(Symbol* c);
	void disconnect(Symbol* c);

public:
	Element* detach(Element* c);
	Element* attach(Element* c);

public: // container
	void clear();
	void pushBack(Element* what);
	void erase(Element* what);
	size_t size() const{ return components().size(); }

private: // used in erase?
//	void       deleteComp(Component*c);

public: // gone
	size_t wireCount() const{ return wires().size(); }
	size_t nodeCount() const{ return nodes().size(); }
	unsigned numberOfNets() const{ return Nets.size(); }

public:
	size_t numWires() const{ return wires().size(); }
	size_t numNodes() const{ return nodes().size(); }
	unsigned numNets() const{ return Nets.size(); }

public: // why?
	Node* insertNode(int x, int y, Element* owner);
//	void insertSymbolNodes(Symbol *c, bool noOptimize);

private:
//	void removeNode(Node const* n);

public: // scene interaction
	void toScene(QGraphicsScene& s, QList<ElementGraphics*>* l=nullptr) const;

private:
	WireList& wires();
	DiagramList& diagrams();
	PaintingList& paintings();
	ElementList& components(); // possibly "devices". lets see.
	CmdEltList& commands();

public:
	NodeMap& nodes();
	bool isNode(pos_t p) const{
		return nodes().find_at(p);
	}
	Node const* nodeAt(pos_t p) const{
		return nodes().find_at(p);
	}
	WireList const& wires() const;
	NodeMap const& nodes() const;
	DiagramList const& diagrams() const;
	PaintingList const& paintings() const;
	ElementList const& components() const;
	PrototypeMap const& declarations() const;

	Symbol const* findProto(QString const& what) const;
	void cacheProto(Symbol const* what) const;

//	iterator find_(const std::string& short_name)
//					{return find_again(short_name, begin());}
//	iterator find_again(const std::string& short_name, iterator);
  // return a const_iterator
	const_iterator begin()const {return components().begin();}
	const_iterator end()const {return components().end();}
	const_iterator find_again(const std::string& short_name, const_iterator)const;
	const_iterator find_(const std::string& short_name)const
					{return find_again(short_name, begin());}

	SchematicDoc* doc();
	SchematicDoc const* doc() const{
		auto c=const_cast<SchematicModel*>(this);
		return c->doc();
	}
	QString const& portType(int i) const{
		return PortTypes[i];
	}

	unsigned numPorts() const;

	// not so sure about these
	void setPort(unsigned i, Node* n);
	Node const* portValue(unsigned i) const;

private:
	mutable PrototypeMap _protos; // bit of a hack.
	ElementList Components;
	PaintingList Paintings;
	NetList Nets;
	NodeMap Nodes;
	DiagramList Diagrams;
	WireList _wires;
//	SchematicSymbol* _symbol;
	QStringList PortTypes; // obsolete.
	std::vector<Node*> _ports; // -> symbol?
	QFileInfo FileInfo;
	QString DevType; // BUG move to parent

public:
	void attachDoc(SchematicDoc*d){
		assert(!_doc_);
		_doc_ = d;
	}

public:
	// HACK
	unsigned nextIdx(std::string const& s) const;
	PARAM_LIST* params();
	PARAM_LIST const* params() const;

private:
	const SchematicModel* _parent;
	std::multimap<std::string, Element*> _map;
	mutable PARAM_LIST* _params;
	SchematicDoc* _doc_;

public: // for now.
	friend class SchematicDoc;
	friend class NodeMap;
	friend class SchematicEdit;
	friend class SchematicSymbol;
}; // schematicmodel
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
