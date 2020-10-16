/***************************************************************************
    copyright            : (C) 2016, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// base class for circuit components.
// basically Component, but "Component" already existed.
// maybe rename later.
#ifndef QUCS_SYMBOL_H
#define QUCS_SYMBOL_H
/*--------------------------------------------------------------------------*/
#include "element.h"
#include "exception.h"
#include "geometry.h"
/*--------------------------------------------------------------------------*/
class QPainter;
class NodeMap;
class Net;
/*--------------------------------------------------------------------------*/
class Symbol : public Element{
protected:
	explicit Symbol(Symbol const&);
public: // construct
	explicit Symbol();
	virtual ~Symbol();

protected: // Element
	void paint(ViewPainter*) const;
	virtual std::pair<int, int> center()const override;

public: // interface
	// what is this?
	// some kind of "init"??!
	virtual void recreate(); // SchematicModel const& ctx);

	virtual /*bug*/ std::string /* const& */ typeName() const{
		return _type;
	}
	virtual /*bug*/ void setTypeName(std::string const& x){
		_type = x;
	}
	virtual void build() {} // what does it do?
	virtual unsigned paramCount()const;
	virtual bool paramIsPrintable()const;

public: // TODO. something like this.
	virtual void expand() { untested(); }
	virtual void unExpand() { untested(); }
	// TODO: clarify relation to "recreate"...
	// virtual void prepare();
	// virtual void unPrepare();
	//
public: // TODO: something like this.
	//   // common
	//   COMMON_COMPONENT* mutable_common()	  {return _common;}
	//   const COMMON_COMPONENT* common()const	  {return _common;}
	//   bool	has_common()const		  {return _common;}
	//   void	attach_common(COMMON_COMPONENT*c) {COMMON_COMPONENT::attach_common(c,&_common);}
	//   void	detach_common()			  {COMMON_COMPONENT::detach_common(&_common);}
	//   void	deflate_common();

public:
	SchematicModel const* scope() const;

protected: // needed in netlister
	virtual SchematicModel* scope();

public: // Parameters
	virtual void setParameter(std::string const& name, std::string const&){
		throw ExceptionCantFind(name, label().toStdString());
	}
	virtual void setParameter(unsigned i, std::string const&){
		throw ExceptionCantFind(std::to_string(i), label().toStdString());
	}

	virtual std::string paramValue(std::string const&s) const;
	virtual std::string paramValue(unsigned i) const;
	virtual std::string paramName(unsigned i) const;

public: // qt?
	void setParameter(QString const&, QString const&);
	void setParameter(unsigned, QString const&);

public: // non-virtual (on purpose)
	QString const& netLabel(unsigned i) const;

public: // Node stuff
	virtual Node* connectNode(unsigned idx, NodeMap&);
	virtual Node* disconnectNode(unsigned idx, NodeMap&);

private:
	Port const& port(unsigned) const; // TODO. don't expose "Port"

public: // Port access
	virtual pos_t portPosition(unsigned) const; // why virtual?
	virtual pos_t nodePosition(unsigned) const; // why virtual?

	virtual Net const* portValue(unsigned) const; // why virtual?
	Node const* portNode(unsigned) const;
	// TODO: rethink Port/Node semantics
	virtual unsigned numPorts() const = 0;
	virtual bool portExists(unsigned) const{ return false; }
	virtual /*?*/ QString portName(unsigned) const{return "invalid"; }

	SchematicModel const* subckt() const{ return _subckt; }
	SchematicModel* subckt(){ return _subckt; }
	void new_subckt();

private: // internal port access
	virtual Port& port(unsigned i){
		throw ExceptionCantFind(std::to_string(i), label().toStdString());
	}

public: // graphics
	// hmm, maybe just dispatch a gfx object.
	// leave it like that, for now.
	virtual unsigned height()const {return 0;}
	virtual unsigned width()const {return 0;}
	//  virtual void draw(QPainter&)const=0;
	//...  more to come
protected: // maybe not here. but need to rebase MultiViewComponent to ScktProto first.
	SchematicModel* _subckt; // stuff contained in this symbol.
	// such as subckt components. meta data or symbol gfx
private:
	std::string _type;
}; // symbol
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
