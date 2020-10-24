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
#include "io.h"
#include <QFile>

DocumentStream::DocumentStream(QFile* /* BUG const */ file)
	: QTextStream(file)
{
}

QString const istream_t::readLine()
{
	_current_start = pos(); // really?

	_cnt = 0;
	_cmd = QTextStream::readLine().toStdString();
	return QString::fromStdString(_cmd);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if 0
template<class T>
istream_t::istream_t(T t)
	: QTextStream(t), _cmd(""), _cnt(0)
{
}
template<>
istream_t::istream_t(QFile* t);
#endif
/*--------------------------------------------------------------------------*/
istream_t::istream_t(istream_t::STRING, const std::string&s )
	: _cmd(s), _cnt(0), _ok(true)
{
}
/*--------------------------------------------------------------------------*/
/// borrowed from ap_*.cc
int istream_t::ctoi()
{
	trace3("skipbl", fullString(), cursor(), _ok);
  int val = 0;
  int sign = 1;

  skipbl();
  size_t here = cursor();
  if (skip1("-")) {
    sign = -1;
  }else{
    skip1("+");
  }

  while (is_digit()) {
    val = 10 * val + (ctoc()-'0');
  }
  skipcom();
  _ok = cursor() > here;
  return val * sign;
}

istream_t& istream_t::skipbl()
{
  while (peek() && (!isgraph(peek()))) {
	trace1("skipbl", peek());
    skip();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
istream_t& istream_t::skip1b(char t)
{ untested();
  skipbl();
  skip1(t);
  skipbl();
  return *this;
}
/*--------------------------------------------------------------------------*/
char istream_t::ctoc()
{
	trace1("ctoc", _cmd);
  char c=_cmd[_cnt];
  if(_cnt<=_cmd.size()) {
    ++_cnt;
  }else{untested();
  }
  return c;
}
/*--------------------------------------------------------------------------*/
istream_t& istream_t::skip1b(const std::string& t)
{
  skipbl();
  skip1(t);
  skipbl();
  return *this;
}
/*--------------------------------------------------------------------------*/
istream_t& istream_t::skip1(const std::string& t)
{
  if (match1(t)) {
    skip();
    assert(_ok);
  }else{
    _ok = false;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
istream_t& istream_t::skip1(char t)
{ untested();
  if (match1(t)) { untested();
    skip();
    assert(_ok);
  }else{ untested();
    _ok = false;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
std::string istream_t::ctos(const std::string& term,
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
	_cnt = i;
	return full.substr(0, i);
#endif
}
/*--------------------------------------------------------------------------*/
