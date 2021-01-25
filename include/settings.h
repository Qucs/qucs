#ifndef QUCS_SETTINGS_H
#define QUCS_SETTINGS_H

// #include <QFont>
// #include <QColor>
// #include <QDir>
// #include "qt_compat.h"
#include "io_trace.h"

class Simulator;
class DocumentLanguage;

// BUG: class. fix visibility
struct tQucsSettings {
	explicit tQucsSettings();
	tQucsSettings(tQucsSettings const&) = delete;
  int x, y, dx, dy;    // position and size of main window
  std::string font;
  float largeFontSize;
  std::string BGColor;      // background color of view area
  std::string Language;
  static DocumentLanguage* language;

  // syntax highlighting
  std::string Comment, String, Integer, Real, Character, Type,
    Attribute, Directive, Task;

  unsigned int maxUndo;    // size of undo stack
  std::string Editor;
  std::string Qucsator;
  std::string Qucsconv;
  std::string BinDir;
  std::string LangDir;
  std::string const& libDir() const{ return _libDir;}
  void setLibDir(std::string const& l){
	  trace1("setLibDir", l);
	  _libDir = l;
  }
  std::string OctaveDir;  // m-files location
  std::string ExamplesDir;
  std::string DocDir;

  unsigned int NodeWiring;
  std::string QucsWorkDir; // Qucs user directory where user currently works (usually QucsHomeDir or subdir)
  std::string QucsHomeDir; // Qucs initial user projects directory
  std::string projsDir; // current user directory where projects are located
  std::string AdmsXmlBinDir;  // dir of admsXml executable
  std::string AscoBinDir;     // dir of asco executable
  // QDir OctaveBinDir;   // dir of octave executable
  std::string OctaveExecutable; // OctaveExecutable location
  std::string QucsOctave; // OUCS_OCTAVE variable

  // registered filename extensions with program to open the file
  std::string FileTypes;

  // List of extensions used for spice files
  std::string spiceExtensions;

  unsigned int numRecentDocs;
  std::string RecentDocs;

  bool IgnoreFutureVersion;
  bool GraphAntiAliasing;
  bool TextAntiAliasing;
  bool ShowDescriptionProjectTree;

public:
  void setHomeDir(std::string const& s) { QucsHomeDir = s; }
  std::string const& homeDir() const { return QucsHomeDir; }

  void setSimulator(Simulator const* s);
  Simulator const* simulator() const;
private:
  std::string _libDir;
  Simulator const* _simulator;
};
extern tQucsSettings QucsSettings;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
