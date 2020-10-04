// (c) 2020 felix
//
#include "component_widget.h"
#include <QMimeData>
#include "io_trace.h"
#include "element.h"
#include <QDrag>

void ComponentWidget::startDrag(Qt::DropActions x)
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
{
	assert(e);
	QString File = e->iconBasename();
	auto Name = e->label();
	setIcon( QPixmap(":/bitmaps/" + File + ".png"));
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

QDataStream & ComponentListWidgetItem::load(QDataStream &x){
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
