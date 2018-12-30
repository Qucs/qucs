/***************************************************************************
                              schematic_model.h
                             --------------------
    copyright            : (C) 2018 Felix Salfelder
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

#include <QGraphicsScene>
#include <QFileInfo> // BUG
#include "object.h"
#include "wire.h"
#include "node.h"
#include "io.h"
#include "schematic_scene.h"
#include "schematic_symbol.h"
#include "components/subcircuit.h" // BUG
#include "qucsdoc.h"

class QPlainTextEdit; //??!

// TODO: refactor here
class WireList : public Q3PtrList<Wire> {
};
// TODO: refactor here
class NodeList : public Q3PtrList<Node> {
};
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
	bool load(QTextStream *stream);
public:
	void sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax) const;
};

// hmm, which?
class DocumentLanguage;
class SchematicLanguage;

class SchematicModel{
private:
  SchematicModel() = delete;
#if 0
  SchematicModel() : _doc(nullptr),
	  _symbol(new SchematicSymbol());
	{
		incomplete();
	}
#endif
public:
  SchematicModel(Schematic* s);
public: // stuff saved from Schematic
  QString createClipboardFile();
  void sizeOfAll(int&, int&, int&, int&, float) const;
  void simpleInsertComponent(Component* c);
  void simpleInsertCommand(Command* c);
  void simpleInsertWire(Wire*);
  //private??
  bool giveNodeNames(DocumentStream&, int&, QStringList&, QPlainTextEdit*, int,
		  bool creatingLib, NetLang const&);
  bool throughAllComps(DocumentStream&, int&, QStringList&, QPlainTextEdit *, int,
		  bool creatingLib, NetLang const&);
  bool createLibNetlist(DocumentStream&, QPlainTextEdit*, int, NetLang const&);
  bool createSubNetlist(DocumentStream&, int&, QStringList&, QPlainTextEdit*, int,
		  bool creatingLib, NetLang const&);
  void throughAllNodes(bool, QStringList&, int&);
  void propagateNode(QStringList&, int&, Node*);
  void collectDigitalSignals(void);
  QString createNetlist(DocumentStream&, int, NetLang const&);
  void createSubNetlistPlain(DocumentStream&, QPlainTextEdit*, int,
		  bool creatingLib
		  );
  QFileInfo const& getFileInfo ()const;
  void print(QPrinter*, QPainter*, bool, bool);
  void setFileInfo(QString FileName) { FileInfo = QFileInfo(FileName); }
  void setDevType(QString const& type); // BUG. move to parent.
  QString const& devType() const;

public:
  void parse(DocumentStream& stream, SchematicLanguage const*l=nullptr);
  int  prepareNetlist(DocumentStream&, QStringList&, QPlainTextEdit*,
		  bool creatingLib, NetLang const&);
  Component* loadComponent(const QString& _s, Component* c) const;
  Command* loadCommand(const QString& _s, Command* c) const;
  bool loadDocument(QFile& /*BUG*/ file);
  bool loadPaintings(QTextStream*, PaintingList* p=NULL);
  bool loadProperties(QTextStream*);
  bool loadComponents(QTextStream*);
  // bool loadDiagrams(QTextStream*);
  bool loadWires(QTextStream*);

  void clear();
  void pushBack(Element* what);
  void erase(Element* what);
public:
  void merge(SchematicModel&);

public: // scene interaction
  void toScene(QGraphicsScene& s, QList<ElementGraphics*>* l=nullptr) const;
public: // obsolete.
  static void saveComponent(QTextStream& s, Component const* c);
private: // TODO: actually store here.
  WireList& wires();
  NodeList& nodes();
  DiagramList& diagrams();
  PaintingList& paintings();
  ComponentList& components();
  PaintingList& symbolPaintings();
public:
  WireList const& wires() const;
  NodeList const& nodes() const;
  DiagramList const& diagrams() const;
  PaintingList const& paintings() const;
  ComponentList const& components() const;

  Schematic* doc();
  QString const& portType(int i) const{
	  return PortTypes[i];
  }
private: // TODO: remove. store parent in ElementGraphics.
  Schematic* const _doc;
private:
  ComponentList Components;
  NodeList Nodes;
  DiagramList Diagrams;
  WireList Wires;
  SchematicSymbol* _symbol;
  QStringList PortTypes;
  QFileInfo FileInfo;
  QString DevType; // BUG move to parent
public: // for now.
  friend class Schematic;
};

#endif
