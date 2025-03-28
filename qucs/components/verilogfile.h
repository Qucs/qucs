/***************************************************************************
                               verilogfile.h
                               -------------
    begin                : Sat Mar 31 2007
    copyright            : (C) 2007 by Stefan Jahn
    email                : stefa@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VERILOGFILE_H
#define VERILOGFILE_H

#include "component.h"

class QTextStream;
class QString;

class Verilog_File_Info; // BUG. use Component.

class Verilog_File : public MultiViewComponent  {
public:
  Verilog_File(Verilog_File const&);
  Verilog_File();
 ~Verilog_File();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool createSubNetlist(QTextStream *);
  QString getErrorText() { return ErrText; }
  QString getSubcircuitFile();
private:
  Verilog_File_Info* _proto{nullptr};

public:
  bool param_is_printable(int)const override;
  std::string dev_type()const override;
  void set_dev_type(std::string const&)override;
  void set_attribute(std::string name, std::string value)override;

  std::string attr_get() const override {
	  std::string ret = Component::attr_get();
	  ret += ", qucs_Type=\"Verilog\"";
	  assert(Props.size());
	  ret += ", qucs_File=\"" + prop(0).Value.toStdString() + "\"";
	  return ret;
  }
  std::string port_name(int i)const override;
  std::string port_value(int i)const override;

protected:
  QString verilogCode(int);
  void createSymbol();
  QString loadFile();

  QString ModuleName;
  QString ErrText;
};

// BUG. use Component
class Verilog_File_Info {
public:
  Verilog_File_Info();
  Verilog_File_Info(QString, bool isfile = false);
 ~Verilog_File_Info() {};
  QString parsePorts(QString, int);
  std::string port_value(int i)const /* override */;

public:
  QString ModuleName;
  QString PortNames;
private:
  std::vector<std::string> _portnames;
};

#endif
