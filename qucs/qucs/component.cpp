/***************************************************************************
                               component.cpp
                              ---------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdlib.h>
#include <cmath>

#include "components/component.h"
#include "node.h"
#include "qucs.h"
#include "schematic_doc.h"
#include "viewpainter.h"
#include "module.h"
#include "misc.h"
#include "io_trace.h"
#include "globals.h"

#include <QPen>
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>

#include <assert.h>

#include "some_font_stuff.h"
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


class ComponentDialog : public SchematicDialog {
public:
  ComponentDialog(QucsDoc*);
  ~ComponentDialog();

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
  QValidator  *Validator, *ValRestrict, *Validator2;
  QRegExp     Expr;
  QIntValidator *ValInteger;
  QTableWidget  *prop;
  QLineEdit   *edit, *NameEdit, *CompNameEdit;
  QComboBox   *ComboEdit;
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
  QComboBox *comboSim, *comboType;

  void updateCompPropsList(void);

private:
  ElementGraphics* _gfx;
  Component const* _comp;
};

const int Component::num_component_params = 2; // tx and ty.

Component::Component(Component const& p)
  : Symbol(p),
    mirroredX(p.mirroredX),
    _rotated(p._rotated),
    isActive(p.isActive),
    //cx(p.cx),
    //cy(p.cy),
    tx(p.tx),
    ty(p.ty),
    x1(p.x1),
    y1(p.y1),
    x2(p.x2),
    y2(p.y2),
    showName(p.showName)
{
  trace3("Component::Component", p.Name, p.Model, _rotated);
  trace2("Component::Component", typeName(), p.typeName());

  setTypeName(p.Model.toStdString()); // BUG

  assert(!Props.count());
  for(auto i : p.Props){
    Props.append(new Property(*i));
  }

  for(auto i : p.Ports){
    Ports.append(new ComponentPort(*i));
  }

  assert(typeName() == p.typeName());

  { // copyGraphics(p)
    // graphics must be shared (and transformed upon use).
    // but leave it like that in legacy Components.
    //
    // perhaps old Qt did not have transforms, and gave rise to the
    // obsolete mirror and rotate code.
    Arcs.clear();
    for(auto p1 : p.Arcs) {
	    Arcs.append(new Arc(*p1));
    }
    Lines.clear();
    for(auto p1 : p.Lines) {
	    Lines.append(new Line(*p1));
    }
  }
}

/*!
 * \class Component
 * \brief The Component class implements a legacy qucs component symbol
 */
Component::Component() : Symbol(),  _rotated(0)
{
  Type = isAnalogComponent;

  mirroredX = false;
  isActive = COMP_IS_ACTIVE;
  showName = true;

  tx = 0;
  ty = 0;

  Props.setAutoDelete(true);

  Model=""; //remove later.

  setLabel(typeName());
}

Element* Component::clone() const
{
  Component const* e=this;
  Component* E=const_cast<Component*>(e);
  Component* ret = E->newOne();
  return ret;
}

// -------------------------------------------------------
void Component::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{ untested();
  _x1 = x1; // +cx();
  _y1 = y1; // +cy();
  _x2 = x2; // +cx();
  _y2 = y2; // +cy();
}

// -------------------------------------------------------
// Size of component text.
int Component::textSize(int& _dx, int& _dy)
{itested();
  // get size of text using the screen-compatible metric
  FontMetrics metrics;

  int tmp, count=0;
  _dx = _dy = 0;
  if(showName) {itested();
    _dx = metrics.width(Name);
    _dy = metrics.height();
    count++;
  }
  for(Property *pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display) {itested();
      // get width of text
      tmp = metrics.width(pp->Name+"="+pp->Value);
      if(tmp > _dx)  _dx = tmp;
      _dy += metrics.height();
      count++;
    }
  return count;
}

// -------------------------------------------------------
// Boundings including the component text.
void Component::entireBounds(int& _x1, int& _y1, int& _x2, int& _y2, float Corr)
{itested();
  _x1 = x1+cx();
  _y1 = y1+cy();
  _x2 = x2+cx();
  _y2 = y2+cy();

  // text boundings
  if(tx < x1) _x1 = tx+cx();
  if(ty < y1) _y1 = ty+cy();

  int dx, dy, ny;
  ny = textSize(dx, dy);
  dy = int(float(ny) / Corr);  // correction for unproportional font scaling

  if((tx+dx) > x2) _x2 = tx+dx+cx();
  if((ty+dy) > y2) _y2 = ty+dy+cy();
}

// -------------------------------------------------------
#if 1 // REMOVE
void Component::getCenter(int& x, int& y) const
{itested();
  x = cx();
  y = cy();
}
#endif

