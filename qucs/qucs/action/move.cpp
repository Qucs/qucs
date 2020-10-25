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

class MoveSelection : public SchematicEdit {
public:
	template<class IT>
	MoveSelection(QPoint delta, SchematicDoc& ctx, IT selection)
	: SchematicEdit(*ctx.sceneHACK()) { itested();
		trace1("MoveSelection", delta);
		size_t k = 0;
		for(auto i : selection){itested();
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){itested();
				++k;
				Element* elt = eg->cloneElement();
				eg->hide();
				auto ec = elt->center();
				eg->setPos(QPoint(getX(ec), getY(ec)));
				eg->show();
				assert(elt);
				int dx = getX(delta);
				int dy = getY(delta);

				pos_t c = elt->center() + pos_t(dx, dy);
				elt->setCenter(c);

				qSwap(eg, elt); // gaah.
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
		setText("move " + QString::number(k) + " items by "
				+ QString::number(delta.x()) + ", "
				+ QString::number(delta.y())) ;
	}
}; // MoveSelection
