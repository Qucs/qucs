/***************************************************************************
    copyright            : (C) 2020, 2021 by Felix Salfelder
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

#include "component_widget.h"
#include <QMimeData>
#include "io_trace.h"
#include "element.h"
#include "symbol.h"
#include <QDrag>
#include <QPainter>
#include "viewpainter.h"
#include "element_list.h"
#include "painting.h"

using qucs::ViewPainter;
using qucs::Element;

// ElementWidget??
void ComponentWidget::startDrag(Qt::DropActions)
{ untested();
  QListWidgetItem *item = currentItem();
  assert(item);
  // QListWidget::startDrag(x); // trying to do something similar.
										  // why not piggyback?!

  if(auto ci=dynamic_cast<ComponentListWidgetItem*>(item)){

	  QByteArray itemData;
	  QDataStream dataStream(&itemData, QIODevice::WriteOnly);

	  assert(ci->cloneElement());

	  dataStream << *ci;

	  QMimeData *mimeData = new QMimeData;

	  QDrag *drag = new QDrag(this);
	  auto pixmap = item->icon().pixmap(200); // why 200?
	  mimeData->setData(itemMimeType(), itemData);
	  drag->setPixmap(pixmap);

//	  drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));
//	  drag->setDragCursor(pixmap, Qt::MoveAction);

	  incomplete();
	  drag->setMimeData(mimeData);

	  drag->exec(Qt::MoveAction);
	  // drag->exec(Qt::CopyAction);
  }else{
	  unreachable();
  }
}

ComponentListWidgetItem::ComponentListWidgetItem(qucs::Element const* e)
	: _e(e)
{itested();
	assert(e);
	QString File = e->iconBasename();
	QString Name = QString::fromStdString(e->short_label());
	qucs::Painting const* p = dynamic_cast<qucs::Painting const*>(e);
	if(p){
	}else{
		trace1("not a painting?", e->short_label());
		return;

	}

////	if(auto s=dynamic_cast<Symbol const*>(e)){
////		Name = QString::fromStdString(s->typeName());
////	}else{
////	}
//	setIcon( QPixmap(":/bitmaps/" + File + ".png"));

	static const int N = 128;
	QRectF br = p->bounding_rect().toRectF();
	QRectF bb = p->bounding_rect().toRectF();
	auto bbc = bb.center();
	QRectF bbt = bb.transposed();
	bbt.moveCenter(bbc);
	bb |= bbt;
	assert(bbt.center() == bb.center());
	assert(bb.height() == bb.width());

	QPixmap pix(N, N);
	pix.fill();
	QPainter* q = new QPainter(&pix);

	QPointF center(N/2, N/2);
	q->translate(center);
	double s = double(N)/bb.width();
	q->scale(s, s); // double(N)/bb.width(), double(N)/bb.height());
	q->translate(- br.center());

	ViewPainter v(q);

#if 0
	auto sym = dynamic_cast<SchematicSymbol const*>(e);

	if(!sym){
		q->setPen(QColor(255,34,255,255));
		q->drawRect(bb);
	}else if(auto s = sym->symbolPaintings()){
		// HACK
		for(auto i : *s){
			Element* e = i;
			e->paint(&v);
		}
	}else{
	}

#else
	p->paint(&v);
#endif

	delete q;
	setIcon(pix);

	setText(Name);
	setToolTip("element: " + Name);
}

Element const* ComponentListWidgetItem::proto() const
{
	return _e;
}

Element* ComponentListWidgetItem::cloneElement() const
{
	if(_e){
		return _e->clone();
	}else{
		unreachable();
		return nullptr;
	}
}

QDataStream & ComponentListWidgetItem::load(QDataStream &x)
{ untested();
	quint64 v;
	x >> v;
	trace1("E", v);
	intptr_t vv = v;
	_e = static_cast<Element*>((void*)vv);
	trace1("E", _e);
	return x;
}

QDataStream & ComponentListWidgetItem::dump(QDataStream &x) const
{ untested();
	quint64 v;
	v = static_cast<quint64>((intptr_t(_e)));
	trace1("save", v);
	x << v;
	return x;
}
