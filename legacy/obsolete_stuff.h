// ripped off element.h & others. used in legacy code only
#ifndef QUCS_OBSOLETE_STUFF
#define QUCS_OBSOLETE_STUFF

# include <QPen> // BUG

#if 1 // BUG: used in component? and in Subcircuit?
struct Line {
	Line(Line const&) = default;
  Line(int _x1, int _y1, int _x2, int _y2, QPen _style)
       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
  int   x1, y1, x2, y2;
  QPen  style;
};

struct Arc {
	Arc(Arc const&) = default;
  Arc(int _x, int _y, int _w, int _h, int _angle, int _arclen, QPen _style)
      : x(_x), y(_y), w(_w), h(_h), angle(_angle),
	arclen(_arclen), style(_style) {};
  int   x, y, w, h, angle, arclen;
  QPen  style;
};

struct Area {
  Area(int _x, int _y, int _w, int _h, QPen _Pen,
	QBrush _Brush = QBrush(Qt::NoBrush))
	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
  int    x, y, w, h;
  QPen   Pen;
  QBrush Brush;    // filling style/color
};

// does not work. move to legacy stuff.
struct Text {
  Text(int _x, int _y, const QString& _s, QColor _Color = QColor(0,0,0),
	float _Size = 10.0, float _mCos=1.0, float _mSin=0.0)
	: x(_x), y(_y), s(_s), Color(_Color), Size(_Size),
	  mSin(_mSin), mCos(_mCos) { over = under = false; };
  int	  x, y;
  QString s;
  QColor  Color;
  float	  Size, mSin, mCos; // font size and rotation coefficients
  bool	  over, under;      // text attributes
};
#endif

