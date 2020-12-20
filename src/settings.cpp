
#include "settings.h"
#include <QSettings>
#include "platform.h"

extern QStringList qucsPathList; // BUG
DocumentLanguage* tQucsSettings::language;

// Loads the settings file and stores the settings.
// BUG: move to QucsSettings::QucsSettings
tQucsSettings::tQucsSettings()
  : _simulator(nullptr)
{ incomplete(); 
  font = QFont("Helvetica", 12);
  largeFontSize = 16.0;
  maxUndo = 20;
  NodeWiring = 0;
  Editor = "qucs";

  QucsSettings.QucsHomeDir.setPath(QDir::homePath()+QDir::toNativeSeparators("/.qucs"));
  QucsSettings.QucsWorkDir.setPath(QucsSettings.QucsHomeDir.canonicalPath());
}

bool loadSettings()
{
    QSettings settings("qucs","qucs");

    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("dx"))QucsSettings.dx=settings.value("dx").toInt();
    if(settings.contains("dy"))QucsSettings.dy=settings.value("dy").toInt();
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("LargeFontSize"))QucsSettings.largeFontSize=settings.value("LargeFontSize").toDouble(); // use toDouble() as it can interpret the string according to the current locale
    if(settings.contains("maxUndo"))QucsSettings.maxUndo=settings.value("maxUndo").toInt();
    if(settings.contains("NodeWiring"))QucsSettings.NodeWiring=settings.value("NodeWiring").toInt();
    if(settings.contains("BGColor"))QucsSettings.BGColor.setNamedColor(settings.value("BGColor").toString());
    if(settings.contains("Editor"))QucsSettings.Editor=settings.value("Editor").toString();
    if(settings.contains("FileTypes"))QucsSettings.FileTypes=settings.value("FileTypes").toStringList();
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();
    if(settings.contains("Comment"))QucsSettings.Comment.setNamedColor(settings.value("Comment").toString());
    if(settings.contains("String"))QucsSettings.String.setNamedColor(settings.value("String").toString());
    if(settings.contains("Integer"))QucsSettings.Integer.setNamedColor(settings.value("Integer").toString());
    if(settings.contains("Real"))QucsSettings.Real.setNamedColor(settings.value("Real").toString());
    if(settings.contains("Character"))QucsSettings.Character.setNamedColor(settings.value("Character").toString());
    if(settings.contains("Type"))QucsSettings.Type.setNamedColor(settings.value("Type").toString());
    if(settings.contains("Attribute"))QucsSettings.Attribute.setNamedColor(settings.value("Attribute").toString());
    if(settings.contains("Directive"))QucsSettings.Directive.setNamedColor(settings.value("Directive").toString());
    if(settings.contains("Task"))QucsSettings.Task.setNamedColor(settings.value("Task").toString());

    if(settings.contains("Qucsator"))QucsSettings.Qucsator = settings.value("Qucsator").toString();
    //if(settings.contains("BinDir"))QucsSettings.BinDir = settings.value("BinDir").toString();
    //if(settings.contains("LangDir"))QucsSettings.LangDir = settings.value("LangDir").toString();
    //if(settings.contains("LibDir"))QucsSettings.LibDir = settings.value("LibDir").toString();
    if(settings.contains("AdmsXmlBinDir"))QucsSettings.AdmsXmlBinDir.setPath(settings.value("AdmsXmlBinDir").toString());
    if(settings.contains("AscoBinDir"))QucsSettings.AscoBinDir.setPath(settings.value("AscoBinDir").toString());
    //if(settings.contains("OctaveDir"))QucsSettings.OctaveDir = settings.value("OctaveDir").toString();
    //if(settings.contains("ExamplesDir"))QucsSettings.ExamplesDir = settings.value("ExamplesDir").toString();
    //if(settings.contains("DocDir"))QucsSettings.DocDir = settings.value("DocDir").toString();
    if(settings.contains("OctaveExecutable")) {
        QucsSettings.OctaveExecutable = settings.value("OctaveExecutable").toString();
    } else { untested();
        if(settings.contains("OctaveBinDir")) { untested();
            QucsSettings.OctaveExecutable = settings.value("OctaveBinDir").toString() +
                    QDir::separator() + "octave" + QString(executableSuffix);
        } else QucsSettings.OctaveExecutable = "octave" + QString(executableSuffix);
    }

    if(settings.contains("QucsHomeDir"))
      if(settings.value("QucsHomeDir").toString() != "")
         QucsSettings.QucsHomeDir.setPath(settings.value("QucsHomeDir").toString());
    QucsSettings.QucsWorkDir = QucsSettings.QucsHomeDir;

    if (settings.contains("IgnoreVersion")) QucsSettings.IgnoreFutureVersion = settings.value("IgnoreVersion").toBool();
    // check also for old setting name with typo...
    else if (settings.contains("IngnoreVersion")) QucsSettings.IgnoreFutureVersion = settings.value("IngnoreVersion").toBool();
    else QucsSettings.IgnoreFutureVersion = false;

    if (settings.contains("GraphAntiAliasing")) QucsSettings.GraphAntiAliasing = settings.value("GraphAntiAliasing").toBool();
    else QucsSettings.GraphAntiAliasing = false;

    if (settings.contains("TextAntiAliasing")) QucsSettings.TextAntiAliasing = settings.value("TextAntiAliasing").toBool();
    else QucsSettings.TextAntiAliasing = false;

    if(settings.contains("Editor")) QucsSettings.Editor = settings.value("Editor").toString();

    if(settings.contains("ShowDescription")) QucsSettings.ShowDescriptionProjectTree = settings.value("ShowDescription").toBool();

    QucsSettings.RecentDocs = settings.value("RecentDocs").toString().split("*",QString::SkipEmptyParts);
    QucsSettings.numRecentDocs = QucsSettings.RecentDocs.count();


    QucsSettings.spiceExtensions << "*.sp" << "*.cir" << "*.spc" << "*.spi";

    // If present read in the list of directory paths in which Qucs should
    // search for subcircuit schematics
    int npaths = settings.beginReadArray("Paths");
    for (int i = 0; i < npaths; ++i)
    { untested();
        settings.setArrayIndex(i);
        QString apath = settings.value("path").toString();
        qucsPathList.append(apath);
    }
    settings.endArray();

    QucsSettings.numRecentDocs = 0;

    // BUG: not here.
    if(char* qucslibdir=getenv("QUCS_LIBRARY")){
      trace1("override library", qucslibdir);
      QucsSettings.setLibDir( QDir(QString(qucslibdir)).canonicalPath() );
    }else{ untested();
    }
    trace2("loadSettings done", QucsSettings.libDir(), &QucsSettings);

    return true;
}
/*--------------------------------------------------------------------------*/
Simulator const* tQucsSettings::simulator() const
{
	return _simulator;
}
/*--------------------------------------------------------------------------*/
void tQucsSettings::setSimulator(Simulator const* s)
{
	_simulator = s;
}
/*--------------------------------------------------------------------------*/
