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
#ifndef QUCS_ELEMENT_LIST
#define QUCS_ELEMENT_LIST

#include "platform.h"
#include "io_trace.h"
//#include "object.h"
//#include "tasks.h"
//#include "element.h"
//#include "symbol.h"
//#include <algorithm>
/*--------------------------------------------------------------------------*/
class QFileInfo; // BUG
class QPrinter; // BUG?
class QPainter; // BUG?
/*--------------------------------------------------------------------------*/
class ostream_t;
class istream_t;
class ParamList;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class Node;
class NetList;
class NodeMap;
class TaskElement;
class Diagram;
class Painting;
class Element;
class Symbol; // BUG
/*--------------------------------------------------------------------------*/
class ElementList{
public: // stub
	typedef std::list<Element*> container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;
	class fat_iterator {
	private:
		ElementList* _list;
		iterator   _iter;
	private:
		explicit	  fat_iterator()	{unreachable();}
	public:
		fat_iterator(const fat_iterator& p)
			: _list(p._list), _iter(p._iter) {}
		explicit	  fat_iterator(ElementList* l, iterator i)
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
	ElementList(ElementList const&) = delete;
public:
	explicit ElementList();
	~ElementList();
public: // stuff saved from Schematic
	void sizeOfAll(int&, int&, int&, int&, float) const;

private:
	void detachFromNode(Element* what, Node* from);

public:
	ElementList& erase(const_iterator what);
	//ElementList& erase(Element* c);
	//ElementList& erase_all();

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
	ElementList& insert(ElementList::iterator i, Element* c) { untested();
		_cl.insert(i, c);  return *this;
	}
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

	// mutable
	iterator find_(const std::string& short_name)
					{return find_again(short_name, begin());}
	iterator find_again(const std::string& short_name, iterator);

  // return a const_iterator
	ElementList const* parent() const;
	const_iterator begin()const {return _cl.begin();}
	const_iterator end()const {return _cl.end();}
	iterator begin() {return _cl.begin();}
	iterator end() {return _cl.end();}
	const_iterator find_again(const std::string& short_name, const_iterator)const;
	const_iterator find_(const std::string& short_name)const
					{return find_again(short_name, begin());}

private:
	container_type _cl;
	NetList* Nets;
	NodeMap* _nm;
//	QFileInfo FileInfo;

public: // HACK
	unsigned nextIdx(std::string const& s) const;
	ParamList* params();
	ParamList const* params() const;

private:
	const ElementList* _parent;
	std::multimap<std::string, Element*> _map;
	mutable ParamList* _params;
public:
  static ElementList card_list; // in qucs::globals.cc
}; // ElementList
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
qucs::ElementList::fat_iterator findbranch(istream_t&, qucs::ElementList::fat_iterator);
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
inline ElementList::fat_iterator findbranch(istream_t& cmd, ElementList* cl)
{
  assert(cl);
  return findbranch(cmd, ElementList::fat_iterator(cl, cl->begin()));
}
/*--------------------------------------------------------------------------*/
size_t numWires(ElementList const& m);
/*--------------------------------------------------------------------------*/
} // namespace
#endif
