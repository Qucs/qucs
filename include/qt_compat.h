/***************************************************************************
    begin                : 2018
    copyright            : QUCS Developers
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// a compatibility layer between different qt releases,
// collect all QT_VERSION dependent code here (TODO: remove QT<5).
//
// some Qt accessibility functions.

#ifndef QUCS_QT_COMPAT_H
#define QUCS_QT_COMPAT_H

#include <list>
#ifndef UNTANGLE_QT // later
#include <QDebug>
#include <QColor>
#include <QDir>
#include <QFont>
#include <QRectF>
#endif

#include "geometry.h"
#include "io_trace.h"
#include "platform.h"

#ifndef QT_MAJOR_VERSION
#define QT_MAJOR_VERSION (QT_VERSION >> 16)
#endif


#if QT_VERSION >= 0x050000

// TODO: replace all uses in non-legacy and get rid of this
# define setResizeMode setSectionResizeMode
# define setClickable setSectionsClickable

# define setCodecForTr setCodecForLocale
# define DockRight RightDockWidgetArea
# define Q_UINT16 quint16
# define Q_UINT32 quint32
# define Q_ULONG qulonglong
# define IO_WriteOnly QIODevice::WriteOnly
# define WFlags WindowFlags
# define toAscii toLatin1

# define qInstallMsgHandler qInstallMessageHandler

# define languageChange() incomplete()

# define contentsMousePressEvent mousePressEvent
# define contentsMouseMoveEvent mouseMoveEvent
# define contentsMouseReleaseEvent mouseReleaseEvent
# define contentsMouseDoubleClickEvent mouseDoubleClickEvent

# define contentsWheelEvent wheelEvent
# define contentsDropEvent dropEvent
# define contentsDragEnterEvent dragEnterEvent
# define contentsDragLeaveEvent dragLeaveEvent
# define contentsDragMoveEvent dragMoveEvent
#endif

inline std::ostream& operator<<(std::ostream& o, QString const& q)
{
	return o << q.toStdString();
}

#include <QPoint>
#include <QPointF>
inline int getX(QPoint const p)
{
	return p.x();
}
inline int getY(QPoint const p)
{
	return p.y();
}
inline float getX(QPointF const p)
{
	return p.x();
}
inline float getY(QPointF const p)
{
	return p.y();
}

inline std::ostream& operator<<(std::ostream&o, QPoint const& p)
{
  return o << "(" << p.x() << ", " << p.y() << ")";
}
inline std::ostream& operator<<(std::ostream&o, QPointF const& p)
{
  return o << "(" << p.x() << ", " << p.y() << ")";
}

inline QPointF makeQPointF(std::pair<int,int> p)
{
	return QPointF(p.first, p.second);
}

inline QDir QDir_(std::string const& c)
{
	auto cc = QString::fromStdString(c);
	return QDir(cc);
}
inline QString QString_(std::string const& c)
{
	return QString::fromStdString(c);
}
inline QColor QColor_(std::string const& c)
{
	auto cc = QString::fromStdString(c);
	return QColor(cc);
}
inline std::string str_(QString const& c)
{
	return c.toStdString();
}
inline std::string str_(QColor const& c)
{
	auto cc = c.name();
	return cc.toStdString();
}
inline std::string str_(QFont const& c)
{
	auto cc = c.toString();
	return cc.toStdString();
}
inline QRectF QRectF_(rect_t const& p)
{
	return QRectF(p.tl().first, p.tl().second, p.w(), p.h());
}

#endif

//vim:ts=8:sw=4:et
