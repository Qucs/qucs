#include "components/component.h" // BUG
#include "schematic.h"

// FIXME: must be Component* SchematicParser::loadComponent(Stream&, Component*);
Component* Schematic::loadComponent(const QString& _s, Component* c) const
{
  bool ok;
  int  ttx, tty, tmp;
  QString s = _s;

  if(s.at(0) != '<'){
    return NULL;
  }else if(s.at(s.length()-1) != '>'){
    return NULL;
  }
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString label=s.section(' ',1,1);
  c->setName(label);

  QString n;
  n  = s.section(' ',2,2);      // isActive
  tmp = n.toInt(&ok);
  if(!ok){
    return NULL;
  }
  c->isActive = tmp & 3;

  if(tmp & 4){
    c->showName = false;
  }else{
    // use default, e.g. never show name for GND (bug?)
  }

  n  = s.section(' ',3,3);    // cx
  c->cx = n.toInt(&ok);
  if(!ok) return NULL;

  n  = s.section(' ',4,4);    // cy
  c->cy = n.toInt(&ok);
  if(!ok) return NULL;

  n  = s.section(' ',5,5);    // tx
  ttx = n.toInt(&ok);
  if(!ok) return NULL;

  n  = s.section(' ',6,6);    // ty
  tty = n.toInt(&ok);
  if(!ok) return NULL;

  if(!dynamic_cast<Command const*>(c)) {
    n  = s.section(' ',7,7);    // mirroredX
    if(n.toInt(&ok) == 1){
      c->mirrorX();
    }
    if(!ok) return NULL;

    n  = s.section(' ',8,8);    // rotated
    tmp = n.toInt(&ok);
    if(!ok) return NULL;
    if(c->rotated > tmp)  // neccessary because of historical flaw in ...
      tmp += 4;        // ... components like "volt_dc"
    for(int z=c->rotated; z<tmp; z++){
      c->rotate();
    }
  }else{
    // assert(c->obsolete_model_hack().at(0) == '.');
  }

  c->tx = ttx;
  c->ty = tty; // restore text position (was changed by rotate/mirror)

  QString Model = c->obsolete_model_hack(); // BUG: don't use names

  unsigned int z=0, counts = s.count('"');
  // FIXME. use c->paramCount()
  if(Model == "Sub")
    tmp = 2;   // first property (File) already exists
  else if(Model == "Lib")
    tmp = 3;
  else if(Model == "EDD")
    tmp = 5;
  else if(Model == "RFEDD")
    tmp = 8;
  else if(Model == "VHDL")
    tmp = 2;
  else if(Model == "MUTX")
    tmp = 5; // number of properties for the default MUTX (2 inductors)
  else tmp = counts + 1;    // "+1" because "counts" could be zero

  /// BUG FIXME. dont use Component parameter dictionary.
  for(; tmp<=(int)counts/2; tmp++)
    c->Props.append(new Property("p", "", true, " "));

  // load all properties
  Property *p1;
  for(p1 = c->Props.first(); p1 != 0; p1 = c->Props.next()) {
    z++;
    n = s.section('"',z,z);    // property value
    z++;
    //qDebug() << "LOAD: " << p1->Description;

    // not all properties have to be mentioned (backward compatible)
    if(z > counts) {
      if(p1->Description.isEmpty()){
        c->Props.remove();    // remove if allocated in vain
      }

      if(Model == "Diode") { // BUG: don't use names
	if(counts < 56) {  // backward compatible
          counts >>= 1;
          p1 = c->Props.at(counts-1);
          for(; p1 != 0; p1 = c->Props.current()) {
            if(counts-- < 19){
              break;
	    }

            n = c->Props.prev()->Value;
            p1->Value = n;
          }

          p1 = c->Props.at(17);
          p1->Value = c->Props.at(11)->Value;
          c->Props.current()->Value = "0";
        }
      }
      else if(Model == "AND" || Model == "NAND" || Model == "NOR" ||
	      Model == "OR" ||  Model == "XNOR"|| Model == "XOR") {
	if(counts < 10) {   // backward compatible
          counts >>= 1;
          p1 = c->Props.at(counts);
          for(; p1 != 0; p1 = c->Props.current()) {
            if(counts-- < 4)
              break;
            n = c->Props.prev()->Value;
            p1->Value = n;
          }
          c->Props.current()->Value = "10";
	}
      }
      else if(Model == "Buf" || Model == "Inv") {
	if(counts < 8) {   // backward compatible
          counts >>= 1;
          p1 = c->Props.at(counts);
          for(; p1 != 0; p1 = c->Props.current()) {
            if(counts-- < 3)
              break;
            n = c->Props.prev()->Value;
            p1->Value = n;
          }
          c->Props.current()->Value = "10";
	}
      }

      return c;
    }

    // for equations
    if(Model != "EDD" && Model != "RFEDD" && Model != "RFEDD2P")
    if(p1->Description.isEmpty()) {  // unknown number of properties ?
      p1->Name = n.section('=',0,0);
      n = n.section('=',1);
      // allocate memory for a new property (e.g. for equations)
      if(c->Props.count() < (counts>>1)) {
        c->Props.insert(z >> 1, new Property("y", "1", true));
        c->Props.prev();
      }
    }
    if(z == 6)  if(counts == 6)     // backward compatible
      if(Model == "R") {
        c->Props.getLast()->Value = n;
        return c;
      }
    p1->Value = n;

    n  = s.section('"',z,z);    // display
    p1->display = (n.at(1) == '1');
  }

  return c;
}

