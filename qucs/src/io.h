/***************************************************************************
    copyright            : 2018, 2020 Felix Salfelder
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
#include "io_trace.h"

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
	//bool atEnd() const{return QTextStream::atEnd();}
};

typedef DocumentStream ostream_t;

#define CS istream_t
#define INTERFACE
#include "ap.h"
#undef CS
#undef INTERFACE
#define fullString fullstring


// not here
inline std::ostream& operator<<(std::ostream&o, std::pair<int, int> const& p)
{ // still used? yes :(
  return o << "(" << p.first << ", " << p.second << ")";
}


#endif