// -------------------------------------------------------
int Component::getTextSelected(int x_, int y_, float Corr)
{ untested();
  x_ -= cx();
  y_ -= cy();
  if(x_ < tx) return -1;
  if(y_ < ty) return -1;

  x_ -= tx;
  y_ -= ty;
  int w, dy = int(float(y_) * Corr);  // correction for font scaling
  // use the screen-compatible metric
  FontMetrics  metrics;
  if(showName) { untested();
    w  = metrics.width(Name);
    if(dy < 1) { untested();
      if(x_ < w) return 0;
      return -1;
    }
    dy--;
  }

  Property *pp;
  for(pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display)
      if((dy--) < 1) break;
  if(!pp) return -1;

  // get width of text
  w = metrics.width(pp->Name+"="+pp->Value);
  if(x_ > w) return -1; // clicked past the property text end - selection invalid
  return Props.at()+1;  // number the property
}

// -------------------------------------------------------
bool Component::getSelected(int x_, int y_)
{ untested();
  unreachable(); // obsolete
  x_ -= cx();
  y_ -= cy();
  if(x_ >= x1) if(x_ <= x2) if(y_ >= y1) if(y_ <= y2)
    return true;

  return false;
}

// -------------------------------------------------------
void Component::paint(ViewPainter *p) const
{itested();
  int x, y;
  int cx = 0; // cx_(); // "positionX"
  int cy = 0; // cy_(); // "positionY"
//  int a, b, xb, yb;
  Element::paint(p);
  QFont f = p->font();   // save current font
  QFont newFont = f;
  if(dynamic_cast<TaskElement const*>(this)) { untested();
    unreachable();
  }else{itested();
    // normal components go here
    assert(!Model.size() || Model.at(0) != '.');

    // paint all lines
    foreach(Line *p1, Lines) {itested();
      p->setPen(p1->style);
      p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);
    }

    // paint all arcs
    foreach(Arc *p3, Arcs) {itested();
      p->setPen(p3->style);
      p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
    }

    // paint all rectangles
    foreach(Area *pa, Rects) { untested();
      p->setPen(pa->Pen);
      p->setBrush(pa->Brush);
      p->drawRect(cx+pa->x, cy+pa->y, pa->w, pa->h);
    }

    // paint all ellipses
    foreach(Area *pa, Ellips) { untested();
      p->setPen(pa->Pen);
      p->setBrush(pa->Brush);
      p->drawEllipse(cx+pa->x, cy+pa->y, pa->w, pa->h);
    }
    p->setBrush(Qt::NoBrush);

    newFont.setWeight(QFont::Light);

    // keep track of painter state
    p->save();

#if 0
    auto wm = p->worldMatrix();
    // write all text
    foreach(Text *pt, Texts) {itested();
      p->setWorldMatrix(
          QMatrix(pt->mCos, -pt->mSin, pt->mSin, pt->mCos,
                   p->DX + float(cx+pt->x) * p->Scale,
                   p->DY + float(cy+pt->y) * p->Scale));
      newFont.setPointSizeF(p->Scale * pt->Size);
      newFont.setOverline(pt->over);
      newFont.setUnderline(pt->under);
      p->setFont(newFont);
      p->setPen(pt->Color);
      if (0) { untested();
	p->drawText(0, 0, 0, 0, Qt::AlignLeft|Qt::TextDontClip, pt->s);
      } else {itested();
	int w, h;
	w = p->drawTextMapped (pt->s, 0, 0, &h);
    Q_UNUSED(w);
      }
    }
    p->setWorldMatrix(wm);
    p->setWorldMatrixEnabled(false);
#endif

    // restore painter state
    p->restore();
  }

  // restore old font
  p->setFont(f);

  p->setPen(QPen(Qt::black,1));
  p->map(cx+tx, cy+ty, x, y);
///  if(showName) {itested();
///    p->drawText(x, y, 0, 0, Qt::TextDontClip, Name);
///    y += p->LineSpacing;
///  }
  // write all properties
#if 0
  for(Property *p4 = Props.first(); p4 != 0; p4 = Props.next()){
    if(p4->display) {itested();
      p->drawText(x, y, 0, 0, Qt::TextDontClip, p4->Name+"="+p4->Value);
      y += p->LineSpacing;
    }
  }
#endif

  // not here.
  if(isActive == COMP_IS_OPEN){
    p->setPen(QPen(Qt::red,0));
  }else if(isActive & COMP_IS_SHORTEN){
    p->setPen(QPen(Qt::darkGreen,0));
  }else{
  }
  if(isActive != COMP_IS_ACTIVE) { untested();
    p->drawRect(cx+x1, cy+y1, x2-x1+1, y2-y1+1);
    p->drawLine(cx+x1, cy+y1, cx+x2, cy+y2);
    p->drawLine(cx+x1, cy+y2, cx+x2, cy+y1);
  }else{
  }

  Symbol::paint(p);
}
// -------------------------------------------------------
// For output on a printer device.
void Component::print(ViewPainter *p, float FontScale)
{ untested();
  foreach(Text *pt, Texts)
    pt->Size *= FontScale;

  paint(p);

 foreach(Text *pt, Texts)
    pt->Size /= FontScale;
}

