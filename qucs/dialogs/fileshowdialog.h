/***************************************************************************
                          fileshowdialog.h  -  description
                             -------------------
    begin                : Mon Nov 17 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILESHOWDIALOG_H
#define FILESHOWDIALOG_H

#include <qdialog.h>

/**
  *@author Michael Margraf
  */

class FileShowDialog : public QDialog  {
   Q_OBJECT
public: 
	FileShowDialog(const QString& _FileName, QWidget *parent=0, const char *name=0);
	~FileShowDialog();
};

#endif
