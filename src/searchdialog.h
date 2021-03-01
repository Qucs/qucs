/***************************************************************************
    copyright            : (C) 2006 by Michael Margraf
                               2014, Yodalee, lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_SEARCHDIALOG_H
#define QUCS_SEARCHDIALOG_H

#include <QDialog>

namespace qucs {
class Doc;
}

namespace Ui {
  class SearchDialog;
}

class SearchDialog : public QDialog
{
  Q_OBJECT

public:
  SearchDialog(QWidget *);
 ~SearchDialog();

  void initSearch(qucs::Doc *doc, const QString &text, bool replace=false);

signals:
  void search(const QString &str, bool CaseSensitive, bool wordOnly, bool backward);
  void replace(const QString &str, const QString &str2, bool needConfirmed,
               bool CaseSensitive, bool wordOnly, bool backward);

private slots:
  void slotSearch();
  void slotDisconnect();

private:
  Ui::SearchDialog *ui;
  QWidget *doc;
};

#endif
