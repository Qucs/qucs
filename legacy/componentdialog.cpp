// TODO
#include "qucs_app.h"
#include "schematic_doc.h"
#include "misc.h"

#include <cmath>

#include <QLabel>
#include <QLayout>
#include <QValidator>
#include <QTableWidget>
#include <QHeaderView>
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include "swap.h"
/*--------------------------------------------------------------------------*/

// TODO
//
/*--------------------------------------------------------------------------*/
/*!
  Add description if missing.
  Is called if the add button is pressed. This is only possible for some
 properties.
 If desc is empy, ButtAdd is enabled, this slot handles if it is clicked.
 Used with: Equation, ?

 Original behavior for an Equation block
  - start with props
    y=1 (Name, Value)
    Export=yes
  - add equation, results
    y=1
    y2=1
    Export=yes

  Behavior:
   If Name already exists, set it to focus
   If new name, insert item after selected, set it to focus

*/
void ComponentDialog::slotButtAdd()
{
  // Set existing equation into focus, return
  for(int row=0; row < prop->rowCount(); row++) {
    QString name  = prop->item(row, 0)->text();
    if( name == NameEdit->text()) {
      prop->setCurrentItem(prop->item(row,0));
      slotSelectProperty(prop->item(row,0));
      return;
    }
  }

  // toggle display flag
  QString s = tr("no");
  if(disp->isChecked())
    s = tr("yes");

  // get number for selected row
  int curRow = prop->currentRow();

  // insert new row under current
  int insRow = curRow+1;
  prop->insertRow(insRow);

  // append new row
  QTableWidgetItem *cell;
  cell = new QTableWidgetItem(NameEdit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 0, cell);
  cell = new QTableWidgetItem(edit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 1, cell);
  cell = new QTableWidgetItem(s);
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 2, cell);
  // no description? add empty cell
  cell = new QTableWidgetItem("");
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 3, cell);

  // select new row
  prop->selectRow(insRow);
}
/*--------------------------------------------------------------------------*/
// only eqn?
void ComponentDialog::slotButtRem()
{
  if(prop->rowCount() < 3)
    return;  // the last property cannot be removed

  QTableWidgetItem *item = prop->selectedItems()[0];
  int row = item->row();

  if(item == 0)
    return;

  // peek next, delete current, set next current
  if ( row < prop->rowCount()) {
    prop->setCurrentItem(prop->item(row+1,0));
    slotSelectProperty(prop->item(row+1,0));
    prop->removeRow(row);
    }
}

