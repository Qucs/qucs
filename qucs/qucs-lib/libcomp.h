#include "symbol.h"
#include "qucslib_common.h"
/*!
 * \brief this library provides symbols. these.
 */



class QucsLibComponent : public Symbol{
        private:
        QString SymbolString, LibraryName, ComponentName;
	public:
	QucsLibComponent( QString& , const QString&, const QString& );
        int draw(QWidget&);
};

extern tQucsSettings QucsSettings;

QucsLibComponent::QucsLibComponent( QString& SymbolString_,
                            const QString& Lib_, const QString& Comp_)
	: SymbolString(SymbolString_), LibraryName(Lib_), ComponentName(Comp_)
{
  if (SymbolString.isEmpty())//Whenever SymbolString is empty, it tries to load the default symbol
  {
      //Load the default symbol for the current Qucs library
      ComponentLibrary parsedlib;
      QString libpath = QucsSettings.LibDir + Lib_ + ".lib";
      int result = parseComponentLibrary (libpath, parsedlib);

		// BUG: throw if there is an error. don't randomly spawn Messageboxes
		// (cleanup later)
      switch (result)//Check if the library was properly loaded
      {
        case QUCS_COMP_LIB_IO_ERROR:
            QMessageBox::critical(0, "Error", QString("Cannot open \"%1\".").arg (libpath));
            return;
        case QUCS_COMP_LIB_CORRUPT:
            QMessageBox::critical(0, "Error", "Library is corrupt.");
            return;
        default:
            break;
      }

    // copy the contents of default symbol section to a string
    SymbolString = parsedlib.defaultSymbol;
  }
}



int QucsLibComponent::draw(QWidget& w)
{
  w.Arcs.clear();
  w.Lines.clear();
  w.Rects.clear();
  w.Ellips.clear();
  w.Texts.clear();

  QString Line;
  ///QString foo = SymbolString;
  QTextStream stream(&SymbolString, QIODevice::ReadOnly);

  w.x1 = w.y1 = INT_MAX;
  w.x2 = w.y2 = INT_MIN;

  int z=0, Result;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line.at(0) != '<') return -1; // check for start char
    if(Line.at(Line.length()-1) != '>') return -1; // check for end char
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = w.analyseLine(Line);
    if(Result < 0) return -6;   // line format error
    z += Result;
  }

  w.x1 -= 4;   // enlarge component boundings a little
  w.x2 += 4;
  w.y1 -= 4;
  w.y2 += 4;
  w.cx  = -w.x1 + w.TextWidth;
  w.cy  = -w.y1;

  int dx = w.x2-w.x1 + w.TextWidth;
  if((w.x2-w.x1) < w.DragNDropWidth)
    dx = (w.x2-w.x1 + w.DragNDropWidth)/2 + w.TextWidth;
  if(dx < w.DragNDropWidth)
    dx = w.DragNDropWidth;
  w.setMinimumSize(dx, w.y2-w.y1 + w.TextHeight+4);
  if(w.width() > dx)  dx = w.width();
  w.resize(dx, w.y2-w.y1 + w.TextHeight+4);
  w.update();
  return z;      // return number of ports
}

