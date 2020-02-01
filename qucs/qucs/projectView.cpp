/*
 * ProjectView.cpp - implementation of project model
 *   the model manage the files in project directory
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

#include "projectView.h"
#include "schematic.h"

#include <QString>
#include <QStringList>
#include <QDir>
#include <QStandardItemModel>
#include <QFileSystemWatcher>
#include <QDebug>

ProjectView::ProjectView(QWidget *parent)
  : QTreeView(parent)
{
  m_projPath = QString();
  m_projName = QString();
  m_valid = false;
  m_model = new QStandardItemModel(8, 2, this);
  watcher = new QFileSystemWatcher(this);
  connect(watcher, SIGNAL(directoryChanged(const QString&)), SLOT(dirChanged(const QString&)));

  init();

  this->setModel(m_model);
  this->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

ProjectView::~ProjectView()
{
  delete m_model;
  delete watcher;
}

void
ProjectView::setProjPath(const QString &path)
{
  // check if path exist
  m_valid = !path.isEmpty() && QDir(path).exists();

  if (m_valid) {
    if (!m_projPath.isEmpty()) {
      watcher->removePath(m_projPath); // stop watching the previous directry
    }
    m_projPath = path; // full path
    watcher->addPath(path); // start watching the current directory
    m_projName = QDir(m_projPath).dirName(); // only project directory name
    if (m_projName.endsWith("_prj")) {
      m_projName.chop(4);// remove "_prj" from name
    } else { // should not happen
      qWarning() << "ProjectView::setProjPath() : path does not end in '_prj' (" << m_projName << ")";
    }
  }
  refresh();
  // expand only the Schematics section, to show all the schematic files
  for (int i=0; i<m_model->rowCount(); i++) {
    setExpanded(m_model->index(i, 0), i==6);
  }
  // make sure the whole schematics name are shown
  resizeColumnToContents(0);
}

// initialize the view
void
ProjectView::init()
{
  m_model->clear();

  APPEND_ROW(m_model, tr("Datasets")     );
  APPEND_ROW(m_model, tr("Data Displays"));
  APPEND_ROW(m_model, tr("Verilog")      );
  APPEND_ROW(m_model, tr("Verilog-A")    );
  APPEND_ROW(m_model, tr("VHDL")         );
  APPEND_ROW(m_model, tr("Octave")       );
  APPEND_ROW(m_model, tr("Schematics")   );
  APPEND_ROW(m_model, tr("Others")       );

  if (!m_valid) {
    return;
  }
}

// refresh the view using the current projectPath
void
ProjectView::refresh()
{
  // put all files into "Content"-ListView
  QDir workPath(m_projPath);
  QStringList files = workPath.entryList(QStringList() << "*", QDir::Files, QDir::Name);
  QStringList::iterator it;
  QString extName, fileName;
  QList<QStandardItem *> columnData;

  // update project name in header as it may have changed
  QStringList header;
  header << tr("Content of %1").arg(m_projName) << tr("Note");
  m_model->setHorizontalHeaderLabels(header);

  for (int i=0; i<m_model->rowCount(); i++) {
    // delete_childrens
    m_model->item(i, 0)->removeRows(0, m_model->item(i, 0)->rowCount());
  }

#define APPEND_CHILD(category, data) \
  m_model->item(category, 0)->appendRow(data);

  for(it = files.begin(); it != files.end(); ++it) {
    fileName = (*it).toAscii();
    extName = QFileInfo(workPath.filePath(fileName)).suffix();

    columnData.clear();
    QStandardItem * d = new QStandardItem(fileName);
    if (QucsSettings.ShowDescriptionProjectTree)
    { // In case of the ShowDescriptionProjectTree property is set, 
      // it reads the schematic header looking for the message to be displayed
      // and the variable which sets the visibility of the frame
       QString description = ReadDescription(workPath.absoluteFilePath(fileName));
       d->setToolTip(description);
    }
    columnData.append(d);

    if(extName == "dat") {
      APPEND_CHILD(0, columnData);
    }
    else if(extName == "dpl") {
      APPEND_CHILD(1, columnData);
    }
    else if(extName == "v") {
      APPEND_CHILD(2, columnData);
    }
    else if(extName == "va") {
      APPEND_CHILD(3, columnData);
    }
    else if((extName == "vhdl") || (extName == "vhd")) {
      APPEND_CHILD(4, columnData);
    }
    else if((extName == "m") || (extName == "oct")) {
      APPEND_CHILD(5, columnData);
    }
    else if(extName == "sch") {
      // test if it's a valid schematic file
      int n = Schematic::testFile(workPath.filePath(fileName));
      if(n >= 0) {
        if(n > 0) { // is a subcircuit
          columnData.append(new QStandardItem(QString::number(n)+tr("-port")));
        }
        APPEND_CHILD(6, columnData);
      }
    }
    else {
      APPEND_CHILD(7, columnData);
    }
  }

  resizeColumnToContents(0);
}

QStringList
ProjectView::exportSchematic()
{
  QStringList list;
  QStandardItem *item = m_model->item(6, 0);
  for (int i = 0; i < item->rowCount(); ++i) {
    if (item->child(i,1)) {
      list.append(item->child(i,0)->text());
    }
  }
  return list;
}

void ProjectView::dirChanged(const QString &path)
{
  Q_UNUSED(path);
  //qDebug() << "watcher:" << path;
  refresh();
}

// This function reads the text inside the <description></description> tags from the given file location
QString ProjectView::ReadDescription(QString file)
{
    QFile QucsDocument(file);
    if (!QucsDocument.open(QIODevice::ReadOnly)) return "";
    QTextStream in (&QucsDocument);
    QString line, description;
    int showFrame;
    int index, index2;
    do {
        line = in.readLine();
        index = line.indexOf("FrameText0=");
        if (index != -1) {
            index2 = line.indexOf(">", index);
            index+=11;//Skip FrameText0=
            description = line.mid(index, index2-index);
        }
        index = line.indexOf("showFrame=");
        if (index != -1) {
            index2 = line.indexOf(">", index);
            index+=10;//Skip showFrame=
            showFrame = line.mid(index, index2-index).toInt();
        }
    } while (!line.isNull());

    description.replace("\\n", "<br>");
    QucsDocument.close();
    if ((description == tr("Title")) || (showFrame == 0)) description = "";//Don't show the tooltip
    return description;
}
