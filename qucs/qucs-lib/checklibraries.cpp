/***************************************************************************
                                checklibraries.cpp
                               ----------
    begin                : Sat Mar 21 2020
    copyright            : (C) 2020 by qucsTeam
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QProcess>
#include "qucslib.h"
#include "checklibraries.h"

int CheckComponentLibraries::parseLibraries (const QString LibDir, QList<ComponentLibrary> &libList){
  QStringList LibFiles;
  QStringList::iterator it;

  LibFiles = QDir(LibDir).entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

  // create top level library items, base on the library names
  for(it = LibFiles.begin(); it != LibFiles.end(); it++){
      QString libPath(*it);
      libPath.chop(4); // remove extension

      ComponentLibrary parsedlibrary;

      int ret = parseComponentLibrary (libPath , parsedlibrary);
      if (ret!=QUCS_COMP_LIB_OK){
        return ret;
      }
      libList.insert(libList.end(), parsedlibrary); //add element
  }
  return QUCS_COMP_LIB_OK;
}

int CheckComponentLibraries::exeProcess(const QString processPath, const QStringList arguments, QString &stdOut){
  QProcess simProcess;
  simProcess.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  simProcess.setReadChannelMode(QProcess::MergedChannels);
  simProcess.start(processPath, arguments);
  simProcess.waitForFinished();
  stdOut = QString(simProcess.readAllStandardOutput().data());
  return simProcess.exitCode();
}
int CheckComponentLibraries::checkComponentLibraries(const char *argv0, const QString componentName, QStringList &ignoList){
  qucslibProgName = argv0;
  const char * SCH_FILE="tmpQucsLib.sch";
  const char * NET_FILE="tmpQucsLib.net";
  const char * LOG_FILE="qucsLib.log";
  QList<ComponentLibrary> libList;
  int ret = parseLibraries(QucsSettings.LibDir, libList);
  if (ret != QUCS_COMP_LIB_OK){
    fprintf(stderr, "Error %d in parseLibraries\n", ret);
    return -1;
  }

  QStringList summary;
  QRegExp errLine("\\n([^\\n]+error[^\\n]+)\\n");  //checker error, value of `Vj' (20.8385) is out of range `]0,10]' in `Diode:D2'
  errLine.setMinimal(true); // non greedy
  QString libComp;
  bool ok=true;
  QFile logFile(LOG_FILE);
  if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text))
      return -1;
  QTextStream log(&logFile);
  for(ComponentLibrary parsedLibrary : libList){
    if(ignoList.contains(parsedLibrary.name+".lib")){
      continue;
    }
    for (int i = 0; i < parsedLibrary.components.count (); i++){
      if (!componentName.isEmpty() && parsedLibrary.components[i].name != componentName){
        continue;
      }
      libComp = parsedLibrary.name + " " + parsedLibrary.components[i].name;
      //<Components>
      //<Lib PVR100AZ_B12V_1 1 90 120 20 -66 0 0 "Regulators" 0 "PVR100AZ-B12V" 0>
      //<.DC DC1 1 100 210 0 42 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
      //</Components>
      QFile schFile(SCH_FILE);
      if (!schFile.open(QIODevice::WriteOnly | QIODevice::Text))
          return -1;
      QTextStream f(&schFile);
      f << "<Qucs Schematic 0.0.20>"    << endl;
      f << "<Properties>"               << endl;
      f << "  <View=0,0,800,800,1,0,0>" << endl;
      f << "  <Grid=10,10,1>"           << endl;
      f << "  <DataSet=XXX.dat>"        << endl;
      f << "  <DataDisplay=XXX.dpl>"    << endl;
      f << "  <OpenDisplay=1>"          << endl;
      f << "  <Script=XXX.m>"           << endl;
      f << "  <RunScript=0>"            << endl;
      f << "  <showFrame=0>"            << endl;
      f << "  <FrameText0=Title>"       << endl;
      f << "  <FrameText1=Drawn By:>"   << endl;
      f << "  <FrameText2=Date:>"       << endl;
      f << "  <FrameText3=Revision:>"   << endl;
      f << "</Properties>"              << endl;
      f << "<Symbol>"                   << endl;
      f << "</Symbol>"                  << endl;
      f << "<Components>"               << endl;
      f << parsedLibrary.components[i].modelString << endl; //<Lib XXXX,... or
      f << "<.DC DC1 1 100 210 0 42 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>" << endl;
      f << "</Components>"              << endl;
      f << "<Wires>"                    << endl;
      f << "</Wires>"                   << endl;
      f << "<Diagrams>"                 << endl;
      f << "</Diagrams>"                << endl;
      f << "<Paintings>"                << endl;
      f << "</Paintings>"               << endl;
      schFile.close();

      QStringList arguments;
      QString stdOut;

      QFile::remove(NET_FILE);
      QString qucsProgName = qucslibProgName;
      qucsProgName.replace("/qucslib", "/qucs");
      arguments << "-n" << "-i" << SCH_FILE << "-o" << NET_FILE;
      ret = exeProcess(qucsProgName, arguments, stdOut);
      if (ret!=0){
        log << endl;
        log << "--------------------------" << parsedLibrary.name << "--------------------------" << endl;
        log << qucsProgName << ": error " << ret << " " << libComp;
        // cat SCH_FILE
        QFile schFile(SCH_FILE);
        if (!schFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return -1;
        QTextStream in(&schFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            log << line << endl;
        }
        schFile.close();
        if (!QFile::exists(NET_FILE)){
          continue;
        }
      }
      QString qucsatorProgName = qucslibProgName;
      qucsatorProgName.replace("/qucslib", "/qucsator");
      arguments << "--check" << "-i" << NET_FILE; // qucsator --check -i tmp.net
      if (exeProcess(qucsatorProgName, arguments, stdOut) != 0){
        log << endl;
        log << "--------------------------" << parsedLibrary.name << "--------------------------" << endl;
        log << "name      : " << parsedLibrary.components[i].name << endl;
        log << "definition: " << parsedLibrary.components[i].definition << endl;
        log << "model     : " << parsedLibrary.components[i].modelString << endl;
        log << stdOut << endl;
        ok=false;
        if (errLine.indexIn(stdOut)>=0){
          summary << (libComp.leftJustified(24, ' ')+": "+errLine.cap(1));
        }
      }
    }//for
  }//for
  log << endl;
  log << "--------------------------summary--------------------------" << endl;
  if (summary.size()>0){
    for (QString li : summary){
      log << li << endl;
    }
  }else{
    log << "no errorneous components found!" << endl;
  }
  logFile.close();
  //
  if (componentName.isEmpty()){
    QFile f1(NET_FILE); f1.remove();
    QFile f2(SCH_FILE); f2.remove();
  }
  return ok? 0 : -1;
}


