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
	//bool atEnd() const{return QTextStream::atEnd();}
};

typedef DocumentStream ostream_t;

// CS like stream. replace by CS...
class istream_t : private QTextStream{
public:
  enum STRING {_STRING};
private:
	istream_t(istream_t const&) = delete;
public:
	template<class T>
	istream_t(T t) : QTextStream(t), _current("") {}
	istream_t(istream_t::STRING, const std::string&s ) : _current(QString::fromStdString(s)){}

private:
	std::string ctos(const std::string& term=",=(){};",
	                 const std::string& b="\"'{",
	                 const std::string& e="\"'}",
	                 const std::string& trap="");
public:
	QString const& readLine(){
		_current = QTextStream::readLine();
		return _current;
	}
	std::string fullString() const { return _current.toStdString(); }
	bool atEnd() const{return QTextStream::atEnd();}
	istream_t& operator>>(std::string& x) {x=ctos();return *this;}
	istream_t& warn(int, size_t, const std::string&){ incomplete(); return *this;}
	istream_t& warn(int i, const std::string& s)	{return warn(i,0 , s);}
private:
	QString _current;
};

// not here
inline std::ostream& operator<<(std::ostream&o, std::pair<int, int> const& p)
{ itested(); // still used? yes :(
  return o << "(" << p.first << ", " << p.second << ")";
}

// borrowed from ap_convert.cc
inline std::string istream_t::ctos(const std::string& term, 
		     const std::string& begin_quote,
		     const std::string& end_quote,
		     const std::string& trap)
{
#if 0 // later
	assert(begin_quote.length() == end_quote.length());

	// skipbl();
	size_t begin_string = cursor();
	size_t end_string = cursor();

	std::string s;
	std::string::size_type which_quote = find1(begin_quote);
	if (which_quote != std::string::npos) {
		int quotes = 1;
		skip(); // the quote
		begin_string = cursor();
		char the_begin_quote = begin_quote[which_quote];
		char the_end_quote = end_quote[which_quote];
		for (;;) {
			if (!ns_more()) {itested();
				end_string = cursor();
				warn(bDANGER, std::string("need ") + the_end_quote);
				break;
			}else if (skip1(the_end_quote)) {
				if (--quotes <= 0) {
					end_string = cursor() - 1;
					break;
				}else{
				}
			}else if (skip1(the_begin_quote)) {
				++quotes;
				skip();
			}else if (skip1('\\')) {
				end_string = cursor() - 1;
				s += _cmd.substr(begin_string, end_string-begin_string);
				begin_string = cursor();
				skip1(the_end_quote);
			}else{
				skip();
			}
		}
		s += _cmd.substr(begin_string, end_string-begin_string);
	}else{
		while(ns_more() && !is_term(term)) {
			skip();
		}
		if (match1(trap)) {untested();
			warn(bDANGER, "ap_convert trap-exit");
		}else{
		}
		end_string = cursor();
		s = _cmd.substr(begin_string, end_string-begin_string);
	}

	skipcom();
	_ok = end_string > begin_string;
	return s;
#else
	(void)term;
	(void)begin_quote;
	(void)end_quote;
	(void)trap;
	std::string full = fullString();
	size_t i = full.find(" ");
	return full.substr(0, i);
#endif
}

#endif
