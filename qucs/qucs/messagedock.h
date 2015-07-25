/***************************************************************************
                             messagedock.h
                             -------------
    begin                : Tue Mar 11 2014
    copyright            : (C) 2014 by Guilherme Brondani Torri
    email                : guitorri AT gmail DOT com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGEDOCK_H
#define MESSAGEDOCK_H

#include "qucs.h"

#include <QWidget>

class QDockWidget;
class QTabWidget;
class QPlainTextEdit;

/*!
 * \file messagedock.h
 * \brief The MessagesWindow class definition (nvdl: todo: It is not a dock anymore)
 */
class MessagesWindow : public QWidget {
  Q_OBJECT
public:
  //MessageDock(QucsApp*);
  MessagesWindow(QDockWidget*);
 ~MessagesWindow() {};

public:

  //QDockWidget *msgDock;

 QTabWidget *builderTabs;

  /*!
   * \brief admsOutput holds the make output of running admsXml
   */
  QPlainTextEdit *admsOutput;
  /*!
   * \brief cppOutput holds the make output of running a C++ compiler
   */
  QPlainTextEdit *cppOutput;

  /*!
   * \brief To hold the program messages, warnings and errors
   */
  QPlainTextEdit *messages, *warnings, *errors;

  void message(QString);
  void warning(QString);
  void error(QString);
  void reset();

  QDockWidget *parent;

private slots:
  void slotAdmsChanged();
  void slotCppChanged();
  void slotCursor();
};

#endif // MESSAGEDOCK_H
