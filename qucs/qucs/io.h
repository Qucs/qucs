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

// output stream
class DocumentStream : public QTextStream {
private:
	DocumentStream(DocumentStream const&) = delete;
public:
//	explicit DocumentStream(){ incomplete(); }
	explicit DocumentStream(QFile* /* BUG const */ file);
	explicit DocumentStream(QString /* BUG const */ * filename) :
		QTextStream(filename, QIODevice::WriteOnly){}

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
	bool atEnd() const{return QTextStream::atEnd();}
};

typedef DocumentStream ostream_t;

// CS like stream. replace by CS...
class istream_t : private QTextStream{
private:
	istream_t(istream_t const&) = delete;
public:
	template<class T>
	istream_t(T t) : QTextStream(t), _current("") {}
public:
	QString const& readLine(){
		_current = QTextStream::readLine();
		return _current;
	}
	std::string fullString() const { return _current.toStdString(); }
	bool atEnd() const{return QTextStream::atEnd();}
private:
	QString _current;
};

// not here
inline std::ostream& operator<<(std::ostream&o, std::pair<int, int> const& p)
{ untested(); // still used?
  return o << "(" << p.first << ", " << p.second << ")";
}

#endif
