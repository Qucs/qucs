/***************************************************************************
    copyright            : (C) 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_TABS_H
#define QUCS_TABS_H

#include <QTabWidget>

class QucsApp;
class QucsDoc;

class QucsTabWidget : public QTabWidget
{
  Q_OBJECT
public:
  QucsTabWidget(QucsApp *parent = 0);
  void setSaveIcon(bool state=true, int index=-1);
public: // BUG, obsolete.
  QucsDoc *createEmptySchematic(const QString &name);
  QucsDoc *createEmptyTextDoc(const QString &name);

  QucsDoc* current();

  void setCurrentIndex(int i);

public slots:
  void showContextMenu(const QPoint& point);
private slots:
  void slotCxMenuClose();
  void slotCxMenuCloseOthers();
  void slotCxMenuCloseAll();
  void slotCxMenuCloseRight();
  void slotCxMenuCloseLeft();
  void slotCxMenuCopyPath();
  void slotCxMenuOpenFolder();

public slots:
  void slotDCbias();

private: // BUG
  QString docName(); // name of the document where context menu was opened

private:
  int contextTabIndex; // what?
  QucsApp *App; // BUG
  QucsDoc* _current{nullptr};
};

#endif
