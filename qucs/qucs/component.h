/***************************************************************************
                          component.h  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
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
#include "node.h"

#include <qptrlist.h>
#include <qpen.h>
#include <qpoint.h>
#include <qwidget.h>

class QString;

/**
  *@author Michael Margraf
  */

struct Line {
  Line(int _x1, int _y1, int _x2, int _y2, QPen _style)
       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
  int   x1, y1, x2, y2;
  QPen  style;
};

struct Arc {
  Arc(int _x, int _y, int _w, int _h, int _angle, int _arclen, QPen _style)
       : x(_x), y(_y), w(_w), h(_h), angle(_angle), arclen(_arclen), style(_style) {};
  int   x, y, w, h, angle, arclen;
  QPen  style;
};

struct Port {
  Port() {};
  Port(int _x, int _y) : x(_x), y(_y) {};
  int   x, y;
  Node *Connection;
};
  
struct Text {
  Text(int _x, int _y, const QString& _s) : x(_x), y(_y), s(_s) {};
  int   x, y;
  QString s;
};

struct Property {
  Property(const QString& _Name="", const QString& _Value="", bool _display=false, const QString& Desc="")
           : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
  QString Name, Value;
  bool    display;   // show on schematic or not ?
  QString Description;
};

class Component : public Element {
public: 
	Component();
	virtual ~Component();

  virtual Component* newOne();
  virtual void paintScheme(QPainter *p);
  virtual void setCenter(int x, int y, bool relative=false);
  void    Bounding(int& _x1, int& _y1, int& _x2, int& _y2);
  void    entireBounds(int& _x1, int& _y1, int& _x2, int& _y2);
  void    paint(QPainter *p);
  void    rotate();
  void    mirrorX();  // mirror about X axis
  void    mirrorY();  // mirror about Y axis
  QString NetList();
  QString save();
  bool    load(const QString& s);

  // to hold track of the component appearance for saving and copying
  bool mirroredX;   // is it mirrored about X axis or not
  int  rotated;     // rotation angle divided by 90 degrees


  QPtrList<Line>     Lines;
  QPtrList<Arc>      Arcs;
  QPtrList<Port>     Ports;
  QPtrList<Text>     Texts;
  QPtrList<Property> Props;

  bool     isActive; // should it be used in simulation or not ?
  int      tx, ty;   // upper left corner of text (position)
  QString  Description;
  QString  Sign, Model, Name;
};


class SParamFile : public Component  {
public:
	SParamFile(int No);
	virtual ~SParamFile();
  virtual SParamFile* newOne();
};

class Subcircuit : public Component  {
public:
 	Subcircuit(int No);
	virtual ~Subcircuit();
  virtual Subcircuit* newOne();
};

class Equation : public Component  {
public:
	Equation();
	virtual ~Equation();
  virtual Equation* newOne();
};

// ####################################################################################
// ##########                                                                ##########
// ##########                         Lumped Components                      ##########
// ##########                                                                ##########
// ####################################################################################

class SubCirPort : public Component  {
public:
	SubCirPort();
	virtual ~SubCirPort();
  virtual SubCirPort* newOne();
};

class Resistor : public Component  {
public:
	Resistor();
	virtual ~Resistor();
  virtual Resistor* newOne();
};

class ResistorUS : public Component  {
public:
	ResistorUS();
	virtual ~ResistorUS();
  virtual ResistorUS* newOne();
};

class Capacitor : public Component  {
public:
	Capacitor();
	virtual ~Capacitor();
  virtual Capacitor* newOne();
};

class dcBlock : public Component  {
public:
	dcBlock();
	virtual ~dcBlock();
  virtual dcBlock* newOne();
};

class Inductor : public Component  {
public:
	Inductor();
	virtual ~Inductor();
  virtual Inductor* newOne();
};

class dcFeed : public Component  {
public:
	dcFeed();
	virtual ~dcFeed();
  virtual dcFeed* newOne();
};

class BiasT : public Component  {
public:
	BiasT();
	virtual ~BiasT();
  virtual BiasT* newOne();
};

class Transformer : public Component  {
public:
	Transformer();
	virtual ~Transformer();
  virtual Transformer* newOne();
};

class symTrafo : public Component  {
public:
	symTrafo();
	virtual ~symTrafo();
  virtual symTrafo* newOne();
};

