#include "io.h"
#include "docfmt.h"
#include "globals.h"
#include "language.h"
#include "schematic_lang.h"
#include "schematic_model.h"
#include "misc.h" //?!!

#include "schematic.h" // BUG, transition

namespace{

class LegacySchematicFormat : public DocumentFormat{
	void save(DocumentStream& stream, ModelAccess const&) const;
	void load(DocumentStream& stream, ModelInserter&) const{ incomplete(); }

private: // legacy cruft
	bool isSymbolMode() const{ return false; }
	PaintingList const& symbolPaints(ModelAccess const& m) const{
		return m.schematicModel().symbolPaints();
	}
	DiagramList const& diagrams(ModelAccess const& m) const{
		return m.schematicModel().diagrams();
	}
	PaintingList const& paintings(ModelAccess const& m) const{
		return m.schematicModel().paintings();
	}
	WireList const& wires(ModelAccess const& m) const{
		return m.schematicModel().wires();
	}
	NodeList const& nodes(ModelAccess const& m) const{
		return m.schematicModel().nodes();
	}
	ComponentList const& components(ModelAccess const& m) const{
		return m.schematicModel().components();
	}
}D;
static Dispatcher<DocumentFormat>::INSTALL
    p(&docfmt_dispatcher, "leg_sch", &D);


static QString QG(ModelAccess const& m, std::string const& key)
{
	return QString::fromStdString(m.getParameter(key));
}

void LegacySchematicFormat::save(DocumentStream& stream, ModelAccess const& m) const
{
	// get legacy "parameters"
	float tmpScale;
	int tmpViewX1;
	int tmpViewX2;
	int tmpViewY1;
   int tmpViewY2;
   int tmpPosX;
   int tmpPosY;
	int ViewX1;
	int ViewY1;
	int ViewX2;
	int ViewY2;
	int Scale;
	int GridX;
	int GridY;
	int GridOn;

	try{
		ViewX1=std::stoi(m.getParameter("ViewX1"));
		ViewY1=std::stoi(m.getParameter("ViewY1"));
		ViewX2=std::stoi(m.getParameter("ViewX2"));
		ViewY2=std::stoi(m.getParameter("ViewY2"));
		Scale=std::stoi(m.getParameter("Scale"));
		GridX=std::stoi(m.getParameter("GridX"));
		GridY=std::stoi(m.getParameter("GridY"));
		GridOn=std::stoi(m.getParameter("GridOn"));

		tmpViewX1=std::stoi(m.getParameter("tmpViewX1"));
		tmpViewX2=std::stoi(m.getParameter("tmpViewX2"));
		tmpViewY1=std::stoi(m.getParameter("tmpViewY1"));
		tmpViewY2=std::stoi(m.getParameter("tmpViewY2"));
		tmpScale=std::stof(m.getParameter("tmpScale"));
		tmpPosX=std::stoi(m.getParameter("tmpPosX"));
		tmpPosY=std::stoi(m.getParameter("tmpPosY"));
	}catch (std::invalid_argument){
		incomplete();
	}

	auto D=doclang_dispatcher["leg_sch"];
	auto L = dynamic_cast<DocumentLanguage const*>(D);
	assert(L);

	stream << "<Qucs Schematic 0.0.20>\n";

	stream << "<Properties>\n";
	if(isSymbolMode()) {
		incomplete();
		assert(false); // => symbol_format.cc
		stream << "  <View=" << tmpViewX1<<","<<tmpViewY1<<","
			<< tmpViewX2<<","<<tmpViewY2<< ",";
		stream <<tmpScale<<","<<tmpPosX<<","<<tmpPosY << ">\n";
	}else{
		stream << "  <View=" << ViewX1<<","<<ViewY1<<","
			<< ViewX2<<","<<ViewY2<< ",";
		incomplete(); // scale.
		/// \todo  stream << Scale <<","<<contentsX()<<","<<contentsY() << ">\n";
		stream << Scale <<","<< 0 <<","<< 0 << ">\n";
	}
	stream << "  <Grid=" << GridX<<","<<GridY<<","
		<< GridOn << ">\n";
	stream << "  <DataSet=" << QG(m, "DataSet") << ">\n";
	stream << "  <DataDisplay=" << QG(m, "DataDisplay") << ">\n";
	stream << "  <OpenDisplay=" << QG(m, "SimOpenDpl") << ">\n";
	stream << "  <Script=" << QG(m, "Script") << ">\n";
	stream << "  <RunScript=" << QG(m, "SimRunScript") << ">\n";
	stream << "  <showFrame=" << QG(m, "showFrame") << ">\n";

	QString Frame_Text0(QG(m,"FrameText0"));
	QString Frame_Text1(QG(m,"FrameText1"));
	QString Frame_Text2(QG(m,"FrameText2"));
	QString Frame_Text3(QG(m,"FrameText3"));

	QString t;
	misc::convert2ASCII(t = Frame_Text0);
	stream << "  <FrameText0=" << t << ">\n";
	misc::convert2ASCII(t = Frame_Text1);
	stream << "  <FrameText1=" << t << ">\n";
	misc::convert2ASCII(t = Frame_Text2);
	stream << "  <FrameText2=" << t << ">\n";
	misc::convert2ASCII(t = Frame_Text3);
	stream << "  <FrameText3=" << t << ">\n";
	stream << "</Properties>\n";

	stream << "<Symbol>\n";     // save all paintings for symbol
	for(auto pp : symbolPaints(m)){
		stream << "  <" << pp->save() << ">\n";
	}
	stream << "</Symbol>\n";

	stream << "<Components>\n";    // save all components
	for(auto pc : components(m)){ untested();
		stream << "  ";
		L->printItem(pc, stream);
		stream << "\n"; // BUG?
	}
	stream << "</Components>\n";

	stream << "<Wires>\n";    // save all wires
	for(auto pw : wires(m)){
		stream << "  " << pw->save() << "\n";
	}

	for(auto pn : nodes(m)){
		if(pn->Label) stream << "  " << pn->Label->save() << "\n";
	}
	stream << "</Wires>\n";

	stream << "<Diagrams>\n";    // save all diagrams
	for(auto pd : diagrams(m)){
		stream << "  " << pd->save() << "\n";
	}
	stream << "</Diagrams>\n";

	stream << "<Paintings>\n";     // save all paintings
	for(auto pp : paintings(m)){
		stream << "  <" << pp->save() << ">\n";
	}
	stream << "</Paintings>\n";

#if 0 // yikes (wtf?)
	// additionally save symbol C++ code if in a symbol drawing and the
	// associated file is a Verilog-A file
	if (fileSuffix () == "sym") {
		if (fileSuffix (DataDisplay) == "va") {
			saveSymbolCpp ();
			saveSymbolJSON ();

			// TODO slit this into another method, or merge into saveSymbolJSON
			// handle errors in separate
			qDebug() << "  -> Run adms for symbol";

			QString vaFile;

			//      QDir prefix = QDir(QucsSettings.BinDir);

			QDir include = QDir(QucsSettings.BinDir+"../include/qucs-core");

			//pick admsXml from settings
			QString admsXml = QucsSettings.AdmsXmlBinDir.canonicalPath();

#ifdef __MINGW32__
			admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml.exe");
#else
			admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml");
#endif
			// BUG: duplicated from qucs_actions.cpp
			char const* var = getenv("QUCS_USE_PATH");
			if(var != NULL) {
				// just use PATH. this is currently bound to a contition, to maintain
				// backwards compatibility with QUCSDIR
				qDebug() << "QUCS_USE_PATH";
				admsXml = "admsXml";
			}else{
			}

			QString workDir = QucsSettings.QucsWorkDir.absolutePath();

			qDebug() << "App path : " << qApp->applicationDirPath();
			qDebug() << "workdir"  << workDir;
			qDebug() << "homedir"  << QucsSettings.QucsHomeDir.absolutePath();
			qDebug() << "projsdir"  << QucsSettings.projsDir.absolutePath();

			vaFile = QucsSettings.QucsWorkDir.filePath(fileBase()+".va");

			QStringList Arguments;
			Arguments << QDir::toNativeSeparators(vaFile)
				<< "-I" << QDir::toNativeSeparators(include.absolutePath())
				<< "-e" << QDir::toNativeSeparators(include.absoluteFilePath("qucsMODULEguiJSONsymbol.xml"))
				<< "-A" << "dyload";

			//      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

			QFile file(admsXml);
			if(var) {
				// don't do this. it will always report an error.
			}else if ( !file.exists() ){
				QMessageBox::critical(this, tr("Error"),
						tr("Program admsXml not found: %1\n\n"
							"Set the admsXml location on the application settings.").arg(admsXml));
				return -1;
			}

			qDebug() << "Command: " << admsXml << Arguments.join(" ");

			// need to cd into project to run admsXml?
			QDir::setCurrent(workDir);

			QProcess builder;
			builder.setProcessChannelMode(QProcess::MergedChannels);

			builder.start(admsXml, Arguments);


			// how to capture [warning]? need to modify admsXml?
			// TODO put stdout, stderr into a dock window, not messagebox
			if (!builder.waitForFinished()) {
				QString cmdString = QString("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
				cmdString = cmdString + builder.errorString();
				QMessageBox::critical(this, tr("Error"), cmdString);
			}
			else {
				QString cmdString = QString("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
				cmdString = cmdString + builder.readAll();
				QMessageBox::information(this, tr("Status"), cmdString);
			}

			// Append _sym.json into _props.json, save into _symbol.json
			QFile f1(QucsSettings.QucsWorkDir.filePath(fileBase()+"_props.json"));
			QFile f2(QucsSettings.QucsWorkDir.filePath(fileBase()+"_sym.json"));
			f1.open(QIODevice::ReadOnly | QIODevice::Text);
			f2.open(QIODevice::ReadOnly | QIODevice::Text);

			QString dat1 = QString(f1.readAll());
			QString dat2 = QString(f2.readAll());
			QString finalJSON = dat1.append(dat2);

			// remove joining point
			finalJSON = finalJSON.replace("}{", "");

			QFile f3(QucsSettings.QucsWorkDir.filePath(fileBase()+"_symbol.json"));
			f3.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream out(&f3);
			out << finalJSON;

			f1.close();
			f2.close();
			f3.close();

			// TODO choose icon, default to something or provided png

		} // if DataDisplay va
	} // if suffix .sym
#endif
}

}
