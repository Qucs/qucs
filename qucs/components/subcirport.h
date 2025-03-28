/***************************************************************************
                               subcirport.h
                              --------------
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

#ifndef SUBCIRPORT_H
#define SUBCIRPORT_H

#include "component.h"


class SubCirPort : public MultiViewComponent  {
public:
  SubCirPort();
 ~SubCirPort() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool param_is_printable(int i)const override {
    switch(i){
    case 0: // "Num"
      return false;
    case 1: // "Type"
      return false;
    default:
      return Component::param_is_printable(i);
    }
  }
  std::string attr_get() const {
    std::string ret = Component::attr_get();
	 assert(Props.size()>1);
    ret += ", qucs_Num=" + prop(0).Value.toStdString();
    ret += ", qucs_Type=\"" + prop(1).Value.toStdString() + "\"";
    return ret;
  }
  void set_attribute(std::string n, std::string v)override;

protected:
  QString netlist();
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();
};

#endif
