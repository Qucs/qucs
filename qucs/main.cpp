/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Aug 28 18:17:41 CEST 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "qucs.h"
#include "qucsview.h"
#include "main.h"
#include "node.h"

#include <qapplication.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>

#include <math.h>



tQucsSettings QucsSettings
     = {0, 0, 600, 400,    // position and size
	QFont("Helvetica", 12), 16.0,
	QColor(255, 250, 225), 20,
	BINARYDIR "qucsedit"};

QFont savingFont;    // to remember which font to save in "qucsrc"

// just dummies for empty lists
QPtrList<Wire>      SymbolWires;
QPtrList<Node>      SymbolNodes;
QPtrList<Diagram>   SymbolDiags;
QPtrList<Component> SymbolComps;

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  QFile file(QucsHomeDir.filePath("qucsrc"));
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
	savingFont = QucsSettings.font;

	QucsSettings.largeFontSize
		= floor(4.0/3.0 * QucsSettings.font.pointSize());
	}
    else if(Setting == "BGColor") {
	QucsSettings.BGColor.setNamedColor(Line);
	if(!QucsSettings.BGColor.isValid())
	  QucsSettings.BGColor.setRgb(255, 250, 225); }
    else if(Setting == "maxUndo") {
	QucsSettings.maxUndo = Line.toInt(&ok); }
    else if(Setting == "Editor") {
	QucsSettings.Editor = Line; }
  }

  file.close();
  return true;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsApp *qucs)
{
  QFile file(QucsHomeDir.filePath("qucsrc"));
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
    << "Font=" << savingFont.toString() << "\n"
    << "BGColor=" << qucs->view->viewport()->paletteBackgroundColor().name()
    << "\n"
    << "maxUndo=" << QucsSettings.maxUndo << "\n"
    << "Editor=" << QucsSettings.Editor << "\n";
  file.close();

  return true;
}

// #########################################################################
QString complexRect(double real, double imag, int Precision)
{
  QString Text;
  if(fabs(imag) < 1e-250) Text = QString::number(real,'g',Precision);
  else {
    Text = QString::number(imag,'g',Precision);
    if(Text.at(0) == '-') {
      Text.at(0) = 'j';
      Text = '-'+Text;
    }
    else  Text = "+j"+Text;
    Text = QString::number(real,'g',Precision) + Text;
  }
  return Text;
}

QString complexDeg(double real, double imag, int Precision)
{
  QString Text;
  if(fabs(imag) < 1e-250) Text = QString::number(real,'g',Precision);
  else {
    Text  = QString::number(sqrt(real*real+imag*imag),'g',Precision) + " / ";
    Text += QString::number(180.0/M_PI*atan2(imag,real),'g',Precision) + '°';
  }
  return Text;
}

QString complexRad (double real, double imag, int Precision)
{
  QString Text;
  if(fabs(imag) < 1e-250) Text = QString::number(real,'g',Precision);
  else {
    Text  = QString::number(sqrt(real*real+imag*imag),'g',Precision);
    Text += " / " + QString::number(atan2(imag,real),'g',Precision) + "rad";
  }
  return Text;
}

// #########################################################################
QString StringNum(double num, char form, int Precision)
{
  int a = 0;
  char *p, Buffer[512], Format[6] = "%.00g";
  QString s;

  Format[4]  = form;
  Format[2] += Precision / 10;
  Format[3] += Precision % 10;
  sprintf(Buffer, Format, num);
  p = strchr(Buffer, 'e');
  if(p) {
    p++;
    if(*(p++) == '+') { a = 1; }
    if(*p == '0') { a++; p++; }
    if(a > 0)
      do {
        *(p-a) = *p;
      } while(*(p++) != 0);
  }

  s = Buffer;
  return s;
}


// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################

int main(int argc, char *argv[])
{
  QucsWorkDir.setPath(QDir::homeDirPath()+"/.qucs");
  QucsHomeDir.setPath(QDir::homeDirPath()+"/.qucs");
  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  tor.load( QString("qucs_") + QTextCodec::locale(), LANGUAGEDIR );
  a.installTranslator( &tor );

  QucsApp *qucs = new QucsApp();
  a.setMainWidget(qucs);
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
