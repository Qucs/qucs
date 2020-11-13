/*$Id: ap.h,v 26.130 2009/11/15 21:51:59 al Exp $  -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * stuff for the "ap" family of parsing functions
 * --- HACKED FOR QUCS ---
 */
#ifndef AP_H
#define AP_H
//#include "md.h"
/*--------------------------------------------------------------------------*/
INTERFACE char* getcmd(const char*,char*,int);
/*--------------------------------------------------------------------------*/
enum AP_MOD{
  mNONE,	/* nothing special */
  mSCALE,	/* scale it after reading */
  mOFFSET,	/* add an offset */
  mINVERT,	/* save 1 / the number */
  mPOSITIVE,	/* store absolute value */
  mOCTAL,	/* read the number in octal */
  mHEX		/* read the number in hex */
};

const bool ONE_OF = false;

class istream_t : private QTextStream{
public:
  enum STDIN {_STDIN};
  enum INC_FILE {_INC_FILE};
  enum WHOLE_FILE {_WHOLE_FILE};
  enum STRING {_STRING};
private:
  FILE* _file;
  std::string _name;
  std::string _cmd;
  size_t  _cnt;
  size_t  _length;
  size_t  _begin_match;
  size_t  _end_match;
  bool _ok;
  int _line_number;
public:
  // construction, destruction, and re-construction
  explicit    CS(STDIN);
  explicit    CS(INC_FILE, const std::string& name);
  explicit    CS(WHOLE_FILE, const std::string& name);
  explicit    CS(STRING, const std::string& s);
  CS(const CS& p) = delete;
  template<class T>
  istream_t(T* t) : QTextStream(t), _cmd(""), _cnt(0), _ok(true) {
    trace1("istream construct", t);
  }
  CS&	      operator=(const std::string& s);
  CS&	      operator=(const CS& p);
  CS&	      get_line(const std::string& prompt);
  std::string	      read_line(); // HACK
	      ~CS()		{} // {if (is_file()) {fclose(_file);}}
  
  // status - non-consuming
  size_t cursor()const	{return _cnt;}
  bool	stuck(size_t* last)	{bool ok=*last<_cnt; *last=_cnt; return !ok;}
  bool	gotit(size_t last)	{return last<_cnt;}
	operator bool()const	{return _ok;}

  // get -- non-consuming
  const std::string fullstring()const		{return _cmd;}
  const std::string substr(size_t i)const {return ((_cmd.length()>=i) ? _cmd.substr(i) : "");}
  const std::string substr(size_t i, size_t n)const	{return _cmd.substr(i,n);}
  const std::string tail()const			{return substr(_cnt);}
  char		    peek()const			{return _cmd[_cnt];}

  // status - may consume whitespace only
  bool	      ns_more()const	{return peek()!='\0';}
  bool	      more()		{skipbl(); return ns_more();}
  bool	      is_end()		{return !more();}
  bool atEnd() const{return QTextStream::atEnd();}
//  bool	      is_file()		{return (_file && !isatty(fileno(_file)));}
  bool	      is_first_read()const {untested(); return (_line_number == 0);}

  // control
  CS&	      reset(size_t c=0) {_cnt=c; _ok=true; return *this;}

  // exception handling (ap_error.cc) non-consuming
  CS&	      check(int, const std::string&);
  CS&	      warn(int, size_t, const std::string&);
  CS&         warn(int i, const std::string& s)	{return warn(i,cursor(), s);}

  // string matching (ap_match.cc) possibly consuming, sets _ok
  CS&	      umatch(const std::string&);
  CS&	      scan(const std::string&);
  std::string last_match()const;
  std::string trimmed_last_match(const std::string& = " ,=;")const;

  // character tests - non-consuming, no _ok
  bool	      match1(char c)const{return (peek()==c);}
  bool	      match1(const std::string& c)const
		{return ns_more() && strchr(c.c_str(),peek());}
  size_t      find1(const std::string& c)const
	{return ((ns_more()) ? c.find_first_of(peek()) : std::string::npos);}
  bool	      is_xdigit()const
		{untested(); return (match1("0123456789abcdefABCDEF"));}
  bool	      is_digit()const	{return (match1("0123456789"));}
  bool	      is_pfloat()const	{return (match1(".0123456789"));}
  bool	      is_float()const	{return (match1("+-.0123456789"));}
  bool	      is_argsym()const	{return (match1("*?$%_&@"));}
  bool	      is_alpha()const	{return !!isalpha(toascii(peek()));}
  bool	      is_alnum()const   {return !!isalnum(toascii(peek()));}
  bool	      is_term(const std::string& t = ",=(){};")
	{char c=peek(); return (c=='\0' || isspace(c) || match1(t));}

  // conversions (ap_convert.cc) always consuming
  char	      ctoc();
  void        ctostr(char*,int,const std::string&);
  std::string ctos(const std::string& term=",=(){};",
		   const std::string& b="\"'{",
		   const std::string& e="\"'}",
		   const std::string& trap="");
  std::string get_to(const std::string& term);

  // conversions (ap_convert.cc) consumes if successful, sets _ok
  double      ctof();
  bool	      ctob();
  int	      ctoi();
  unsigned    ctou();
  int	      ctoo();
  int	      ctox();
  double      ctopf()			 {return std::abs(ctof());}
  CS&	      operator>>(bool& x)	 {x=ctob();return *this;}
  CS&	      operator>>(char& x)	 {untested(); x=ctoc();return *this;}
  CS&         operator>>(int& x)	 {x=ctoi();return *this;}
  CS&         operator>>(unsigned& x)	 {x=ctou();return *this;}
  CS&         operator>>(double& x)	 {x=ctof();return *this;}
  CS&	      operator>>(std::string& x) {x=ctos();return *this;}

  // skip (ap_skip.cc) possibly consuming, sets _ok
  CS&	      skip(int c=1) 
    {_cnt=static_cast<size_t>(static_cast<int>(_cnt)+c); _ok=_cnt<=_length; return *this;}
  CS&	      skipbl();
  CS&	      skip1b(char);
  CS&	      skip1(char);
  CS&	      skip1b(const std::string&);
  CS&	      skip1(const std::string&);
  CS&	      skiparg();
  CS&	      skipto1(const std::string&);
  CS&	      skipto1(char);
  CS&	      skipcom()			{return skip1b(",");}
  CS&	      operator>>(const char& x)	{return skip1b(x);}
  CS&	      operator>>(const char* x)	{return umatch(x);}
};	
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// these are non-member to provide a consistent interface,
// like the templates to follow
INTERFACE bool Get(CS& cmd, const std::string&, bool*);
INTERFACE bool Get(CS& cmd, const std::string&, int*,    AP_MOD=mNONE, int=0);
INTERFACE bool Get(CS& cmd, const std::string&, double*, AP_MOD, double=0.);
/*--------------------------------------------------------------------------*/
template <class T>
bool Get(CS& cmd, const std::string& key, T* val)
{
  if (cmd.umatch(key + " {=}")) {
    cmd >> *val;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline bool scan_get(CS& cmd, const std::string& key, T* val)
{
  if (cmd.scan(key)) {
    cmd >> '=' >> *val;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline bool Set(CS& cmd, const std::string& key, T* val, T newval)
{
  if (cmd.umatch(key + ' ')) {
    *val = newval;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline CS& operator>>(CS& cmd, T& val)
{
  val.parse(cmd);
  return cmd;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
