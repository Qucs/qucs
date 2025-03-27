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

Component* new_verilog_file();
bool is_verilog_file(Component const*); // BUG

#endif
