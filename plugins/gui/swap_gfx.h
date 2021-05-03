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
#include "schematic_scene.h"

namespace qucs {
/*--------------------------------------------------------------------------*/
class SwapElementGfx : public SchematicEdit {
public:
  ~SwapElementGfx(){
  }
  SwapElementGfx(ElementGraphics* g, ElementGraphics* g_new) :
	  SchematicEdit(prechecked_cast<SchematicScene*>(g->scene()))
  {
    assert(g);
    assert(g_new);
	 qInsert(g_new);
	 qDelete(g);
	 assert(g->scene() == g_new->scene());
    setText("Swap Element " + QString_(element(g)->label()));
  }
};
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