// partly implement Q3Ptrlist, see Qt3 documentation.
// used in legacy code only?
template <class T>
class Q3PtrList {
public:
	typedef std::list<T*> container_type;
	typedef typename container_type::const_iterator const_iterator;
	typedef typename container_type::iterator iterator;
public:
	Q3PtrList() : _autodelete(false) { };
	Q3PtrList(const T &val) : _autodelete(false) { untested();
		localList(val);
		cur = localList.end();
	};
	~Q3PtrList() {
		if(_autodelete){
			incomplete();
			for(auto x : localList){ untested();
				incomplete(); // this does not work
				(void)x;
				// delete x;
			}
		}else{
		}
	};
	T* at(unsigned int i) {
		if (i < (unsigned int)localList.size()) {
			cur = localList.begin();
			std::advance(cur, i);
			return *cur;
		} else {
			cur = localList.end();
			return nullptr;
		}
	};
	int at() { untested();
		incomplete();
		return -1;
	};
	void setAutoDelete(bool) {
		incomplete(); // this is nuts
		_autodelete = false; // b;
	};
	T* first() {
		cur = localList.begin();
		if (localList.size() == 0)
			return nullptr;
		return *cur;
	};
	T* first() const {
		cur = localList.begin();
		if (localList.size() == 0)
			return nullptr;
		return *cur;
	};
	T* last() {
		if (localList.size()) {
			cur = localList.end();
			--cur;
			return *cur;
		} else { untested();
			return nullptr;
		}
	};
	void append(T *t) {
		cur = localList.insert(localList.end(), t);
	};
	int contains(T *) { untested();
		incomplete();
		return -1;
	};
	int findNext(T *) { untested();
		incomplete();
		return -1;
	};
	bool isEmpty() const {
		return !localList.size();
	};
	int findPrev() { untested();
		incomplete();
		return -1;
	};
	bool replace(int, T*) { untested();
		incomplete();
		return true;
	};
	unsigned containsRef(T *t) const{ untested();
		unsigned i=0;
		for (const_iterator c=begin(); c!=end(); ++c){
			i += (t == *c);
		}
		return i;
	};
	// yikes. don't use
	int findRef(T const*t) const {
		auto* s=const_cast<Q3PtrList<T>* >(this);
		return s->findRef(t);
	};
	int findRef(T *t) {
		auto i=0;
		for (cur=begin(); cur!=end(); ++cur){
			if (t == *cur){
				return i;
			}
			++i;
		}
		return -1;
	};
	iterator find(T const *t) {
		for (cur=begin(); cur!=end(); ++cur){
			if (t == *cur){
				break;
			}else{
			}
		}
		return cur;
	};
	bool removeRef(T const*tt) {
		T* t=const_cast<T*>(tt);
		int nr = findRef(t);
		if (nr >= 0) {
			remove(nr);
			return true;
		}
		return false;
	};
	void prepend(T *t) { untested();
		localList.push_front(t);
		cur=begin();
	};
	T const* take(uint i) const { untested();
		auto* s=const_cast<Q3PtrList<T>* >(this);
		return const_cast<T*>(s->take(i));
	};
	T* take(uint i) { untested();
		if(within_range(i)){
			cur = begin();
			std::advance(cur, i);
			T* ret=*cur;
			cur = localList.erase(cur);
			return ret;
		}else{ untested();
			return nullptr;
		}
	};
	T* take() { untested();
		if (cur != localList.end()){ untested();
			auto t=*cur;
			auto newcur=cur;
			++newcur;
			localList.erase(cur);
			cur=newcur;
			return t;
		} else { untested();
			return nullptr;
		}
	};
	T* getFirst() const {
		return localList.front();
	};
	T* getLast() const {
		return localList.back();
	};
	T* getFirst() {
		return localList.front();
	};
	T* getLast() {
		return localList.back();
	};
	T const* next() const{
		if (cur == localList.end()){
			return nullptr;
		}else{
			cur++;
			if (cur==localList.end()){
				return nullptr;
			}else{
				return *cur;
			}
		}
	};
	T* next() {
		auto const* t=this;
		return const_cast<T*>(t->next());
	}
	T* prev() {
		if (cur == localList.end()) { untested();
			return nullptr;
		}else if (cur == localList.begin()){ itested();
			cur=localList.end();
			return nullptr;
		}else{
			cur--;
			return *cur;
		}
	};
	T* current() {
		if (cur==localList.end()){ untested();
			return nullptr;
		}else{
			return *cur;
		}
	};
	size_t size() const{
		return localList.size();
	};
	unsigned int count() const{
		return (unsigned int)localList.size();
	};
	void remove(int i) {
		if (i < 0){ untested();
		}else if(i<int(localList.size())) {
			cur=begin();
			std::advance(cur, i);
			remove();
		}
	};
	void remove() {
		if (cur!=localList.end()){
			auto next=cur;
			++next;
			maybe_delete(*cur);
			localList.erase(cur);
			cur = next;
		}else{ untested();
		}
	};
	bool removeLast() { untested();
		auto c = localList.size();
		if(c) { untested();
			cur=end();
			--cur;
			maybe_delete(*cur);
			cur = localList.erase(cur);
			--cur;
			return true;
		}else{ untested();
			return false;
		}
	};
	bool removeFirst() { untested();
		auto c = localList.size();
		if(c) { untested();
			cur=begin();
			cur=localList.erase(cur);
			return true;
		}else{ untested();
			return false;
		}
	};
	void insert(unsigned int i, T *t) {
		cur=localList.begin();
		std::advance(cur, i);
		cur=localList.insert(cur, t);
	};
#if 0
   // check: this doesn't look right
	int find(T *t) { untested();
		incomplete();
		return 0;

		// int i=0;
		// for (auto x : localList){ untested();
		// 	// NB: this is what compareitem would do... it does not work
		// 	if (*t == *x) { untested();
		// 		return i;
		// 	}else{
		// 		++i;
		// 	}
		// }
		// return -1;
	};
#endif
	const_iterator begin() const{
		return localList.begin();
	}
	const_iterator end() const{
		return localList.end();
	}
	iterator begin(){
		return localList.begin();
	}
	iterator end(){
		return localList.end();
	}
	void clear(){
		return localList.clear();
	}
private:
	void maybe_delete(T* x){
		if(_autodelete){ untested();
			delete x;
		}else{
		}
	}
	bool within_range(int i) const{
		if (i < 0){ untested();
			return false;
		}else if(i<int(localList.size())) { untested();
			return true;
		}else{ untested();
			return false;
		}
	}
private:
	container_type localList;
	bool _autodelete;
	mutable iterator cur;
};

#endif
