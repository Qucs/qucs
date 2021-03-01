/***************************************************************************
    copyright            : (C) 2016-2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SYMBOLDIALG_H
#define SYMBOLDIALG_H

#include "symbol.h"
#include "settings.h"
#include "schematic_dialog.h"
#include "symboldialog.h"
#include "schematic_edit.h"
#include "qt_compat.h"
#include "qucs_globals.h"

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
#include "swap_gfx.h"
/*--------------------------------------------------------------------------*/
namespace {

using qucs::SchematicDialog;
using qucs::SwapElementGfx;
using qucs::Element;

class SymbolDialog : public QDialog, public SchematicDialog {
    Q_OBJECT
//    Q_INTERFACES(SchematicDialog)
public:
	explicit SymbolDialog();
	~SymbolDialog();
	Widget* clone() const override{
		incomplete(); // does not copy.
		SchematicDialog* wd = new SymbolDialog();
		return wd;
	}

	void attach(ElementGraphics* c) override;

private slots:
	void slotButtOK();
	void slotButtCancel();
	void slotSelectProperty(QTableWidgetItem * /*item*/);
	void slotApplyInput();
	void slotApplyState(int /* State */);
	void slotBrowseFile(){incomplete();}
	void slotEditFile(){incomplete();}
	void slotApplyChange(const QString& /*Text*/);
	void slotApplyProperty();
	void slotApplyPropName();

	void slotButtAdd(){incomplete();}
	void slotButtRem(){incomplete();}

	void slotButtUp(){incomplete();}
	void slotButtDown(){incomplete();}

	void slotSimTypeChange(int){incomplete();}
	void slotNumberChanged(const QString&){incomplete();}
	void slotStepChanged(const QString&){incomplete();}

	void slotParamEntered();
//	void slotSimEntered(int);
	void slotValuesEntered();
//	void slotStartEntered();
//	void slotStopEntered(){incomplete();}
//	void slotStepEntered(){incomplete();}
	void slotNumberEntered();
	void slotHHeaderClicked(int /*headerIdx*/){incomplete();}

public:
	void enableButtons();
	void disableButtons();

	protected slots:
		void reject();
	bool eventFilter(QObject *obj, QEvent *event);

private:
	QVBoxLayout *_all{nullptr};
	QTableWidget  *prop;
	QLineEdit   *edit, *NameEdit, *CompNameEdit;
	QLabel      *Name, *Description;
	QPushButton *BrowseButt, *EditButt, *ButtAdd, *ButtRem;
	QPushButton *ButtUp, *ButtDown;
	QCheckBox   *disp;
	bool        changed;
	int         tx_Dist, ty_Dist;   // remember the text position
	bool        setAllVisible; // used for toggling visibility of properties

	QLabel    *textType;
	QLabel    *textSim, *textParam, *textValues, *textStart, *textStop,
				 *textStep, *textNumber;
	QLineEdit *editParam, *editValues, *editStart, *editStop,
				 *editStep, *editNumber;
	QCheckBox *checkSim, *checkParam, *checkValues, *checkStart, *checkStop,
				 *checkNumber, *checkType, *showName;

	void updateCompPropsList(void);

private:
	ElementGraphics* _gfx{nullptr};
	ElementGraphics* _gfxclone{nullptr};
	Symbol* _sym{nullptr};
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SymbolDialog::SymbolDialog() : SchematicDialog()
{
	resize(450, 250);
	setWindowTitle(tr("Edit Component Properties"));
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::attach(ElementGraphics* gfx)
{
	trace1("attach", element(gfx)->label());
	assert(gfx);
	_gfx = gfx;

	if(_gfxclone){
		incomplete();
	}else{
		_gfxclone = gfx->clone();
	}
	Element* c = element(_gfxclone);

	_sym = prechecked_cast<Symbol*>(c);
	assert(_sym);
	QString s;
	setAllVisible = true; // state when toggling properties visibility

	delete _all; // BUG?: maybe recycle it.
	_all = new QVBoxLayout; // to provide neccessary size
	this->setLayout(_all);
	_all->setContentsMargins(1,1,1,1);
	QGridLayout *gp1;

	checkSim  = 0;  checkParam = 0;
	editStart = 0;  editStop = 0;  editNumber = 0;

	gp1 = new QGridLayout();
	_all->addLayout(gp1);

	// ...........................................................
	// BUG: memory leak
	gp1->addWidget(new QLabel(QString_(_sym->label()), this), 0,0,1,2);

	QHBoxLayout *h5 = new QHBoxLayout(this);
	h5->setSpacing(5);

	h5->addWidget(new QLabel(tr("Name:"), this) );

	CompNameEdit = new QLineEdit;
	h5->addWidget(CompNameEdit);

	// CompNameEdit->setValidator(ValRestrict);
	connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

	showName = new QCheckBox(tr("display in schematic"), this);
	h5->addWidget(showName);

	QWidget *hTop = new QWidget(this);
	hTop->setLayout(h5);

	gp1->addWidget(hTop,1,0);

	QGroupBox *PropertyBox = new QGroupBox(tr("Properties"));
	gp1->addWidget(PropertyBox,2,0);

	// H layout inside the GroupBox
	QHBoxLayout *hProps = new QHBoxLayout(this);
	PropertyBox->setLayout(hProps);

	// left pane
	QWidget *vboxPropsL = new QWidget(this);
	QVBoxLayout *vL = new QVBoxLayout(this);
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
	//NameEdit->setValidator(ValRestrict);
	connect(NameEdit, SIGNAL(returnPressed()), SLOT(slotApplyPropName()));

	edit = new QLineEdit;
	v1->addWidget(edit);
	edit->setMinimumWidth(150);
	connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyProperty()));

