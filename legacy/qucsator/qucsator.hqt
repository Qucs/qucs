/***************************************************************************
    copyright            : (C) 2015, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_QUCSATOR_H
#define QUCS_QUCSATOR_H
#include <QProcess>
#include "io_trace.h"

#include "simulator.h"
#include "platform.h"
#include "sckt_base.h"
#include "net.h"
#include "docfmt.h" // <<
#include "qucs_globals.h"
#include "settings.h" //??
//#include "schematic_doc.h"
#include <QProcess>
#include "painting.h"
#include "qio.h"
#include "node.h"
#include "qucsdoc.h"
#include "qt_compat.h"
/* -------------------------------------------------------------------------------- */
static const std::string typesep(":");
static const char _typesep = ':';
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
using namespace qucs;
/* -------------------------------------------------------------------------------- */
class Qucsator;
/* -------------------------------------------------------------------------------- */
// TODO: merge QucsatorProcess into Qucsator
class QucsatorProcess : public QProcess {
	Q_OBJECT;
public:
	explicit QucsatorProcess(Qucsator* q) : QProcess(), _simulator(q) {
	}

	void start(QString const& a, QStringList const& b){itested();
		trace0("QucsatorProcess::start");
		disconnect(this, 0, 0, 0);
		connect(this, SIGNAL(stateChanged(QProcess::ProcessState)),
				SLOT(slotStateChanged(QProcess::ProcessState)));
		connect(this, SIGNAL(readyReadStandardError()), SLOT(stderr_()));
		connect(this, SIGNAL(readyReadStandardOutput()), SLOT(stdout_()));

		QProcess::start(a, b);
		trace0("QucsatorProcess started");
	}

protected slots:
	void slotStateChanged(QProcess::ProcessState newState); // { incomplete(); }
	void stdout_();
	void stderr_();

private:
	QProcess::ProcessState oldState;
	Qucsator* _simulator;
}; // QucsatorProcess

// not sure what this is about
/// #ifdef __MINGW32__ // -> platform.h
/// #include <windows.h>
/// static QString pathName(QString longpath) { untested();
///   const char * lpath = QDir::toNativeSeparators(longpath).toAscii();
///   char spath[2048];
///   int len = GetShortPathNameA(lpath,spath,sizeof(spath)-1);
///   spath[len] = '\0';
///   return QString(spath);
/// }
/// #else
/// static QString pathName(QString longpath) { untested();
///   return longpath;
/// }
/// #endif
/* -------------------------------------------------------------------------------- */
static std::string netLabel(Node const* nn)
{ untested();
	if(!nn){ untested();
		return "(null)";
	}else{ untested();

	}
	Net const* n = nn->net();

	if(!n){ untested();
		unreachable();
		return "(null)";
	}else if(n->hasLabel()){ untested();
		return n->short_label();
	}else{ untested();
		return "_net" + std::to_string(n->pos());
	}
}
/* -------------------------------------------------------------------------------- */
static void printDefHack(Symbol const* p, ostream_t& s)
{ untested();
	std::string hack = p->paramValue("qucsatorsckthack");
	s << hack;
}
/* -------------------------------------------------------------------------------- */
// legacy Qucsator process (wrapping a QProcess)
class Qucsator : public Simulator{
public:
	explicit Qucsator() : Simulator(), _process(this) {
		set_label("qucsator");
	}
	Qucsator(Qucsator const&) = delete;
	~Qucsator(){}
private: // Simulator
  Simulator* clone() const override {return new Qucsator();}
  NetLang const* netLang() const override {
	  return dynamic_cast<NetLang const*>(language_dispatcher["qucsator"]);
  }
  DocumentFormat const* netLister() const override { untested();
	  return dynamic_cast<DocumentFormat const*>(command_dispatcher["legacy_nl"]);
  }
  void run(istream_t&, SimCtrl*) override; // really?
  void do_it(istream_t&, ElementList const*) override;
  void join() override;
  void kill() override{incomplete();}
  void init() override{incomplete();}
  std::string errorString() const{ return "incomplete"; }

