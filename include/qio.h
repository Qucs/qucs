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

#include "io_trace.h"
std::string findFile(const std::string& filename, const std::string& path, int mode);

class QFile;
class QTextStream;
class QString;

// output stream
class ostream_t {
private:
	ostream_t(ostream_t const&) = delete;
public:
	~ostream_t();
//	explicit ostream_t(){ incomplete(); }
	explicit ostream_t(std::string f);
	explicit ostream_t(FILE* f);
	explicit ostream_t(QFile* /* BUG const */ file);
	explicit ostream_t(QString /* BUG const */ * filename);

public:
	ostream_t& operator<<(std::string const& x);
	ostream_t& operator<<(double x){
		return *this << std::to_string(x);
	}
	ostream_t& operator<<(long unsigned int x){
		return *this << std::to_string(x);
	}
	ostream_t& operator<<(const char* x){
		return *this << std::string(x);
	}
	template<class T>
	ostream_t& operator<<(T x){
		if(_s){
			(*_s) << x;
		}else{
		}
		return *this;
	}
	void flush();
	//bool atEnd() const{return QTextStream::atEnd();}
private:
	QTextStream* _s{nullptr};
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
#include "ap.h"
#undef CS
#define fullString fullstring


// not here
inline std::ostream& operator<<(std::ostream&o, std::pair<int, int> const& p)
{ // still used? yes :(
  return o << "(" << p.first << ", " << p.second << ")";
}

// inline ostream_t& operator<<(ostream_t& o, std::string const& s)
// {
// 	incomplete();
// 	return o;
// }

// it does not work with Qt. switch implementation first.
// namespace IO{
//   extern ostream_t mstdout;
// }

#endif
