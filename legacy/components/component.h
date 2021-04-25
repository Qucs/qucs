/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_LEGACY_COMPONENT_H
#define QUCS_LEGACY_COMPONENT_H

#include "symbol.h"
#include "qt_compat.h"
#include "../legacy/obsolete_stuff.h"
#include "property.h"
#include "qucs_globals.h"

#define isComponent        0x30000
#define isComponentText    0x30002
#define isAnalogComponent  0x10000
#define isDigitalComponent 0x20000

namespace qucs{
	class ViewPainter;
	class Module;
}
using qucs::Widget;
using qucs::ViewPainter;
using qucs::Module;
using qucs::Element;
using qucs::ElementList;
using qucs::symbol_dispatcher;
using qucs::NetLang; // what?
using qucs::Symbol;

class QString;
class QPen;
class ComponentDialog; // BUG?
class QTextStream; // BUG

class Line;
class Text;
class Area;

class ComponentPort : public Port{
public:
  ComponentPort(const ComponentPort&p) : Port(), _p(p._p), avail(p.avail),
					Type(p.Type), Name(p.Name) {}
  explicit ComponentPort(int x, int y, bool _avail=true)
	  : Port(), _p(x, y), avail(_avail)
  {
    Type=""; Name="";
  }

public:
  int x_()const{return getX(_p);}
  int y_()const{return getY(_p);}
  pos_t const& position() const { return _p; }
  void setPosition(int x, int y) { _p=pos_t(x,y); }
  void setPosition(pos_t p) { _p=p; }
  int x() const{ return getX(_p); }
  int y() const{ return getY(_p); }

  QString const& name() const{return Name;}

private:
  pos_t _p;

public:
  bool  avail;
  QString Type; // BUG. type is "Port".
  QString Name; // BUG?

};

class Component : public qucs::Symbol {
protected:
  Component(Component const&);

public: //??!
  Component();

  virtual ~Component() {};
  QString getNetlist() const; // BUG. use netlister to create netlist
  QString get_VHDL_Code(int);
  QString get_Verilog_Code(int);

protected:
  void paint(ViewPainter*) const override;
private: // Element override
  Widget* schematicWidget(qucs::Doc*) const override;
  rect_t bounding_rect() const override;
  bool legacyTransformHack() const override;

private: // Symbol interface.
  virtual void recreate() {}; // obsolete?

protected: // wrap legacy. don't use in new code.
  void recreate(int i){
	  assert(i==0);
	  return recreate();
  }

public: // legacy stuff. don't use
  //private:
  virtual Component* newOne(){unreachable(); return nullptr;} // use clone instead.
  virtual void tAC(QTextStream&, ElementList const*, QStringList&, int&, int, NetLang const&){
	  unreachable();
  }

public: // Element interface, private?!
  Element* clone() const;

protected: // Symbol
  virtual index_t numPorts() const override{ return Ports.count(); }
  Port& port(index_t i) override{
	  assert(i<index_t(Ports.count()));
	  return *Ports[i]; 
  }
  pos_t portPosition(index_t i) const override{
	  return Ports[i]->position();
  }
  //virtual Port const* portValue(unsigned i) const { return Ports[i]; } ... ?

//  void setPort(unsigned i, Node* n){
//	  assert(i<unsigned(Ports.count()));
//	  Ports[i]->connect(n);
//  }

