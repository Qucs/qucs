#ifndef QUCS_EXCEPTION_H
#define QUCS_EXCEPTION_H
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
};

class ExceptionCantParse : public std::exception{
};


#endif
