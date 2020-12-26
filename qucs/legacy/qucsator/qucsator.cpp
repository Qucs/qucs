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

// qucsator simulator "driver", legacy

#include "simulator.h"
#include "sckt_base.h"
#include "net.h"
#include "docfmt.h" // <<
#include "globals.h"
#include "settings.h" //??
#include "schematic_doc.h"
#include <QProcess>
#include "qucsator.h"
#include "dat.h"
#include "painting.h"
/* -------------------------------------------------------------------------------- */
void Simulator::notifyState(Simulator::state_t st)
{
	setState(st);
	if(_ctrl){
		_ctrl->stateChange();
	}else{
	}
}
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
static const std::string typesep(":");
static const char _typesep = ':';

// not sure what this is about
/// #ifdef __MINGW32__ // -> platform.h
/// #include <windows.h>
/// static QString pathName(QString longpath) {
///   const char * lpath = QDir::toNativeSeparators(longpath).toAscii();
///   char spath[2048];
///   int len = GetShortPathNameA(lpath,spath,sizeof(spath)-1);
///   spath[len] = '\0';
///   return QString(spath);
/// }
/// #else
/// static QString pathName(QString longpath) {
///   return longpath;
/// }
/// #endif
/* -------------------------------------------------------------------------------- */
// temporary kludge.
class QucsatorScktHack : public Symbol {
private:
	QucsatorScktHack(QucsatorScktHack const&) = default;
public:
	QucsatorScktHack() : Symbol() {}
private:
	Element* clone() const override{return new QucsatorScktHack(*this);}
private: // Symbol
	pos_t portPosition(unsigned) const {unreachable(); return pos_t(0,0);}
	unsigned numPorts() const  override{ return 0;}
	Port& port(unsigned) override{unreachable(); return *new Port();}
	void setParameter(std::string const& name, std::string const& value){
		if(name == "qucsatorsckthack"){
			_text = value;
		}else{ untested();
			Symbol::setParameter(name, value);
		}
	}
	std::string paramValue(std::string const& name) const override{
		if(name == "qucsatorsckthack"){
			return _text;
		}else{ untested();
			return Symbol::paramValue(name);
		}
	}

private:
	std::string _text;
}d0;
static Dispatcher<Symbol>::INSTALL p0(&symbol_dispatcher, "qucsatorScktHack", &d0);
/* -------------------------------------------------------------------------------- */
static std::string netLabel(Node const* nn)
{
	if(!nn){
		return "(null)";
	}else{

	}
	Net const* n = nn->net();

	if(!n){ untested();
		unreachable();
		return "(null)";
	}else if(n->hasLabel()){
		return n->label();
	}else{
		return "_net" + std::to_string(n->pos());
	}
}
/* -------------------------------------------------------------------------------- */
static int notalnum(char c)
{
	return !std::isalnum(c);
}
/* -------------------------------------------------------------------------------- */
static std::string mangleType(std::string& t)
{
	if(t == "_BJT"){
		// for some reason...
		t = "BJT";
	}else{
	}
	trace1("mangle", t);
	auto pos = t.find(typesep);
	std::string ret="";
	if(pos == std::string::npos){
	}else{
		auto sub = t.substr(pos+1);
		std::replace_if(sub.begin(), sub.end(), notalnum, '_');
		ret = " Type=\"" + sub + "\"";
	}
	t = t.substr(0, pos);
	if(t=="Lib"){
		t="Sub";
	}else{
	}
	return ret;
}
/* -------------------------------------------------------------------------------- */
static void printDefHack(Symbol const* p, ostream_t& s)
{
	std::string hack = p->paramValue("qucsatorsckthack");
	s << hack;
}
/* -------------------------------------------------------------------------------- */
// legacy Qucsator process (wrapping a QProcess)
class Qucsator : public Simulator{
public:
	explicit Qucsator() : Simulator(), _process(this), _ctrl(nullptr) {
	}
	Qucsator(Qucsator const&) = delete;
	~Qucsator(){}
private: // Simulator
  Simulator* clone() const override {return new Qucsator();}
  NetLang const* netLang() const override { untested();
	  return dynamic_cast<NetLang const*>(language_dispatcher["qucsator"]);
  }
  DocumentFormat const* netLister() const override {
	  return dynamic_cast<DocumentFormat const*>(command_dispatcher["legacy_nl"]);
  }
  void run(istream_t&, SimCtrl*) override;
  void kill() override{incomplete();}
  void init() override{incomplete();}
  std::string errorString() const{ return "incomplete"; }

