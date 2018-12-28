/***************************************************************************
                                 container.h
                                -----------
    begin                : 2018
    copyright            : QUCS Developers
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// provide a compatibility layer between different qt releases for qucs.
//
// collect all QT_VERSION dependent code here.

#ifndef QUCS_CONTAINER_H
#define QUCS_CONTAINER_H

#include <list>
#include <QDebug>

#include "trace.h"

#ifndef QT_MAJOR_VERSION
#define QT_MAJOR_VERSION (QT_VERSION >> 16)
#endif

#if QT_MAJOR_VERSION < 5
# define USE_SCROLLVIEW
#endif

// this is also in schematic.h. that's a moc issue.
#ifdef USE_SCROLLVIEW
# define SchematicBase Q3ScrollView
#else
# define SchematicBase QGraphicsView
class ElementGraphics;
#endif


// strictly, this should also work with qt4.

// partly implement Q3Ptrlist, see Qt3 documentation.
// just don't use it in new code, remove what is no longer used.
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
			for(auto x : localList){
				delete x;
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
	void setAutoDelete(bool b) {
		_autodelete = b;
	};
	T* first() {
		cur = localList.begin();
		if (localList.size() == 0)
			return nullptr;
		return *cur;
	};
	T* last() {
		if (localList.size()) { itested();
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
	bool isEmpty() const { untested();
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
	unsigned containsRef(T *t) { untested();
		unsigned i=0;
		for (const_iterator c=begin(); c!=end(); ++c){
			i += (t == *c);
		}
		return i;
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
	bool removeRef(T *t) {
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
	T* getFirst() { itested();
		return localList.front();
	};
	T* getLast() { itested();
		return localList.back();
	};
	T* next() {
		if (cur == localList.end()){ untested();
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
	T* prev() {
		if (cur == localList.end()) { untested();
			return nullptr;
		}else if (cur == localList.begin()){ itested();
			cur=localList.end();
			return nullptr;
		}else{ itested();
			cur--;
			return *cur;
		}
	};
	T* current() {
		if (cur==localList.end()){ untested();
			return nullptr;
		}else{ untested();
			return *cur;
		}
	};
	unsigned int count() {
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
		if (cur!=localList.end()){ untested();
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
	void insert(unsigned int i, T *t) { untested();
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
	bool within_range(int i){
		if (i < 0){ untested();
			return false;
		}else if(i<int(localList.size())) { untested();
			return true;
		}else{ untested();
			return false;
		}
	}
private:
	std::list<T*> localList;
	bool _autodelete;
	iterator cur;
};

#if QT_VERSION >= 0x050000

# define TRUE true
# define FALSE false

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

#endif

//vim:ts=8:sw=4:et
