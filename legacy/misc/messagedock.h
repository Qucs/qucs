/***************************************************************************
    copyright            : (C) 2014 by Guilherme Brondani Torri
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_MESSAGEDOCK_H
#define QUCS_MESSAGEDOCK_H

#include "qucs_app.h"

#include <QWidget>

class QDockWidget;
class QTabWidget;
class QPlainTextEdit;

namespace qucs {
	class App;
}

class MessageDock : public QWidget {
  Q_OBJECT
public:
  MessageDock(qucs::App*);
 ~MessageDock() {};

public:

  QDockWidget *msgDock;

  QTabWidget *builderTabs;

  /*!
   * \brief admsOutput holds the make output of running admsXml
   */
  QPlainTextEdit *admsOutput;
  /*!
   * \brief cppOutput holds the make output of running a C++ compiler
   */
  QPlainTextEdit *cppOutput;

  void reset();


private slots:
  void slotAdmsChanged();
  void slotCppChanged();
  void slotCursor();

};

#endif // MESSAGEDOCK_H
