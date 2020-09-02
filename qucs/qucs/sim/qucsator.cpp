
TODO

namespace{
class Qucsator : public Simulator{
public:
	explicit Qucsator() : Simulator() {}
	Qucsator(Qucsator const&) = delete;
	~Qucsator(){}
private: // Simulator
  NetLang const* netLang() const override {
	  return dynamic_cast<NetLang const*>(doclang_dispatcher["qucsator"]);
  }
  // DocumentFormat const* netLister() const override;
}QS;
static Dispatcher<Simulator>::INSTALL p(&simulator_dispatcher, "qucsator", &QS);
}
