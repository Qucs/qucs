/***************************************************************************
                                vhdlfile.h
                               ------------
    begin                : Sat Apr 15 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef VHDLFILE_H
#define VHDLFILE_H

#include "component.h"


class VHDL_File : public MultiViewComponent  {
public:
  VHDL_File();
 ~VHDL_File() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool createSubNetlist(QTextStream *);
  QString getErrorText() { return ErrText; }

protected:
  QString vhdlCode(int);
  void createSymbol();
  QString loadFile();

  QString EntityName;
  QString ErrText;
};

#endif
