/***************************************************************************
                               component.h
                              -------------
    begin                : 2025
    copyright            : Qucs
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_COMPONENT
#define QUCS_COMPONENT

#include "element.h"
#include "trace.h"

// BUG. enum?
#define COMP_IS_OPEN    0
#define COMP_IS_ACTIVE  1
#define COMP_IS_SHORTEN 2

class Schematic;
class ViewPainter;
class QString;
class QPen;
class ComponentDialog;
class QTextStream; // BUG

class Component : public Element {
  std::string _attr;
  std::vector<std::string> _portvalues;

public: // BUG.
  std::list<qucs::Port>   Ports;
  std::list<qucs::Property>   Props;
public: // BUG
  virtual std::list<qucs::Line> const& lines() {static std::list<qucs::Line>x;  return x;}
  virtual std::list<qucs::Arc>  const& arcs()  {static std::list<qucs::Arc>x;   return x;}
  virtual std::list<qucs::Area> const& rects() {static std::list<qucs::Area>x;  return x;}
  virtual std::list<qucs::Area> const& ellips(){static std::list<qucs::Area>x; return x;}
  virtual std::list<qucs::Text> const& texts() {static std::list<qucs::Text>x;  return x;}
public:
  Component();
  virtual ~Component() {}

  virtual Component* newOne();
  virtual void recreate(Schematic*) {}
  QString getNetlist();
  QString get_VHDL_Code(int);
  QString get_Verilog_Code(int);
  virtual void    paint(ViewPainter*) /*const*/ {};
  virtual void    paintScheme(Schematic*) /*const*/ {};
  virtual void    print(ViewPainter*, float) /*const*/ {};
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  int     textSize(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  void    entireBounds(int&, int&, int&, int&, float);
  bool    getSelected(int, int);
  int     getTextSelected(int, int, float);
  virtual void    rotate() {incomplete();}
  virtual void    mirrorX() {incomplete();} // mirror Y
  virtual void    mirrorY() {incomplete();} // mirror X
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
  virtual bool createSubNetlist(QTextStream*){return false;} // BUG: callback. BUG: const?!
  virtual QString getErrorText() { return ""; } // BUG. that's what exceptions are for. BUG: const?!

  //  gets property by index
  qucs::Property &prop(int n);
  const qucs::Property &prop(int n) const;

  //  gets port by index
  qucs::Port &port(int n);
  const qucs::Port &port(int n) const;

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

  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
                   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  virtual void copyComponent(const Component &c);

  qucs::Property &getProperty(const QString&);
  Schematic* containingSchematic;
};

std::shared_ptr<Component> getComponentFromName(QString& Line, Schematic* p=NULL);

#endif
