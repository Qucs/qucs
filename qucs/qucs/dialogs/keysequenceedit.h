/*--------------------------------------------------------------------------------
-*- coding: utf-8 -*-

Filename: keysequenceedit.h
This file is part of project: qucs.
Description: A inheritance to QLineEdit that detect user input hotkey

Copyright (C) 2014 -  You-Tang Lee (YodaLee) <lc85301@gmail.com>
All Rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--------------------------------------------------------------------------------*/

#ifndef KEYSEQUENCEEDIT_H_
#define KEYSEQUENCEEDIT_H_ value

#include <QLineEdit>

class KeySequenceEdit : public QLineEdit
{
  Q_OBJECT
public:
  KeySequenceEdit (QWidget *parent = 0);
  KeySequenceEdit (const QKeySequence &keySequence, QWidget *parent = 0);
  virtual ~KeySequenceEdit ();
protected:
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);
private slots:
  void setKeySequence(const QKeySequence &);
private:
  int keyInt;
  
};


#endif /* end of include guard: KEYSEQUENCEEDIT_H_ */
