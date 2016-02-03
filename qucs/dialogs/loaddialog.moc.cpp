/****************************************************************************
** Meta object code from reading C++ file 'loaddialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "loaddialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'loaddialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LoadDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      28,   11,   11,   11, 0x08,
      50,   45,   11,   11, 0x08,
      90,   11,   11,   11, 0x09,
      99,   11,   11,   11, 0x09,
     114,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_LoadDialog[] = {
    "LoadDialog\0\0slotSelectAll()\0"
    "slotSelectNone()\0item\0"
    "slotSymbolFileClicked(QListWidgetItem*)\0"
    "reject()\0loadSelected()\0slotChangeIcon()\0"
};

void LoadDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LoadDialog *_t = static_cast<LoadDialog *>(_o);
        switch (_id) {
        case 0: _t->slotSelectAll(); break;
        case 1: _t->slotSelectNone(); break;
        case 2: _t->slotSymbolFileClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->reject(); break;
        case 4: _t->loadSelected(); break;
        case 5: _t->slotChangeIcon(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LoadDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LoadDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LoadDialog,
      qt_meta_data_LoadDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LoadDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LoadDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LoadDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LoadDialog))
        return static_cast<void*>(const_cast< LoadDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int LoadDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
