
#include <map>



class PrototypeMap{
public:
	typedef QString string_type;
	typedef std::map<string_type, Symbol const*> map_type; // set?
public:
	// hmm shouldnt use map in the first place...
	class const_iterator : public map_type::const_iterator {
		friend class PrototypeMap;
	protected:
		explicit const_iterator(map_type::const_iterator i)
			: map_type::const_iterator(i){}
	public:
		Symbol const* operator*() const{
			return map_type::const_iterator::operator*().second;
		}
	};
public:
	explicit PrototypeMap();
	~PrototypeMap(){ clear(); }
public:
	void clear();
	Symbol const* operator[](string_type const& s){
		return _map[s];
	}
	Symbol const* operator[](string_type const& s) const;
	void push(QString const& s, Symbol const* e){
		assert(!_map[s]);
		_map[s] = e;
	}

public:
	const_iterator begin() const{
		return const_iterator(_map.begin());
	};
	const_iterator end() const{
		return const_iterator(_map.end());
	};

private:
	map_type _map;
};