// -------------------------------------------------------
// Rotates the component 90 counter-clockwise around its center
void Component::rotate()
{
  // Port count only available after recreate, createSymbol
  if ((Model != "Sub") && (Model !="VHDL") && (Model != "Verilog")) // skip port count
    if(Ports.count() < 1) return;  // do not rotate components without ports
  int tmp, dx, dy;

  // rotate all lines
  foreach(Line *p1, Lines) {
    tmp = -p1->x1;
    p1->x1 = p1->y1;
    p1->y1 = tmp;
    tmp = -p1->x2;
    p1->x2 = p1->y2;
    p1->y2 = tmp;
  }

  // rotate all ports
  for(ComponentPort* p2 : Ports) {
    // p2->rotate();
    tmp = -p2->x_();
    p2->setPosition(p2->y_(), tmp);
  }

  // rotate all arcs
  foreach(Arc *p3, Arcs) {
    tmp = -p3->x;
    p3->x = p3->y;
    p3->y = tmp - p3->w;
    tmp = p3->w;
    p3->w = p3->h;
    p3->h = tmp;
    p3->angle += 16*90;
    if(p3->angle >= 16*360) p3->angle -= 16*360;;
  }

  // rotate all rectangles
  foreach(Area *pa, Rects) { untested();
    tmp = -pa->x;
    pa->x = pa->y;
    pa->y = tmp - pa->w;
    tmp = pa->w;
    pa->w = pa->h;
    pa->h = tmp;
  }

  // rotate all ellipses
  foreach(Area *pa, Ellips) { untested();
    tmp = -pa->x;
    pa->x = pa->y;
    pa->y = tmp - pa->w;
    tmp = pa->w;
    pa->w = pa->h;
    pa->h = tmp;
  }

  // rotate all text
  float ftmp;
  foreach(Text *pt, Texts) {itested();
    tmp = -pt->x;
    pt->x = pt->y;
    pt->y = tmp;

    ftmp = -pt->mSin;
    pt->mSin = pt->mCos;
    pt->mCos = ftmp;
  }

  tmp = -x1;   // rotate boundings
  x1  = y1; y1 = -x2;
  x2  = y2; y2 = tmp;

  tmp = -tx;    // rotate text position
  tx  = ty;
  ty  = tmp;
  // use the screen-compatible metric
  FontMetrics metrics;

  dx = dy = 0;
  if(showName) {
    dx = metrics.width(Name);
    dy = metrics.lineSpacing();
  }
  for(Property *pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display) {
      // get width of text
      tmp = metrics.width(pp->Name+"="+pp->Value);
      if(tmp > dx) dx = tmp;
      dy += metrics.lineSpacing();
    }
  if(tx > x2) ty = y1-ty+y2;    // rotate text position
  else if(ty < y1) ty -= dy;
  else if(tx < x1) { tx += dy-dx;  ty = y1-ty+y2; }
  else ty -= dx;
}
// -------------------------------------------------------

void Component::setParameter(unsigned pos, std::string const& v)
{
  int p = int(pos) - int(Symbol::paramCount()) - num_component_params ;

  trace2("Component::setParameter", pos, v);
  if(pos==Symbol::paramCount()){
    // tx
  }else if(pos==Symbol::paramCount()+1){
    // ty
  }else if(pos<Symbol::paramCount()){ untested();
    Symbol::setParameter(pos, v);
  }else if (p<int(Props.size())){
    assert(Props.at(p));
    //Props.at(p)->setValue(v);
    Props.at(p)->Value = QString::fromStdString(v);
  }else{ untested();
    incomplete();
  }
}

// -------------------------------------------------------
void Component::setParameter(std::string const& name, std::string const& v)
{itested();
  if(name=="$angle"){itested();
    int r = atoi(v.c_str());
    trace3("Component::setParameter", name, v, label());
    assert(!(r % 90)); // for now.
    r /= 90;
    r %= 4;
    set_rotated(r);
  }else if(name=="$vflip"){itested();
    int r = atoi(v.c_str());
    assert(r==1 || r==-1);
    r -= 1;
    r /= -2;
    set_mirror_yaxis(r);
  }else if(name=="$hflip"){itested();
    int r = atoi(v.c_str());
    assert(r==1 || r==-1);
    r -= 1;
    r /= -2;
    set_mirror_xaxis(r);
  }else if(name=="$xposition"){
    int x = atoi(v.c_str());
    auto c = center();
    c.first = x;
    Element::setCenter(c);
  }else if(name=="$yposition"){
    int y = atoi(v.c_str());
    auto c = center();
    c.second = y;
    Element::setCenter(c);
  }else{ untested();
    Symbol::setParameter(name, v);
  }
}


// -------------------------------------------------------
template<class P>
static int paramDisplay(P const& p, int offset)
{
  int ret = 0;
  int s = 1 << offset;

  for(auto i : p){
    if(!i){
    }else if(i->display){
      trace2("display", i->Name, i->Value);
      ret += s;
    }else{
    }
    s *= 2;
  }
  trace1("display", ret);
  return ret;
}

