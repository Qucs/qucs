/****************************************************************************
** Meta object code from reading C++ file 'spicefile.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "spicefile.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spicefile.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpiceFile[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      28,   10,   10,   10, 0x08,
      43,   10,   10,   10, 0x08,
      56,   10,   10,   10, 0x08,
      70,   10,   10,   10, 0x08,
      84,   10,   10,   10, 0x08,
     101,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SpiceFile[] = {
    "SpiceFile\0\0slotGetNetlist()\0slotGetError()\0"
    "slotExited()\0slotSkipOut()\0slotSkipErr()\0"
    "slotGetPrepOut()\0slotGetPrepErr()\0"
};

void SpiceFile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpiceFile *_t = static_cast<SpiceFile *>(_o);
        switch (_id) {
        case 0: _t->slotGetNetlist(); break;
        case 1: _t->slotGetError(); break;
        case 2: _t->slotExited(); break;
        case 3: _t->slotSkipOut(); break;
        case 4: _t->slotSkipErr(); break;
        case 5: _t->slotGetPrepOut(); break;
        case 6: _t->slotGetPrepErr(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SpiceFile::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpiceFile::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SpiceFile,
      qt_meta_data_SpiceFile, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpiceFile::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpiceFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpiceFile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpiceFile))
        return static_cast<void*>(const_cast< SpiceFile*>(this));
    if (!strcmp(_clname, "MultiViewComponent"))
        return static_cast< MultiViewComponent*>(const_cast< SpiceFile*>(this));
    return QObject::qt_metacast(_clname);
}

int SpiceFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
