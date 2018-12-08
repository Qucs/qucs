
class Object{
 public:
  virtual ~Object(){}

  // clone the object.
  virtual Object* newOne()const {return 0 /*NULL, actually*/;}
};