class Ground : public Component  {
public:
	Ground();
	virtual ~Ground();
  virtual Ground* newOne();
};

class Attenuator : public Component  {
public:
	Attenuator();
	virtual ~Attenuator();
  virtual Attenuator* newOne();
};

class Isolator : public Component  {
public:
	Isolator();
	virtual ~Isolator();
  virtual Isolator* newOne();
};

class Circulator : public Component  {
public:
	Circulator();
	virtual ~Circulator();
  virtual Circulator* newOne();
};

// ####################################################################################
// ##########                                                                ##########
// ##########                              Sources                           ##########
// ##########                                                                ##########
// ####################################################################################

class Volt_dc : public Component  {
public:
	Volt_dc();
	virtual ~Volt_dc();
  virtual Volt_dc* newOne();
};

class Ampere_dc : public Component  {
public:
	Ampere_dc();
	virtual ~Ampere_dc();
  virtual Ampere_dc* newOne();
};

class Volt_ac : public Component  {
public:
	Volt_ac();
	virtual ~Volt_ac();
  virtual Volt_ac* newOne();
};

class Source_ac : public Component  {
public:
	Source_ac();
	virtual ~Source_ac();
  virtual Source_ac* newOne();
};

class VCCS : public Component  {
public:
	VCCS();
	virtual ~VCCS();
  virtual VCCS* newOne();
};

class CCCS : public Component  {
public:
	CCCS();
	virtual ~CCCS();
  virtual CCCS* newOne();
};

class VCVS : public Component  {
public:
	VCVS();
	virtual ~VCVS();
  virtual VCVS* newOne();
};

class CCVS : public Component  {
public:
	CCVS();
	virtual ~CCVS();
  virtual CCVS* newOne();
};

// ####################################################################################
// ##########                                                                ##########
// ##########                       Transmission Lines                       ##########
// ##########                                                                ##########
// ####################################################################################

class TLine : public Component  {
public:
	TLine();
	virtual ~TLine();
  virtual TLine* newOne();
};

class Substrate : public Component  {
public:
	Substrate();
	virtual ~Substrate();
  virtual Substrate* newOne();
};

class MSline : public Component  {
public:
	MSline();
	virtual ~MSline();
  virtual MSline* newOne();
};

class MScoupled : public Component  {
public:
	MScoupled();
	virtual ~MScoupled();
  virtual MScoupled* newOne();
};

class MSstep : public Component  {
public:
	MSstep();
	virtual ~MSstep();
  virtual MSstep* newOne();
};

class MScorner : public Component  {
public:
	MScorner();
	virtual ~MScorner();
  virtual MScorner* newOne();
};

class MStee : public Component  {
public:
	MStee();
	virtual ~MStee();
  virtual MStee* newOne();
};

class MScross : public Component  {
public:
	MScross();
	virtual ~MScross();
  virtual MScross* newOne();
};

class Coplanar : public Component  {
public:
	Coplanar();
	virtual ~Coplanar();
  virtual Coplanar* newOne();
};


// ####################################################################################
// ##########                                                                ##########
// ##########                      Nonlinear Components                      ##########
// ##########                                                                ##########
// ####################################################################################

class Diode : public Component  {
public:
	Diode();
	virtual ~Diode();
  virtual Diode* newOne();
};


// ####################################################################################
// ##########                                                                ##########
// ##########                     Simulation Components                      ##########
// ##########                                                                ##########
// ####################################################################################

class DC_Sim : public Component  {
public:
	DC_Sim();
	virtual ~DC_Sim();
  virtual DC_Sim* newOne();
};

class AC_Sim : public Component  {
public:
	AC_Sim();
	virtual ~AC_Sim();
  virtual AC_Sim* newOne();
};

class TR_Sim : public Component  {
public:
	TR_Sim();
	virtual ~TR_Sim();
  virtual TR_Sim* newOne();
};

class SP_Sim : public Component  {
public:
	SP_Sim();
	virtual ~SP_Sim();
  virtual SP_Sim* newOne();
};

class HB_Sim : public Component  {
public:
	HB_Sim();
	virtual ~HB_Sim();
  virtual HB_Sim* newOne();
};

class Param_Sweep : public Component  {
public:
	Param_Sweep();
	virtual ~Param_Sweep();
  virtual Param_Sweep* newOne();
};

#endif
