#include "symbol.h"

/*!
 * \brief this library provides symbols. these.
 */
class QucsLibComponent : public Symbol{
	public:
	QucsLibComponent( QString& , const QString&, const QString& );

};

QucsLibComponent::QucsLibComponent( QString& SymbolString,
                            const QString& Lib_, const QString& Comp_)
	: Symbol(), LibraryName(Lib_), ComponentName(Comp_)
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
            QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (libpath));
            return -1;
        case QUCS_COMP_LIB_CORRUPT:
            QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
            return -1;
        default:
            break;
      }

    // copy the contents of default symbol section to a string
    SymbolString = parsedlib.defaultSymbol;
  }
}

QucsLibComponent::draw(Qwidget& w)
{
  w.Arcs.clear();
  w.Lines.clear();
  w.Rects.clear();
  w.Ellips.clear();
  w.Texts.clear();

  QString Line;
  ///QString foo = SymbolString;
  QTextStream stream(&SymbolString, QIODevice::ReadOnly);

  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  int z=0, Result;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line.at(0) != '<') return -1; // check for start char
    if(Line.at(Line.length()-1) != '>') return -1; // check for end char
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line);
    if(Result < 0) return -6;   // line format error
    z += Result;
  }

  x1 -= 4;   // enlarge component boundings a little
  x2 += 4;
  y1 -= 4;
  y2 += 4;
  cx  = -x1 + TextWidth;
  cy  = -y1;

  int dx = x2-x1 + TextWidth;
  if((x2-x1) < DragNDropWidth)
    dx = (x2-x1 + DragNDropWidth)/2 + TextWidth;
  if(dx < DragNDropWidth)
    dx = DragNDropWidth;
  setMinimumSize(dx, y2-y1 + TextHeight+4);
  if(width() > dx)  dx = width();
  resize(dx, y2-y1 + TextHeight+4);
  update();
  return z;      // return number of ports
}

