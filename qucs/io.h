/***************************************************************************
    begin                : 2018
    copyright            : Felix
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
#ifndef QUCS_IO_H
#define QUCS_IO_H

#include <QTextStream> // BUG
#include "trace.h"

class QFile;
class DocumentStream : public QTextStream {
public:
	explicit DocumentStream(){ incomplete(); }
	explicit DocumentStream(QFile* /* BUG const */ file);
	explicit DocumentStream(QString /* BUG const */ * filename, QIODevice::OpenModeFlag flag = QIODevice::ReadOnly) :
		QTextStream(filename, flag){}

public:
	DocumentStream& operator<<(std::string const& x){
		QTextStream::operator<<(QString::fromStdString(x));
		return *this;
	}
	template<class T>
	DocumentStream& operator<<(T x){
		QTextStream::operator<<(x);
		return *this;
	}
};
typedef QTextStream stream_t; //  BUG

// not here
inline std::ostream& operator<<(std::ostream&o, std::pair<int, int> const& p)
{
  return o << "(" << p.first << ", " << p.second << ")";
}

#endif
