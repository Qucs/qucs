#ifndef QUCS_SETTINGS_H
#define QUCS_SETTINGS_H

#include <QFont>
#include <QColor>
#include <QDir>
#include "qt_compat.h"
#include "io_trace.h"

struct tQucsSettings {
	explicit tQucsSettings();
	tQucsSettings(tQucsSettings const&) = delete;
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  float largeFontSize;
  QColor BGColor;      // background color of view area
  QString Language;

  // syntax highlighting
  QColor Comment, String, Integer, Real, Character, Type,
    Attribute, Directive, Task;

  unsigned int maxUndo;    // size of undo stack
  QString Editor;
  QString Qucsator;
  QString Qucsconv;
  QString BinDir;
  QString LangDir;
  QString const& libDir() const{ return _libDir;}
  void setLibDir(QString const& l){
	  trace1("setLibDir", l);
	  untested(); _libDir = l;
  }
  QString OctaveDir;  // m-files location
  QString ExamplesDir;
  QString DocDir;

  unsigned int NodeWiring;
  QDir QucsWorkDir; // Qucs user directory where user currently works (usually QucsHomeDir or subdir)
  QDir QucsHomeDir; // Qucs initial user projects directory
  QDir projsDir; // current user directory where projects are located
  QDir AdmsXmlBinDir;  // dir of admsXml executable
  QDir AscoBinDir;     // dir of asco executable
  // QDir OctaveBinDir;   // dir of octave executable
  QString OctaveExecutable; // OctaveExecutable location
  QString QucsOctave; // OUCS_OCTAVE variable

  // registered filename extensions with program to open the file
  QStringList FileTypes;

  // List of extensions used for spice files
  QStringList spiceExtensions;

  unsigned int numRecentDocs;
  QStringList RecentDocs;

  bool IgnoreFutureVersion;
  bool GraphAntiAliasing;
  bool TextAntiAliasing;
  bool ShowDescriptionProjectTree;
private:
  QString _libDir;
};
extern tQucsSettings QucsSettings;

#endif
