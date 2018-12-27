/***************************************************************************
                                 spicefile.h
                                -------------
    begin                : Tue Dec 28 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPICEFILE_H
#define SPICEFILE_H
#include "component.h"

#include <QObject>
#include <QDateTime>

class QProcess;
class QTextStream;
class QString;

class SpiceFile : public QObject, public MultiViewComponent  {
 //  Q_OBJECT wtf?
public:
  using MultiViewComponent::name;
public:
  SpiceFile();
 ~SpiceFile() {};
  Object* newOne() const {
	  // BUG. {return new SpiceFile(*this);}
	SpiceFile *p = new SpiceFile();
	p->recreate(0);   // createSymbol() is NOT called in constructor !!!
	return p;
  }

// -------------------------------------------------------
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool withSim;
  bool createSubNetlist(QTextStream *);
  QString getErrorText() { return ErrText; }
  QString getSubcircuitFile() const;

private:
  bool makeSubcircuit;
  bool insertSim;
  bool changed;
  QProcess *QucsConv, *SpicePrep;
  QString NetText, ErrText, NetLine, SimText;
  QTextStream *outstream, *filstream, *prestream;
  QDateTime lastLoaded;
  bool recreateSubNetlist(QString *, QString *);

protected:
  QString netlist() const;
  void createSymbol();

private slots:
  void slotGetNetlist();
  void slotGetError();
  void slotExited();
  void slotSkipOut();
  void slotSkipErr();
  void slotGetPrepOut();
  void slotGetPrepErr();
};

#endif
