/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
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

#include "qucs.h"
#include "main.h"



tQucsSettings QucsSettings
     = {0, 0, 600, 400,    // position and size
	QFont("Helvetica", 12), QFont("Helvetica", 16, QFont::DemiBold),
	QFont("Helvetica", 10, QFont::Light),
	QColor(255, 250, 225)};

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  QFile file(QDir::homeDirPath()+"/.qucs/qucsrc");
  if(!file.open(IO_ReadOnly)) return false; // settings file doesn't exist

  QTextStream stream(&file);
  QString Line, Setting;

  bool ok;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Setting = Line.section('=',0,0);
    Line    = Line.section('=',1,1);
    if(Setting == "Position") {
	QucsSettings.x = Line.section(",",0,0).toInt(&ok);
	QucsSettings.y = Line.section(",",1,1).toInt(&ok); }
    else if(Setting == "Size") {
	QucsSettings.dx = Line.section(",",0,0).toInt(&ok);
	QucsSettings.dy = Line.section(",",1,1).toInt(&ok); }
    else if(Setting == "Font") {
	QucsSettings.font.fromString(Line);

	QucsSettings.largeFont = QucsSettings.font;
	int i = QucsSettings.font.pointSize();  i += i/3;
	QucsSettings.largeFont.setPointSize(i);   // large font greater
	QucsSettings.largeFont.setWeight(QFont::DemiBold);

	QucsSettings.smallFont = QucsSettings.font;
	QucsSettings.smallFont.setPointSize(10);   // small font 10pt
	QucsSettings.smallFont.setWeight(QFont::Light);
	}
    else if(Setting == "BGColor") {
	QucsSettings.BGColor.setNamedColor(Line);
	if(!QucsSettings.BGColor.isValid())
	  QucsSettings.BGColor.setRgb(255, 250, 225); }
  }

  file.close();
  return true;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsApp *qucs)
{
  QFile file(QDir::homeDirPath()+"/.qucs/qucsrc");
  if(!file.open(IO_WriteOnly)) {    // settings file cannot be created
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  stream << "Settings file, Qucs " PACKAGE_VERSION "\n"
    << "Position=" << qucs->x() << "," << qucs->y() << "\n"
    << "Size=" << qucs->width() << "," << qucs->height() << "\n"
    << "Font=" << QucsSettings.font.toString() << "\n"
    << "BGColor=" << qucs->view->viewport()->paletteBackgroundColor().name()
    << "\n";
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

  QucsApp *qucs = new QucsApp();
  a.setMainWidget(qucs);
  qucs->show();
  return a.exec();
}
