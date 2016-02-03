/****************************************************************************
** Meta object code from reading C++ file 'vtabbar.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vtabbar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vtabbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VTabBar[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,    9,    8,    8, 0x05,
      36,   32,    8,    8, 0x05,
      56,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      69,   32,    8,    8, 0x0a,
      93,   90,    8,    8, 0x0a,
     118,  112,    8,    8, 0x0a,
     144,    8,    8,    8, 0x0a,
     174,  163,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VTabBar[] = {
    "VTabBar\0\0p_id\0activatedTab(int)\0tab\0"
    "activatedTab(VTab*)\0allTabsOff()\0"
    "setCurrentTab(VTab*)\0id\0setCurrentTab(int)\0"
    "p_pos\0setPosition(VTabPosition)\0"
    "switchOffAllTabs()\0p_id,state\0"
    "setTabState(int,bool)\0"
};

void VTabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VTabBar *_t = static_cast<VTabBar *>(_o);
        switch (_id) {
        case 0: _t->activatedTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->activatedTab((*reinterpret_cast< VTab*(*)>(_a[1]))); break;
        case 2: _t->allTabsOff(); break;
        case 3: _t->setCurrentTab((*reinterpret_cast< VTab*(*)>(_a[1]))); break;
        case 4: _t->setCurrentTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setPosition((*reinterpret_cast< VTabPosition(*)>(_a[1]))); break;
        case 6: _t->switchOffAllTabs(); break;
        case 7: _t->setTabState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VTabBar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VTabBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VTabBar,
      qt_meta_data_VTabBar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VTabBar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VTabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VTabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VTabBar))
        return static_cast<void*>(const_cast< VTabBar*>(this));
    if (!strcmp(_clname, "VTabInterface"))
        return static_cast< VTabInterface*>(const_cast< VTabBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int VTabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void VTabBar::activatedTab(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VTabBar::activatedTab(VTab * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VTabBar::allTabsOff()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
