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
#ifndef QUCS_CONTAINER_H
#define QUCS_CONTAINER_H

#include <QList>
#include <QDebug>

// TODO: trace.h
#include <iostream>
#define incomplete() ( \
    std::cerr << "@@#\n@@@\nincomplete:" \
              << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )

// implement Q3Ptrlist for use in old code.
// just don't use it in new code.
template <class T>
class Q3PtrList : public QList<T> {
public:
    Q3PtrList() { };
    Q3PtrList(const T &val) { localList(val); cur = -1; autodelete =false; };
    ~Q3PtrList() { localList.clear(); };
    T* at(unsigned int i) {
        if (i < (unsigned int)localList.count()) {
            return localList[i];
        }
        else {
            return NULL;
        }
    };
    int at() {
        return cur;
    };
    void setAutoDelete(bool b) {
        autodelete = b;
    };
    T* first() {   // pointer to first element
        cur = 0;
        if (localList.count() == 0)
            return 0;
        return localList[cur];
    };
    T* last() {    // pointer to last element
        if (localList.count()) {
            cur = localList.count() -1;
            return localList[cur];
        }
        else {
            return 0;
        }
    };
    void append(T *t) {
        localList.append(t);
    };
    int contains(T *) {
		 incomplete();
        return -1;
    };
     int findNext(T *) {
		 incomplete();
        return -1;
    };
    int findNextRef(T *) {
		 incomplete();
        return -1;
    };
    bool isEmpty() {
		 incomplete();
        return false;
    };
    int findPrev() {
		 incomplete();
        return -1;
    };
    bool replace(int, T*) {
		 incomplete();
        return true;
    };
    int containsRef(T *t) { // 
        int n = 0;
        for (int i = 0; i < localList.count(); i++){
            if (t == localList[i]) {
                n++;
            }
        }
        return n;
    };
    int findRef(T *t) { //
        for (int i = 0; i < localList.count(); i++){
            if (t == localList[i]) {
                return i;
            }
        }
        return -1;
    };
    bool removeRef(T *t) { //
        int nr = findRef(t);
        if (nr >= 0) {
            remove(nr);
            return true;
        }
        return false;
    };
    void prepend(T *t) {
        localList.insert(0, t);
    };
    T* take(uint index) { // 
        return localList.takeAt(index);
    };
    T* take() { // 
        if (cur != -1){
            return localList.takeAt(cur);
        }
        else {
            return 0;
        }
    };
    T* getFirst() {
        return localList.first();
    };
    T* getLast() {
        return localList.last();
    };
    T* next() {    // get pointer to next element, correct the current
        if (cur >= 0 && cur < localList.count() - 1) {
            cur++;
            return localList[cur];
        }
        else {
            return 0;
        }
    };
    T* prev() {    // get pointer to prev element, correct the current
        if (cur == 0) {
            return 0;
        }
        if (cur > 0 && cur < localList.count()) {
            cur--;
            return localList[cur];
        }
        else {
            return 0;
        }
    };
    T* current() { // get pointer to current element
        if (cur >= 0 && cur < localList.count()) {
            return localList[cur];
        }
        else {
            return 0;
        }
    };
    unsigned int count() {
        return (unsigned int)localList.count();
    };
    void remove(int i) { //
        if (i >= 0 && i < localList.count()) {
            localList.removeAt(i);
        }
    };
    void remove() { // remove the current element
        if (cur >= 0 && cur < localList.count()) {
            localList.removeAt(cur);
        }
    };
    void removeLast() { 
        int c = localList.count() -1;
        if (c >= 0) {
            localList.removeAt(c);
        }
    };
    void removeFirst() { 
        int c = localList.count() -1;
        if (c >= 0) {
            localList.removeAt(0);
        }
    };
    void insert(unsigned int i, T *t) { // 
        localList.insert(i, t);
    };
    int find(T *t) {
        for (int i = 0; i < localList.count(); i++){
            if (t == localList[i]) {
                return i;
            }
        }
        return -1;
    };

private:
    QList<T*> localList; // why not std::list!?
    bool autodelete;
    int cur; // current element number, bug, why not iterator?!
};

#endif
