/***************************************************************************
    copyright            : 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "command.h"
#include "io.h"
#include "qucs_globals.h"
#include "qucs_app.h"
#include "module.h"
#include "settings.h"
#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QDesktopWidget>
#include "qt_compat.h"

namespace{

class g: public Command{
	void do_it(istream_t& cs, SchematicModel*) override{

		cs >> "startgui";
		std::string filename;
		cs >> filename;

		int argc=0;
		char** argv=nullptr;

		QApplication a(argc, argv);
		//  Q_INIT_RESOURCE();
		QDesktopWidget *d = a.desktop();
		int w = d->width();
		int h = d->height();
		QucsSettings.x = w/8;
		QucsSettings.y = h/8;
		QucsSettings.dx = w*3/4;
		QucsSettings.dy = h*3/4;
		a.setFont(QString_(QucsSettings.font));

		{
			// set codecs
			QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
			QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

			QTranslator tor( 0 );
			QString lang = QString_(QucsSettings.Language);
			if(lang.isEmpty()) {
				QLocale loc;
				lang = loc.name();
			}else{ untested();
			}
			tor.load( QString("qucs_") + lang, QString_(QucsSettings.LangDir));

			a.installTranslator( &tor );
		}

		Module::registerModules (); // BUG
		auto QucsMain = new QucsApp();

		QucsMain->show();
		QucsMain->openFileAtStartup(QString::fromStdString(filename));
		int result = a.exec();
		exit(result); // FIXME: throw exception_exit or so
		//saveApplSettings(QucsMain);
	}
}p;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "startgui", &p);
}// namespace
