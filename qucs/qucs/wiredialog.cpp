/***************************************************************************
    copyright            : (C) 2003, 2004 by Michael Margraf
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

#include "wiredialog.h"
#include "qucs.h"
#include "schematic_doc.h"
#include "misc.h"
#include <QLineEdit>
#include <QLabel>
#include <QRegExpValidator>
#include <QVBoxLayout>
#include "swap.h"

WireDialog::WireDialog(QucsDoc* d) : SchematicDialog(d)
{
  resize(450, 250);
  setWindowTitle(tr("Edit Wire Properties"));
}

void WireDialog::attach(ElementGraphics* gfx)
{
  trace0("WireDialog::attach");
  auto Comp = prechecked_cast<Symbol*>(element(gfx));
  assert(Comp);
  if(_comp){
    incomplete();
  }else{
  }
  _comp = Comp;
  _gfx = gfx;
  QString s;

  _all = new QVBoxLayout; // to provide neccessary size
  this->setLayout(_all);
  _all->setContentsMargins(1,1,1,1);
  QGridLayout *gp1;
  gp1 = new QGridLayout();
  _all->addLayout(gp1);

  // ...........................................................
  // BUG: memory leak? CHECK: does addWidget transfer??
  gp1->addWidget(new QLabel(Comp->description()), 0,0,1,2);

  QHBoxLayout *h5 = new QHBoxLayout;
  h5->setSpacing(5);

  h5->addWidget(new QLabel(tr("Name:")) );

  CompNameEdit = new QLineEdit;
  h5->addWidget(CompNameEdit);

  QRegExp expr;
  expr.setPattern("[\\w_]+");  // valid expression for property 'NameEdit'
  _labelValidator = new QRegExpValidator(expr, this);

  CompNameEdit->setValidator(_labelValidator);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  QWidget *hTop = new QWidget;
  hTop->setLayout(h5);

  gp1->addWidget(hTop,1,0);

  // H layout inside the GroupBox
  QHBoxLayout *hProps = new QHBoxLayout;
  // PropertyBox->setLayout(hProps);

  // left pane
  QWidget *vboxPropsL = new QWidget;
  QVBoxLayout *vL = new QVBoxLayout;
  vboxPropsL->setLayout(vL);


  // ...........................................................
  QHBoxLayout *h2 = new QHBoxLayout;
  QWidget * hbox2 = new QWidget;
  hbox2->setLayout(h2);
  h2->setSpacing(5);
  _all->addWidget(hbox2);
  QPushButton *ok = new QPushButton(tr("OK"));
  QPushButton *apply = new QPushButton(tr("Apply"));
  QPushButton *cancel = new QPushButton(tr("Cancel"));
  h2->addWidget(ok);
  h2->addWidget(apply);
  h2->addWidget(cancel);
  connect(ok,     SIGNAL(clicked()), SLOT(slotButtOK()));
  connect(apply,  SIGNAL(clicked()), SLOT(slotApplyInput()));
  connect(cancel, SIGNAL(clicked()), SLOT(slotButtCancel()));

  // ------------------------------------------------------------
  CompNameEdit->setText(Comp->label());
  // showName->setChecked(Comp->showName);
  _changed = false;

}

WireDialog::~WireDialog()
{
  delete _all;
  delete _labelValidator;
}

// check if Enter is pressed while the ComboEdit has focus
// in case, behave as for the LineEdits
// (QComboBox by default does not handle the Enter/Return key)
bool WireDialog::eventFilter(QObject *obj, QEvent *event)
{
#if 0
  if (obj == ComboEdit) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if ((keyEvent->key() == Qt::Key_Return) ||
          (keyEvent->key() == Qt::Key_Enter)) {
        slotApplyProperty();
        return true;
      }
    }
  }
  return QDialog::eventFilter(obj, event); // standard event processing
#endif
  return false;
}
// -------------------------------------------------------------------------
// Is called if the "OK"-button is pressed.
void WireDialog::slotButtOK()
{
  slotApplyInput();
  slotButtCancel();
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void WireDialog::slotButtCancel()
{
  if(_changed){
    // changed could have been done before
    done(1);
  } else{
    // (by "Apply"-button)
    done(0);	
  }
}

//-----------------------------------------------------------------
// To get really all close events (even <Escape> key).
void WireDialog::reject()
{
  slotButtCancel();
}

// -------------------------------------------------------------------------
// Is called, if the "Apply"-button is pressed.
void WireDialog::slotApplyInput()
{
  assert(_comp);
  auto C = _comp->clone();
  C->setOwner( _comp->mutable_owner() );
  auto Comp = prechecked_cast<Symbol*>(C);
  assert(Comp);

///  if(Comp->showName != showName->isChecked()) {
///    Comp->showName = showName->isChecked();
///    changed = true;
///  }else{
///  }
///
  QString tmp;
  Component *pc = nullptr;
  if(CompNameEdit->text().isEmpty()){
    CompNameEdit->setText(Comp->label());
  }else if(CompNameEdit->text() != Comp->label()) {
    trace2("Apply", Comp->label(), CompNameEdit->text());
#if 0
    for(pc = schematic()->components().first(); pc!=0;
        pc=schematic()->components().next()){
      if(pc->name() == CompNameEdit->text()){
        break;  // found component with this name?
      }else{
      }
    }
#endif
    if(pc){
      CompNameEdit->setText(Comp->label());
    } else if (Comp->label() != CompNameEdit->text()) {
      Comp->setLabel(CompNameEdit->text().toStdString());
      _changed = true;
    }
  }

  /*! Walk the original Comp->Props and compare with the
   *  data in the dialog.
   *  The pointers to the combo, edits,... are set to 0.
   *  Only check if the widgets were created (pointers checks are 'true')
   */
  bool display;
//  Property *pp = Comp->Props.first();

  // pick selected row
  QTableWidgetItem *item = 0;

  //  make sure we have one item, take selected

  if(_changed) {
    auto pos = _gfx->pos();

    auto cmd = new SwapSymbolCommand(_gfx, Comp);
    execute(cmd);

    assert(_gfx->pos() == pos); // for now.

    _comp = Comp; //  = component(_gfx);

    auto V=schematic()->viewport();
    assert(V);
    V->repaint();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
