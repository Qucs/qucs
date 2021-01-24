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
#include "exception.h"
/*--------------------------------------------------------------------------*/
class QPainter;
class NodeMap;
class PaintingList;
class Net;
class Symbol;
class ElementList;
/*--------------------------------------------------------------------------*/
enum {CC_STATIC=27342};
static std::string invalid_ = "invalid";
/*--------------------------------------------------------------------------*/
// borrowed (modified) from e_compon.h
class COMMON_COMPONENT : public Object {
public:
  static void attach_common(COMMON_COMPONENT* c, COMMON_COMPONENT** to);
  static void detach_common(COMMON_COMPONENT** from);
private:
  COMMON_COMPONENT& operator=(const COMMON_COMPONENT&)
			      {unreachable(); return *this;}
  explicit COMMON_COMPONENT() : Object() {unreachable();incomplete();}
protected:
  explicit COMMON_COMPONENT(const COMMON_COMPONENT& p);
  explicit COMMON_COMPONENT(int c);
public:
  virtual ~COMMON_COMPONENT();

  void attach_model(const Symbol*)const;
//  COMMON_COMPONENT& attach(const MODEL_CARD* m) {_model = m; return *this;}
//  void set_modelname(const std::string& n) {_modelname = n;}

  virtual COMMON_COMPONENT* clone()const = 0;

  // virtual bool is_trivial()const {return false;}

public: // params, not yet.
//   virtual bool param_is_printable(int)const;
//   virtual std::string param_name(int)const;
//   virtual std::string param_name(int,int)const;
//   virtual std::string param_value(int)const;
//   virtual void set_param_by_name(std::string, std::string);
//   void Set_param_by_name(std::string, std::string); //BUG// see implementation
//   virtual void set_param_by_index(int, std::string&, int);
//   virtual int param_count()const {return 4;}
public: // also, not yet
//  virtual void precalc_first(const CARD_LIST*)	{}
//  virtual void expand(const COMPONENT*)		{}
//  virtual COMMON_COMPONENT* deflate()		{return this;}
//  virtual void precalc_last(const CARD_LIST*);

  // virtual std::string name()const	= 0;
  virtual bool  operator==(const COMMON_COMPONENT&x)const;

  bool operator!=(const COMMON_COMPONENT& x)const {return !(*this == x);}
//  std::string	      modelname()const	{return _modelname;}
//  const MODEL_CARD*   model()const	{assert(_model); return _model;}
//  bool		      has_model()const	{return _model;}
//   const PARAMETER<double>& mfactor()const {return _mfactor;}
//   const PARAMETER<double>& value()const {return _value;}
private:
//  std::string	_modelname;
//  mutable const MODEL_CARD* _model;
  int		_attach_count;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Symbol : public Element{
protected:
	explicit Symbol(Symbol const&);
public: // construct
	explicit Symbol();
	virtual ~Symbol();

protected: // Element
	void paint(ViewPainter*) const;
	QDialog* schematicWidget(QucsDoc*) const override;

public: // Element
	pos_t center()const override;

public:
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
public: // manage shared data across symbols
	COMMON_COMPONENT* mutable_common()	  {return _common;}
	const COMMON_COMPONENT* common()const	  {return _common;}
	bool	has_common()const		  {return _common;}
	void	attach_common(COMMON_COMPONENT*c) {COMMON_COMPONENT::attach_common(c,&_common);}
	void	detach_common()			  {COMMON_COMPONENT::detach_common(&_common);}
	void	deflate_common();

public:
	SchematicModel const* scope() const;
	virtual ElementList const* symbolPaintings() const;

// protected: // needed in netlister. public use in parse...
	virtual SchematicModel* scope();

public: // Parameters
	virtual void setParameter(std::string const& name, std::string const&);
	virtual void setParameter(unsigned i, std::string const&);

	virtual std::string paramValue(std::string const&s) const;
	virtual std::string paramValue(unsigned i) const;
	virtual std::string paramName(unsigned i) const;

public: // qt?
	void setParameter(QString const&, QString const&);
	void setParameter(unsigned, QString const&);

public: // non-virtual (on purpose)
	QString const& netLabel(unsigned i) const;

public: // Node stuff
	virtual bool is_device() const{return true;}
	virtual Node* connectNode(unsigned idx, NodeMap&);
	virtual Node* disconnectNode(unsigned idx, NodeMap&);

private:
	Port const& port(unsigned) const; // TODO. don't expose "Port"

public: // Port stuff
	virtual unsigned max_nodes() const{
		return 100; // numPorts(); // for now.
	}
	virtual void set_port_by_index(index_t, std::string const&);
	virtual void set_port_by_name(std::string const&, std::string const&);
	virtual pos_t portPosition(unsigned) const = 0;
	pos_t nodePosition(unsigned) const;

	std::string port_value(unsigned) const;
	virtual Node const* portValue(unsigned) const; // why virtual?
	Node const* portNode(unsigned) const;
	// TODO: rethink Port/Node semantics
	virtual unsigned numPorts() const = 0; // net_nodes?
	virtual bool portExists(unsigned i) const{ return i<numPorts(); }
	virtual /*?*/ std::string const& portName(unsigned) const{return invalid_; }

public: // hierarchy. move to SubcktBase
	SchematicModel const* subckt() const{ return _subckt; }
	SchematicModel* subckt(){ return _subckt; }
	void new_subckt();

private: // internal port access
	virtual Port& port(unsigned i) = 0;

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
	int vflip() const{return _vflip;}
	int hflip() const{return _hflip;}
	int angle() const{return _angle;}
	void setAngle(int a){_angle = a;}

private:
	std::string _type;
	int _vflip; // vscale??
	int _hflip; // hscale??
	int _angle;
	unsigned _param_display;
	bool _label_display;
	COMMON_COMPONENT* _common;
	unsigned _net_nodes;
}; // Symbol
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
