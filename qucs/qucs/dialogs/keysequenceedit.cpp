/*--------------------------------------------------------------------------------
-*- coding: utf-8 -*-

Filename: keysequenceedit.cpp
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

#include <QLineEdit>
#include <QDebug>
#include <QKeyEvent>

#include "keysequenceedit.h"

using namespace std;

KeySequenceEdit::KeySequenceEdit(QWidget *parent)
  : QLineEdit(parent)
{
}

KeySequenceEdit::KeySequenceEdit(const QKeySequence &keySequence, QWidget *parent)
  : QLineEdit(parent)
{
  setKeySequence(keySequence);
}

KeySequenceEdit::~KeySequenceEdit()
{
}

void
KeySequenceEdit::setKeySequence(const QKeySequence &keySequence)
{
  setText(QString(keySequence));
}

void 
KeySequenceEdit::keyPressEvent(QKeyEvent *event) 
{
  keyInt = event->key(); 
  Qt::Key key = static_cast<Qt::Key>(keyInt); 
  if(key == Qt::Key_unknown){ 
    qDebug() << "Unknown key from a macro probably"; 
    return; 
  } 
  // the user have clicked just and only the special keys Ctrl, Shift, Alt, Meta. 
  if(key == Qt::Key_Control || 
      key == Qt::Key_Shift || 
      key == Qt::Key_Alt || 
      key == Qt::Key_Meta) { 
    return; 
  } 

  // check for a combination of user clicks 
  // if the keyText is empty than it's a special key like F1, F5, ... 
  Qt::KeyboardModifiers modifiers = event->modifiers(); 
  if(modifiers & Qt::ShiftModifier) 
    keyInt += Qt::SHIFT; 
  if(modifiers & Qt::ControlModifier) 
    keyInt += Qt::CTRL; 
  if(modifiers & Qt::AltModifier) 
    keyInt += Qt::ALT; 
  if(modifiers & Qt::MetaModifier) 
    keyInt += Qt::META; 
} 

void
KeySequenceEdit::keyReleaseEvent(QKeyEvent *event)
{
  qDebug() << "Key Release: " << QKeySequence(keyInt).toString(QKeySequence::NativeText);
  setText(QKeySequence(keyInt).toString(QKeySequence::NativeText));
  clearFocus();
}