// -------------------------------------------------------
std::string Component::paramValue(std::string const& name) const
{itested();
  if(name=="$angle"){itested();
    trace1("Component::paramValue", _rotated);
    return std::to_string(_rotated*90);
  }else if(name=="$tx"){
    return std::to_string(tx);
  }else if(name=="$ty"){
    return std::to_string(ty);
  }else if(name=="$xposition"){
    return std::to_string(center().first);
  }else if(name=="$yposition"){
    return std::to_string(center().second);
  }else if(name=="$vflip"){itested();
    // 0 |-> 1
    // 1 |-> -1
    int m = 1 - mirroredX * 2;
    return std::to_string(m);
  }else if(name=="$hflip"){itested();
    return "1";
  }else if(name=="$param_display"){
    return std::to_string(paramDisplay(Props, 2 + Symbol::paramCount()));
  }else if(name=="$mfactor"){
    return isActive?"1":"0";
  }else{ untested();
    return Symbol::paramValue(name);
  }
}

// -------------------------------------------------------
void Component::set_rotated(unsigned r)
{
  assert(r<4);
  while(r != unsigned(_rotated)){
    rotate();

    // keep track of what's done
    ++_rotated;
    _rotated %= 4;
  }
  trace3("Component::set_rotated", label(), r, _rotated);
}

