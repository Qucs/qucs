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
#include "component.h"

#include <qfile.h>


/**
  *@author Michael Margraf
  */



class Diagram : public Element {
public: 
	Diagram(int _cx=0, int _cy=0);
	virtual ~Diagram();

  virtual Diagram* newOne();
  virtual void calcDiagram();
  virtual void calcData(Graph *g);
  virtual void paintScheme(QPainter *p);
  virtual void setCenter(int x, int y, bool relative=false);
  void    paint(QPainter *p);
  void    Bounding(int& _x1, int& _y1, int& _x2, int& _y2);
  QString save();
  bool    load(const QString& Line, QTextStream *stream);

  void loadGraphData(const QString& defaultDataSet);
  bool loadVarData(const QString& fileName);
  int  loadIndepVarData(const QString& var, const QString& fileName);
  
  QString Name;   // identity string for the diagram type (e.g. Polar)
  bool    GridOn;
  int     GridX, GridY;
  QPtrList<Graph> Graphs;
  QPtrList<Arc>   Arcs;
  QPtrList<Line>  Lines;
  QPtrList<Text>  Texts;

  double xmin, ymin, xmax, ymax;    // least and greatest values of all graph data
};

#endif
