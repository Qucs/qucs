/***************************************************************************
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIBCOMP_H
#define LIBCOMP_H

#include "component.h"
#include "qucsdoc.h" // BUG

class QTextStream;
class DocumentStream;
class QString;


class LibComp : public MultiViewComponent  {
public:
  LibComp();
  ~LibComp() {};
  Component* newOne(); // BUG{return new LibComp(*this);}

  bool createSubNetlist(DocumentStream&, QStringList&, int type=1); // BUG
  QString getSubcircuitFile() const;

protected:
  QString netlist() const;
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();

private:
  int  loadSymbol();
  int  loadSection(const QString&, QString&, QStringList* i=0);
  QString createType() const;
};

#endif
