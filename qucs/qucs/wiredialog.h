/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_WIRE_DIALOG_H
#define QUCS_WIRE_DIALOG_H

#include "symbol.h"
#include "platform.h"
#include "schematic_dialog.h"

#include <QDialog>
#include <assert.h>

class Schematic;

class QValidator;
class QIntValidator;
class QRegExpValidator;
class QTableWidget;
class QTableWidgetItem;
class QLabel;
class QLineEdit;
class QCheckBox;
class QRegExp;
class QComboBox;
class QPushButton;
class QVBoxLayout;


class WireDialog : public SchematicDialog {
public:
  WireDialog(QucsDoc*);
  ~WireDialog();

  void attach(ElementGraphics* c) override;

private: // slot overrides.
  void slotButtOK() override;
  void slotButtCancel() override;
  void slotApplyInput() override;

protected slots:
    void reject();
    bool eventFilter(QObject *obj, QEvent *event);

private:
  QValidator* _labelValidator;
  QVBoxLayout* _all;
  QLineEdit *CompNameEdit;
//  QLineEdit   *edit, *NameEdit;
  ElementGraphics* _gfx;
  Symbol const* _comp;
  bool _changed;
};

#endif
