
// # (c) 2020 felix
//
#ifndef QUCS_COMPONENT_WIDGET_H
#define QUCS_COMPONENT_WIDGET_H

#include <QListWidget>
#include <QStringLiteral>

class ComponentWidget : public QListWidget{
	Q_OBJECT
public:
	explicit ComponentWidget(QWidget*q): QListWidget(q) {}

	static QString const itemMimeType() { return QStringLiteral("image/x-componentwidgetitem"); }
public:
	void startDrag(Qt::DropActions /*supportedActions*/);
};

class Element;
class ComponentListWidgetItem : public QListWidgetItem, public QObject{
public:
	explicit ComponentListWidgetItem() : QListWidgetItem(), QObject(), _e(nullptr) {};
	ComponentListWidgetItem(Element const* e);
	ComponentListWidgetItem(ComponentListWidgetItem const&) = delete;

public:
	Element* cloneElement() const;
	Element const* proto() const;

public:
	QDataStream & dump(QDataStream &x) const;
	QDataStream & load(QDataStream &x);

private:
	Element const* _e;
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
