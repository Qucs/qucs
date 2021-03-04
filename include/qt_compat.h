/***************************************************************************
    begin                : 2018
    copyright            : QUCS Developers
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// a compatibility layer between different qt releases,
// collect all QT_VERSION dependent code here (TODO: remove QT<5).
//
// some Qt accessibility functions.

#ifndef QUCS_QT_COMPAT_H
#define QUCS_QT_COMPAT_H

#include <list>
#ifndef UNTANGLE_QT // later
#include <QDebug>
#include <QColor>
#include <QDir>
#include <QFont>
#include <QRectF>
#endif

#include "geometry.h"
#include "io_trace.h"
#include "platform.h"

#ifndef QT_MAJOR_VERSION
#define QT_MAJOR_VERSION (QT_VERSION >> 16)
#endif

// partly implement Q3Ptrlist, see Qt3 documentation.
// used in legacy code only (?). should be moved out.
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

#if QT_VERSION >= 0x050000

# include "platform.h"

// TODO: replace all uses in non-legacy and get rid of this
# define setResizeMode setSectionResizeMode
# define setClickable setSectionsClickable

# define setCodecForTr setCodecForLocale
# define DockRight RightDockWidgetArea
# define Q_UINT16 quint16
# define Q_UINT32 quint32
# define Q_ULONG qulonglong
# define IO_WriteOnly QIODevice::WriteOnly
# define WFlags WindowFlags
# define toAscii toLatin1

# define qInstallMsgHandler qInstallMessageHandler

# define languageChange() incomplete()

# define contentsMousePressEvent mousePressEvent
# define contentsMouseMoveEvent mouseMoveEvent
# define contentsMouseReleaseEvent mouseReleaseEvent
# define contentsMouseDoubleClickEvent mouseDoubleClickEvent

# define contentsWheelEvent wheelEvent
# define contentsDropEvent dropEvent
# define contentsDragEnterEvent dragEnterEvent
# define contentsDragLeaveEvent dragLeaveEvent
# define contentsDragMoveEvent dragMoveEvent
#endif

inline std::ostream& operator<<(std::ostream& o, QString const& q)
{
	return o << q.toStdString();
}

#include <QPoint>
#include <QPointF>
inline int getX(QPoint const p)
{
	return p.x();
}
inline int getY(QPoint const p)
{
	return p.y();
}
inline float getX(QPointF const p)
{
	return p.x();
}
inline float getY(QPointF const p)
{
	return p.y();
}

inline std::ostream& operator<<(std::ostream&o, QPoint const& p)
{
  return o << "(" << p.x() << ", " << p.y() << ")";
}
inline std::ostream& operator<<(std::ostream&o, QPointF const& p)
{
  return o << "(" << p.x() << ", " << p.y() << ")";
}

inline QPointF makeQPointF(std::pair<int,int> p)
{
	return QPointF(p.first, p.second);
}

inline QDir QDir_(std::string const& c)
{
	auto cc = QString::fromStdString(c);
	return QDir(cc);
}
inline QString QString_(std::string const& c)
{
	return QString::fromStdString(c);
}
inline QColor QColor_(std::string const& c)
{
	auto cc = QString::fromStdString(c);
	return QColor(cc);
}
inline std::string str_(QString const& c)
{
	return c.toStdString();
}
inline std::string str_(QColor const& c)
{
	auto cc = c.name();
	return cc.toStdString();
}
inline std::string str_(QFont const& c)
{
	auto cc = c.toString();
	return cc.toStdString();
}
inline QRectF QRectF_(rect_t const& p)
{
	return QRectF(p.tl().first, p.tl().second, p.w(), p.h());
}

#endif

//vim:ts=8:sw=4:et
