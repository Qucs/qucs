/****************************************************************************
** Meta object code from reading C++ file 'librarydialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "librarydialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'librarydialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LibraryDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      32,   14,   14,   14, 0x08,
      43,   14,   14,   14, 0x08,
      60,   14,   14,   14, 0x08,
      76,   14,   14,   14, 0x08,
     103,   14,   14,   14, 0x08,
     119,   14,   14,   14, 0x08,
     135,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LibraryDialog[] = {
    "LibraryDialog\0\0slotCreateNext()\0"
    "slotSave()\0slotSelectNone()\0slotSelectAll()\0"
    "slotCheckDescrChanged(int)\0slotPrevDescr()\0"
    "slotNextDescr()\0slotUpdateDescription()\0"
};

void LibraryDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LibraryDialog *_t = static_cast<LibraryDialog *>(_o);
        switch (_id) {
        case 0: _t->slotCreateNext(); break;
        case 1: _t->slotSave(); break;
        case 2: _t->slotSelectNone(); break;
        case 3: _t->slotSelectAll(); break;
        case 4: _t->slotCheckDescrChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->slotPrevDescr(); break;
        case 6: _t->slotNextDescr(); break;
        case 7: _t->slotUpdateDescription(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LibraryDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LibraryDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LibraryDialog,
      qt_meta_data_LibraryDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LibraryDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LibraryDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LibraryDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LibraryDialog))
        return static_cast<void*>(const_cast< LibraryDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int LibraryDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