/*--------------------------------------------------------------------------*/
// EQN only
void ComponentDialog::slotButtUp()
{
  qDebug() << "slotButtUp" << prop->currentRow() << prop->rowCount();

  int curRow = prop->currentRow();
  if (curRow == 0)
    return;

  // swap current and row above it
  QTableWidgetItem *source = prop->takeItem(curRow  ,0);
  QTableWidgetItem *target = prop->takeItem(curRow-1,0);
  prop->setItem(curRow-1, 0, source);
  prop->setItem(curRow, 0, target);
  source = prop->takeItem(curRow  ,1);
  target = prop->takeItem(curRow-1,1);
  prop->setItem(curRow-1, 1, source);
  prop->setItem(curRow, 1, target);


  // select moved row
  prop->selectRow(curRow-1);
}
/*--------------------------------------------------------------------------*/
// EQN only
void ComponentDialog::slotButtDown()
{
  qDebug() << "slotButtDown" << prop->currentRow() << prop->rowCount();

  int curRow = prop->currentRow();
  // Leave Export as last
  if (curRow == prop->rowCount()-2)
    return;

  // swap current and row below it
  QTableWidgetItem *source = prop->takeItem(curRow,0);
  QTableWidgetItem *target = prop->takeItem(curRow+1,0);
  prop->setItem(curRow+1, 0, source);
  prop->setItem(curRow, 0, target);
  source = prop->takeItem(curRow,1);
  target = prop->takeItem(curRow+1,1);
  prop->setItem(curRow+1, 1, source);
  prop->setItem(curRow, 1, target);

  // select moved row
  prop->selectRow(curRow+1);
}
/*--------------------------------------------------------------------------*/
// looks like simTask?
void ComponentDialog::slotSimTypeChange(int)
{
#if 0
  if(Type < 2) {  // new type is "linear" or "logarithmic"
    if(!editNumber->isEnabled()) {  // was the other mode before ?
      // this text change, did not emit the textChange signal !??!
      editStart->setText(
	editValues->text().section(';', 0, 0).trimmed());
      editStop->setText(
	editValues->text().section(';', -1, -1).trimmed());
      editNumber->setText("2");
      slotNumberChanged(0);

      checkStart->setChecked(true);
      checkStop->setChecked(true);
    }
    textValues->setDisabled(true);
    editValues->setDisabled(true);
    checkValues->setDisabled(true);
    textStart->setDisabled(false);
    editStart->setDisabled(false);
    checkStart->setDisabled(false);
    textStop->setDisabled(false);
    editStop->setDisabled(false);
    checkStop->setDisabled(false);
    textStep->setDisabled(false);
    editStep->setDisabled(false);
    textNumber->setDisabled(false);
    editNumber->setDisabled(false);
    checkNumber->setDisabled(false);
    if(Type == 1)   // logarithmic ?
      textStep->setText(tr("Points per decade:"));
    else
      textStep->setText(tr("Step:"));
  }
  else {  // new type is "list" or "constant"
    if(!editValues->isEnabled()) {   // was the other mode before ?
      editValues->setText(editStart->text() + "; " + editStop->text());
      checkValues->setChecked(true);
    }

    textValues->setDisabled(false);
    editValues->setDisabled(false);
    checkValues->setDisabled(false);
    textStart->setDisabled(true);
    editStart->setDisabled(true);
    checkStart->setDisabled(true);
    textStop->setDisabled(true);
    editStop->setDisabled(true);
    checkStop->setDisabled(true);
    textStep->setDisabled(true);
    editStep->setDisabled(true);
    textNumber->setDisabled(true);
    editNumber->setDisabled(true);
    checkNumber->setDisabled(true);
    textStep->setText(tr("Step:"));
  }
#endif
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotParamEntered()
{
  if(editValues->isEnabled()){ untested();
    editValues->setFocus();
  }else{ untested();
    editStart->setFocus();
  }
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotSimEntered(int)
{
  unreachable();
  editParam->setFocus();
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotValuesEntered()
{
  slotButtOK();
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotStartEntered()
{
  editStop->setFocus();
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotStopEntered()
{
  editStep->setFocus();
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotStepEntered()
{
  editNumber->setFocus();
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotNumberEntered()
{
  slotButtOK();
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotHHeaderClicked(int headerIdx)
{
  if (headerIdx != 2) return; // clicked on header other than 'display'

  QString s;
  QTableWidgetItem *cell;

  if (setAllVisible) {
    s = tr("yes");
    disp->setChecked(true);
  } else {
    s = tr("no");
    disp->setChecked(false);
  }

  // go through all the properties table and set the visibility cell
  for (int row = 0; row < prop->rowCount(); row++) {
    cell = prop->item(row, 2);
    cell->setText(s);
  }
  setAllVisible = not setAllVisible; // toggle visibility for the next double-click
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::disableButtons()
{
  ButtUp->setEnabled(false);
  ButtDown->setEnabled(false);
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::enableButtons()
{
  ButtUp->setEnabled(true);
  ButtDown->setEnabled(true);
}
ComponentDialog::ComponentDialog(QucsDoc* d) : SchematicDialog(d)
{
  resize(450, 250);
  setWindowTitle(tr("Edit Component Properties"));
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::attach(ElementGraphics* gfx)
{
  trace0("ComponentDialog::attach");
  auto Comp = dynamic_cast<Component*>(element(gfx));
  assert(Comp);
  if(_comp){
    incomplete();
  }else{
  }
  _comp = Comp;
  _gfx = gfx;
  QString s;
  setAllVisible = true; // state when toggling properties visibility

  _all = new QVBoxLayout; // to provide neccessary size
  this->setLayout(_all);
  _all->setContentsMargins(1,1,1,1);
  QGridLayout *gp1;

  ValInteger = new QIntValidator(1, 1000000, this);

  Expr.setPattern("[^\"=]*");  // valid expression for property 'edit'
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[^\"]*");   // valid expression for property 'edit'
  Validator2 = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property 'NameEdit'
  ValRestrict = new QRegExpValidator(Expr, this);

  checkSim  = 0;  comboSim  = 0;  comboType  = 0;  checkParam = 0;
  editStart = 0;  editStop = 0;  editNumber = 0;
  
  Property *pp = 0; // last property shown elsewhere outside the properties table, not to put in TableView
  // ...........................................................
  //gp1 = new QGridLayout(0, 9,2,5,5);
  gp1 = new QGridLayout();
  _all->addLayout(gp1);


  // ...........................................................
  // BUG: memory leak
  gp1->addWidget(new QLabel(Comp->description()), 0,0,1,2);

  QHBoxLayout *h5 = new QHBoxLayout;
  h5->setSpacing(5);

  h5->addWidget(new QLabel(tr("Name:")) );

  CompNameEdit = new QLineEdit;
  h5->addWidget(CompNameEdit);

  CompNameEdit->setValidator(ValRestrict);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  showName = new QCheckBox(tr("display in schematic"));
  h5->addWidget(showName);

  QWidget *hTop = new QWidget;
  hTop->setLayout(h5);

  gp1->addWidget(hTop,1,0);

  QGroupBox *PropertyBox = new QGroupBox(tr("Properties"));
  gp1->addWidget(PropertyBox,2,0);

  // H layout inside the GroupBox
  QHBoxLayout *hProps = new QHBoxLayout;
  PropertyBox->setLayout(hProps);

  // left pane
  QWidget *vboxPropsL = new QWidget;
  QVBoxLayout *vL = new QVBoxLayout;
  vboxPropsL->setLayout(vL);

  /// \todo column min width
  prop = new QTableWidget(0,4); //initialize
  vL->addWidget(prop);
  prop->verticalHeader()->setVisible(false);
  prop->setSelectionBehavior(QAbstractItemView::SelectRows);
  prop->setSelectionMode(QAbstractItemView::SingleSelection);
  prop->setMinimumSize(200, 150);
  prop->horizontalHeader()->setStretchLastSection(true);
  // set automatic resize so all content will be visible, 
  //  horizontal scrollbar will appear if table becomes too large
  prop->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  prop->horizontalHeader()->setClickable(false); // no action when clicking on the header 

  connect(prop->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),
              this, SLOT(slotHHeaderClicked(int)));

  QStringList headers;
  headers << tr("Name")
          << tr("Value")
          << tr("display")
          << tr("Description");
  prop->setHorizontalHeaderLabels(headers);

  // right pane
  QWidget *vboxPropsR = new QWidget;
  QVBoxLayout *v1 = new QVBoxLayout;
  vboxPropsR->setLayout(v1);

  v1->setSpacing(3);

  hProps->addWidget(vboxPropsL, 5); // stretch the left pane (with the table) when resized
  hProps->addWidget(vboxPropsR);

  Name = new QLabel;
  v1->addWidget(Name);

  Description = new QLabel;
  v1->addWidget(Description);

  // hide, because it only replaces 'Description' in some cases
  NameEdit = new QLineEdit;
  v1->addWidget(NameEdit);
  NameEdit->setVisible(false);
  NameEdit->setValidator(ValRestrict);
  connect(NameEdit, SIGNAL(returnPressed()), SLOT(slotApplyPropName()));

  edit = new QLineEdit;
  v1->addWidget(edit);
  edit->setMinimumWidth(150);
  edit->setValidator(Validator2);
  connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyProperty()));

  // hide, because it only replaces 'edit' in some cases
  ComboEdit = new QComboBox;
  v1->addWidget(ComboEdit);
  ComboEdit->setVisible(false);
  ComboEdit->installEventFilter(this); // to catch Enter keypress
  connect(ComboEdit, SIGNAL(activated(const QString&)),
	  SLOT(slotApplyChange(const QString&)));

  QHBoxLayout *h3 = new QHBoxLayout;
  v1->addLayout(h3);

  EditButt = new QPushButton(tr("Edit"));
  h3->addWidget(EditButt);
  EditButt->setEnabled(false);
  EditButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(EditButt, SIGNAL(clicked()), SLOT(slotEditFile()));

  BrowseButt = new QPushButton(tr("Browse"));
  h3->addWidget(BrowseButt);
  BrowseButt->setEnabled(false);
  BrowseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowseFile()));

  disp = new QCheckBox(tr("display in schematic"));
  v1->addWidget(disp);
  connect(disp, SIGNAL(stateChanged(int)), SLOT(slotApplyState(int)));

  // keep group above together
  v1->addStretch(5);

  QGridLayout *bg = new QGridLayout;
  v1->addLayout(bg);
  ButtAdd = new QPushButton(tr("Add"));
  bg->addWidget(ButtAdd, 0, 0);
  ButtAdd->setEnabled(false);
  ButtRem = new QPushButton(tr("Remove"));
  bg->addWidget(ButtRem, 0, 1);
  ButtRem->setEnabled(false);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  connect(ButtRem, SIGNAL(clicked()), SLOT(slotButtRem()));
  // Buttons to move equations up/down on the list
  ButtUp = new QPushButton(tr("Move Up"));
  bg->addWidget(ButtUp, 1, 0);
  ButtDown = new QPushButton(tr("Move Down"));
  bg->addWidget(ButtDown, 1, 1);
  connect(ButtUp,   SIGNAL(clicked()), SLOT(slotButtUp()));
  connect(ButtDown, SIGNAL(clicked()), SLOT(slotButtDown()));


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
  CompNameEdit->setText(QString::fromStdString(Comp->label()));
  showName->setChecked(Comp->showName);
  changed = false;

  Comp->textSize(tx_Dist, ty_Dist);
  int tmp = Comp->tx+tx_Dist - Comp->x1_();
  if((tmp > 0) || (tmp < -6))  tx_Dist = 0;  // remember the text position
  tmp = Comp->ty+ty_Dist - Comp->y1_();
  if((tmp > 0) || (tmp < -6))  ty_Dist = 0;

  /*! Insert all \a Comp properties into the dialog \a prop list */
  int row=0; // row counter
  Comp->Props.findRef(pp);
  Property *p=Comp->Props.current();
  if(p){
    p = Comp->Props.next();
  }else{
    p = Comp->Props.first();
  }
  for(; p!=nullptr; p=Comp->Props.next()) {

      // do not insert if already on first tab
      // this is the reason it was originally from back to front...
      // the 'pp' is the lasted property stepped over while filling the Swep tab
  //    if(p == pp)
  //      break;
      if(p->display){
        s = tr("yes");
      }else{
        s = tr("no");
      }

      // add Props into TableWidget
      qDebug() << " Loading Comp->Props :" << p->Name << p->Value << p->display << p->Description ;

      prop->setRowCount(prop->rowCount()+1);

      QTableWidgetItem *cell;
      cell = new QTableWidgetItem(p->Name);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 0, cell);
      cell = new QTableWidgetItem(p->Value);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 1, cell);
      cell = new QTableWidgetItem(s);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 2, cell);
      cell = new QTableWidgetItem(p->Description);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 3, cell);

      row++;
    }

    if(prop->rowCount() > 0) {
        prop->setCurrentItem(prop->item(0,0));
        slotSelectProperty(prop->item(0,0));
    }


  /// \todo add key up/down brose and select prop
  connect(prop, SIGNAL(itemClicked(QTableWidgetItem*)),
                SLOT(slotSelectProperty(QTableWidgetItem*)));
}
/*--------------------------------------------------------------------------*/
ComponentDialog::~ComponentDialog()
{
  delete _all;
  delete Validator;
  delete Validator2;
  delete ValRestrict;
  delete ValInteger;
}
/*--------------------------------------------------------------------------*/
// check if Enter is pressed while the ComboEdit has focus
// in case, behave as for the LineEdits
// (QComboBox by default does not handle the Enter/Return key)
bool ComponentDialog::eventFilter(QObject *obj, QEvent *event)
{
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
}
/*--------------------------------------------------------------------------*/
// Updates component property list. Useful for MultiViewComponents (really?)
void ComponentDialog::updateCompPropsList()
{
 auto Comp=_comp;
    int last_prop=0; // last property not to put in ListView

    QString s;
    int row=0; // row counter
    //for(Property *p = Comp->Props.first(); p != 0; p = Comp->Props.next()) {}
    for(Property *p = Comp->Props.at(last_prop); p != 0; p = Comp->Props.next()) {

      // do not insert if already on first tab
      // this is the reason it was originally from back to front...
      // the 'pp' is the lasted property stepped over while filling the Swep tab
  //    if(p == pp)
  //      break;
      if(p->display){
        s = tr("yes");
      }else{
        s = tr("no");
      }

      // add Props into TableWidget
      qDebug() << " Loading Comp->Props :" << p->Name << p->Value << p->display << p->Description ;

      if (row > prop->rowCount()-1) { // Add new rows
          prop->setRowCount(prop->rowCount()+1);
      }

      QTableWidgetItem *cell;
      cell = new QTableWidgetItem(p->Name);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 0, cell);
      cell = new QTableWidgetItem(p->Value);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 1, cell);
      cell = new QTableWidgetItem(s);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 2, cell);
      cell = new QTableWidgetItem(p->Description);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 3, cell);

      row++;
    }

    if(prop->rowCount() > 0) {
        prop->setCurrentItem(prop->item(0,0));
        slotSelectProperty(prop->item(0,0));
    }else{
    }

    if (row < prop->rowCount()-1) {
        prop->setRowCount(row);
    }else{
    }
}
/*--------------------------------------------------------------------------*/
// Is called if a property is selected.
// Handle the Property editor tab.
// It transfers the values to the right side for editing.
void ComponentDialog::slotSelectProperty(QTableWidgetItem *item)
{
  auto Comp=_comp;
  if(item == 0) return;
  item->setSelected(true);  // if called from elsewhere, this was not yet done

  qDebug() << "row " << item->row(); //<< item->text()

  QString name  = prop->item(item->row(),0)->text();
  QString value = prop->item(item->row(),1)->text();
  QString show  = prop->item(item->row(),2)->text();
  QString desc  = prop->item(item->row(),3)->text();

  if(show == tr("yes")){
    disp->setChecked(true);
  }else{
    disp->setChecked(false);
  }

  if(name == "File") {
    EditButt->setEnabled(true);
    BrowseButt->setEnabled(true);
  }else{
    EditButt->setEnabled(false);
    BrowseButt->setEnabled(false);
  }

  /// \todo enable edit of description anyway...
  /// empy or "-" (no comment from verilog-a)
  if(desc.isEmpty()) {
    // show two line edit fields (name and value)
    ButtAdd->setEnabled(true);
    ButtRem->setEnabled(true);

    Comp->dialgButtStuff(*this);
    Name->setText("");
    NameEdit->setText(name);
    edit->setText(value);

    edit->setVisible(true);
    NameEdit->setVisible(true);
    Description->setVisible(false);
    ComboEdit->setVisible(false);

    NameEdit->setFocus();   // edit QLineEdit
  }else{  // show standard line edit (description and value)
    ButtAdd->setEnabled(false);
    ButtRem->setEnabled(false);
    ButtUp->setEnabled(false);
    ButtDown->setEnabled(false);

    Name->setText(name);
    edit->setText(value);

    NameEdit->setVisible(false);
    NameEdit->setText(name); // perhaps used for adding properties
    Description->setVisible(true);

    // handle special combobox items
    QStringList List;
    int b = desc.indexOf('[');
    int e = desc.lastIndexOf(']');
    if (e-b > 2) {
      QString str = desc.mid(b+1, e-b-1);
      str.replace( QRegExp("[^a-zA-Z0-9_,]"), "" );
      List = str.split(',');
      qDebug() << "List = " << List;
    }

    // use the screen-compatible metric
    QFontMetrics metrics(QString_(QucsSettings.font), 0);   // get size of text
    qDebug() << "desc = " << desc << metrics.horizontalAdvance(desc);
    while(metrics.horizontalAdvance(desc) > 270) {  // if description too long, cut it nicely
      // so 270 above will be the maximum size of the name label and associated edit line widget 
      if (desc.lastIndexOf(' ') != -1)
        desc = desc.left(desc.lastIndexOf(' ')) + "....";
      else
        desc = desc.left(desc.length()-5) + "....";
    }
    Description->setText(desc);

    if(List.count() >= 1) {    // ComboBox with value list or line edit ?
      ComboEdit->clear();
      ComboEdit->insertItems(0,List);

      for(int i=ComboEdit->count()-1; i>=0; i--)
       if(value == ComboEdit->itemText(i)) {
         ComboEdit->setCurrentIndex(i);
	 break;
       }
      edit->setVisible(false);
      ComboEdit->setVisible(true);
      ComboEdit->setFocus();
    }else{
      edit->setVisible(true);
      ComboEdit->setVisible(false);
      edit->setFocus();   // edit QLineEdit
    }
  }
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotApplyChange(const QString& Text)
{ untested();
  /// \bug what if the table have no items?
  // pick selected row
  QList<QTableWidgetItem *> items = prop->selectedItems();
  Q_ASSERT(!items.isEmpty());
  QTableWidgetItem *item = items.first();
  
  int row = item->row();
  
  edit->setText(Text);
  // apply edit line
  prop->item(row, 1)->setText(Text);

  ComboEdit->setFocus();

  // step to next item if not at the last line
  if ( row < (prop->rowCount() - 1)) {
    prop->setCurrentItem(prop->item(row+1,0));
    slotSelectProperty(prop->item(row+1,0));
  }
}
/*--------------------------------------------------------------------------*/
/*!
 Is called if the "RETURN" key is pressed in the "edit" Widget.
 The parameter is edited on the right pane.
 Return key commits the change, and steps to the next parameter in the list.
*/
void ComponentDialog::slotApplyProperty()
{
  // pick selected row
  QTableWidgetItem *item = prop->currentItem();
  
  if(!item)
    return;
  
  int row = item->row();

  QString name  = prop->item(row, 0)->text();
  QString value = prop->item(row, 1)->text();

  if (!ComboEdit->isHidden())   // take text from ComboBox ?
    edit->setText(ComboEdit->currentText());

  // apply edit line
  if(value != edit->text()) {
       prop->item(row, 1)->setText(edit->text());
    }

  if (!NameEdit->isHidden())	// also apply property name ?
    if (name != NameEdit->text()) {
//      if(NameEdit->text() == "Export")
//        item->setText(0, "Export_");   // name must not be "Export" !!!
//      else
//      item->setText(0, NameEdit->text());  // apply property name
      prop->item(row, 0)->setText(NameEdit->text());
    }

  // step to next item
  if ( row < prop->rowCount()-1) {
    prop->setCurrentItem(prop->item(row+1,0));
    slotSelectProperty(prop->item(row+1,0));
  }
  else {
    slotButtOK();   // close dialog, if it was the last property
    return;
  }
}
/*--------------------------------------------------------------------------*/
// Is called if the "RETURN"-button is pressed in the "NameEdit" Widget.
void ComponentDialog::slotApplyPropName()
{
  // pick selected row
  QTableWidgetItem *item = prop->selectedItems()[0];
  int row = item->row();

  QString name  = prop->item(row, 0)->text();

  if(name != NameEdit->text()) {
//    if(NameEdit->text() == "Export") {
//	item->setText(0, "Export_");   // name must not be "Export" !!!
//	NameEdit->setText("Export_");
//    }
//      else
    prop->item(row, 0)->setText(NameEdit->text());
  }
  edit->setFocus();   // cursor into "edit" widget
}
/*--------------------------------------------------------------------------*/
// Is called if the checkbox is pressed (changed).
void ComponentDialog::slotApplyState(int State)
{
  // pick selected row
  QTableWidgetItem *item = prop->selectedItems()[0];
  int row = item->row();

  QString disp  = prop->item(row, 2)->text();

  if(item == 0) return;

  QString ButtonState;
  if(State)
    ButtonState = tr("yes");
  else
    ButtonState = tr("no");

  if(disp != ButtonState) {
    prop->item(row, 2)->setText(ButtonState);
  }
}
/*--------------------------------------------------------------------------*/
// Is called if the "OK"-button is pressed.
void ComponentDialog::slotButtOK()
{
  slotApplyInput();
  slotButtCancel();
}
/*--------------------------------------------------------------------------*/
// Is called if the "Cancel"-button is pressed.
void ComponentDialog::slotButtCancel()
{
  if(changed){
    // changed could have been done before
    done(1);
  } else{
    // (by "Apply"-button)
    done(0);	
  }
}
/*--------------------------------------------------------------------------*/
// To get really all close events (even <Escape> key).
void ComponentDialog::reject()
{
  slotButtCancel();
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotApplyInput()
{
  assert(_comp);
  auto C = _comp->clone(); // TODO: clone during attach?
  C->set_owner(_comp->owner());
  auto Comp = prechecked_cast<Component*>(C);
  assert(Comp);

  qDebug() << " \n == Apply ";
  if(Comp->showName != showName->isChecked()) {
    Comp->showName = showName->isChecked();
    changed = true;
  }else{
  }

  QString tmp;
  Component *pc = nullptr;
  if(CompNameEdit->text().isEmpty()){
    CompNameEdit->setText(QString::fromStdString(Comp->label()));
  }else if(CompNameEdit->text().toStdString() != Comp->label()) {
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
      CompNameEdit->setText(QString::fromStdString(Comp->label()));
    } else if (Comp->label() != CompNameEdit->text().toStdString()) {
      Comp->setLabel(CompNameEdit->text().toStdString());
      changed = true;
    }
  }

  /*! Walk the original Comp->Props and compare with the
   *  data in the dialog.
   *  The pointers to the combo, edits,... are set to 0.
   *  Only check if the widgets were created (pointers checks are 'true')
   */
  bool display;
  Property *pp = Comp->Props.first();
  // apply all the new property values

  if(comboSim) {
    display = checkSim->isChecked();
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    if(pp->Value != comboSim->currentText()) {
      pp->Value = comboSim->currentText();
      changed = true;
    }
    pp = Comp->Props.next();
  }
  if(comboType) {
    display = checkType->isChecked();
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    switch(comboType->currentIndex()) {
      case 1:  tmp = "log";   break;
      case 2:  tmp = "list";  break;
      case 3:  tmp = "const"; break;
      default: tmp = "lin";   break;
    }
    if(pp->Value != tmp) {
      pp->Value = tmp;
      changed = true;
    }
    pp = Comp->Props.next();
  }
  if(checkParam) if(checkParam->isEnabled()) {
    display = checkParam->isChecked();
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    if(pp->Value != editParam->text()) {
      pp->Value = editParam->text();
      changed = true;
    }
    pp = Comp->Props.next();
  }
  if(editStart) {
    if(comboType->currentIndex() < 2) {
      display = checkStart->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      pp->Name  = "Start";
      if(pp->Value != editStart->text()) {
        pp->Value = editStart->text();
        changed = true;
      }
      pp = Comp->Props.next();

      display = checkStop->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      pp->Name  = "Stop";
      if(pp->Value != editStop->text()) {
        pp->Value = editStop->text();
        changed = true;
      }
      pp = Comp->Props.next();

      display = checkNumber->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      if((pp->Value != editNumber->text()) || (pp->Name != "Points")) {
        pp->Value = editNumber->text();
        pp->Name  = "Points";
        changed = true;
      }
      qDebug() << "====> before ad"
               << pp->Description;

      pp = Comp->Props.next();
    }else{
      // If a value list is used, the properties "Start" and "Stop" are not
      // used. -> Call them "Symbol" to omit them in the netlist.
      pp->Name = "Symbol";
      pp->display = false;
      pp = Comp->Props.next();
      pp->Name = "Symbol";
      pp->display = false;
      pp = Comp->Props.next();

      display = checkValues->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      tmp = "["+editValues->text()+"]";
      if((pp->Value != tmp) || (pp->Name != "Values")) {
        pp->Value = tmp;
        pp->Name  = "Values";
        changed = true;
      }
      qDebug() << "====> before ad"
               << pp;

      pp = Comp->Props.next();
    }
  }


  // pick selected row
  QTableWidgetItem *item = 0;

  //  make sure we have one item, take selected
  if (prop->selectedItems().size()) {
    item = prop->selectedItems()[0];
  }

  /*! Walk the dialog list of 'prop'
   */
   if(item != 0) {
     int row = item->row();
     QString name  = prop->item(row, 0)->text();
     QString value = prop->item(row, 1)->text();

     // apply edit line
     if(value != edit->text())
       prop->item(row, 1)->setText(edit->text());

     // apply property name
     if (!NameEdit->isHidden())
       if (name != NameEdit->text())
         prop->item(row, 0)->setText(NameEdit->text());

     // apply all the new property values in the ListView
     for( int row = 0; row < prop->rowCount(); row++ ) {

       QString name  = prop->item(row, 0)->text();
       QString value = prop->item(row, 1)->text();
       QString disp = prop->item(row, 2)->text();
       QString desc = prop->item(row, 3)->text();

       qDebug() << "====>" <<name << value
                << Comp->Props.count()
                << prop->rowCount() +1
                << pp;

       display = (disp == tr("yes"));
       if( pp ) {

         if(pp->display != display) {
             pp->display = display;
             changed = true;
         }else{
         }

         if(pp->Value != value) {
            pp->Value = value;
            changed = true;
         }else{
         }

         if(pp->Name != name) {
           pp->Name = name;   // override if previous one was removed
           changed = true;
         }else{
         }

         pp->Description = desc;
       } else {
         // if properties where added in the dialog
         // -> create new on the Comp
         Q_ASSERT(prop->rowCount() >= 0);
         if ( (int) Comp->Props.count() < prop->rowCount() +1) {
             qDebug() << "adding to Comp ";
             Comp->Props.append(new Property(name, value, display, desc));
             changed = true;
         }
    }
    pp = Comp->Props.next();
  }

  // original Comp still has properties? (removed some in the dialog?)
  // if more properties than in ListView -> delete the rest
  if(pp) {
    pp = Comp->Props.prev();
    Comp->Props.last();
    while(pp != Comp->Props.current())
      Comp->Props.remove();
    changed = true;
  }

 } // end if (item !=0)

  if(changed) {
    int dx, dy;
    Comp->textSize(dx, dy);
    if(tx_Dist != 0) {
      Comp->tx += tx_Dist-dx;
      tx_Dist = dx;
    }else{
    }
    if(ty_Dist != 0) {
      Comp->ty += ty_Dist-dy;
      ty_Dist = dy;
    }else{
    }


    auto pos = _gfx->pos();

    auto cmd = new SwapSymbolCommand(_gfx, Comp);
    execute(cmd);

    assert(_gfx->pos() == pos); // for now.

    Comp = dynamic_cast<Component*>(element(_gfx));
    _comp = Comp;

    // BUG: cannot modify while shown.
    // maybe do in ElementGraohics::attach?
//    schematic()->recreateSymbol(Comp);
//
    auto V=schematic()->viewport();
    assert(V);
    V->repaint();
    if ( (int) Comp->Props.count() != prop->rowCount()) { // If props count was changed after recreation
      Q_ASSERT(prop->rowCount() >= 0);
      updateCompPropsList(); // of component we need to update properties
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotBrowseFile()
{
  incomplete();
  // current file name from the component properties
  QString currFileName = prop->item(prop->currentRow(), 1)->text();
  QFileInfo currFileInfo(currFileName);
  // name of the schematic where component is instantiated (may be empty)
  QFileInfo schematicFileInfo; // = Comp->getSchematic()->getFileInfo();
  QString schematicFileName; //  = schematicFileInfo.fileName();
  // directory to use for the file open dialog
  QString currDir = "unknown_dir";

  if (!currFileName.isEmpty()) { // a file name is already defined
    if (currFileInfo.isRelative()) { // but has no absolute path
      if (!schematicFileName.isEmpty()) { // if schematic has a filename
        incomplete();
	// build the an absolute file name using the schematic path
	// currDir = schematicFileInfo.absolutePath() + 
	//        QDir::separator() +
        //        currFileInfo.fileName();
      } else { // no absolute paths around
	// use the WorkDir path
	currDir = QString_(QucsSettings.QucsWorkDir) + QDir::separator() +
	  currFileInfo.fileName();
      }
    } else { // current file name is absolute
      // use it
      currDir = currFileName;
    }
  } else { // a file name is not defined
    if (!schematicFileName.isEmpty()) { // if schematic has a filename
      // use the schematic absolute path
      currDir = schematicFileInfo.absolutePath();
    } else { // no absolute paths around
      // use the WorkDir path
      currDir = QString_(QucsSettings.QucsWorkDir);
    }
  }
  
  QString s = QFileDialog::getOpenFileName (
          this,
          tr("Select a file"),
          currDir,
          tr("All Files")+" (*.*);;"
            + tr("Touchstone files") + " (*.s?p);;"
            + tr("CSV files") + " (*.csv);;"
            + tr("SPICE files") + " (*.cir *.spi);;"
            + tr("VHDL files") + " (*.vhdl *.vhd);;"
            + tr("Verilog files")+" (*.v)"  );

  if(!s.isEmpty()) {
    // snip path if file in current directory
    QFileInfo file(s);
    if(!QDir_(QucsSettings.QucsWorkDir).exists(file.fileName())){
    }else if( QDir_(QucsSettings.QucsWorkDir).absolutePath() == file.absolutePath()){
      s = file.fileName();
    }else{
    }
    edit->setText(s);
  }
  /* FIX
  prop->currentIndex()->setText(1, s); */
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotEditFile()
{
  // yikes
  schematic()->_app->editFile(QDir_(QucsSettings.QucsWorkDir).filePath(edit->text()));
}
