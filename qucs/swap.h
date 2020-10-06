
#include <QUndoCommand>

class SwapSymbolCommand : public QUndoCommand {
  SwapSymbolCommand() = delete;
  SwapSymbolCommand(SwapSymbolCommand const&) = delete;
public:
  ~SwapSymbolCommand(){
    delete _elt;
  }
  SwapSymbolCommand(ElementGraphics* g, Element* e)
    : _gfx(g), _elt(e)
  {
    assert(e);
    setText("Swap Element " + e->label());
  }
private:
  void undo(){
    redo();
  }
  void redo(){
    assert(_gfx);
    _gfx->hide();
    if(1){
      Element* tmp = _gfx->detachElement();
      _gfx->attachElement(_elt);
      _elt = tmp;
    }
    _gfx->show();
  }
private:
  ElementGraphics* _gfx;
  Element* _elt;
};
