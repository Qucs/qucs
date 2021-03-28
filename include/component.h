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
// Base class for components.

#ifndef QUCS_COMPONENT_H
#define QUCS_COMPONENT_H
/*--------------------------------------------------------------------------*/
#include "element.h"
#include "exception.h"
/*--------------------------------------------------------------------------*/
enum {CC_STATIC_=27342};
/*--------------------------------------------------------------------------*/
namespace qucs{
/*--------------------------------------------------------------------------*/
class Component;
class ElementList;
class Net;
class NodeMap;
class Widget;
/*--------------------------------------------------------------------------*/
// borrowed (modified) from e_compon.h
class CommonComponent : public Object {
public:
  static void attach_common(CommonComponent* c, CommonComponent** to);
  static void detach_common(CommonComponent** from);
private:
  CommonComponent& operator=(const CommonComponent&)
			      {unreachable(); return *this;}
  explicit CommonComponent() : Object() {unreachable();incomplete();}
protected:
  explicit CommonComponent(const CommonComponent& p);
  explicit CommonComponent(int c);
public:
  virtual ~CommonComponent();

  void attach_model(const Component*)const;
//  CommonComponent& attach(const MODEL_CARD* m) {_model = m; return *this;}
  void set_modelname(const std::string& n) {_modelname = n;}
  virtual CommonComponent* clone()const = 0;
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
//  virtual CommonComponent* deflate()		{return this;}
//  virtual void precalc_last(const CARD_LIST*);

  // virtual std::string name()const	= 0;
  virtual bool  operator==(const CommonComponent&x)const;

  bool operator!=(const CommonComponent& x)const {return !(*this == x);}
  std::string	      modelname()const	{return _modelname;}
//  const MODEL_CARD*   model()const	{assert(_model); return _model;}
//  bool		      has_model()const	{return _model;}
//   const PARAMETER<double>& mfactor()const {return _mfactor;}
//   const PARAMETER<double>& value()const {return _value;}
private:
  std::string	_modelname;
//  mutable const MODEL_CARD* _model;
  int		_attach_count;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// qucs::Component. not to be confused with legacy Component
class Component : public Element {
protected:
	explicit Component(Component const&);
public: // construct
	explicit Component();
	virtual ~Component();

public: // manage shared data across components
	CommonComponent* mutable_common()	  {return _common;}
	const CommonComponent* common()const	  {return _common;}
	bool	has_common()const		  {return _common;}
	void	attach_common(CommonComponent*c) {CommonComponent::attach_common(c,&_common);}
	void	detach_common()			  {CommonComponent::detach_common(&_common);}
	void	deflate_common();

public:
	void set_dev_type(const std::string& new_type) override;
	virtual std::string dev_type()const{ return typeName();}

public: // Parameters
	virtual index_t param_count()const;
	virtual bool param_is_printable(index_t i)const;

	virtual void set_param_by_name(std::string const&, std::string const&) override;
	virtual void set_param_by_index(index_t i, std::string const&) override;

	virtual std::string param_value_by_name(std::string const&s) const override;
	virtual std::string param_value(index_t i) const override;
	virtual std::string param_name(index_t i) const; // override?

public: // Node stuff
	virtual bool is_device() const{return true;}
	virtual Node* connectNode(unsigned, NodeMap&){unreachable(); return nullptr;} // obsolete
	virtual Node* disconnectNode(unsigned, NodeMap&){unreachable(); return nullptr;} // obsolete

protected:
	Port const& port(unsigned) const; // TODO. don't expose "Port"

public: // Port stuff
	virtual unsigned max_nodes() const{
		return 100; // numPorts(); // for now.
	}
	virtual void set_port_by_index(index_t, std::string const&);
	virtual void set_port_by_name(std::string const&, std::string const&);
//	virtual pos_t portPosition(unsigned) const = 0;
//	pos_t nodePosition(unsigned) const;

	std::string port_value(unsigned) const;
	virtual Node const* portValue(unsigned) const; // why virtual?
	Node const* portNode(unsigned) const;
	// TODO: rethink Port/Node semantics
	virtual unsigned numPorts() const = 0; // net_nodes?
	virtual bool portExists(unsigned i) const{ return i<numPorts(); }
	virtual /*?*/ std::string const portName(unsigned) const{return "invalid"; }

public: // hierarchy. move to SubcktBase
	ElementList const* subckt() const{ return _subckt; }
	ElementList* subckt(){ return _subckt; }
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
protected: // maybe not here
	ElementList* _subckt; // stuff contained in this symbol.
	// such as subckt components. meta data or symbol gfx
	int vflip() const{return _vflip;}
	int hflip() const{return _hflip;}
	int angle() const{return _angle;}
	void setAngle(int a){_angle = a;}

private:
	int _vflip; // vscale??
	int _hflip; // hscale??
	int _angle;
	unsigned _param_display;
	bool _label_display;
	CommonComponent* _common;
	unsigned _net_nodes;
}; // Component
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