	bool param_is_printable(index_t) const override;
	index_t param_count() const override;
	std::string param_value(index_t i) const override;
	std::string param_name(index_t i) const override;
	std::string param_value_by_name(std::string const& name) const override;
public: // BUG
	void set_param_by_name(std::string const& name, std::string const& value) override;


protected:
	void set_param_by_index(index_t i, std::string const&) override;

protected: // really?
	static const int num_component_params;

public: // BUG
  void    print(ViewPainter*, float);
//  void    setCenter(int, int, bool relative=false) override;
  void    getCenter(int&, int&) const; // BUG
  int     textSize(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  void    entireBounds(int&, int&, int&, int&, float);
  bool    getSelected(int, int);
  int     getTextSelected(int, int, float);


	int const& x1_() const { return x1; }
	int const& y1_() const { return y1; }
	int const& x2_() const { return x2; }
	int const& y2_() const { return y2; }

private:
  void rotate();

protected:
  void set_rotated(unsigned r);
  void set_mirror_xaxis(unsigned r);
  void set_mirror_yaxis(unsigned r);

public: // BUG
  unsigned rotated() const{return _rotated;}
  void    mirrorX();  // mirror Y axis
  void    mirrorY();  // mirror X axis
  bool    load(const QString&);

  // to hold track of the component appearance for saving and copying
  bool mirroredX;   // is it mirrored about X axis or not
  bool mirroredY{false};   // is it mirrored about X axis or not
  int  _rotated;     // rotation angle divided by 90 degrees

  virtual QString getSubcircuitFile() const { return ""; }
  // set the pointer scematic associated with the component
  // do somehting with buttons. can sb think of a more descriptive name?
  virtual void dialgButtStuff(ComponentDialog&)const;

  QList<Line *>     Lines;
  QList<struct Arc *>      Arcs;
  QList<Area *>     Rects;
  QList<Area *>     Ellips;
  QList<ComponentPort*> Ports;
  QList<Text *>     Texts;
  mutable //bug: Q3PtrList does not support constness
      Q3PtrList<Property> Props;

  #define COMP_IS_OPEN    0
  #define COMP_IS_ACTIVE  1
  #define COMP_IS_SHORTEN 2
  bool isShort() const{return isActive==COMP_IS_SHORTEN;}
  bool isOpen() const{return isActive==COMP_IS_OPEN;}
//  QString label() const{return Name;}
  Q3PtrList<Property> const& params() const{return Props;}
  QList<ComponentPort*>const& ports() const{return Ports;}
// private: // not yet
  int  isActive; // should it be used in simulation or not ?
  mutable /*BUGBUGBUG*/ int  tx, ty;   // upper left corner of text (position)

public:
  bool showLabel() const override{ return showName; }

public:
  int cx() const{return getX(Element::center());}
  int cy() const{return getY(Element::center());}

protected:
  QString Name; // the label, but sometimes the type. yikes.

public:
  QString const& obsolete_model_hack() const{
	  // BUG. do not use
	  return Model;
  }
  void setName(QString x){
	  incomplete();
	  set_label(x.toStdString());
	  Name = x;
  }
  QString const& description() const{
	  return Description;
  }

public: // BUG
  std::string dev_type() const override {
	  return Model.toStdString();
  }
  void set_dev_type(std::string const& x) override{
	  Symbol::set_dev_type(x);
	  // tmp kludge, store type in Model...
	  Model = QString::fromStdString(x);
  }

protected:
public: // old mess
  virtual bool useObsoleteProps() const {return true;}

private:
  char const* iconBasename() const{return bitmap_file;}
protected: // BUG
  QString Model;
protected: // BUG => Element.
  char* bitmap_file;
protected: // BUG
  QString  Description;
protected:
  virtual QString netlist() const;
  virtual QString vhdlCode(int);
  virtual QString verilogCode(int);

  int  analyseLine(const QString&, int);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
                   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  void copyComponent(Component*);
  Property * getProperty(const QString&);

private: // (hopefully) obsolete callbacks
  void recreateCallback();

public: // BUG
  // center and relative boundings
  int x1, y1;
  int x2, y2;

// private: !!!
  bool showName;

protected: // obsolete
  int Type;
}; // Component



class MultiViewComponent : public ::Component {
public:
  MultiViewComponent(MultiViewComponent const&c) : Component(c) {}
  MultiViewComponent() {};
  virtual ~MultiViewComponent() {};

private: // Symbol
  void recreate();

protected: // wrap legacy. don't use.
  void recreate(int i){
	  assert(i==0);
	  return recreate();
  }

protected:
  virtual void createSymbol() {};
};


class GateComponent : public MultiViewComponent {
public:
  GateComponent();
  QString netlist() const;
  QString vhdlCode(int);
  QString verilogCode(int);

protected:
  void createSymbol();
};

#endif
