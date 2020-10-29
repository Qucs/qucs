// (c) 2020 felix
//
#include "component_widget.h"
#include <QMimeData>
#include "io_trace.h"
#include "element.h"
#include "symbol.h"
#include <QDrag>
#include <QPainter>
#include "viewpainter.h"
#include "schematic_model.h"
#include "painting.h"

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

ComponentListWidgetItem::ComponentListWidgetItem(Element const* e)
	: _e(e)
{ untested();
	assert(e);
	QString File = e->iconBasename();
	auto Name = e->label();

////	if(auto s=dynamic_cast<Symbol const*>(e)){
////		Name = QString::fromStdString(s->typeName());
////	}else{
////	}
//	setIcon( QPixmap(":/bitmaps/" + File + ".png"));

	static const int N = 128;
	QRectF br = e->boundingRect();
	QRectF bb = e->boundingRect();
	QPointF bbc = bb.center();
	QRectF bbt = bb.transposed();
	bbt.moveCenter(bbc);
	bb |= bbt;
	assert(bbt.center() == bb.center());
	assert(bb.height() == bb.width());

	QPixmap p(N, N);
	p.fill();
	QPainter* q = new QPainter(&p);

	QPointF center(N/2, N/2);
	q->translate(center);
	double s = double(N)/bb.width();
	q->scale(s, s); // double(N)/bb.width(), double(N)/bb.height());
	q->translate(- br.center());

	ViewPainter v(q);

	e->paint(&v);
	auto sym = dynamic_cast<SchematicSymbol const*>(e);

	if(!sym){
		q->setPen(QColor(255,34,255,255));
		q->drawRect(bb);
	}else if(auto s = sym->symbolPaintings()){
		for(auto i : *sym->symbolPaintings()){
			Element* e = i;
			e->paint(&v);
		}
	}else{
	}

	delete q;
	setIcon(p);

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