// -------------------------------------------------------
// save a component
// FIXME: part of corresponding SchematicSerializer implementation
// BUG: c must be const (cannot because of QT3)
void Schematic::saveComponent(QTextStream& s, Component /*const*/ * c) const
{
#if XML
  QDomDocument doc;
  QDomElement el = doc.createElement (Model);
  doc.appendChild (el);
  el.setTagName (Model);
  el.setAttribute ("inst", Name.isEmpty() ? "*" : Name);
  el.setAttribute ("display", isActive | (showName ? 4 : 0));
  el.setAttribute ("cx", cx);
  el.setAttribute ("cy", cy);
  el.setAttribute ("tx", tx);
  el.setAttribute ("ty", ty);
  el.setAttribute ("mirror", mirroredX);
  el.setAttribute ("rotate", rotated);

  for (Property *pr = Props.first(); pr != 0; pr = Props.next()) {
    el.setAttribute (pr->Name, (pr->display ? "1@" : "0@") + pr->Value);
  }
  qDebug (doc.toString());
#endif
  s << "<";
  if(dynamic_cast<Command const*>(this)){ // FIXME: separate Commands from Components
    s << ".";
  }
  s << c->obsolete_model_hack();

  s << " ";
  if(c->name().isEmpty()){
    s << "*";
  }else{
    s << c->name();
  }
  s << " ";

  int i=0;
  if(!c->showName){
    i = 4;
  }
  i |= c->isActive;
  s << QString::number(i);
  s << " "+QString::number(c->cx)+" "+QString::number(c->cy);
  s << " "+QString::number(c->tx)+" "+QString::number(c->ty);
  s << " ";
  if(c->mirroredX){
    s << "1";
  }else{
    s << "0";
  }
  s << " " << QString::number(c->rotated);

  // write all properties
  // FIXME: ask component for properties, not for dictionary
  for(Property *p1 = c->Props.first(); p1 != 0; p1 = c->Props.next()) {
    if(p1->Description.isEmpty()){
      s << " \""+p1->Name+"="+p1->Value+"\"";   // e.g. for equations
    }else{
      s << " \""+p1->Value+"\"";
    }
    s << " ";
    if(p1->display){
      s << "1";
    }else{
      s << "0";
    }
  }

  s << ">";
}
