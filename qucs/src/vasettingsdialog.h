/***************************************************************************
                             vasettingsdialog.h
                            --------------------
    begin                : Sun Oct 26 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefa@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VASETTINGSDIALOG_H
#define VASETTINGSDIALOG_H

#include <QDialog>
#include <QRegExp>
#include <QLabel>

class TextDoc;
class QLineEdit;
class QPushButton;
class QRegExpValidator;
class QLabel;
class QCheckBox;
class QButtonGroup;
class QVBoxLayout;

class VASettingsDialog : public QDialog  {
Q_OBJECT
public:
  VASettingsDialog (TextDoc *);
 ~VASettingsDialog ();

  QLineEdit * IconEdit{nullptr}, * OutputEdit{nullptr}, * NameEdit{nullptr}, * ShortDescEdit{nullptr},
    * LongDescEdit{nullptr};
  QPushButton * BrowseButt{nullptr};
  QLabel * IconButt{nullptr};
  QCheckBox * RecreateCheck{nullptr};
  QButtonGroup * toggleGroupDev{nullptr}, * toggleGroupTyp{nullptr};

private slots:
  void slotOk ();
  void slotBrowse ();

private:
  TextDoc * Doc{nullptr};
  QRegExp Expr;
  QRegExpValidator * Validator{nullptr};
  QVBoxLayout *vLayout{nullptr};
};

#endif