  void collectData();

public: // QProcess callback.
	void slotStateChanged(QProcess::ProcessState st){itested();
		trace2("slotStateChanged", st, _process.error());
		switch(st){
		case QProcess::NotRunning:itested();
			trace2("slotStateChanged to not running", st, _process.error());
			if(_oldState == QProcess::Starting){ untested();
				// bug/feature: errorString is not useful.
				message(MsgFatal, "Failed to start: "
						 + _process.errorString().toStdString());
				notifyState(Simulator::sst_error);
			}else{
				collectData();
				notifyState(Simulator::sst_idle);
			}
			break;
		case QProcess::Starting:
			trace2("slotStateChanged to starting", st, _process.error());
			if(_process.error()==5){itested();
				// this does not mean anything.
			}else{ untested();
			}
//			notifyState(Simulator::sst_starting); // not interesting.
			break;
		case QProcess::Running:
			trace2("slotStateChanged to running", st, _process.error());
			notifyState(Simulator::sst_running);
			break;
		default:
			trace1("qprocess state change", st);
			notifyState(Simulator::sst_error);
		}
		_oldState = st;
	}

private:
	QString DataSet;
//	QFile _netlistFile;
	QucsatorProcess _process;
	QProcess::ProcessState _oldState;
	Data* _dat{nullptr};
}QS;
static Dispatcher<Data>::INSTALL qq(&data_dispatcher, "qucsator", &QS);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
struct default_sim{
	default_sim(){
		QucsSettings.setSimulator(&QS);
	}
}ds;
/* -------------------------------------------------------------------------------- */
void Qucsator::join()
{
	_process.waitForFinished();
}
/* -------------------------------------------------------------------------------- */
void Qucsator::run(istream_t& cs, SimCtrl* ctrl)
{ untested();
	Simulator::attachCtrl(ctrl);
	do_it(cs, nullptr);
	// Simulator::detachCtrl(ctrl)?
}
/* -------------------------------------------------------------------------------- */
void Qucsator::do_it(istream_t& cs, ElementList const* scope)
{itested();
	assert(scope);
	assert(has_ctrl());

	cs >> "run"; //?
	std::string what;
	cs >> what;

	if(what==""){
		incomplete();
		what="all"; // BUG?
	}else{ untested();
	}

	// possibly not a good idea.
	QString f = QDir_(QucsSettings.QucsHomeDir).filePath("netlist.txt");

	message(MsgLog, "writing " + f.toStdString() + "...");


	auto dl = command_dispatcher["legacy_nl"];
	std::string s = "netlist mode=" + what + " " + f.toStdString();
	trace1("qucsator nl?", s);
	istream_t nlcmd(istream_t::_STRING, s);
	ElementList* hack = nullptr;
	if(scope){itested();
		hack = const_cast<ElementList*>(scope);
	}else{ untested();
		assert(false);
//	auto d = dynamic_cast<SchematicDoc const*>(doc()); // BUG.
//		incomplete();
//		// HACK
//		assert(d);
//		assert(d->root());
//		hack = const_cast<ElementList*>(d->root()->subckt());
	}
	assert(hack);
	trace1("qucsator", nlcmd.fullstring());
	dl->do_it(nlcmd, hack);
	//      ErrText->appendPlainText(tr("ERROR: Cannot write netlist file!"));
	//      FinishSimulation(-1);
	//      incomplete();
	//      return false;


#if 0
	if(Info.suffix() == "m" || Info.suffix() == "oct") { untested();
		// It is an Octave script.
		if(Doc->DocChanged)
			Doc->save();
		slotViewOctaveDock(true);
		octave->runOctaveScript(Doc->docName());
		return;
	}else{ untested();
	}
#endif

	QString DocName;

	if(doc()){ untested();
		DocName = doc()->docName();
		QFileInfo Info(DocName);
		DataSet = QDir::toNativeSeparators(Info.path()) +
			QDir::separator() + doc()->DataSet;
	} else{
		char* t = strdup(TMPFILENAME);
//		int h = mkstemps(t, 4); // not portable -> so what
		int h = mkstemp(t);
		close(h);
		DataSet = QString(t);
		free(t);
	}

	QString Program = QString_(QucsSettings.Qucsator);
	if(Program==""){
		unreachable();
		Program = "qucsator";
	}else{ untested();
	}
	QStringList Arguments;
	Arguments << "-b" << "-g" << "-i"
		<< QDir_(QucsSettings.QucsHomeDir).filePath("netlist.txt")
		<< "-o" << DataSet;

	trace2("start", Program, DataSet);
	_process.start(Program, Arguments);

	QString cmd = Program +" "+ Arguments.join(" ");
	message(MsgLog, cmd.toStdString());
}
/* -------------------------------------------------------------------------------- */
void Qucsator::collectData()
{itested();
	trace1("collectData", DataSet);
	_dat = data_dispatcher.clone("datfile"); // really? table?
	assert(_dat);
 	std::string fn = DataSet.toStdString();
	istream_t cs(istream_t::_WHOLE_FILE, fn);
 	_dat->set_param_by_name("filename", fn);
	netLang()->parseItem(cs, _dat);
	releaseOutput(_dat->common());
	delete _dat;
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
void QucsatorProcess::slotStateChanged(QProcess::ProcessState newState)
{itested();
	trace1("QucsatorProcess callback", newState);
	assert(_simulator);
	_simulator->slotStateChanged(newState);
}
/* -------------------------------------------------------------------------------- */
void QucsatorProcess::stderr_()
{ untested();
	assert(_simulator);
	std::string msg = readAllStandardError().toStdString();
	_simulator->message(MsgWarning, msg);
}
/* -------------------------------------------------------------------------------- */
void QucsatorProcess::stdout_()
{itested();
	assert(_simulator);
	std::string msg = readAllStandardOutput().toStdString();
	trace1("QucsatorProcess stdout", msg);
	_simulator->message(MsgLog, msg);
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */

}//namespace
#endif
