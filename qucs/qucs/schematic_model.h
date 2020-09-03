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
#include "wire.h"
#include "node.h"
#include "io.h"
#include "schematic_scene.h"
#include "schematic_symbol.h"
#include "qucsdoc.h"
#include "nodemap.h"
#include "netlist.h"
#include "protomap.h"
#include "sim/tasks.h"


class SchematicDoc;
class QPlainTextEdit; //??!
// class QFileInfo;
//

// TODO: refactor here
class WireList : public Q3PtrList<Wire> {
};
class NodeMap;
// TODO: refactor here
class DiagramList : public Q3PtrList<Diagram> {
};
// TODO: refactor here
class ComponentList : public Q3PtrList<Component> {
	// void first(){} // GOAL: hide, still compile.
};
// TODO: refactor here
class PaintingList : public Q3PtrList<Painting> {
public:
	bool load(QTextStream& stream);
public:
	void sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax) const;
};

// hmm, which?
class DocumentLanguage;
class SchematicLanguage;


// Base class for all schematic models.
// currently containging chunks/cruft from legacy Schematic implementation
// reminiscent of "subckt", "cardlist" ...
class SchematicModel{
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
	QString createClipboardFile();
	void sizeOfAll(int&, int&, int&, int&, float) const;
	void simpleInsertComponent(Component* c);
	void simpleInsertCommand(Command* c);
	void simpleInsertWire(Wire*);
	void recreateSymbol(Symbol* s); // yikes.
	//private??
	bool giveNodeNames(DocumentStream&, int&, QStringList&, QPlainTextEdit*, int,
			bool creatingLib, NetLang const&);
	bool throughAllComps(DocumentStream&, int&, QStringList&, QPlainTextEdit *, int,
			bool creatingLib, NetLang const&);
	bool createLibNetlist(DocumentStream&, QPlainTextEdit*, int, NetLang const&);
	bool createSubNetlist(stream_t&, int&, QStringList&, QPlainTextEdit*, int,
			bool creatingLib, NetLang const&);
	void throughAllNodes(unsigned& z) const;
	void propagateNode(Node* z) const;
//	void updateNetLabels() const;

private:
	void detachFromNode(Element* what, Node* from);

public:
	void collectDigitalSignals(void);
//	QString createNetlist(DocumentStream&, int, NetLang const&);
	void createSubNetlistPlain(stream_t&, QPlainTextEdit*, int,
			bool creatingLib, NetLang const&);
	QFileInfo const& getFileInfo ()const;
	void print(QPrinter*, QPainter*, bool, bool);
	void setFileInfo(QString FileName) { FileInfo = QFileInfo(FileName); }
	void setDevType(QString const& type); // BUG. move to parent.
	QString const& devType() const;

public:
	void setOwner(Element* s);
	int  prepareNetlist(DocumentStream&, QStringList&, QPlainTextEdit*,
			bool creatingLib, NetLang const&);
	Component* loadComponent(const QString& _s, Component* c) const;
	Command* loadCommand(const QString& _s, Command* c) const;
//	bool loadPaintings(QTextStream*, PaintingList* p=NULL);
	bool loadProperties(QTextStream*);
	bool loadComponents(QTextStream*);

public: // not sure. leaves unconnected objects in the main container...
	void connect(Symbol* c);
	void disconnect(Symbol* c);

public:
	// TODO: take iterator.
	Element* detach(Element* c);
	Element* attach(Element* c);

public: // container
	void clear();
	void pushBack(Element* what);
	void erase(Element* what);
//	void merge(SchematicModel&);

private: // used in erase?
	void       deleteComp(Component*c);

public: // node stuff. why public?
	size_t wireCount(){ return wires().size(); }
	size_t nodeCount(){ return nodes().size(); }
	Node* insertNode(int x, int y, Element* owner);
	void insertSymbolNodes(Symbol *c, bool noOptimize);
	bool  oneTwoWires(Node* n);
	Wire* splitWire(Wire*, Node*);
	int   insertWireNode1(Wire* w);
	bool  connectHWires1(Wire* w);
	bool  connectVWires1(Wire* w);
	int   insertWireNode2(Wire* w);
	bool  connectHWires2(Wire* w);
	bool  connectVWires2(Wire* w);
	int   insertWire(Wire* w); // BUG, what is simpleInsertWire?
	void  deleteWire(Wire*);

private:
	void removeNode(Node const* n);

public: // scene interaction
	void toScene(QGraphicsScene& s, QList<ElementGraphics*>* l=nullptr) const;
public: // obsolete.
	static void saveComponent(QTextStream& s, Component const* c);
private:
	WireList& wires();
	NodeMap& nodes();
	DiagramList& diagrams();
	PaintingList& paintings();
	ComponentList& components(); // possibly "devices". lets see.
//	CommandList& commands(); // really??
public:
	bool isNode(int x, int y) const{
		return nodes().find_at(x,y);
	}
	WireList const& wires() const;
	NodeMap const& nodes() const;
	DiagramList const& diagrams() const;
	PaintingList const& paintings() const;
	ComponentList const& components() const;
	PrototypeMap const& declarations() const;

	Symbol const* findProto(QString const& what) const;
	void cacheProto(Symbol const* what) const;

	SchematicDoc* doc();
	QString const& portType(int i) const{
		return PortTypes[i];
	}
	unsigned numberOfNets() const{ // numNets
		return Nets.size();
	}

	unsigned numPorts() const;

	// not so sure about these
	void setPort(unsigned i, Node* n);
	Net const* portValue(unsigned i) const;

private:
	mutable PrototypeMap _protos; // bit of a hack.
	ComponentList Components;
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

private:
	SchematicDoc* _doc_;

public: // for now.
	friend class SchematicDoc;
	friend class NodeMap;
	friend class SchematicSymbol;
}; // schematicmodel

#endif
