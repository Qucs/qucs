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

std::string findFile(const std::string& filename, const std::string& path, int mode);

class QFile;

// output stream
class ostream_t : public QTextStream /*BUG*/ {
private:
	ostream_t(ostream_t const&) = delete;
public:
//	explicit ostream_t(){ incomplete(); }
	explicit ostream_t(FILE* f);
	explicit ostream_t(QFile* /* BUG const */ file);
	explicit ostream_t(QString /* BUG const */ * filename) :
		QTextStream(filename, QIODevice::WriteOnly){}

public:
	ostream_t& operator<<(std::string const& x){
		QTextStream::operator<<(QString::fromStdString(x));
		return *this;
	}
	template<class T>
	ostream_t& operator<<(T x){
		QTextStream::operator<<(x);
		return *this;
	}
	//bool atEnd() const{return QTextStream::atEnd();}
};

#define CS istream_t
#define AP_MOD AP_MOD_
#define ONE_OF oneOf
#define  mNONE	    qmNONE	/* nothing special */
#define  mSCALE	 qmSCALE	/* scale it after reading */
#define  mOFFSET	 qmOFFSET	/* add an offset */
#define  mINVERT	 qmINVERT	/* save 1 / the number */
#define  mPOSITIVE qmPOSITIVE	/* store absolute value */
#define  mOCTAL	 qmOCTAL	/* read the number in octal */
#define  mHEX		 qmHEX		/* read the number in hex */
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

// it does not work with Qt. switch implementation first.
// namespace IO{
//   extern ostream_t mstdout;
// }

#endif
