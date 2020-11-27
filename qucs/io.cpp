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

std::string istream_t::read_line()
{
	// _current_start = pos(); // really?

	_ok = true;
	_cnt = 0;
	_cmd = QTextStream::readLine().toStdString();
	_length = _cmd.length();
	return _cmd;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if 0
template<class T>
istream_t::istream_t(T t)
	: QTextStream(t), _cmd(""), _cnt(0)
{ untested();
}
template<>
istream_t::istream_t(QFile* t);
#endif
/*--------------------------------------------------------------------------*/
istream_t::istream_t(istream_t::STRING, const std::string&s )
	: _cmd(s), _cnt(0), _ok(true)
{ untested();
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
static const int bDANGER=5;
// borrowed from ap_match
std::string istream_t::ctos(const std::string& term,
		     const std::string& begin_quote,
		     const std::string& end_quote,
		     const std::string& trap)
{
#if 1 // later
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
				}else{ untested();
				}
			}else if (skip1(the_begin_quote)) { untested();
				++quotes;
				skip();
			}else if (skip1('\\')) { untested();
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
	trace2("ctos", fullString(), s);
	return s;
#else
	(void)term;
	(void)begin_quote;
	(void)end_quote;
	(void)trap;
	std::string full = fullString();
	size_t i = full.find(" ");
	_cnt = i;
	std::string s = full.substr(0, i);
	trace2("ctos", fullString(), s);
	return s;
#endif
}
/*--------------------------------------------------------------------------*/
// borrowed from ap_match
istream_t& istream_t::umatch(const std::string& s)
{
  size_t start = cursor();
  skipbl();
  size_t begin_match = cursor();
  const char* str2 = s.c_str();
  bool optional = 0;

  for (;;) {
    if ((!*str2) || (*str2 == '|')) {
      _ok = true;
      break;
    }else if ((str2[0] == '\\') && (peek() == str2[1])) { untested();
      skip();
      str2 += 2;
    }else if ((!optional) && (*str2 == '{')) { untested();
      ++str2;
      optional = true;
    }else if ((optional) && (*str2 == '}')) { untested();
      ++str2;
      optional = false;
    }else if ((*str2 == ' ') && is_term()) { untested();
      // blank in ref string matches anything that delimits tokens
      skipbl();
      ++str2;
    }else if (peek() == *str2) {
      skip();
      ++str2;
//    }else if ((OPT::case_insensitive) && (tolower(peek()) == tolower(*str2))) { untested();
//      skip();
//      ++str2;
    }else if (optional) { untested();
      while (*str2 != '}') { untested();
			++str2;
      }
    }else{
      // mismatch
      const char* bar = strchr(str2, '|');
      if (bar && (bar[-1] != '\\')) { untested();
	str2 = bar+1;
	reset(start);
      }else{
	_ok = false;
	break;
      }
    }
  }

  if (_ok) {
    _begin_match = begin_match;
    _end_match = cursor();
    skipcom();
    _ok = true;
  }else{
    reset(start);
    _ok = false;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
// from ap_error.cc
istream_t& istream_t::warn(int badness, size_t spot, const std::string& message)
{
	incomplete();
	std::cerr << "parse warning " << message << "\n";
#if 0
  if (badness >= OPT::picky) { untested();
    if (spot < 40) { untested();
      IO::error << _cmd.substr(0,70) << '\n';
      IO::error.tab(spot);
    }else{ untested();
      IO::error << _cmd.substr(0,15) << " ... " << _cmd.substr(spot-20, 56) << '\n';
      IO::error.tab(40);
    }
    IO::error << "^ ? " + message + '\n';
  }else{ untested();
  }
#endif
  return *this;
}
/*--------------------------------------------------------------------------*/
#define CS istream_t
#include <math.h>
/*--------------------------------------------------------------------------*/
// from ap_convert.cc
double CS::ctof()
{
  double val = 0.0;
  double power = 1.0;
  int    sign = 1;

  skipbl();
  if (!is_float()) {
    skipcom();
    _ok = false;
    return 0.;
  }else{
  }

  if (skip1("-")) {			// sign
    sign = -1;
  }else{
    skip1("+");
  }

  while (is_digit()) {			// up to dec pt
    val = 10.0 * val + (ctoc()-'0');
  }
  skip1(".");				// dec pt

  while (is_digit()) {			// after dec pt
    val = 10.0 * val + (ctoc()-'0');
    power *= .1;
  }

  if (skip1("eE")) {			// exponent: E form
    int expo = 0;
    int es = 1;
    if (skip1("-")) {
      es = -1;
    }else{
      skip1("+");
    }
    while (is_digit())
      expo = 10 * expo + (ctoc()-'0');
    expo *= es;
    power *= pow(10., expo);
//  }else if ((OPT::units == uSPICE) && skip1("mM")) {		// M is special
//    if (skip1("eE")) {			// meg
//      power *= 1e6;
//    }else if (skip1("iI")) {		// mil
//      power *= 25.4e-6;
//    }else{				// plain m (milli)
//      power *= 1e-3;
//    }
  }else if (skip1("M")) {
//    assert(OPT::units == uSI);
    power *= 1e6;
  }else if (skip1("m")) {
//    assert(OPT::units == uSI);
    power *= 1e-3;
  }else if (skip1("uU")) {		// other letters
    power *= 1e-6;
  }else if (skip1("nN")) {
    power *= 1e-9;
  }else if (skip1("p")) {
    power *= 1e-12;
  }else if (skip1("P")) {
    power *= 1e15; // ((OPT::units == uSI) ? (1e15) : 1e-12);
  }else if (skip1("fF")) {
    power *= 1e-15;
  }else if (skip1("aA")) {
    power *= 1e-18;
  }else if (skip1("kK")) {
    power *= 1e3;
  }else if (skip1("gG")) {
    power *= 1e9;
  }else if (skip1("tT")) {
    power *= 1e12;
  }else if (skip1("%")) {untested();
    power *= 1e-2;
  }else{
  }
  while (is_alpha()) {			// skip letters
    skip();
  }
  skipcom();
  _ok = true;
  return (sign*val*power);
}
/*--------------------------------------------------------------------------*/
CS & CS::check(int badness, const std::string& message)
{
  skipbl();
  switch (peek()) {
  case '/':	_ok = umatch("//"); skip(); break;
  case ';':
  case '\'':	_ok = true;  skip(); break;
  case '\0':	_ok = true; break;
  default:	_ok = false; warn(badness, message); break;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
