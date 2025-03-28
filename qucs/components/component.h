/***************************************************************************
                               component.h
                              -------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COMPONENT_H
#define COMPONENT_H

#include "element.h"
#include "trace.h"
#include <list>

#include <QTextStream>

class Schematic;
class ViewPainter;
class QString;
class QPen;
class ComponentDialog;

class Component : public Element {
  std::string _attr;
public:
  Component();
  virtual ~Component() {}

  virtual Component* newOne();
  virtual void recreate(Schematic*) {}
  QString getNetlist();
  QString get_VHDL_Code(int);
  QString get_Verilog_Code(int);
  void    paint(ViewPainter*);
  void    paintScheme(Schematic*);
  void    print(ViewPainter*, float);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  int     textSize(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  void    entireBounds(int&, int&, int&, int&, float);
  bool    getSelected(int, int);
  int     getTextSelected(int, int, float);
  void    rotate();
  void    mirrorX();  // mirror about X axis
  void    mirrorY();  // mirror about Y axis
  bool    load(const QString&);

public: // attributes
  std::string attr_get()const override;
  virtual void attr_add(std::string s) {_attr += ", " + s;}
  virtual void set_attribute(std::string name, std::string value);
  virtual void set_label(std::string const& name);

public: // parameter access
  virtual int param_count() const;
  virtual bool param_is_printable(int i) const;
  virtual std::string param_name(int i) const;
  virtual std::string param_value(int i) const;
  virtual void set_param_by_index(int i, std::string const& Value);
  virtual void set_param_by_name(std::string const& name, std::string const& v);

public: // ports
  virtual int net_nodes()const{return Ports.size();}
  virtual std::string port_name(int)const{incomplete(); return "???";}
  virtual std::string port_value(int)const{incomplete(); return "???";}
  virtual void set_port_by_name(std::string const&, std::string const&);
  virtual void set_port_by_index(int num, std::string const& ext_name);

public:
  virtual std::string dev_type()const {
	  // incomplete();
	  return obsolete_model_hack().toStdString();
  }
  virtual void set_dev_type(std::string const& type);

public:
  virtual void set_qucs_text_position(int x, int y) {_tx=x;_ty=y;}
  virtual void set_qucs_rotated(int x) {_qucs_rotated=x;}
  virtual void set_qucs_mirrored(int x) {_qucs_mirrored=x;}
  virtual void set_qucs_x1(int x) {_qucs_x1=x;}
  virtual void set_qucs_y1(int y) {_qucs_y1=y;}
  virtual void apply_qucs_values();

  // to hold track of the component appearance for saving and copying
  bool mirroredX;   // is it mirrored about X axis or not
  int  rotated;     // rotation angle divided by 90 degrees

  virtual QString getSubcircuitFile() { return ""; }
  // set the pointer scematic associated with the component
  virtual void setSchematic (Schematic* p) { containingSchematic = p; }
  virtual Schematic* getSchematic () {return containingSchematic; }
  // do somehting with buttons. can sb think of a more descriptive name?
  virtual void dialgButtStuff(ComponentDialog&)const;

  //  gets property by index
  qucs::Property &prop(int n);
  const qucs::Property &prop(int n) const;

  //  gets port by index
  qucs::Port &port(int n);
  const qucs::Port &port(int n) const;

  std::list<qucs::Line>   Lines;
  std::list<qucs::Arc>    Arcs;
  std::list<qucs::Area>   Rects;
  std::list<qucs::Area>   Ellips;
  std::list<qucs::Port>   Ports;
  std::list<qucs::Text>   Texts;
  std::list<qucs::Property>   Props;

  #define COMP_IS_OPEN    0
  #define COMP_IS_ACTIVE  1
  #define COMP_IS_SHORTEN 2
  int  isActive; // should it be used in simulation or not ?
private:
  int _tx{0}, _ty{0}; // upper left corner of text (position)
  int _qucs_x1{0}, _qucs_y1{0}; // position of port 1 after transform
  bool _qucs_mirrored{false}; // value from XML document
  int _qucs_rotated{0}; // value from XML document
public:
  int tx()const {return _tx;}
  int ty()const {return _ty;}

public: // private: // TODO
  bool showName;

public:
  QString const& obsolete_model_hack() const{
	  // BUG. do not use
	  return Model;
  }
  void setName(QString x){
	  // FIXME move to Element.
	  Name = x;
  }
  QString const& name() const{
	  // yikes. this should not be necessary
	  return Name;
  }
  void obsolete_name_override_hack(QString x){
	  Name = x;
  }
  void gnd_obsolete_model_override_hack(QString x){
	  //assert (this is a gnd component); // fix later
	  Model = x;
  }
  QString const& description() const{
	  return Description;
  }

private:
protected: // BUG
  QString Model;
protected: // BUG => Element.
  QString Name;
protected: // BUG
  QString  Description;
protected:
  virtual QString netlist();
  virtual QString vhdlCode(int);
  virtual QString verilogCode(int);

  int  analyseLine(const QString&, int);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
                   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  void copyComponent(const Component &);
  qucs::Property &getProperty(const QString&);
  Schematic* containingSchematic;
};


class MultiViewComponent : public Component {
public:
  MultiViewComponent() {};
  virtual ~MultiViewComponent() {};

  void recreate(Schematic*);

protected:
  virtual void createSymbol() {};
};


// BUG wrong place.
class GateComponent : public MultiViewComponent {
public:
  GateComponent();
  QString netlist();
  QString vhdlCode(int);
  QString verilogCode(int);

  bool param_is_printable(int i)const override;

protected:
  void createSymbol();
};

// prototype of independent function
std::shared_ptr<Component> getComponentFromName(QString& Line, Schematic* p=NULL);

#endif
