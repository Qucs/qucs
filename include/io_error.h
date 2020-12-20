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

#ifndef QUCS_ERROR_H
#define QUCS_ERROR_H

/// for now.
#define bNOERROR	0
#define bTRACE		1
#define bLOG		2
#define bDEBUG		3
#define bPICKY		4
#define bWARNING	5
#define bDANGER   6

class Exception : public std::exception{
public:
	explicit Exception(std::string w="?") : _what(w) {}
	const char* what() const noexcept { return _what.c_str(); }
	std::string const& message() const noexcept { return _what; }

private:
	std::string _what;
};

class ExceptionCantFind : public Exception{
public:
	explicit ExceptionCantFind(std::string w="?") : Exception("can't find:" + w) {}
	explicit ExceptionCantFind(std::string w, std::string ww) : Exception("can't find:" + w + " in " + ww) {}
	explicit ExceptionCantFind(std::string const& dev, std::string const& key, std::string const& scope)
	: Exception(dev + ": can't find: " + key + " in " + scope) {}
};

class ExceptionCantParse : public std::exception{
};

struct Exception_End_Of_Input :public Exception{
  Exception_End_Of_Input(const std::string& Message) 
    :Exception(Message) {
  }
};
struct Exception_Quit :public Exception{
  Exception_Quit(const std::string& Message) 
    :Exception(Message) {
  }
};
struct Exception_File_Open :public Exception{
  Exception_File_Open(const std::string& Message)
    :Exception(Message) {
  }
};

#endif