  void collectData();

public: // QProcess callback.
	void slotStateChanged(QProcess::ProcessState st){ untested();
		trace2("slotStateChanged", st, _process.error());
		switch(st){
		case QProcess::NotRunning: untested();
			if(_oldState == QProcess::Starting){ untested();
				message(QucsMsgFatal, "Failed to start process.");
				notifyState(Simulator::sst_error);
			}else{ untested();
				collectData();
				notifyState(Simulator::sst_idle);
			}
			break;
		case QProcess::Starting:
//			notifyState(Simulator::sst_starting); // not interesting.
			break;
		case QProcess::Running:
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
	QFile _netlistFile;
	QucsatorProcess _process;
	SimCtrl* _ctrl;
	QProcess::ProcessState _oldState;
}QS;
static Dispatcher<Simulator>::INSTALL qq(&simulator_dispatcher, "qucsator", &QS);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
struct default_sim{
	default_sim(){
		QucsSettings.setSimulator(&QS);
	}
}ds;
/* -------------------------------------------------------------------------------- */
void Qucsator::run(istream_t& cs, SimCtrl* ctrl)
{
	Simulator::attachCtrl(ctrl);

	cs.umatch("run");
	std::string what;
	cs >> what;

	// possibly not a good idea.
	QString f = QucsSettings.QucsHomeDir.filePath("netlist.txt");

	message(QucsMsgLog, "writing " + f.toStdString() + "...");
	_netlistFile.setFileName(f);

	if(!_netlistFile.open(QIODevice::WriteOnly | QFile::Truncate)){
		message(QucsMsgFatal, "cannot open netlist file");
		throw Exception("cannot write");
	}else{
	}

	ostream_t Stream(&_netlistFile);

#if 0
	auto dl = netLister();
	DocumentFormat const* n = prechecked_cast<DocumentFormat const*>(dl);
#else
	auto n = netLang();
	assert(n);
#endif

	assert(doc());
	auto d = dynamic_cast<SchematicDoc const*>(doc()); // BUG.
	assert(d);

	SubcktBase const* m = d->root();
	assert(m);
	try{
		incomplete();
		n->printItem(Stream, m);
	}catch(...){
		message(QucsMsgFatal, "Error writing netlist file.");
		throw;
	}
	//      ErrText->appendPlainText(tr("ERROR: Cannot write netlist file!"));
	//      FinishSimulation(-1);
	//      incomplete();
	//      return false;

	NetLang const* nl = netLang();

	if(what=="all"){
		for(auto c : d->commands()){
			trace1("cmd", c->label());
			nl->printItem(Stream, c);
		}
	}else if(what=="dcop"){
		Element const* dc = element_dispatcher["DC"];
		nl->printItem(Stream, dc);
	}else{
		assert(false);
		throw Exception("nothing to do");
	}

	_netlistFile.close();

#if 0
	if(Info.suffix() == "m" || Info.suffix() == "oct") { untested();
		// It is an Octave script.
		if(Doc->DocChanged)
			Doc->save();
		slotViewOctaveDock(true);
		octave->runOctaveScript(Doc->docName());
		return;
	}else{
	}
#endif

  QString DocName = doc()->docName();
  QFileInfo Info(DocName);
	  DataSet = QDir::toNativeSeparators(Info.path()) +
		 QDir::separator() + doc()->DataSet;

	QString Program = QucsSettings.Qucsator;
	QStringList Arguments;
	Arguments << "-b" << "-g" << "-i"
		<< QucsSettings.QucsHomeDir.filePath("netlist.txt")
		<< "-o" << DataSet;

	trace2("start", Program, DataSet);
	_process.start(Program, Arguments);

	QString cmd = Program +" "+ Arguments.join(" ");
	message(QucsMsgLog, cmd.toStdString());
}

void Qucsator::collectData()
{
	auto data = new SimOutputDat(DataSet.toStdString(), "");
	releaseOutput(data);
}

}//namespace
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// BUG: must be anonymous
/* -------------------------------------------------------------------------------- */
void QucsatorProcess::slotStateChanged(QProcess::ProcessState newState)
{
	trace1("QucsatorProcess callback", newState);
	assert(_simulator);
	_simulator->slotStateChanged(newState);
}
/* -------------------------------------------------------------------------------- */
void QucsatorProcess::stderr_()
{ untested();
	assert(_simulator);
	std::string msg = readAllStandardError().toStdString();
	_simulator->message(Object::QucsMsgWarning, msg);
}
/* -------------------------------------------------------------------------------- */
void QucsatorProcess::stdout_()
{ untested();
	assert(_simulator);
	std::string msg = readAllStandardOutput().toStdString();
	trace1("QucsatorProcess stdout", msg);
	_simulator->message(Object::QucsMsgLog, msg);
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
