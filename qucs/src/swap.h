
#include <QUndoCommand>

// BUG: SchematicEdit.
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
    setText("Swap Element " + QString::fromStdString(e->label()));
  }
private:
  void undo(){
    redo();
  }
  void redo(){
    assert(_gfx);
//	  _elt = _gfx->swap(_elt);

	 bool sel = _gfx->isSelected();
    _gfx->hide();
    {
      Element* tmp = _gfx->detachElement();
      _gfx->attachElement(_elt);
      _elt = tmp;
    }
    // _gfx->update();
    _gfx->show();
	 _gfx->setSelected(sel);
  }
private:
  ElementGraphics* _gfx{nullptr};
  Element* _elt{nullptr};
};
