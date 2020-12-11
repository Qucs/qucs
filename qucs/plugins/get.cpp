
// TODO.
Schematic *openSchematic(QString schematic)
{ untested();
  qDebug() << "*** try to load schematic :" << schematic;

  // QString to *char
  QByteArray ba = schematic.toLatin1();
  const char *c_sch = ba.data();

  QFile file(schematic);  // save simulator messages
  if(file.open(QIODevice::ReadOnly)) { untested();
    file.close();
  }else{ untested();
    fprintf(stderr, "Error: No such file %s\n", c_sch);
    return NULL;
  }

  // new schematic from file
  SchematicModel *sch = new SchematicModel(schematic);

  // load schematic file if possible
  if(!sch->loadDocument()) { untested();
    fprintf(stderr, "Error: Could not load schematic %s\n", c_sch);
    delete sch;
    return NULL;
  }else{ untested();
  }
  return sch;
}
