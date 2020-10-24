
#ifndef QUCS_ERROR_H
#define QUCS_ERROR_H

/// for now.
#define	bNOERROR	0
#define bTRACE		1
#define bLOG		2
#define bDEBUG		3
#define bPICKY		4
#define	bWARNING	5
#define bDANGER		6

class Exception : public std::exception{
public:
	explicit Exception(std::string w="?") : _what(w) {}
	const char* what() const noexcept { return _what.c_str(); }

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


#endif
