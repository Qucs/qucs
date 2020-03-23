/***************************************************************************
                               checklibraries.h
                              -----------
    begin                : Sat Mar 21 2020
    copyright            : (C) 2005 by qucsTeam
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHECKLIBRARIES_H
#define CHECKLIBRARIES_H

#include <QList>
#include <QString>
#include <QStringList>
#include "qucslib_common.h"


class CheckComponentLibraries  {
  private:
    QString qucslibProgName;
    static int parseLibraries (const QString LibDir, QList<ComponentLibrary> &libList);
    static int exeProcess(const QString processPath, const QStringList arguments, QString &stdOut);
  public:
    int checkComponentLibraries(const char *argv0, const QString componentName, QStringList &);
};

#endif /* CHECKLIBRARIES_H */
