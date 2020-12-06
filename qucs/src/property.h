
#ifndef QUCS_PROPERTY_H
#define QUCS_PROPERTY_H
struct Property // : public Object
{
  Property(const QString& _Name="", const QString& _Value="",
	   bool _display=false, const QString& Desc="")
	 : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
  QString Name, Value;
  bool    display;   // show on schematic or not ?
  QString Description;

  QString const& name() const{ return Name; }
  QString const& value() const{ return Value; }
};
#endif
