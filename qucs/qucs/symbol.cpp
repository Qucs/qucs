/***************************************************************************
    copyright            : (C) 2016-2020 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "symbol.h"
#include "schematic_model.h"
#include "schematic_dialog.h"
#include <assert.h>
#include "net.h"
#include "geometry.h"
/*--------------------------------------------------------------------------*/
#include <QVBoxLayout>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
/*--------------------------------------------------------------------------*/
class SymbolDialog : public SchematicDialog {
public:
  SymbolDialog(QucsDoc*);
  ~SymbolDialog();

  void attach(ElementGraphics* c) override;

private: // slot overrides.
  void slotButtOK();
  void slotButtCancel();
  void slotSelectProperty(QTableWidgetItem *item);
  void slotApplyInput();
  void slotApplyState(int State);
  void slotApplyProperty();
  void slotApplyPropName();

  void slotParamEntered();
  void slotSimEntered(int);
  void slotValuesEntered();
  void slotStartEntered();
  void slotStopEntered();
  void slotStepEntered();
  void slotNumberEntered();

  void slotApplyChange(const QString& Text);

public:
  void enableButtons();
  void disableButtons();

protected slots:
    void reject();
    bool eventFilter(QObject *obj, QEvent *event);

private:
  QVBoxLayout *_all;
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
  ElementGraphics* _gfx;
  Symbol const* _sym; // needed?
};
/*--------------------------------------------------------------------------*/
// recreate schematic symbol. not sure why, maybe after parameter changes
// (why not just call "Symbol::create??!")
void Symbol::recreate(){ // }SchematicModel const&){ untested();
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol()
    : Element(),
		_subckt(nullptr),
		_vflip(1),
		_hflip(1),
		_angle(0),
		_param_display(0),
		_common(nullptr)
{
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol(Symbol const& s)
    : Element(s),
		_subckt(nullptr),
		_vflip(s._vflip),
		_hflip(s._hflip),
		_angle(s._angle),
		_param_display(s._param_display),
		_common(nullptr)
{
  attach_common(s._common);
  setTypeName(s.typeName());
}
/*--------------------------------------------------------------------------*/
// dup
SchematicModel* Symbol::scope()
{
	return Element::scope();

	if(auto o=dynamic_cast<Symbol*>(owner())){ untested();
		return o->subckt();
	}else if(auto o=dynamic_cast<QucsDoc*>(owner())){ untested();
		assert(subckt());
		return subckt(); // yikes
	}else{untested();
		trace1("owner not a symbol?", label());
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
// reuse overrides to give both const and non-const access.
// (move to header)
SchematicModel const* Symbol::scope() const
{ untested();
	auto s=const_cast<Symbol*>(this);
	return s->scope();
}
/*--------------------------------------------------------------------------*/
#if 0
// obsolete? portValue?
QString const& Symbol::netLabel(unsigned i) const
{ untested();
//	assert(hasPort(i));
	return portValue(i);
}
#endif
/*--------------------------------------------------------------------------*/
// connect to a node. (connectPort?)
// is this a kind of mapNode?
Node* Symbol::connectNode(unsigned i, NodeMap&nm)
{
	trace2("connectNode", label(), i);
	pos_t p = nodePosition(i);
	Node* n = &nm.at(p);
	assert(n->hasNet());

	trace0("connectNode set port");

	Port& mp = port(i); // hmm setPort(...) instead?
	mp.connect(n /*,this*/);

	return n;
}
/*--------------------------------------------------------------------------*/
// disconnect a node. (disconnectPort?)
// (does not use the map, but could)
Node* Symbol::disconnectNode(unsigned i, NodeMap&)
{
	trace2("disconnectNode", label(), i);
	Port& mp = port(i);
	Node* n = mp.value();
	mp.disconnect(n /*, this*/);

	return n;
}
/*--------------------------------------------------------------------------*/
Node const* Symbol::portNode(unsigned i) const
{ untested();
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){ untested();
	  assert( port(i).value() );
	  return port(i).value();
  }else{ untested();
	  return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
Node const* Symbol::portValue(unsigned i) const
{
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){
	  assert( port(i).value() );
	  return port(i).value();
  }else{
	  return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
// same as Element::center?
pos_t Symbol::center()const
{
	return Element::center(); // pos_t(_cx, _cy);
}
/*--------------------------------------------------------------------------*/
Port const& Symbol::port(unsigned i) const
{
	Symbol* s=const_cast<Symbol*>(this);
	return s->port(i);
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramValue(std::string const& n) const
{
	if(n=="$xposition"){
		return std::to_string(cx());
	}else if(n=="$yposition"){
		trace1("..", cy());
		return std::to_string(cy());
	}else if(n=="$mfactor"){
		return "1"; // isActive in Component.
	}else if(n=="$param_display"){
		return std::to_string(_param_display);
	}else if(n=="$hflip"){
		return std::to_string(_hflip);
	}else if(n=="$vflip"){
		return std::to_string(_vflip);
	}else if(n=="$angle"){
		return std::to_string(_angle);
	}else{ untested();
		throw ExceptionCantFind(label(), n, "params");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::paint(ViewPainter* p) const
{itested();

	 // does not make a lot of sense right now
	for(unsigned i=0; i<numPorts(); ++i){ itested();
//		auto pp = common()->portPosition(i);
		auto pp = portPosition(i);
		int x = getX(pp);
		int y = getY(pp);

		if(!port(i).isConnected()){ untested();
			unreachable();
			p->setPen(QPen(Qt::green,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else if(port(i)->numPorts()==0){ untested();
			unreachable();
		}else if(port(i)->numPorts()==1){itested();
			// p->setPen(QPen(Qt::red,2));
			p->setPen(QPen(Qt::red,1));
			p->drawEllipse(x-2, y-2, 4, 4);
		}else if(port(i)->numPorts()>2){itested();
			p->setPen(QPen(Qt::black,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else{itested();
		}
	}
}
/*--------------------------------------------------------------------------*/
// global/external position
pos_t Symbol::nodePosition(unsigned i) const
{
	pos_t pp = portPosition(i);

	if(legacyTransformHack()){
		// ports use external coordinates...
	}else{
		rotate_after_mirror a(_angle, (1-_hflip)/2, (1-_vflip)/2);
		trace4("nodePosition pre", pp, _angle, _hflip, _vflip);
		pp = a.apply(pp);
		trace2("nodePosition post", pp, center());
	}
	return pp + center();
}
/*--------------------------------------------------------------------------*/
// BUG: not here. legacy stuff...
void Symbol::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel(nullptr);
}
/*--------------------------------------------------------------------------*/
Symbol::~Symbol()
{
	delete _subckt;
	_subckt = nullptr;
}
/*--------------------------------------------------------------------------*/
bool Symbol::paramIsPrintable() const
{
	return true;
}
/*--------------------------------------------------------------------------*/
unsigned Symbol::paramCount() const
{
	return 2; // TODO:  vflip hflip angle mfactor ...?
	return 6;
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramValue(unsigned i) const
{
	switch(i){
	case 0:
		return std::to_string(cx());
	case 1:
		return std::to_string(cy());
	default:
		throw ExceptionCantFind(label(), std::to_string(i), "param values");
	}
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramName(unsigned i) const
{
	trace2("Symbol::paramName", label(), i);
	switch(i){
	case 0:
		return "$xposition";
	case 1:
		return "$yposition";
	default:
		throw ExceptionCantFind(label(), std::to_string(i), "param names");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(unsigned n, std::string const& v)
{
	if(n==0){ // xpos
		setPosition(pos_t(atoi(v.c_str()), cy()));
	}else if(n==1){ // ypos
		setPosition(pos_t(cx(), atoi(v.c_str())));
	}else if(n<Symbol::paramCount()){
		unreachable();
	}else{
		throw ExceptionCantFind( label(), std::to_string(n), "params");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(std::string const& name, std::string const& v)
{
	if(name == "$xposition"){
		setPosition(pos_t(atoi(v.c_str()), cy()));
	}else if(name == "$yposition"){
		setPosition(pos_t(cx(), atoi(v.c_str())));
	}else if(name == "$angle"){
		trace1("angle", v);
		_angle = atoi(v.c_str());
	}else if(name == "$param_display"){
		_param_display = atoi(v.c_str());
	}else if(name == "$hflip"){
		_hflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else if(name == "$vflip"){
		_vflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else{
		throw ExceptionCantFind( label(), name, "params");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(QString const& name, QString const& b)
{
	std::string n = name.toStdString();
	std::string v = b.toStdString();
	setParameter(n, v);
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(unsigned pos, QString const& b)
{
	auto v = b.toStdString();
	setParameter(pos, v);
}
/*--------------------------------------------------------------------------*/
QDialog* Symbol::schematicWidget(QucsDoc* Doc) const
{ untested();
  return new SymbolDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(int c)
  :Object(),
//   _tnom_c(NOT_INPUT),
//   _dtemp(0),
//   _temp_c(NOT_INPUT),
//   _mfactor(1),
//   _value(0),
//   _modelname(),
//   _model(0),
   _attach_count(c)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common,destruct", _attach_count);
  assert(_attach_count == 0 || _attach_count == CC_STATIC);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::attach_common(COMMON_COMPONENT*c, COMMON_COMPONENT**to)
{
  assert(to);
  if (c == *to) {
    // The new and old are the same object.  Do nothing.
  }else if (!c) {untested();
    // There is no new common.  probably a simple element
    detach_common(to);
  }else if (!*to) {
    // No old one, but have a new one.
    ++(c->_attach_count);
    trace1("++1", c->_attach_count);
    *to = c;
  }else if (*c != **to) {
    // They are different, usually by edit.
    detach_common(to);
    ++(c->_attach_count);
    trace1("++2", c->_attach_count);
    *to = c;
  }else if (c->_attach_count == 0) {
    // The new and old are identical.
    // Use the old one.
    // The new one is not used anywhere, so throw it away.
    trace1("delete", c->_attach_count);    
    delete c;
  }else{untested();
    // The new and old are identical.
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::detach_common(COMMON_COMPONENT** from)
{
  assert(from);
  if (*from) {
    assert((**from)._attach_count > 0);
    --((**from)._attach_count);
    trace1("--", (**from)._attach_count);
    if ((**from)._attach_count == 0) {
      trace1("delete", (**from)._attach_count);
      delete *from;
    }else{
      trace1("nodelete", (**from)._attach_count);
    }
    *from = NULL;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::operator==(const COMMON_COMPONENT& x)const
{
  return true;
//  (_modelname == x._modelname
//	  && _model == x._model
//	  && _tnom_c == x._tnom_c
//	  && _dtemp == x._dtemp
//	  && _temp_c == x._temp_c
//	  && _mfactor == x._mfactor
//	  && _value == x._value);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// symbolwidget.cpp?
#include "qucs.h"
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
SymbolDialog::SymbolDialog(QucsDoc* d) : SchematicDialog(d)
{
	resize(450, 250);
	setWindowTitle(tr("Edit Component Properties"));
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::attach(ElementGraphics* gfx)
{
	assert(gfx);
	Element const* c = &**gfx;
	if(_sym){
		incomplete();
	}else{
	}
	_sym = prechecked_cast<Symbol const*>(c);
	assert(_sym);
	_gfx = gfx;
	QString s;
	setAllVisible = true; // state when toggling properties visibility

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
	gp1->addWidget(new QLabel(_sym->description()), 0,0,1,2);

	QHBoxLayout *h5 = new QHBoxLayout;
	h5->setSpacing(5);

	h5->addWidget(new QLabel(tr("Name:")) );

	CompNameEdit = new QLineEdit;
	h5->addWidget(CompNameEdit);

	// CompNameEdit->setValidator(ValRestrict);
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
		QFontMetrics metrics(QucsSettings.font, 0);   // get size of text
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
	auto e = _sym->clone();
	auto new_sym=prechecked_cast<Symbol*>(e);
	assert(new_sym);
	new_sym->setOwner( _sym->mutable_owner() );

	unsigned old_show = atoi(new_sym->paramValue("$param_display").c_str());
	trace1("old_show", old_show);

	if(new_sym->showLabel() != showName->isChecked()) {
		incomplete();
//		sym->showLabel() = showName->isChecked();
		changed = true;
	}else{
	}

	QString tmp;
	if(CompNameEdit->text().isEmpty()){
		CompNameEdit->setText(QString::fromStdString(_sym->label()));
	}else if(CompNameEdit->text().toStdString() != _sym->label()) {
		new_sym->setLabel(CompNameEdit->text().toStdString());
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
			new_sym->setParameter(row, value);
			changed = true;
		}else{
		}
	}

	if(old_show != new_show){
		incomplete();
	}else{
	}

	if(changed) {
		auto cmd = new SwapSymbolCommand(_gfx, new_sym);
		execute(cmd);

		Element const* c = &**_gfx;
		_sym = prechecked_cast<Symbol const*>(c);
		assert(_sym);

		if ( (int) _sym->paramCount() != prop->rowCount()) {
			updateCompPropsList();
		}
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
void SymbolDialog::slotSimEntered(int)
{
	unreachable();
	editParam->setFocus();
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotValuesEntered()
{
	slotButtOK();
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotStartEntered()
{
	editStop->setFocus();
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotStopEntered()
{
	editStep->setFocus();
}
/*--------------------------------------------------------------------------*/
void SymbolDialog::slotStepEntered()
{
	editNumber->setFocus();
}
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
/*--------------------------------------------------------------------------*/
