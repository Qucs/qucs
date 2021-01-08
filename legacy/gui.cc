
#include "command.h"
#include "globals.h"
#include "qucs.h"
#include "module.h"
#include "settings.h"
#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QDesktopWidget>

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
		a.setFont(QucsSettings.font);

		{
			// set codecs
			QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
			QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

			QTranslator tor( 0 );
			QString lang = QucsSettings.Language;
			if(lang.isEmpty()) {
				QLocale loc;
				lang = loc.name();
			}else{ untested();
			}
			tor.load( QString("qucs_") + lang, QucsSettings.LangDir);

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