// -------------------------------------------------------
void Component::set_mirror_yaxis(unsigned x)
{
  auto rot_bak = rotated();
  if(x != mirroredX){
    mirrorX(); // sic.
  }else{
    assert(x==0);
  }

  set_rotated(rot_bak);
}
// -------------------------------------------------------
void Component::set_mirror_xaxis(unsigned x)
{
  auto rot_bak = rotated();
  if(x != mirroredX){
    mirrorY(); // six.
  }else{
    assert(x==0);
  }

  set_rotated(rot_bak);
}
// -------------------------------------------------------
// Mirrors the component about the x-axis.
// BUG? mirrors the Y axis.
void Component::mirrorX()
{
  // Port count only available after recreate, createSymbol
  if ((Model != "Sub") && (Model !="VHDL") && (Model != "Verilog")) // skip port count
    if(Ports.count() < 1) return;  // do not rotate components without ports

  // mirror all lines
  foreach(Line *p1, Lines) {
    p1->y1 = -p1->y1;
    p1->y2 = -p1->y2;
  }

  // mirror all ports
  foreach(ComponentPort *p2, Ports){
    p2->setPosition(p2->x_(), -p2->y_());
  }

  // mirror all arcs
  foreach(Arc *p3, Arcs) {
    p3->y = -p3->y - p3->h;
    if(p3->angle > 16*180) p3->angle -= 16*360;
    p3->angle  = -p3->angle;    // mirror
    p3->angle -= p3->arclen;    // go back to end of arc
    if(p3->angle < 0) p3->angle += 16*360;  // angle has to be > 0
  }

  // mirror all rectangles
  foreach(Area *pa, Rects)
    pa->y = -pa->y - pa->h;

  // mirror all ellipses
  foreach(Area *pa, Ellips)
    pa->y = -pa->y - pa->h;

//  QFont f = QucsSettings.font;
  // mirror all text
  foreach(Text *pt, Texts) {itested();
//    f.setPointSizeF(pt->Size);
    // use the screen-compatible metric
    FontMetrics smallMetrics;
    smallMetrics.setSmall();
    QSize s = smallMetrics.size(0, pt->s);   // use size for more lines
    pt->y = -pt->y - int(pt->mCos)*s.height() + int(pt->mSin)*s.width();
  }

  int tmp = y1;
  y1  = -y2; y2 = -tmp;   // mirror boundings
  // use the screen-compatible metric
  FontMetrics metrics;
  int dy = 0;
  if(showName)
    dy = metrics.lineSpacing();   // for "Name"
  for(Property *pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display)  dy += metrics.lineSpacing();
  if((tx > x1) && (tx < x2)) ty = -ty-dy;     // mirror text position
  else ty = y1+ty+y2;

  mirroredX = !mirroredX;    // keep track of what's done

  // 0 |-> 0
  // 1 |-> 3
  // 2 |-> 2
  // 3 |-> 1

  _rotated += _rotated << 1;
  _rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component x axis
void Component::mirrorY()
{
  // Port count only available after recreate, createSymbol
  // BUG.
  if ((Model != "Sub") && (Model !="VHDL") && (Model != "Verilog")) // skip port count
    if(Ports.count() < 1) return;  // do not rotate components without ports

  // mirror all lines
  foreach(Line *p1, Lines) {
    p1->x1 = -p1->x1;
    p1->x2 = -p1->x2;
  }

  // mirror all ports
  foreach(ComponentPort *p2, Ports){
    p2->setPosition(-p2->x_(), p2->y_());
  }

  // mirror all arcs
  foreach(Arc *p3, Arcs) {
    p3->x = -p3->x - p3->w;
    p3->angle = 16*180 - p3->angle - p3->arclen;  // mirror
    if(p3->angle < 0) p3->angle += 16*360;   // angle has to be > 0
  }

  // mirror all rectangles
  foreach(Area *pa, Rects)
    pa->x = -pa->x - pa->w;

  // mirror all ellipses
  foreach(Area *pa, Ellips)
    pa->x = -pa->x - pa->w;

  int tmp;
  QFont f = QucsSettings.font;
  // mirror all text
  foreach(Text *pt, Texts) {itested();
    f.setPointSizeF(pt->Size);
    // use the screen-compatible metric
    FontMetrics smallMetrics;
    smallMetrics.setSmall();

    QSize s = smallMetrics.size(0, pt->s);   // use size for more lines
    pt->x = -pt->x - int(pt->mSin)*s.height() - int(pt->mCos)*s.width();
  }

  tmp = x1;
  x1  = -x2; x2 = -tmp;   // mirror boundings
  // use the screen-compatible metric
  FontMetrics  metrics;
  int dx = 0;
  if(showName)
    dx = metrics.width(Name);
  for(Property *pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display) {
      // get width of text
      tmp = metrics.width(pp->Name+"="+pp->Value);
      if(tmp > dx)  dx = tmp;
    }
  if((ty > y1) && (ty < y2)) tx = -tx-dx;     // mirror text position
  else tx = x1+tx+x2;

  mirroredX = !mirroredX;   // keep track of what's done
  _rotated += _rotated << 1;
  _rotated += 2;
  _rotated &= 3;
}

// -------------------------------------------------------
/*!
 * obsolete function. still used for some special components
 * just indicate that it's obsolete, so the netlister knows.
 */
class obsolete_exception : public std::exception{
  public:
    obsolete_exception(std::string const& w):_what(w){}
    const char* what() const noexcept{return _what.c_str();}
  private:
    std::string _what;
};

QString Component::netlist() const
{ untested();
  return "obsolete";
#if 0
  QString s = Model+":"+Name;
  int i=-1;
  // output all node names
  // This only works in cases where the resistor would be a series
  // with the component, as for the other components, they're accounted
  // as a resistor as well, and the changes were made to their .cpp
  foreach(Port *p1, Ports){ untested();
    i++;
    s += " " + p1->Connection->name();   // node names
  }

  // output all properties
  // BUG: use netlister.
  // BUG. cannot access props without const cast.
  // Qt3 Bug. needs refactoring anyway.
  Q3PtrList<Property>* P=const_cast<Q3PtrList<Property>*>(&Props);

  for(Property *p2 = P->first(); p2 != 0; p2 = P->next())
    if(p2->Name != "Symbol"){ untested();
      s += " "+p2->Name+"=\""+p2->Value+"\"";
    }else{ untested();
      // BUG: what is this?
      // doing name dependent stuff
    }

  return s;
#endif
}

// -------------------------------------------------------
QString Component::getNetlist() const
{ untested();
return "obsolete";
#if 0
  unreachable();
  switch(isActive) {
    case COMP_IS_ACTIVE:
      return netlist();
    case COMP_IS_OPEN:
      return QString("");
  }

  // Component is shortened.
  int z=0;
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  QString Node1 = pp->Connection->name();
  QString s = "";
  while (iport.hasNext())
    s += "R:" + Name + "." + QString::number(z++) + " " +
      Node1 + " " + iport.next()->Connection->name() + " R=\"0\"\n";
  return s;
#endif
}

// -------------------------------------------------------
QString Component::verilogCode(int)
{ untested();
  return QString("");   // no digital model
}


// -------------------------------------------------------
QString Component::vhdlCode(int)
{ untested();
  return QString("");   // no digital model
}

// -------------------------------------------------------
QString Component::get_VHDL_Code(int)
{ untested();
  return "obsolete";
#if 0
  switch(isActive) {
    case COMP_IS_OPEN:
      return QString("");
    case COMP_IS_ACTIVE:
      return vhdlCode(NumPorts);
  }

  // Component is shortened.
  // This puts the signal of the second port onto the first port.
  // This is logically correct for the inverter only, but makes
  // some sense for the gates (OR, AND etc.).
  // Has anyone a better idea?
  QString Node1 = Ports.at(0)->Connection->name();
  return "  " + Node1 + " <= " + Ports.at(1)->Connection->name() + ";\n";
#endif
}

// -------------------------------------------------------

// *******************************************************************
// ***  The following functions are used to load the schematic symbol
// ***  from file. (e.g. subcircuit, library component)

int Component::analyseLine(const QString& Row, int numProps)
{
  incomplete(); // obsolete
  QPen Pen;
  QBrush Brush;
  QColor Color;
  QString s;
  int i1, i2, i3, i4, i5, i6;

  s = Row.section(' ',0,0);    // component type
  if((s == "PortSym") || (s == ".PortSym")) {  // backward compatible
    if(!getIntegers(Row, &i1, &i2, &i3))
      return -1;
    for(i6 = Ports.count(); i6<i3; i6++)  // if ports not in numerical order
      Ports.append(new ComponentPort(0, 0, false));

    ComponentPort *po = Ports.at(i3-1);
    po->setPosition(i1, i2);
    po->avail = true;

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    return 0;   // do not count Ports
  }else if(s == "Line") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    i3 += i1;
    i4 += i2;
    Lines.append(new Line(i1, i2, i3, i4, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i3 < x1)  x1 = i3;
    if(i3 > x2)  x2 = i3;
    if(i4 < y1)  y1 = i4;
    if(i4 > y2)  y2 = i4;
    return 1;
  }else if(s == "EArc") { untested();
    if(!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))
      return -1;
    if(!getPen(Row, Pen, 7))  return -1;
    Arcs.append(new struct Arc(i1, i2, i3, i4, i5, i6, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2 < y1)  y1 = i2;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }else if(s == ".ID") {
    if(!getIntegers(Row, &i1, &i2))  return -1;
    tx = i1;
    ty = i2;
    Name = Row.section(' ',3,3);
    if(Name.isEmpty())  Name = "SUB";

    i1 = 1;
    Property *pp = Props.at(numProps-1);
    for(;;) {
      s = Row.section('"', i1,i1);
      if(s.isEmpty())  break;

      pp = Props.next();
      if(pp == 0) { untested();
        pp = new Property();
        Props.append(pp);

        pp->display = (s.at(0) == '1');
        pp->Value = s.section('=', 2,2);
      }

      pp->Name  = s.section('=', 1,1);
      pp->Description = s.section('=', 3,3);
      if(pp->Description.isEmpty())
        pp->Description = " ";

      i1 += 2;
    }

    while(pp != Props.last())
      Props.remove();
    return 0;   // do not count IDs
  }
  else if(s == "Arrow") { untested();
    if(!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))  return -1;
    if(!getPen(Row, Pen, 7))  return -1;

    double beta   = atan2(double(i6), double(i5));
    double phi    = atan2(double(i4), double(i3));
    double Length = sqrt(double(i6*i6 + i5*i5));

    i3 += i1;
    i4 += i2;
    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i3 < x1)  x1 = i3;
    if(i3 > x2)  x2 = i3;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i4 < y1)  y1 = i4;
    if(i4 > y2)  y2 = i4;

    Lines.append(new Line(i1, i2, i3, i4, Pen));   // base line

    double w = beta+phi;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.append(new Line(i3, i4, i5, i6, Pen)); // arrow head
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    w = phi-beta;
    i5 = i3-int(Length*cos(w));
    i6 = i4-int(Length*sin(w));
    Lines.append(new Line(i3, i4, i5, i6, Pen));
    if(i5 < x1)  x1 = i5;  // keep track of component boundings
    if(i5 > x2)  x2 = i5;
    if(i6 < y1)  y1 = i6;
    if(i6 > y2)  y2 = i6;

    return 1;
  }else if(s == "Ellipse") { untested();
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Ellips.append(new Area(i1, i2, i3, i4, Pen, Brush));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i1+i3 < x1)  x1 = i1+i3;
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2+i4 < y1)  y1 = i2+i4;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }else if(s == "Rectangle") { untested();
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    if(!getBrush(Row, Brush, 8))  return -1;
    Rects.append(new Area(i1, i2, i3, i4, Pen, Brush));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i1+i3 < x1)  x1 = i1+i3;
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2+i4 < y1)  y1 = i2+i4;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }else if(s == "Text") {  // must be last in order to reuse "s" *********
    if(!getIntegers(Row, &i1, &i2, &i3, 0, &i4))  return -1;
    Color.setNamedColor(Row.section(' ',4,4));
    if(!Color.isValid()) return -1;

    s = Row.mid(Row.indexOf('"')+1);    // Text (can contain " !!!)
    s = s.left(s.length()-1);
    if(s.isEmpty()) return -1;
    misc::convert2Unicode(s);

    Texts.append(new Text(i1, i2, s, Color, float(i3),
                          float(cos(float(i4)*pi/180.0)),
                          float(sin(float(i4)*pi/180.0))));

    QFont Font(QucsSettings.font);
    Font.setPointSizeF(float(i3));
    FontMetrics  metrics;
    QSize r = metrics.size(0, s);    // get size of text
    i3 = i1 + int(float(r.width())  * Texts.last()->mCos)
            + int(float(r.height()) * Texts.last()->mSin);
    i4 = i2 + int(float(r.width())  * -Texts.last()->mSin)
            + int(float(r.height()) * Texts.last()->mCos);

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i2 < y1)  y1 = i2;
    if(i1 > x2)  x2 = i1;
    if(i2 > y2)  y2 = i2;

    if(i3 < x1)  x1 = i3;
    if(i4 < y1)  y1 = i4;
    if(i3 > x2)  x2 = i3;
    if(i4 > y2)  y2 = i4;
    return 1;
  }else{
  }

  return 0;
}

