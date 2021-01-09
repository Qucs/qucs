/***************************************************************************
    copyright            : (C) 2015, 2020 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// data containers for qucs
#include "data.h"
/* -------------------------------------------------------------------------------- */
Data::~Data()
{
	CommonData::attach(nullptr, &_common);
}
/* -------------------------------------------------------------------------------- */
CommonData::~CommonData()
{
	assert(_attach_count==0);
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// borrowed from e_compon
void CommonData::attach(CommonData* d, CommonData** to)
{
	assert(to);
	if (d == *to) {
		// The new and old are the same object.  Do nothing.
	}else if (!d) {untested();
		// There is no new common.  probably a simple element
		detach(to);
	}else if (!*to) {
		// No old one, but have a new one.
		++(d->_attach_count);
		trace1("++1", d->_attach_count);
		*to = d;
#if 1
	}else if (*d != **to) { untested();
		incomplete();
		// They are different, usually by edit.
		detach(to);
		++(d->_attach_count);
		trace1("++2", d->_attach_count);
		*to = d;
#endif
	}else if (d->_attach_count == 0) { untested();
		// The new and old are identical.
		// Use the old one.
		// The new one is not used anywhere, so throw it away.
		trace1("delete", d->_attach_count);    
		delete d;
	}else{untested();
		// The new and old are identical.
		// Use the old one.
		// The new one is also used somewhere else, so keep it.
	}
}
/* -------------------------------------------------------------------------------- */
// borrowed from e_compon
void CommonData::detach(CommonData** from)
{
	assert(from);
	if (*from) {
		assert((**from)._attach_count > 0);
		--((**from)._attach_count);
		trace1("--", (**from)._attach_count);
		if ((**from)._attach_count == 0) {
			trace1("delete", (**from)._attach_count);
			delete *from;
		}else{ untested();
			trace1("nodelete", (**from)._attach_count);
		}
		*from = NULL;
	}else{
	}
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
