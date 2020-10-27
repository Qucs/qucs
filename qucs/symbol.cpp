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
  void slotBrowseFile();
  void slotEditFile();
  void slotApplyChange(const QString& Text);
  void slotApplyProperty();
  void slotApplyPropName();

  void slotButtAdd();
  void slotButtRem();

  void slotButtUp();
  void slotButtDown();

  void slotSimTypeChange(int);
  void slotNumberChanged(const QString&);
  void slotStepChanged(const QString&);

  void slotParamEntered();
  void slotSimEntered(int);
  void slotValuesEntered();
  void slotStartEntered();
  void slotStopEntered();
  void slotStepEntered();
  void slotNumberEntered();
  void slotHHeaderClicked(int headerIdx);

public:
  void enableButtons();
  void disableButtons();

protected slots:
    void reject();
    bool eventFilter(QObject *obj, QEvent *event);

private:
  QVBoxLayout *_all;
  // QValidator  *Validator, *ValRestrict, *Validator2;
  // QRegExp     Expr;
  // QIntValidator *ValInteger;
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
  Component const* _comp;
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
		_angle(0)
{
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol(Symbol const& s)
    : Element(s),
		_subckt(nullptr),
		_vflip(s._vflip),
		_hflip(s._hflip),
		_angle(s._angle)
{
  setTypeName(s.typeName());
}
/*--------------------------------------------------------------------------*/
// dup
SchematicModel* Symbol::scope()
{
	if(auto o=dynamic_cast<Symbol*>(owner())){
		return o->subckt();
	}else{itested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
// reuse overrides to give both const and non-const access.
// (move to header)
SchematicModel const* Symbol::scope() const
{
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
{itested();
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
		return "0";
	}else if(n=="$hflip"){
		return std::to_string(_hflip);
	}else if(n=="$vflip"){
		return std::to_string(_vflip);
	}else if(n=="$angle"){
		return std::to_string(_angle);
	}else{ untested();
		throw ExceptionCantFind(label().toStdString(), n, "params");
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
		throw ExceptionCantFind(label().toStdString(), std::to_string(i), "param values");
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
		throw ExceptionCantFind(label().toStdString(), std::to_string(i), "param names");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(unsigned n, std::string const& v)
{
	if(n<Symbol::paramCount()){
		incomplete();
	}else{
		throw ExceptionCantFind( label().toStdString(), std::to_string(n), "params");
	}
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(std::string const& name, std::string const& v)
{
	if(name == "$xposition"){
		setCenter(pos_t(atoi(v.c_str()), cy()));
	}else if(name == "$yposition"){
		setCenter(pos_t(cx(), atoi(v.c_str())));
	}else if(name == "$angle"){
		trace1("angle", v);
		_angle = atoi(v.c_str());
	}else if(name == "$hflip"){
		_hflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else if(name == "$vflip"){
		_vflip = atoi(v.c_str());
		assert(_hflip==1 || _hflip==-1);
	}else{
		throw ExceptionCantFind( label().toStdString(), name, "params");
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
  return nullptr; // new ComponentDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
