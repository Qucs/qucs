
// TODO
int SchematicDoc::saveSymbolCpp (void)
{
  QFileInfo info (docName());
  QString cppfile = info.path () + QDir::separator() + DataSet;
  QFile file (cppfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
		   QObject::tr("Cannot save C++ file \"%1\"!").arg(cppfile));
    return -1;
  }

  DocumentStream stream (&file);

  // automatically compute boundings of drawing
  int xmin = INT_MAX;
  int ymin = INT_MAX;
  int xmax = INT_MIN;
  int ymax = INT_MIN;
  int x1, y1, x2, y2;
  int maxNum = 0;
  Painting * pp;

  stream << "  // symbol drawing code\n";
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
    if (pp->name() == ".ID ") continue;
    if (pp->name() == ".PortSym ") {
      if (((PortSymbol*)pp)->numberStr.toInt() > maxNum)
	maxNum = ((PortSymbol*)pp)->numberStr.toInt();
      x1 = ((PortSymbol*)pp)->cx();
      y1 = ((PortSymbol*)pp)->cy();
      if (x1 < xmin) xmin = x1;
      if (x1 > xmax) xmax = x1;
      if (y1 < ymin) ymin = y1;
      if (y1 > ymax) ymax = y1;
      continue;
    }
    pp->Bounding (x1, y1, x2, y2);
    if (x1 < xmin) xmin = x1;
    if (x2 > xmax) xmax = x2;
    if (y1 < ymin) ymin = y1;
    if (y2 > ymax) ymax = y2;
//    stream << "  " << pp->saveCpp () << "\n";
  }

  stream << "\n  // terminal definitions\n";
  for (int i = 1; i <= maxNum; i++) {
    for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
      if (pp->name() == ".PortSym ")
	if (((PortSymbol*)pp)->numberStr.toInt() == i){
//	  stream << "  " << pp->saveCpp () << "\n";
	  }
    }
  }

  stream << "\n  // symbol boundings\n"
	 << "  x1 = " << xmin << "; " << "  y1 = " << ymin << ";\n"
	 << "  x2 = " << xmax << "; " << "  y2 = " << ymax << ";\n";

  stream << "\n  // property text position\n";
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ())
    if (pp->name() == ".ID "){
//      stream << "  " << pp->saveCpp () << "\n";
    }
  file.close ();
  return 0;
}

// TODO
int SchematicDoc::saveSymbolJSON()
{
  QFileInfo info (docName());
  QString jsonfile = info.path () + QDir::separator()
                   + info.completeBaseName() + "_sym.json";

  qDebug() << "saveSymbolJson for " << jsonfile;

  QFile file (jsonfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
		   QObject::tr("Cannot save JSON symbol file \"%1\"!").arg(jsonfile));
    return -1;
  }

  QTextStream stream (&file);

  // automatically compute boundings of drawing
  int xmin = INT_MAX;
  int ymin = INT_MAX;
  int xmax = INT_MIN;
  int ymax = INT_MIN;
  int x1, y1, x2, y2;
  int maxNum = 0;
  Painting * pp;

  stream << "{\n";

  stream << "\"paintings\" : [\n";

  // symbol drawing code"
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
    if (pp->name() == ".ID ") continue;
    if (pp->name() == ".PortSym ") {
      if (((PortSymbol*)pp)->numberStr.toInt() > maxNum)
	maxNum = ((PortSymbol*)pp)->numberStr.toInt();
      x1 = ((PortSymbol*)pp)->cx();
      y1 = ((PortSymbol*)pp)->cy();
      if (x1 < xmin) xmin = x1;
      if (x1 > xmax) xmax = x1;
      if (y1 < ymin) ymin = y1;
      if (y1 > ymax) ymax = y1;
      continue;
    }
    pp->Bounding (x1, y1, x2, y2);
    if (x1 < xmin) xmin = x1;
    if (x2 > xmax) xmax = x2;
    if (y1 < ymin) ymin = y1;
    if (y2 > ymax) ymax = y2;
//    stream << "  " << pp->saveJSON() << "\n";
  }

  // terminal definitions
  //stream << "terminal \n";
  for (int i = 1; i <= maxNum; i++) {
    for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
      if (pp->name() == ".PortSym ")
	if (((PortSymbol*)pp)->numberStr.toInt() == i){
	//  stream << "  " << pp->saveJSON () << "\n";
	}
    }
  }

  stream << "],\n"; //end of paintings JSON array

  // symbol boundings
  stream
    << "  \"x1\" : " << xmin << ",\n" << "  \"y1\" : " << ymin << ",\n"
    << "  \"x2\" : " << xmax << ",\n" << "  \"y2\" : " << ymax << ",\n";

  // property text position
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ())
    if (pp->name() == ".ID "){
    //  stream << "  " << pp->saveJSON () << "\n";
    }

  stream << "}\n";

  file.close ();
  return 0;
}
