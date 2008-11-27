/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#include "vtabbutton.h"
#include <qstring.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qapplication.h>

VTab::VTab(VTabPosition pos,int p_id,QWidget *p,const char* n) : QPushButton(p,n)
{
  m_position = pos;
  m_id = p_id;
  init();
}

void VTab::init()
{
  setFlat(true);
  setToggleButton(true);
  setAutoDefault(false);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  connect(this,SIGNAL(toggled(bool )),SLOT(slotToggled(bool)));
}

VTab::~VTab()
{}

void VTab::setID(int p_id)
{
  m_id = p_id;
}

void VTab::setPosition(VTabPosition p_pos)
{
  if(m_position != p_pos)
    {
      m_position = p_pos;
      repaint();
    }
}


QSize VTab::sizeHint() const
{
  constPolish();
  QString _text = text();
  if(_text.isNull())
    _text = "XXX";
  QFontMetrics fm = this->fontMetrics();
  int w = fm.height()+5;
  int h = 24+fm.width(_text);
  return style().sizeFromContents(QStyle::CT_ToolButton, this,
				  QSize(w, h)).expandedTo(QApplication::globalStrut());
}

void VTab::drawButton(QPainter *p)
{
  p->save();
  QStyle::SFlags st = QStyle::Style_Default | QStyle::Style_Enabled;
  if (isOn())
    st|=QStyle::Style_On;

  QRect r(0, 0, height(), width());

  if (m_position == TabLeft) {
    p->translate(0, height());
    p->rotate(-90);
  }
  else {
    p->translate(width(), 0);
    p->rotate(90);
  }

  style().drawControl(QStyle::CE_PushButton, p, this, r, colorGroup(), st);
  style().drawControl(QStyle::CE_PushButtonLabel, p, this, r, colorGroup(), st);

  p->restore();
}

void VTab::setText(const QString &s)
{
  QPushButton::setText(s);
  const QSize sz = sizeHint();
  setFixedHeight(sz.height());
  setFixedWidth(sz.width());
  updateGeometry();
}

void VTab::slotToggled(bool b)
{
  emit toggled(m_id,b);
}
