/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Jun 24  2004
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <qapplication.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfont.h>
#include <qtextcodec.h>

#include "qucshelp.h"

QDir QucsHelpDir; // directory to find helps files

struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
};

struct tQucsSettings QucsSettings
     = {60, 30, 640, 400,    // position and size
	QFont("Helvetica", 12)};

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  QFile file(QDir::homeDirPath()+"/.qucs/helprc");
  if(!file.open(IO_ReadOnly)) return false; // settings file doesn't exist

  QTextStream stream(&file);
  QString Line, Setting;

  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1,1);
    if(Setting == "HelpWindow") {
	QucsSettings.x  = Line.section(",",0,0).toInt();
	QucsSettings.y  = Line.section(",",1,1).toInt();
	QucsSettings.dx = Line.section(",",2,2).toInt();
	QucsSettings.dy = Line.section(",",3,3).toInt();
	break; }
  }
  file.close();

  file.setName(QDir::homeDirPath()+"/.qucs/qucsrc");
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
bool saveApplSettings(QucsHelp *qucs)
{
  if(qucs->x() == QucsSettings.x)
    if(qucs->y() == QucsSettings.y)
      if(qucs->width() == QucsSettings.dx)
	if(qucs->height() == QucsSettings.dy)
	  return true;   // nothing has changed


  QFile file(QDir::homeDirPath()+"/.qucs/helprc");
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  stream << "Settings file, Qucs Help System " PACKAGE_VERSION "\n"
	 << "HelpWindow=" << qucs->x() << ',' << qucs->y() << ','
	 << qucs->width() << ',' << qucs->height() << '\n';

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
  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  tor.load( QString("qucs_") + QTextCodec::locale(), LANGUAGEDIR );
  a.installTranslator( &tor );

  QString locale = QTextCodec::locale();
  QucsHelpDir = QString (DOCDIR) + locale;
  if (!QucsHelpDir.exists () || !QucsHelpDir.isReadable ()) {
    int p = locale.find ('_');
    if (p != -1) {
      QucsHelpDir = QString (DOCDIR) + locale.left (p);
      if (!QucsHelpDir.exists () || !QucsHelpDir.isReadable ()) {
	QucsHelpDir = QString (DOCDIR) + "en";
      }
    }
    else QucsHelpDir = QString (DOCDIR) + "en";
  }

  QString Page;
  if(argc > 1) Page = argv[1];

  QucsHelp *qucs = new QucsHelp(Page);
  a.setMainWidget(qucs);
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
