#ifndef ID_TEXT_H
#define ID_TEXT_H

#include <QString>
#include "painting.h"

struct SubParameter {
  SubParameter(bool display_, const QString& Name_, const QString& Descr_)
     : display(display_), Name(Name_), Description(Descr_) { Type = ""; };
  SubParameter(bool display_, const QString& Name_, const QString& Descr_,
        const QString& Type_)
     : display(display_), Name(Name_), Description(Descr_), Type(Type_) {};

  bool display;
  QString Name, Description, Type;
};


// not sure what this is.
class ID_Text : public Painting  {
public:
  ID_Text(int cx_=0, int cy_=0);
  ~ID_Text();
  Element* clone() const{
   return new ID_Text(*this);
  }

  void paintScheme(SchematicDoc*);
  void getCenter(int&, int&);

  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*);
  bool getSelected(float, float, float);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QString Prefix;
  QList<SubParameter *> Parameter;
};

#endif /* ID_TEXT_H */
