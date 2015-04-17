/*
 * schematicview.h - definition view of schematic
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SCHEMATICVIEW_H_
#define SCHEMATICVIEW_H_ value

#include <QGraphicsView>
#include <QString>

#include "schematicscene.h"
#include "qucsdoc.h"

class QucsApp;
class QString;
class QPainter;

class SchematicView : public QGraphicsView, public QucsDoc
{
  Q_OBJECT
public:
  SchematicView (QucsApp *, const QString &);
  virtual ~SchematicView ();

  // function that inherited by QucsDoc
  void setName(const QString&);
  bool load();
  int  save();
  void print(QPrinter*, QPainter*, bool, bool);
  void becomeCurrent(bool);
  float zoomBy(float);
  void showAll();
  void showNoZoom();

  // schematic-special function

signals:
  void signalCursorPosChanged(int, int);

protected slots:

protected:
  virtual void wheelEvent(QWheelEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void resizeEvent(QResizeEvent* event);

private:
  SchematicScene *m_scene;
};

#endif /* SCHEMATICVIEW_H_ */
