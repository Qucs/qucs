/*
 * whatsnewdialog.h - dialog showing textual info about the current release
 *
 * Copyright (C) 2015, Qucs team (see AUTHORS file)
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

#ifndef WHATSNEW_H
#define WHATSNEW_H


#include <QDialog>
#include <QVBoxLayout>
#include <QCheckBox>

class QString;
class QTextBrowser;

class WhatsNewDialog : public QDialog  {
   Q_OBJECT

public:
  WhatsNewDialog(QWidget *parent = 0);
  bool getCheckBoxState();
  void setCheckBoxState(bool state);

protected:
  bool eventFilter(QObject *obj, QEvent *ev);

private:
  QVBoxLayout *all;
  QTextBrowser *mainBrowser;
  QCheckBox *dnsaChk;
};

#endif
