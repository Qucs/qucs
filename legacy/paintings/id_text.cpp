/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "viewpainter.h"
#include "qucs_globals.h"
#include "module.h"
#include "../legacy_painting.h"

// yikes.
#include <QPainter>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>

#include "qt_compat.h"

#include <QDialog> // BUG
#include <QRegExp> // BUG

#include "id_dialog.h" //for now.

namespace{

struct SubParameter {
  SubParameter(bool display_, const QString& Name_, const QString& Descr_)
     : display(display_), Name(Name_), Description(Descr_) { Type = ""; };
  SubParameter(bool display_, const QString& Name_, const QString& Descr_,
	       const QString& Type_)
     : display(display_), Name(Name_), Description(Descr_), Type(Type_) {};

  bool display;
  QString Name, Description, Type;
};


// not sure what this is.
class ID_Text : public LegacyPainting  {
public:
  ID_Text(int cx_=0, int cy_=0);
  ~ID_Text();
  Element* clone() const{
	  return new ID_Text(*this);
  }

  void getCenter(int&, int&);

  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*);
  bool getSelected(float, float, float);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QString Prefix;
  QList<SubParameter *> Parameter;
};
ID_Text D;
Dispatcher<Element>::INSTALL p(&element_dispatcher, ".ID", &D);
Module::INSTALL pp("paintings", &D);
}

ID_Text::ID_Text(int cx_, int cy_) : LegacyPainting(cx_, cy_)
{
  Name = ".ID ";
  x2 = y2 = 20;

  Prefix = ""; // what?!

  setLabel("?????");
}

ID_Text::~ID_Text()
{
}

// --------------------------------------------------------------------------
void ID_Text::paint(ViewPainter *p)
{
	// incomplete();
	return;

	auto cx=0;
	auto cy=0;
  int x, y;
  p->setPen(QPen(Qt::black,1));
  p->map(cx, cy, x, y);

  QRect r;
  p->drawText(QRect(x, y, 0, 0), Qt::TextDontClip, Prefix, &r);
  x2 = r.width();
  y2 = p->LineSpacing;

  p->drawText(QRect(x, y+y2, 0, 0), Qt::TextDontClip, "File=name", &r);
  if(x2 < r.width())  x2 = r.width();
  y2 += p->LineSpacing;

  QList<SubParameter *>::const_iterator it;
  for(it = Parameter.constBegin(); it != Parameter.constEnd(); it++) {
    if((*it)->display) {
      p->drawText(QRect(x, y+y2, 0, 0), Qt::TextDontClip, (*it)->Name, &r);
      if(x2 < r.width())  x2 = r.width();
      y2 += p->LineSpacing;
    }
  }

  x2 = int(float(x2) / p->Scale);
  y2 = int(float(y2) / p->Scale);
}

// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
void ID_Text::getCenter(int& x, int &y)
{
	  auto cx=Element::cx();
      auto cy=Element::cy();
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
//void ID_Text::setCenter(int x, int y, bool relative)
//{
//	  auto cx=Element::cx();
//      auto cy=Element::cy();
//  if(relative) { cx += x;  cy += y; }
//  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
//}

// --------------------------------------------------------------------------
bool ID_Text::load(const QString& s)
{
	incomplete(); // fix later.
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  int cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  int cy = n.toInt(&ok);
  if(!ok) return false;

  setPosition(pos_t(cx, cy));

  Prefix = s.section(' ',3,3);    // Prefix
  if(Prefix.isEmpty()) return false;

  int i = 1;
  for(;;) {
    n = s.section('"', i,i);
    if(n.isEmpty())  break;

    Parameter.append(new SubParameter(
       (n.at(0) == '0') ? false : true,
       n.section('=', 1,2),
       n.section('=', 3,3),
       n.section('=', 4,4)));

    i += 2;
  }

  return true;
}

// --------------------------------------------------------------------------
QString ID_Text::save()
{
	  auto cx=Element::cx();
     auto cy=Element::cy();
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
//  s += Prefix; //what?
  s += QString::fromStdString(label());

  QList<SubParameter *>::const_iterator it;
  for(it = Parameter.constBegin(); it != Parameter.constEnd(); it++) {
    s += (((*it)->display)? " \"1=" : " \"0=");
    s += (*it)->Name + "=" + (*it)->Description + "=" + (*it)->Type + "\"";
  }

  return s;
}

// --------------------------------------------------------------------------
#if 0
QString ID_Text::saveCpp()
{
  QString s =
    QString ("tx = %1; ty = %2;").arg(cx()).arg(cy());
  return s;
}

QString ID_Text::saveJSON()
{
  QString s =  QString ("\"tx\" : %1,\n  \"ty\" : %2,").arg(cx()).arg(cy());
  return s;
}
#endif

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool ID_Text::getSelected(float fX, float fY, float)
{
	 auto cx=Element::cx();
     auto cy=Element::cy();

  if(int(fX) < cx)  return false;
  if(int(fY) < cy)  return false;
  if(int(fX) > cx+x2)  return false;
  if(int(fY) > cy+y2)  return false;

  return true;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void ID_Text::rotate()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void ID_Text::mirrorX()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void ID_Text::mirrorY()
{
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool ID_Text::Dialog()
{
  ID_Dialog *d = new ID_Dialog(this);
  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  delete d;
  return true;
}

#include "id_dialog.cpp" //for now.
