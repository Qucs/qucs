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

// listing components and other elements to the left.
//  BUG: dragndrop appears to be intended, but does not work

#ifndef QUCS_COMPONENT_WIDGET_H
#define QUCS_COMPONENT_WIDGET_H

#include <QListWidget>
#include <QStringLiteral>

namespace qucs {
class Element;
}

class ComponentWidget : public QListWidget{
	Q_OBJECT
public:
	explicit ComponentWidget(QWidget*q): QListWidget(q) {}

	static QString const itemMimeType() { return QStringLiteral("image/x-componentwidgetitem"); }
public:
	void startDrag(Qt::DropActions /*supportedActions*/);
};

class ComponentListWidgetItem : public QListWidgetItem, public QObject{
public:
	explicit ComponentListWidgetItem() : QListWidgetItem(), QObject(), _e(nullptr) {};
	ComponentListWidgetItem(qucs::Element const* e);
	ComponentListWidgetItem(ComponentListWidgetItem const&) = delete;

public:
	qucs::Element* cloneElement() const;
	qucs::Element const* proto() const;

public:
	QDataStream & dump(QDataStream &x) const;
	QDataStream & load(QDataStream &x);

private:
	qucs::Element const* _e;
};

inline QDataStream &operator<<(QDataStream &x, const ComponentListWidgetItem &y)
{
	y.dump(x);
	return x;
}
inline QDataStream &operator>>(QDataStream &x, ComponentListWidgetItem &y)
{
	y.load(x);
	return x;
}

#endif
