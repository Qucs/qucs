/*
 * schematicview.cpp - implementation view of schematic
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

#include <QAction>
#include <QFileInfo>

#include "schematicview.h"
#include "qucs.h"

SchematicView::SchematicView(QucsApp *App_, const QString &Name_)
  : QGraphicsView(App_), QucsDoc(App_, Name_)
{
  m_scene = new SchematicScene(this);
  m_scene->setBackgroundBrush(QColor("white"));
  m_scene->setSceneRect(0, 0, 800, 800);
  setScene(m_scene);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

SchematicView::~SchematicView()
{
}

// ---------------------------------------------------
void SchematicView::becomeCurrent(bool update)
{
  QString *ps;
  emit signalCursorPosChanged(0, 0);

  // update appropriate menu entry
  bool symbolMode = 0;
  if (symbolMode) {
    if (DocName.right(4) == ".sym") {
      App->symEdit->setText(tr("Edit Text"));
      App->symEdit->setStatusTip(tr("Edits the Text"));
      App->symEdit->setWhatsThis(tr("Edit Text\n\nEdits the text file"));
    }
    else {
      App->symEdit->setText(tr("Edit Schematic"));
      App->symEdit->setStatusTip(tr("Edits the schematic"));
      App->symEdit->setWhatsThis(tr("Edit Schematic\n\nEdits the schematic"));
    }
  }
  else {
    App->symEdit->setText(tr("Edit Circuit Symbol"));
    App->symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
    App->symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  }

  if(symbolMode) {
  }
  else {
  }
}

// ---------------------------------------------------
void SchematicView::setName (const QString& Name_)
{
  DocName = Name_;
  QFileInfo Info (DocName);
  QString base = Info.completeBaseName ();
  QString ext = Info.suffix();
  DataSet = base + ".dat";
  Script = base + ".m";
  if (ext != "dpl")
    DataDisplay = base + ".dpl";
  else
    DataDisplay = base + ".sch";
}

bool SchematicView::load()
{
  return false;
}

int SchematicView::save()
{
  return 0;
}

void SchematicView::print(QPrinter *, QPainter *, bool, bool)
{
}

float SchematicView::zoomBy(float)
{
}

void SchematicView::showAll()
{
}

void SchematicView::showNoZoom()
{
}

void SchematicView::wheelEvent(QWheelEvent *event)
{
  QGraphicsView::wheelEvent(event);
}

void SchematicView::keyPressEvent(QKeyEvent *event)
{
  QGraphicsView::keyPressEvent(event);
}

void SchematicView::resizeEvent(QResizeEvent *event)
{
  QGraphicsView::resizeEvent(event);
}