// ---------------------------------------------------------------------
bool Component::getIntegers(const QString& s, int *i1, int *i2, int *i3,
			     int *i4, int *i5, int *i6)
{
  bool ok;
  QString n;

  if(!i1) return true;
  n  = s.section(' ',1,1);
  *i1 = n.toInt(&ok);
  if(!ok) return false;

  if(!i2) return true;
  n  = s.section(' ',2,2);
  *i2 = n.toInt(&ok);
  if(!ok) return false;

  if(!i3) return true;
  n  = s.section(' ',3,3);
  *i3 = n.toInt(&ok);
  if(!ok) return false;

  if(i4) {
    n  = s.section(' ',4,4);
    *i4 = n.toInt(&ok);
    if(!ok) return false;
  }

  if(!i5) return true;
  n  = s.section(' ',5,5);
  *i5 = n.toInt(&ok);
  if(!ok) return false;

  if(!i6) return true;
  n  = s.section(' ',6,6);
  *i6 = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// ---------------------------------------------------------------------
bool Component::getPen(const QString& s, QPen& Pen, int i)
{
  bool ok;
  QString n;

  n = s.section(' ',i,i);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  i++;
  n = s.section(' ',i,i);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  i++;
  n = s.section(' ',i,i);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

// ---------------------------------------------------------------------
bool Component::getBrush(const QString& s, QBrush& Brush, int i)
{ untested();
  bool ok;
  QString n;

  n = s.section(' ',i,i);    // fill color
  QColor co;
  co.setNamedColor(n);
  Brush.setColor(co);
  if(!Brush.color().isValid()) return false;

  i++;
  n = s.section(' ',i,i);    // fill style
  Brush.setStyle((Qt::BrushStyle)n.toInt(&ok));
  if(!ok) return false;

  i++;
  n = s.section(' ',i,i);    // filled
  if(n.toInt(&ok) == 0) Brush.setStyle(Qt::NoBrush);
  if(!ok) return false;

  return true;
}

// ---------------------------------------------------------------------
Property * Component::getProperty(const QString& name)
{ untested();
  for(Property *pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->Name == name) { untested();
      return pp;
    }
  return NULL;
}

// ***********************************************************************
// ********                                                       ********
// ********          Functions of class MultiViewComponent        ********
// ********                                                       ********
// ***********************************************************************
void MultiViewComponent::recreate()
{
  incomplete();
  Ellips.clear();
  Texts.clear();
  Ports.clear();
  Lines.clear();
  Rects.clear();
  Arcs.clear();

  // save
  bool mmir = mirroredX;
  int  rrot = _rotated;

  _rotated = 0;
  mirroredX = 0;

  createSymbol(); // requires context //

  // restore symbol orientation state.
  // this is half-fixed optimised legacy code. don't copy.

  if (mirroredX && rrot==2){ // mirrorX and rotate 180 = mirrorY
    mirrorY();
  } else  {
    if(mmir){
      mirrorX();
    }
    if (rrot){itested();
      set_rotated( rrot%4 );
    }else{
    }
  }

  // let's see if it worked
  assert(mmir==mirroredX);
  assert(rrot==_rotated);
}


// ***********************************************************************
// ********                                                       ********
// ********            Functions of class GateComponent           ********
// ********                                                       ********
// ***********************************************************************
GateComponent::GateComponent()
{ untested();
  Type = isComponent;   // both analog and digital
  Name  = "Y";

  // the list order must be preserved !!!
  Props.append(new Property("in", "2", false,
		QObject::tr("number of input ports")));
  Props.append(new Property("V", "1 V", false,
		QObject::tr("voltage of high level")));
  Props.append(new Property("t", "0", false,
		QObject::tr("delay time")));
  Props.append(new Property("TR", "10", false,
		QObject::tr("transfer function scaling factor")));

  // this must be the last property in the list !!!
  Props.append(new Property("Symbol", "old", false,
		QObject::tr("schematic symbol")+" [old, DIN40900]"));
}

// -------------------------------------------------------
QString GateComponent::netlist() const
{ untested();
  return "obsolete";
#if 0
  QString s = Model+":"+Name;

  // output all node names
  foreach(Port *pp, Ports)
    s += " "+pp->Connection->name();   // node names

  // Qt3 BUG
  Q3PtrList<Property>* P=const_cast<Q3PtrList<Property>*>(&Props);
  //
  // output all properties
  Property *p = P->at(1);
  s += " " + p->Name + "=\"" + p->Value + "\"";
  p = P->next();
  s += " " + p->Name + "=\"" + p->Value + "\"";
  p = P->next();
  s += " " + p->Name + "=\"" + p->Value + "\"\n";
  return s;
#endif
}

// -------------------------------------------------------
QString GateComponent::vhdlCode(int )
{ untested();
  return "obsolete";
#if 0
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  QString s = "  " + pp->Connection->name() + " <= ";  // output port

  // xnor NOT defined for std_logic, so here use not and xor
  if (Model == "XNOR") { untested();
    QString Op = " xor ";

    // first input port
    pp = iport.next();
    QString rhs = pp->Connection->name();

    // output all input ports with node names
    while(iport.hasNext()) { untested();
      pp = iport.next();
      rhs = "not ((" + rhs + ")" + Op + pp->Connection->name() + ")";
    }
    s += rhs;
  }
  else { untested();
    QString Op = ' ' + Model.toLower() + ' ';
    if(Model.at(0) == 'N') { untested();
      s += "not (";    // nor, nand is NOT assoziative !!! but xnor is !!!
      Op = Op.remove(1, 1);
    }

    pp = iport.next();
    s += pp->Connection->name();   // first input port

    // output all input ports with node names
    while(iport.hasNext()) { untested();
      pp = iport.next();
      s += Op + pp->Connection->name();
    }
    if(Model.at(0) == 'N')
      s += ')';
  }

  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = Props.at(2)->Value;        // delay time
    if(!misc::VHDL_Delay(td, name())) return td;
    s += td;
  }

  s += ";\n";
  return s;
#endif
}

// -------------------------------------------------------
QString GateComponent::verilogCode(int)
{ untested();
  return "obsolete";
#if 0
  bool synthesize = true;
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  QString s("");

  if(synthesize) { untested();
    QString op = Model.toLower();
    if(op == "and" || op == "nand")
      op = "&";
    else if (op == "or" || op == "nor")
      op = "|";
    else if (op == "xor")
      op = "^";
    else if (op == "xnor")
      op = "^~";

    s = "  assign";

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = Props.at(2)->Value;        // delay time
      if(!misc::Verilog_Delay(td, name())) return td;
      s += td;
    }
    s += " " + pp->Connection->name() + " = ";  // output port
    if(Model.at(0) == 'N') s += "~(";

    pp = iport.next();
    s += pp->Connection->name();   // first input port

    // output all input ports with node names
    while (iport.hasNext()) { untested();
      pp = iport.next();
      s += " " + op + " " + pp->Connection->name();
    }

    if(Model.at(0) == 'N') s += ")";
    s += ";\n";
  }
  else { untested();
    s = "  " + Model.toLower();

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = Props.at(2)->Value;        // delay time
      if(!misc::Verilog_Delay(td, name())) return td;
      s += td;
    }
    s += " " + name() + " (" + pp->Connection->name();  // output port

    pp = iport.next();
    s += ", " + pp->Connection->name();   // first input port

    // output all input ports with node names
    while (iport.hasNext()) { untested();
      pp = iport.next();
      s += ", " + pp->Connection->name();
    }

    s += ");\n";
  }
  return s;
