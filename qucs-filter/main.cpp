/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
    copyright            : (C) 2005 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>

#include <qapplication.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfont.h>

#include "qucsfilter.h"

struct tQucsSettings QucsSettings;

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/filterrc"));
  if(!file.open(IO_ReadOnly)) return false; // settings file doesn't exist

  QTextStream stream(&file);
  QString Line, Setting;

  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1,1);
    if(Setting == "FilterWindow") {
	QucsSettings.x  = Line.section(",",0,0).toInt();
	QucsSettings.y  = Line.section(",",1,1).toInt();
	break; }
  }
  file.close();

  file.setName(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/qucsrc"));
  if(!file.open(IO_ReadOnly)) return true; // qucs settings not necessary

  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1,1);
    if(Setting == "Font") {     // get font from "qucsrc"
	QucsSettings.font.fromString(Line);
	break;
    }
  }
  file.close();

  return true;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsFilter *qucs)
{
  if(qucs->x() == QucsSettings.x)
    if(qucs->y() == QucsSettings.y)
      return true;   // nothing has changed


  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/filterrc"));
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  stream << "Settings file, Qucs Filter " PACKAGE_VERSION "\n"
	 << "FilterWindow=" << qucs->x() << ',' << qucs->y() << '\n';

  file.close();
  return true;
}


// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################

int main(int argc, char *argv[])
{
  // apply default settings
  QucsSettings.x = 200;
  QucsSettings.y = 100;
  QucsSettings.font = QFont("Helvetica", 12);

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  if (var != NULL) {
    QDir QucsDir = QDir (var);
    QString QucsDirStr = QucsDir.canonicalPath ();
    QucsSettings.BitmapDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/bitmaps/");
    QucsSettings.LangDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/lang/");
  } else {
    QucsSettings.BitmapDir = BITMAPDIR;
    QucsSettings.LangDir = LANGUAGEDIR;
  }

  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  tor.load( QString("qucs_") + QTextCodec::locale(), QucsSettings.LangDir );
  a.installTranslator( &tor );

  QucsFilter *qucs = new QucsFilter();
  a.setMainWidget(qucs);
  qucs->move(QucsSettings.x, QucsSettings.y);  // position before "show" !!!
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
