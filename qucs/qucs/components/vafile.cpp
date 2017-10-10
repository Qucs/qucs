/***************************************************************************
                              vafile.cpp
                             ------------
    begin                : Sun Oct 26 2009
    copyright            : (C) 2009 by Stefan Jahn
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

#include <QString>
#include <QRegExp>
#include <QFile>
#include <QFileInfo>

#include "vafile.h"

// -------------------------------------------------------
VerilogA_File_Info::VerilogA_File_Info () {
  ModuleName = "";
  PortNames = "";
}

// -------------------------------------------------------
VerilogA_File_Info::VerilogA_File_Info (QString File, bool isfile)
{
  if (isfile) {
    QFile f (File);
    if (!f.open (QIODevice::ReadOnly))
      File = "";
    else {
      QByteArray FileContent = f.readAll ();
      File = QString (FileContent);
    }
    f.close();
  }
  
  QString s;
  int i=0, j, k=0;
  while((i=File.indexOf("//", i)) >= 0) { // remove all Verilog-A comments
    j = File.indexOf('\n', i+2);          // (This also finds "//" within a ...
    if(j < 0)                          //  string, but as no strings are ...
      File = File.left(i);             //  allowed in module headers, it ...
    else                               //  does not matter.)
      File.remove(i, j-i);
  }

  i=0;
  while((i=File.indexOf("/*", i)) >= 0) { // remove all Verilog-A comments
    j = File.indexOf("*/", i+2);          // (This also finds "/*" within a ...
    if(j < 0)                          //  string, but as no strings are ...
      File = File.left(i);             //  allowed in module headers, it ...
    else                               //  does not matter.)
      File.remove(i, j-i+2);
  }

  QRegExp Expr,Expr1;
  Expr.setCaseSensitivity(Qt::CaseSensitive);
  Expr1.setCaseSensitivity(Qt::CaseSensitive);
  k--;
  Expr.setPattern("\\bmodule\\b");  // start of last module
  k = File.lastIndexOf(Expr, k);
  if(k < 0)
    return;

  Expr.setPattern("\\bendmodule\\b");    // end of last module
  i = File.indexOf(Expr, k+7);
  if(i < 0)
    return;
  s = File.mid(k+7, i-k-7);  // cut out module declaration

  Expr.setPattern("\\b");
  i = s.indexOf(Expr);
  if(i < 0)
    return;
  j = s.indexOf(Expr, i+1);
  if(j < 0)
    return;
  ModuleName = s.mid(i, j-i);  // save module name

  i = s.indexOf('(', j);
  if(i < 0)
    return;

  j = s.indexOf(')', i);
  if(j < 0)
    return;
  s = s.mid(i+1, j-i-1);

  // parse ports, i.e. network connections
  PortNames = parsePorts (s, 0);
}

// -------------------------------------------------------
QString VerilogA_File_Info::parsePorts(QString s, int i)
{
  QRegExp Expr,Expr1;
  Expr.setCaseSensitivity(Qt::CaseSensitive);
  Expr1.setCaseSensitivity(Qt::CaseSensitive);

  int j;
  i = 0;    // remove all Verilog-A identifiers (e.g. "input")
  Expr.setPattern("(\\binput\\b|\\boutput\\b|\\binout\\b)");
  Expr1.setPattern("(\\b)");
  while((i=s.indexOf(Expr, i)) >= 0) {
    j = s.indexOf(Expr1, i+1);
    if(j < 0)
      s = s.left(i);
    else
      s.remove(i, j-i);
  }

  s.remove(' ');
  s.remove('\n');
  s.remove('\t');
  return s;
}