#endif
}

// -------------------------------------------------------


// ***********************************************************************
// ********                                                       ********
// ******** The following function does not belong to any class.  ********
// ******** It creates a symbol by getting the identification     ********
// ******** string used in the schematic file and for copy/paste. ********
// ********                                                       ********
// ***********************************************************************

// do something with Dialog Buttons
void Component::dialgButtStuff(ComponentDialog&)const
{ untested();
  incomplete();
  // d.disableButtons();
}

/*--------------------------------------------------------------------------*/
QRectF Component::boundingRect() const
{
  QRectF b(x1, y1, x2-x1, y2-y1);
  return b;
}
/*--------------------------------------------------------------------------*/
unsigned Component::paramCount() const
{
  return Props.count() + Symbol::paramCount() + num_component_params;
}
/*--------------------------------------------------------------------------*/
std::string Component::paramValue(unsigned i) const
{
  unsigned s = Symbol::paramCount();
  if(i<Symbol::paramCount()){ untested();
    return Symbol::paramValue(i);
  }else if(i==s){ untested();
    return std::to_string(tx);
  }else if(i==s+1){ untested();
    return std::to_string(tx);
  }else{
    assert(i>=s+num_component_params);
    i -= (s + num_component_params);

    assert( Props.at(i));
    return Props.at(i)->value().toStdString();
  }
}
/*--------------------------------------------------------------------------*/
std::string Component::paramName(unsigned i) const
{
  unsigned s = Symbol::paramCount();
  if(i<Symbol::paramCount()){
    return Symbol::paramName(i);
  }else if(i==s){
    return "$tx";
  }else if(i==s+1){
    return "$ty";
  }else{
    i -= (s + num_component_params);
    assert( Props.at(i));
    return Props.at(i)->name().toStdString();
  }
}
/*--------------------------------------------------------------------------*/
QDialog* Component::schematicWidget(QucsDoc* Doc) const
{ untested();
  trace0("Component::editElement");
  return new ComponentDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
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
ComponentDialog::ComponentDialog(QucsDoc* d) : SchematicDialog(d)
{
  resize(450, 250);
  setWindowTitle(tr("Edit Component Properties"));
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::attach(ElementGraphics* gfx)
{
  trace0("ComponentDialog::attach");
  auto Comp=component(gfx);
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
  CompNameEdit->setText(Comp->label());
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
    QFontMetrics metrics(QucsSettings.font, 0);   // get size of text
    qDebug() << "desc = " << desc << metrics.width(desc);
    while(metrics.width(desc) > 270) {  // if description too long, cut it nicely
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
  auto C = _comp->clone();
  C->setOwner( _comp->mutable_owner() );
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
    CompNameEdit->setText(Comp->name());
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
      CompNameEdit->setText(Comp->name());
    } else if (Comp->label() != CompNameEdit->text()) {
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

    _comp = Comp = component(_gfx);

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
	currDir = QucsSettings.QucsWorkDir.path() + 
	          QDir::separator() +
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
      currDir = QucsSettings.QucsWorkDir.path();
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
    if(QucsSettings.QucsWorkDir.exists(file.fileName()) &&
       QucsSettings.QucsWorkDir.absolutePath() == file.absolutePath()) s = file.fileName();
    edit->setText(s);
  }
  /* FIX
  prop->currentIndex()->setText(1, s); */
}
/*--------------------------------------------------------------------------*/
void ComponentDialog::slotEditFile()
{
  schematic()->App->editFile(QucsSettings.QucsWorkDir.filePath(edit->text()));
}
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
