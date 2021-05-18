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
#include "platform.h"
#include "qio.h"
#include "exception.h"
#include "qt_compat.h"
#include <QFile>

/*--------------------------------------------------------------------------*/
enum {
  BUFLEN = 256,
  BIGBUFLEN = 4096
};
/*--------------------------------------------------------------------------*/
static char* trim(char *string)
{
  size_t idx = strlen(string);
  while (idx > 0  &&  !isgraph(string[--idx])) {
    string[idx] = '\0' ;
  }
  return string;
}
/*--------------------------------------------------------------------------*/
ostream_t& ostream_t::operator<<(std::string const& x)
{
	if(_s){
		(*_s) << QString_(x);
	}else{
	}
	return *this;
}
/*--------------------------------------------------------------------------*/
ostream_t::~ostream_t()
{
	delete _s;
}
/*--------------------------------------------------------------------------*/
void ostream_t::flush()
{
	if(_s){
		_s->flush();
	}else{
	}
}
/*--------------------------------------------------------------------------*/
ostream_t::ostream_t(std::string f)
{
	auto q = new QFile(QString_(f)); // memory leak
	if(!q->open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
		throw qucs::Exception("can't open " + f + " for writing");
	}else{
	}
	_s = new QTextStream(q);
}
/*--------------------------------------------------------------------------*/
ostream_t::ostream_t(QString /* BUG const */ * filename)
{
	_s = new QTextStream(filename, QIODevice::WriteOnly);
}
/*--------------------------------------------------------------------------*/
ostream_t::ostream_t(FILE* file)
{
	incomplete(); // still used...?
	_s = new QTextStream(file, QIODevice::WriteOnly);
}
/*--------------------------------------------------------------------------*/
// BUG.
ostream_t::ostream_t(QFile* /* BUG const */ file)
{
	incomplete(); // still used...?
	_s = new QTextStream(file);
}
/*--------------------------------------------------------------------------*/
std::string istream_t::read_line()
{
	get_line("incomplete");
	return fullstring();

	// _current_start = pos(); // really?

	_ok = true;
	_cnt = 0;
	if(_stream){
	   if(_stream->atEnd()){ untested();
			throw qucs::Exception_End_Of_Input("");
		}else{ itested();
		}

		_cmd = _stream->readLine().toStdString();
	}else{
		unreachable();
//		_file??
	}
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
	: _file(nullptr), _cmd(s), _cnt(0), _ok(true), _stream(nullptr)
{
	auto qs = new QString(QString::fromStdString(s)); // BUG: memory leak.
	_stream = new QTextStream(qs); // yikes.
	_length = s.size();
}
/*--------------------------------------------------------------------------*/
istream_t::istream_t(istream_t::STDIN)
	: _file(stdin), _cnt(0), _ok(true), _stream(nullptr)
{
}
/*--------------------------------------------------------------------------*/
istream_t::istream_t(istream_t::WHOLE_FILE, const std::string& name)
	: _file(nullptr), _cnt(0), _ok(true), _stream(nullptr)
{
	auto qfn = QString::fromStdString(name);
	auto d = new QFile(qfn); // BUG: memory leak. (get rid of QTextStream...)
	d->open(QIODevice::ReadOnly);
	if(!d->isOpen()){ itested();
		throw qucs::Exception_File_Open(name + ':' + " error"); // strerror(errno));
	}else{
		_stream = new QTextStream(d);
	}
}
/*--------------------------------------------------------------------------*/
/// borrowed from ap_*.cc
int istream_t::ctoi()
{
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
    skip();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
istream_t& istream_t::skip1b(char t)
{
  skipbl();
  skip1(t);
  skipbl();
  return *this;
}
/*--------------------------------------------------------------------------*/
char istream_t::ctoc()
{
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
	trace3("skip1", _cnt, _length, _cmd);
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
// borrowed from ap_match
std::string istream_t::ctos(const std::string& term,
		     const std::string& begin_quote,
		     const std::string& end_quote,
		     const std::string& trap)
{
	trace2("hmmm", fullstring(), cursor());
	assert(begin_quote.length() == end_quote.length());

	skipbl();
	trace2("hmmm", fullstring(), cursor());
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
			if (!ns_more()) {
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
    }else if ((!optional) && (*str2 == '{')) {
      ++str2;
      optional = true;
    }else if ((optional) && (*str2 == '}')) {
      ++str2;
      optional = false;
    }else if ((*str2 == ' ') && is_term()) {
      // blank in ref string matches anything that delimits tokens
      skipbl();
      ++str2;
    }else if (peek() == *str2) {
      skip();
      ++str2;
//    }else if ((OPT::case_insensitive) && (tolower(peek()) == tolower(*str2))) { untested();
//      skip();
//      ++str2;
    }else if (optional) {
      while (*str2 != '}') {
			++str2;
      }
    }else{
      // mismatch
      const char* bar = strchr(str2, '|');
      if (bar && (bar[-1] != '\\')) {
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
bool istream_t::is_file() const
{
	if(_stream){
		return true;
	}else{
		return (_file && !isatty(fileno(_file)));
	}
}
/*--------------------------------------------------------------------------*/
// looks a bit wonky..
static std::string getlines(FILE *fileptr)
{
	assert(fileptr);
	const int buffer_size = BIGBUFLEN;
	std::string s;

	bool need_to_get_more = true;  // get another line (extend)
	while (need_to_get_more) {
		char buffer[buffer_size+1];
		char* got_something = fgets(buffer, buffer_size, fileptr);
		if (!got_something) { // probably end of file
			need_to_get_more = false;
			if (s == "") {
				throw qucs::Exception_End_Of_Input("");
			}else{untested();
			}
		}else{
			trim(buffer);
			size_t count = strlen(buffer);
			if (buffer[count-1] == '\\') {
				buffer[count-1] = '\0';
			}else{
				// look ahead at next line
				//int c = fgetc(fileptr);
				int c;
				while (isspace(c = fgetc(fileptr))) {
					// skip
				}
				if (c == '+') {
					need_to_get_more = true;
				}else if (c == '\n') {unreachable();
					need_to_get_more = true;
					ungetc(c,fileptr);
				}else{
					need_to_get_more = false;
					ungetc(c,fileptr);
				}
			}
			s += buffer;
			s += ' ';
		}
	}
	return s;
}
/*--------------------------------------------------------------------------*/
CS& istream_t::get_line(std::string const& prompt)
{
  ++_line_number;

  if(_stream){
    // yikes.
	   if(_stream->atEnd()){
			throw qucs::Exception_End_Of_Input("");
		}else{
		}
      _cmd = _stream->readLine().toStdString();
      _cnt = 0;
      _length = _cmd.length();
      _ok = true;
  }else if (is_file()) {
    _cmd = getlines(_file);
    _cnt = 0;
    _length = _cmd.length();
    _ok = true;
  }else{untested();
    assert(_file == stdin);
    char cmdbuf[BUFLEN];
    getcmd(prompt.c_str(), cmdbuf, BUFLEN);
    _cmd = cmdbuf;
    _cnt = 0;
    _length = _cmd.length();
    _ok = true;
  }

//  if (OPT::listing) {
//    IO::mstdout << "\"" << fullstring() << "\"\n";
//  }else{
//  }
  return *this;
}
/*--------------------------------------------------------------------------*/
bool istream_t::atEnd()
{
	if(_stream){
		return _stream->atEnd();
	}else{itested();
		return is_end();
	}
}
/*--------------------------------------------------------------------------*/
/* getcmd: get a command.
 * if "fin" is stdin, display a prompt first.
 * Also, actually do logging, echo, etc.
 */
char *getcmd(const char *prompt, char *buffer, int buflen)
{
	assert(prompt);
	assert(buffer);
	if (isatty(fileno(stdin))) {
		// stdin is keyboard
#if defined(HAVE_LIBREADLINE)
		if (OPT::edit) {
			char* line_read = readline(prompt);
			if (!line_read) {itested();
				throw qucs::Exception_End_Of_Input("EOF on stdin");
			}else{
			}
			// readline gets a new buffer every time, so copy it to where we want it
			char* end_of_line = (char*)memccpy(buffer, line_read, 0, static_cast<size_t>(buflen-1));
			if (!end_of_line) {
				buffer[buflen-1] = '\0';
			}else{
				*end_of_line = '\0';
			}
			free(line_read);

			if (*buffer) {
				add_history(buffer);
			}else{
			}
		}else
#endif
		{
			// IO::mstdout << prompt;	/* prompt & flush buffer */
			std::cout << prompt; // yikes.
			if (!fgets(buffer, buflen, stdin)) {untested();	/* read line */
				throw qucs::Exception_End_Of_Input("EOF on stdin");
			}else{
			}
		}
//		(IO::mstdout - mout) << '\r';	/* reset col counter */
		trim(buffer);
//		(mlog + mout) << buffer << '\n';
		return buffer;
	}else{
		// stdin is file
		if (!fgets(buffer, buflen, stdin)) {itested();	/* read line */
			throw qucs::Exception_End_Of_Input("EOF on stdin");
		}else{
		}
		trim(buffer);
//		(mlog + mout) << buffer << '\n';
		return buffer;
	}
}
/*--------------------------------------------------------------------------*/
namespace IO{
// does not work (yet). Qt?
//  ostream_t mstdout(stdout);
}
/*--------------------------------------------------------------------------*/
void CS::ctostr(char* des, int len, const std::string& term)
{
  skipbl();
  int ii;
  for (ii = 0;  ii < len && !is_term(term);  ++ii) {
    des[ii] = ctoc();
  }
  des[ii] = '\0';

  while (!is_term(term)) {untested();
    skip();
  }
  skipcom();
}
/*--------------------------------------------------------------------------*/
int CS::ctoo()
{
  int val = 0;

  skipbl();
  size_t here = cursor();
  while (is_digit()) {
    val = 8 * val + (ctoc()-'0');
  }
  skipcom();
  _ok = cursor() > here;
  return val;
}
/*--------------------------------------------------------------------------*/
int CS::ctox()
{untested();
  int val = 0;

  skipbl();
  size_t here = cursor();
  while (is_xdigit()) {untested();
    if (is_digit()) {untested();
      val = 16 * val + (ctoc()-'0');
    }else{untested();
      val = 16 * val + (tolower(ctoc())-'a'+10);
    }
  }
  skipcom();
  _ok = cursor() > here;
  return val;
}
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, int* val, AP_MOD mod, int scale)
{
  if (cmd.umatch(key + " {=}")) {
    switch(mod) {
    case mNONE:			*val = int(cmd.ctof());		break;
    case mSCALE:    untested(); *val = int(cmd.ctof())*scale;	break;
    case mOFFSET:   untested(); *val = int(cmd.ctof())+scale;	break;
    case mINVERT:   untested(); *val = 1 / int(cmd.ctof());	break;
    case mPOSITIVE: untested(); *val = std::abs(int(cmd.ctof())); break;
    case mOCTAL:		*val = cmd.ctoo();		break;
    case mHEX:      untested(); *val = cmd.ctox();		break;
    }
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
