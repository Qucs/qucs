/***************************************************************************
                               libcomp.h
                              -----------
    begin                : Fri Jun 10 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifndef LIBCOMP_H
#define LIBCOMP_H

#include "component.h"

class QTextStream;
class QString;


class LibComp : public MultiViewComponent  {
public:
  explicit LibComp();
  explicit LibComp(LibComp const&);
 ~LibComp() {};
  Component* newOne();

  bool createSubNetlist(QTextStream *, QStringList&, int type=1);
  QString getSubcircuitFile();

private: // parameters
  std::string param_name(int i)const override;
  std::string param_value(int i)const override;

public:
  std::string dev_type()const override;
  void set_dev_type(std::string const&)override;

protected:
  QString netlist();
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();

private:
  int  loadSymbol();
  int  loadSection(const QString&, QString&, QStringList* i=0);
  QString createType();
};

#endif
