/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QUndoCommand>
#include "element_graphics.h"

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
  ElementGraphics* _gfx;
  Element* _elt;
};
