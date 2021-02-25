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
// #include "qio.h"
//#include "schematic_scene.h"
//#include "qucsdoc.h"
//#include "nodemap.h"
//#include "netlist.h"
//#include "protomap.h"
#include "tasks.h"
#include "element.h"
#include "symbol.h"
#include <vector>
#include <map>
#include <algorithm>
/*--------------------------------------------------------------------------*/
class Node;
class NetList; // BUG
class QPlainTextEdit; //??!
class QGraphicsScene; // bug?
class QFileInfo; // BUG
class QPrinter; // BUG?
class TaskElement;
class ParamList;
class NetLang;
/*--------------------------------------------------------------------------*/
class WireList;
class NodeMap;
class CmdEltList;
class ElementList;
class Diagram;
class Painting;
class ElementGraphics; // BUG
class ostream_t;
/*--------------------------------------------------------------------------*/
class NodeMap;
/*--------------------------------------------------------------------------*/
// TODO: rename to ObjectList
class SchematicModel{
public: // stub
	typedef std::list<Element*> ElementList;
	typedef ElementList::iterator iterator;
	typedef ElementList::const_iterator const_iterator;
  class fat_iterator {
  private:
    SchematicModel* _list;
    iterator   _iter;
  private:
    explicit	  fat_iterator()	{unreachable();}
  public:
		  fat_iterator(const fat_iterator& p)
					: _list(p._list), _iter(p._iter) {}
    explicit	  fat_iterator(SchematicModel* l, iterator i)
					: _list(l), _iter(i) {}
    fat_iterator& operator=(const fat_iterator& p)
					{_list = p._list, _iter = p._iter; return *this;}
    bool	  is_end()const		{return _iter == _list->end();}
    Element*	  operator*()		{return (is_end()) ? NULL : *_iter;}
    fat_iterator& operator++()		{assert(!is_end()); ++_iter; return *this;}
    fat_iterator  operator++(int)
				{assert(!is_end()); fat_iterator t(*this); ++_iter; return t;}
    bool	  operator==(const fat_iterator& x)const
				{unreachable(); assert(_list==x._list); return (_iter==x._iter);}
    bool	  operator!=(const fat_iterator& x)const
					{assert(_list==x._list); return (_iter!=x._iter);}
    iterator	  iter()const		{return _iter;}
    fat_iterator  end()const		{return fat_iterator(_list, _list->end());}
    void	  insert(Element* c)	{_list->insert(iter(),c);}
  };
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

public:
	SchematicModel& erase(const_iterator what);
	//SchematicModel& erase(Element* c);
	//SchematicModel& erase_all();

public:
	void collectDigitalSignals(void);
	void print(QPrinter*, QPainter*, bool, bool);
//	void setDevType(QString const& type); // BUG. move to parent.
//	QString const& devType() const;

public:
	void prepare(); // precalc_first?
	void set_owner(Element* s);
//	int  prepareNetlist(ostream_t&, QStringList&, QPlainTextEdit*,
//			bool creatingLib, NetLang const&);
//	TaskElement* loadtaskElement(const QString& _s, TaskElement* c) const;
//	bool loadProperties(QTextStream*);

public: // not sure. leaves unconnected objects in the main container...
	void connect(Symbol* c);
	void disconnect(Symbol* c);

public:
	Element* detach(Element* c);

public: // container
	void clear();
	void push_back(Element* what);
	SchematicModel& insert(SchematicModel::iterator i, Element* c) { untested();
		_cl.insert(i, c);  return *this;
	}
	void pushBack(Element* what);
	void erase(Element* what);
	bool is_empty() const { return !_cl.size(); }
	size_t size() const{ return _cl.size(); }

public: // compat? test? debug?
	size_t nodeCount() const {return numNodes();}
	size_t numberOfNets() const{ return numNets(); }
	size_t numNodes() const;
	size_t numNets() const;

public: // obsolete?
	Node* insertNode(int x, int y, Element* owner);

public:
	NodeMap* nodes() const;
//	NodeMap const* nodes() const;

//	iterator find_(const std::string& short_name)
//					{return find_again(short_name, begin());}
//	iterator find_again(const std::string& short_name, iterator);
  // return a const_iterator
	SchematicModel const* parent() const;
	const_iterator begin()const {return _cl.begin();}
	const_iterator end()const {return _cl.end();}
	iterator begin() {return _cl.begin();}
	iterator end() {return _cl.end();}
	const_iterator find_again(const std::string& short_name, const_iterator)const;
	const_iterator find_(const std::string& short_name)const
					{return find_again(short_name, begin());}

private:
	ElementList _cl;
	NetList* Nets;
	NodeMap* _nm;
//	QFileInfo FileInfo;

public: // HACK
	unsigned nextIdx(std::string const& s) const;
	ParamList* params();
	ParamList const* params() const;

private:
	const SchematicModel* _parent;
	std::multimap<std::string, Element*> _map;
	mutable ParamList* _params;
}; // SchematicModel
/*--------------------------------------------------------------------------*/
SchematicModel::fat_iterator findbranch(istream_t&, SchematicModel::fat_iterator);
/*--------------------------------------------------------------------------*/
inline SchematicModel::fat_iterator findbranch(istream_t& cmd, SchematicModel* cl)
{
  assert(cl);
  return findbranch(cmd, SchematicModel::fat_iterator(cl, cl->begin()));
}
/*--------------------------------------------------------------------------*/
size_t numWires(SchematicModel const& m);
/*--------------------------------------------------------------------------*/
#endif
