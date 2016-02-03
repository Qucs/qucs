/****************************************************************************
** Meta object code from reading C++ file 'importdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "importdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'importdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ImportDialog[] = {

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
      14,   13,   13,   13, 0x08,
      31,   13,   13,   13, 0x08,
      55,   48,   13,   13, 0x08,
      77,   13,   13,   13, 0x08,
      90,   13,   13,   13, 0x08,
     102,   13,   13,   13, 0x08,
     115,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ImportDialog[] = {
    "ImportDialog\0\0slotDisplayMsg()\0"
    "slotDisplayErr()\0status\0slotProcessEnded(int)\0"
    "slotImport()\0slotAbort()\0slotBrowse()\0"
    "slotType(int)\0"
};

void ImportDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ImportDialog *_t = static_cast<ImportDialog *>(_o);
        switch (_id) {
        case 0: _t->slotDisplayMsg(); break;
        case 1: _t->slotDisplayErr(); break;
        case 2: _t->slotProcessEnded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->slotImport(); break;
        case 4: _t->slotAbort(); break;
        case 5: _t->slotBrowse(); break;
        case 6: _t->slotType((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ImportDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ImportDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ImportDialog,
      qt_meta_data_ImportDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ImportDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ImportDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ImportDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImportDialog))
        return static_cast<void*>(const_cast< ImportDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ImportDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
