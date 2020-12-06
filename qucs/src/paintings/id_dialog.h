/***************************************************************************
                               id_dialog.h
                              -------------
    begin                : Sat Oct 16 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*! \file id_dialog.h
  * \brief Dialog to edit parameters in a subcircuit symbol.
  */

#ifndef ID_DIALOG_H
#define ID_DIALOG_H

#include <QDialog> // oops?

class QTableWidget;
class QLineEdit;
class QCheckBox;
class QVBoxLayout;
class QRegExpValidator;
class Element;

/*!
 * \brief The ID_Dialog class, edit subcircuit symbol properties.
 */
class ID_Dialog : public QDialog  {
Q_OBJECT
public:
  ID_Dialog(Element*);
 ~ID_Dialog();

private:
  Element *idText;

  QVBoxLayout *all;
  QLineEdit *Prefix;

  QTableWidget *ParamTable;
  QCheckBox *showCheck;
  QLineEdit *ParamNameEdit, *ValueEdit, *DescriptionEdit, *TypeEdit;

  QRegExp Expr;
  QRegExpValidator *SubVal, *NameVal, *ValueVal, *DescrVal, *TypeVal;

private slots:
  void slotOk();
  void slotApply();
  void slotAddParameter();
  void slotRemoveParameter();
  void slotEditParameter();
};

#endif
