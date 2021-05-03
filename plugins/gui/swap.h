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
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class SwapGfxCommand : public QUndoCommand {
	SwapGfxCommand() = delete;
	SwapGfxCommand(SwapGfxCommand const&) = delete;
public:
	~SwapGfxCommand(){
		delete _ng;
	}
	SwapGfxCommand(ElementGraphics* g, ElementGraphics* e, bool skip=false)
		: _gfx(g), _ng(e), _skip(skip)
	{
		assert(e);
		setText("Swap Element " + QString::fromStdString(element(e)->short_label()));
	}
private:
	void undo(){
		redo();
	}
	void redo(){
		if(_skip){
		}else{ untested();
			assert(_gfx);
			//	  _elt = _gfx->swap(_elt);

			bool sel = _gfx->isSelected();
			_gfx->hide();
			_ng->show_();
			_ng->setSelected(sel);
			std::swap(_gfx, _ng);
		}
		_skip = false;
	}
private:
	ElementGraphics* _gfx;
	ElementGraphics* _ng;
	bool _skip{false};
};
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
