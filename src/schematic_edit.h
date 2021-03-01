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

#ifndef QUCS_SCHEMATIC_EDIT
#define QUCS_SCHEMATIC_EDIT

#include <QUndoCommand> // really?
#include "element_graphics.h"
#include "place.h"
#include <set>

namespace qucs {

// edit a schematic add/delete/alter
class SchematicEdit : public QUndoCommand {
public:
	typedef std::list<ElementGraphics*> list_t;
	struct swap_t{
		swap_t(ElementGraphics* gfx, Element* elt)
			: _gfx(gfx), _elt(elt) {}
		ElementGraphics* _gfx;
		Element* _elt;
		~swap_t(){
			delete _elt;
		}
	};
protected:
	explicit SchematicEdit(SchematicScene* s)
	  : QUndoCommand(), _first(true), _scn(s) {}
	SchematicEdit(SchematicEdit const&) = delete;

	template<class IT>
	void qDelete(IT const& deletelist) {
		for(auto i : deletelist){ untested();
			// ++k; // TODO: set label
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){ untested();
				qDelete(eg);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
	}
	void qDelete(ElementGraphics* eg);
	template<class IT>
	void qInsert(IT const& deletelist) {
		for(auto i : deletelist){ untested();
			// ++k; // TODO: set label
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){ untested();
				qInsert(eg);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
	}
	void qInsert(ElementGraphics* eg);
	void qSwap(ElementGraphics* eg, Element* e); // BUG?

private: // QUndoCommand
	void undo() override {itested();
		redo();
	}
	void redo() override;

private:
	void do_it_first();
	void do_it();

	template<class T>
	void expandSwap(T& rem);
	template<class T>
	void postRmPort(Place const*, T&);
	template<class T>
	bool addmerge(ElementGraphics*, T& rem);
	template<class T>
	void save(T& rem, T& add);

	QList<ElementGraphics*> items(QRectF const& area) const;
	QList<ElementGraphics*> items(QPointF const &pos,
                                 Qt::ItemSelectionMode mode=Qt::IntersectsItemShape,
                                 Qt::SortOrder order = Qt::DescendingOrder) const;
	Node const* nodeAt(pos_t const&) const;
	SchematicScene const* scene() const{
		return _scn;
	}
	SchematicScene* scene(){
		return _scn;
	}

private:
	list_t _ins;
	list_t _del;
	std::vector<swap_t*> _swap;
	bool _first;
	SchematicScene* _scn;
	std::set<ElementGraphics*> _check_places;
};
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
