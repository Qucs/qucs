/***************************************************************************
                          diagram.h  -  description
                             -------------------
    begin                : Thu Oct 2 2003
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

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "graph.h"

#include <qfile.h>


/**
  *@author Michael Margraf
  */

class Diagram {
public: 
	Diagram(int _cx=0, int _cy=0);
	virtual ~Diagram();

  virtual Diagram* newOne();
  virtual void paint(QPainter *p);
  virtual void calcData(Graph *g);
  QString save();
  bool load(const QString& Line, QTextStream *stream);

  void paintScheme(QPainter *p);
  void setCenter(int _cx, int _cy);
  bool loadVarData(const QString& fileName);
  int  loadIndepVarData(const QString& var, const QString& fileName);
  
  QString Name;
  int     cx, cy, dx, dy;
  bool    GridOn;
  int     GridX, GridY;
  QPtrList<Graph> Graphs;

  double x1, y1, x2, y2;
};

#endif
