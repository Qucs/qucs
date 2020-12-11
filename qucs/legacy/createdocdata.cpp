
pulled out of main. this does not work yet.

/*!
 * \brief createDocData Create data used for documentation.
 *
 * It creates the following:
 *  - list of categories: categories.txt
 *  - category directory, ex.: ./lumped components/
 *    - CSV with component data fields. Ex [component#]_data.csv
 *    - CSV with component properties. Ex [component#]_props.csv
 */
void createDocData()
{ untested();
  QMap<int, QString> typeMap;
  typeMap.insert(0x30000, "Component");
  typeMap.insert(0x30002, "ComponentText");
  typeMap.insert(0x10000, "AnalogComponent");
  typeMap.insert(0x20000, "DigitalComponent") ;

  Module::registerModules ();
  QStringList cats = Category::getCategories ();
  int nCats = cats.size();

  QStringList catHeader;
  catHeader << "# Note: auto-generated file (changes will be lost on update)";
  QFile file("categories.txt");
  if (!file.open(QFile::WriteOnly | QFile::Text)) return;
  QTextStream out(&file);
  out << cats.join("\n");
  file.close();

  int nComps = 0;

  // table for quick reference, schematic and netlist entry
  foreach(QString category, cats) { untested();

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") break;

    // one dir per category
    QString curDir = "./"+category+"/";
    qDebug() << "Creating dir:" << curDir;
    if(!QDir(curDir).exists()){ untested();
        QDir().mkdir(curDir);
    }

    QString File; // what is this, really?
    QString Name;

    int num = 0; // compoment id inside category

    foreach (Module *Mod, Comps) { untested();
        num += 1;

        nComps += 1;

	assert(Mod->has_element());
        Element *e = prechecked_cast<Element*>(Mod->element()->clone()); // memory leak?!
	assert(e);
      // 	(Name, File, true);
        Name = e->label();
        File = e->iconBasename();
        Component *c = dynamic_cast<Component*>(e);
	assert(c); // will fail..

        // object info
        QStringList compData;

        compData << "# Note: auto-generated file (changes will be lost on update)";
        compData << "Caption; "           + Name;
        compData << "Description; "       + c->description();
        compData << "Identifier; ``"      + c->obsolete_model_hack() + "``"; // backticks for reST verbatim
        compData << "Default name; ``"    + c->name()  + "``";
        compData << "Type; "              + typeMap.value(c->Type);
        compData << "Bitmap file; "       + QString(File);
        compData << "Properties; "        + QString::number(c->Props.count());
        compData << "Category; "          + category;

        // 001_data.csv - CSV file with component data
        QString ID = QString("%1").arg(num,3,'d',0,'0');
        QString objDataFile;
        objDataFile = QString("%1_data.csv").arg( ID  ) ;

        QFile file(curDir + objDataFile);
        if (!file.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream out(&file);
        out << compData.join("\n");
        file.close();
	fprintf(stdout, "[%s] %s %s \n", category.toAscii().data(),
	    c->obsolete_model_hack().toAscii().data(),
	    file.fileName().toAscii().data());

        QStringList compProps;
        compProps << "# Note: auto-generated file (changes will be lost on update)";
        compProps << QString("# %1; %2; %3; %4").arg(  "Name", "Value", "Display", "Description");
        foreach(Property *prop, c->Props) { untested();
          compProps << QString("%1; \"%2\"; %3; \"%4\"").arg(
                         prop->Name,
                         prop->Value,
                         prop->display?"yes":"no",
                         prop->Description.replace("\"","\"\"")); // escape quote in quote
        }

        // 001_props.csv - CSV file with component properties
        QString objPropFile = QString("%1_prop.csv").arg( ID ) ;

        QFile fileProps(curDir + objPropFile );
        if (!fileProps.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream outProps(&fileProps);
        outProps << compProps.join("\n");
        compProps.clear();
        file.close();
	fprintf(stdout, "[%s] %s %s \n", category.toAscii().data(),
	    c->obsolete_model_hack().toAscii().data(),
	    fileProps.fileName().toAscii().data());
    } // module
  } // category
  fprintf(stdout, "Created data for %i components from %i categories\n", nComps, nCats);
}