	// hide, because it only replaces 'edit' in some cases
	//  connect(ComboEdit, SIGNAL(activated(const QString&)),
	//	  SLOT(slotApplyChange(const QString&)));

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
	CompNameEdit->setText(QString::fromStdString(_sym->label()));

	incomplete();
//	showName->setChecked(_sym->showName);
	updateCompPropsList();

	/// \todo add key up/down brose and select prop
	connect(prop, SIGNAL(itemClicked(QTableWidgetItem*)),
			SLOT(slotSelectProperty(QTableWidgetItem*)));
}
/*--------------------------------------------------------------------------*/
SymbolDialog::~SymbolDialog()
{
	delete _all;
}
/*--------------------------------------------------------------------------*/
// check if Enter is pressed while the ComboEdit has focus
// in case, behave as for the LineEdits
// (QComboBox by default does not handle the Enter/Return key)
bool SymbolDialog::eventFilter(QObject *obj, QEvent *event)
{
	//  if (obj == ComboEdit) {
	//    if (event->type() == QEvent::KeyPress) {
	//      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
	//      if ((keyEvent->key() == Qt::Key_Return) ||
	//          (keyEvent->key() == Qt::Key_Enter)) {
	//        slotApplyProperty();
	//        return true;
	//      }
	//    }
	//  }
	return QDialog::eventFilter(obj, event); // standard event processing
}
/*--------------------------------------------------------------------------*/
// Update parameters
void SymbolDialog::updateCompPropsList()
{
	auto c = _sym;

	int row=0; // row counter
	//for(Property *p = c->Props.first(); p != 0; p = c->Props.next()) {}
	for(unsigned i=0; i< c->paramCount(); ++i){

		// do not insert if already on first tab
		// this is the reason it was originally from back to front...
		// the 'pp' is the lasted property stepped over while filling the Swep tab
		//    if(p == pp)
		//      break;
		QString s = "yes";
		// if(p->display){
		// 	s = tr("yes");
		// }else{
		// 	s = tr("no");
		// }

		if (row > prop->rowCount()-1) { // Add new rows
			prop->setRowCount(prop->rowCount()+1);
		}

		auto pn = QString::fromStdString(c->paramName(i));
		auto pv = QString::fromStdString(c->paramValue(i));

		QTableWidgetItem *cell;
		cell = new QTableWidgetItem(pn);
		cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
		prop->setItem(row, 0, cell);
		cell = new QTableWidgetItem(pv);
		cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
		prop->setItem(row, 1, cell);
		cell = new QTableWidgetItem(s);
		cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
		prop->setItem(row, 2, cell);
		cell = new QTableWidgetItem("desc");
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
void SymbolDialog::slotSelectProperty(QTableWidgetItem *item)
{

	// assert?
	if(item == 0){
		unreachable();
		return;
	}else{
	}
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

	{  // show standard line edit (description and value)
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
		QFontMetrics metrics(QFont(QString::fromStdString(QucsSettings.font)), 0);   // get size of text
		qDebug() << "desc = " << desc << metrics.horizontalAdvance(desc);
		while(metrics.horizontalAdvance(desc) > 270) {  // if description too long, cut it nicely
			// so 270 above will be the maximum size of the name label and associated edit line widget 
			if (desc.lastIndexOf(' ') != -1)
				desc = desc.left(desc.lastIndexOf(' ')) + "....";
			else
				desc = desc.left(desc.length()-5) + "....";
		}
		Description->setText(desc);

		{
			edit->setVisible(true);
			edit->setFocus();   // edit QLineEdit
		}
	}
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotApplyChange(const QString& Text)
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
void SymbolDialog::slotApplyProperty()
{
	// pick selected row
	QTableWidgetItem *item = prop->currentItem();

	if(!item)
		return;

	int row = item->row();

	QString name  = prop->item(row, 0)->text();
	QString value = prop->item(row, 1)->text();

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
void SymbolDialog::slotApplyPropName()
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
void SymbolDialog::slotApplyState(int State)
{
	// pick selected row
	QTableWidgetItem *item = prop->selectedItems()[0];
	int row = item->row();

	QString disp  = prop->item(row, 2)->text();

	if(item == 0) return;

	QString ButtonState;
	if(State){
		ButtonState = tr("yes");
	} else{
	  	ButtonState = tr("no");
	}

	if(disp != ButtonState) {
		prop->item(row, 2)->setText(ButtonState);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
// Is called if the "OK"-button is pressed.
void SymbolDialog::slotButtOK()
{
	slotApplyInput();
	slotButtCancel();
}
/*--------------------------------------------------------------------------*/
// Is called if the "Cancel"-button is pressed.
void SymbolDialog::slotButtCancel()
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
void SymbolDialog::reject()
{
	slotButtCancel();
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotApplyInput()
{
	assert(_sym);
	auto e = element(_gfxclone);

	auto sym_clone = prechecked_cast<Symbol*>(e);
	assert(sym_clone);
	sym_clone->set_owner(_sym->owner());

	unsigned old_show = atoi(sym_clone->paramValue("$param_display").c_str());
	trace1("old_show", old_show);

	if(sym_clone->showLabel() != showName->isChecked()) {
		incomplete();
//		sym->showLabel() = showName->isChecked();
		changed = true;
	}else{
	}

	QString tmp;
	if(CompNameEdit->text().isEmpty()){
		CompNameEdit->setText(QString::fromStdString(_sym->label()));
	}else if(CompNameEdit->text().toStdString() != _sym->label()) {
		trace1("change_label", CompNameEdit->text().toStdString());
		sym_clone->set_label(CompNameEdit->text().toStdString());
		changed = true;
	}

	unsigned new_show = 0;

	for( int row = 0; row < prop->rowCount(); row++ ) {
		QString name  = prop->item(row, 0)->text();
		auto value = prop->item(row, 1)->text().toStdString();
		QString disp = prop->item(row, 2)->text();
		
		bool display = (disp == tr("yes"));
		new_show *= 2;
		new_show |= display;

		if(_sym->paramValue(row) != value) {
			trace2("symbol apply", row, value);
			sym_clone->setParameter(row, value);
			changed = true;
		}else{
		}
	}

	if(old_show != new_show){
		incomplete();
	}else{
	}

	// changed == !(element(gfx) == element(clonegfx));

	// TODO: do this properly, and move to base class.
	// what is an undoable operation? does it require apply button?
	if(changed) {
		trace2("symbol changed", element(_gfx)->label(), element(_gfxclone)->label());
		auto scene = _gfx->scene();
		assert(scene);
		scene->addItem(_gfxclone);

		auto cmd = new SwapElementGfx(_gfx, _gfxclone);
		execute(cmd);

		Element* c = element(_gfx);
		_sym = prechecked_cast<Symbol*>(c); // was const.
		assert(_sym);

//		if ( (int) _sym->paramCount() != prop->rowCount()) {
//			updateCompPropsList();
//		}else{
//		}

		auto cl = _gfxclone;
		_gfxclone = nullptr;
		_gfx = nullptr;
		attach(cl); // rebuilds it all. may be a bug.
	}else{
	}
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotParamEntered()
{
	if(editValues->isEnabled()){ untested();
		editValues->setFocus();
	}else{ untested();
		editStart->setFocus();
	}
}
/*--------------------------------------------------------------------------*/
//void SymbolDialog::slotSimEntered(int)
//{
//	unreachable();
//	editParam->setFocus();
//}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotValuesEntered()
{
	slotButtOK();
}
/*--------------------------------------------------------------------------*/
// void SymbolDialog::slotStartEntered()
// {
// 	editStop->setFocus();
// }
/*--------------------------------------------------------------------------*/
//void SymbolDialog::slotStopEntered()
//{
//	editStep->setFocus();
//}
/*--------------------------------------------------------------------------*/
//void SymbolDialog::slotStepEntered()
//{
//	editNumber->setFocus();
//}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotNumberEntered()
{
	slotButtOK();
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::disableButtons()
{
	ButtUp->setEnabled(false);
	ButtDown->setEnabled(false);
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::enableButtons()
{
	ButtUp->setEnabled(true);
	ButtDown->setEnabled(true);
}
/*--------------------------------------------------------------------------*/
class DialogFactory : public Widget /*?*/ {
	Widget* clone() const override{
		SchematicDialog* wd = new SymbolDialog();
		return wd;
	}
}F;
static Dispatcher<Widget>::INSTALL p1(&widget_dispatcher, "SymbolDialog", &F);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}

#endif
