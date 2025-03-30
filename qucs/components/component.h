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

#ifndef LEGACY_COMPONENT_H
#define LEGACY_COMPONENT_H

#include "../component.h"
#include "trace.h"
#include <list>

#include <QTextStream>

class LegacyComponent : public Component {

public:  // BUG
  std::list<qucs::Line>   Lines;
  std::list<qucs::Arc>    Arcs;
  std::list<qucs::Area>   Rects;
  std::list<qucs::Area>   Ellips;
  std::list<qucs::Text>   Texts;

public:
  void    rotate()override;
  void    mirrorX()override; // mirror Y
  void    mirrorY()override; // mirror X
  std::list<qucs::Line> const& lines(){return  Lines;}
  std::list<qucs::Arc>  const& arcs(){return   Arcs;}
  std::list<qucs::Area> const& rects(){return  Rects;}
  std::list<qucs::Area> const& ellips(){return Ellips;}
  std::list<qucs::Text> const& texts(){return  Texts;}

public:
  void    paint(ViewPainter*) /*const*/ override;
  void    paintScheme(Schematic*) /*const*/ override;
  void    print(ViewPainter*, float) /*const*/ override;

protected: // legacy schematic
  int  analyseLine(const QString&, int);

protected: // legacy overrides
  void copyComponent(const Component &c)override;
};


class MultiViewComponent : public LegacyComponent {
public:
  MultiViewComponent() {};
  virtual ~MultiViewComponent() {};

  void recreate(Schematic*);

protected:
  virtual void createSymbol() {};
};


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

#endif
