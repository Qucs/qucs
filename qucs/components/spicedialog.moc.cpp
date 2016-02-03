/****************************************************************************
** Meta object code from reading C++ file 'spicedialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "spicedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spicedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpiceDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      26,   12,   12,   12, 0x08,
      43,   12,   12,   12, 0x08,
      59,   12,   12,   12, 0x08,
      76,   12,   12,   12, 0x08,
      91,   12,   12,   12, 0x08,
     105,   12,   12,   12, 0x08,
     122,   12,   12,   12, 0x08,
     150,   12,   12,   12, 0x08,
     181,   12,   12,   12, 0x08,
     198,   12,   12,   12, 0x08,
     213,   12,   12,   12, 0x08,
     227,   12,   12,   12, 0x08,
     241,   12,   12,   12, 0x08,
     258,   12,   12,   12, 0x08,
     275,   12,   12,   12, 0x08,
     296,   12,   12,   12, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SpiceDialog[] = {
    "SpiceDialog\0\0slotButtOK()\0slotButtCancel()\0"
    "slotButtApply()\0slotButtBrowse()\0"
    "slotButtEdit()\0slotButtAdd()\0"
    "slotButtRemove()\0slotAddPort(Q3ListBoxItem*)\0"
    "slotRemovePort(Q3ListBoxItem*)\0"
    "slotGetNetlist()\0slotGetError()\0"
    "slotSkipOut()\0slotSkipErr()\0"
    "slotGetPrepOut()\0slotGetPrepErr()\0"
    "slotPrepChanged(int)\0reject()\0"
};

void SpiceDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpiceDialog *_t = static_cast<SpiceDialog *>(_o);
        switch (_id) {
        case 0: _t->slotButtOK(); break;
        case 1: _t->slotButtCancel(); break;
        case 2: _t->slotButtApply(); break;
        case 3: _t->slotButtBrowse(); break;
        case 4: _t->slotButtEdit(); break;
        case 5: _t->slotButtAdd(); break;
        case 6: _t->slotButtRemove(); break;
        case 7: _t->slotAddPort((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 8: _t->slotRemovePort((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 9: _t->slotGetNetlist(); break;
        case 10: _t->slotGetError(); break;
        case 11: _t->slotSkipOut(); break;
        case 12: _t->slotSkipErr(); break;
        case 13: _t->slotGetPrepOut(); break;
        case 14: _t->slotGetPrepErr(); break;
        case 15: _t->slotPrepChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->reject(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpiceDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpiceDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SpiceDialog,
      qt_meta_data_SpiceDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpiceDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpiceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpiceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpiceDialog))
        return static_cast<void*>(const_cast< SpiceDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int SpiceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
