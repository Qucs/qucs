/****************************************************************************
** Meta object code from reading C++ file 'vtabbutton.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vtabbutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vtabbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VTab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,    6,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   37,    5,    5, 0x0a,
      59,   53,    5,    5, 0x0a,
      87,   85,    5,    5, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VTab[] = {
    "VTab\0\0p_id,p_state\0toggled(int,bool)\0"
    "p_id\0setID(int)\0p_pos\0setPosition(VTabPosition)\0"
    "b\0slotToggled(bool)\0"
};

void VTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VTab *_t = static_cast<VTab *>(_o);
        switch (_id) {
        case 0: _t->toggled((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->setID((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setPosition((*reinterpret_cast< VTabPosition(*)>(_a[1]))); break;
        case 3: _t->slotToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VTab::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_VTab,
      qt_meta_data_VTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VTab))
        return static_cast<void*>(const_cast< VTab*>(this));
    if (!strcmp(_clname, "VTabInterface"))
        return static_cast< VTabInterface*>(const_cast< VTab*>(this));
    return QPushButton::qt_metacast(_clname);
}

int VTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void VTab::toggled(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
