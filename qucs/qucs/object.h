#ifndef QUCS_OBJECT_H
#define QUCS_OBJECT_H

class Object{
 public:
  virtual ~Object(){}

  // clone the object.
  virtual Object* newOne() /*BUG const*/ {return 0 /*NULL, actually*/;}
  virtual Object* clone()const {return 0 /*NULL, actually*/;}
};
#endif
