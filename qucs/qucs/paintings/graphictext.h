/***************************************************************************
                                graphictext.h
                               ---------------
    begin                : Mon Nov 24 2003
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef GRAPHICTEXT_H
#define GRAPHICTEXT_H

#include "painting.h"

#include <QColor>
#include <QFont>
#include <QString>


class GraphicText : public Painting  {
public:
  GraphicText();
 ~GraphicText();

  void paintScheme(SchematicDoc*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  Element* newOne() const {return new GraphicText(*this);}
  Element* clone() const {return new GraphicText(*this);}

  static Element* info(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*);
  void MouseMoving(SchematicDoc*, int, int, int, int, SchematicDoc*, int, int, bool);
  bool MousePressing();
  bool getSelected(float, float, float);
  void Bounding(int&, int&, int&, int&);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QColor   Color;
  QFont    Font;
  QString  Text;
  int      Angle;
};

#endif
