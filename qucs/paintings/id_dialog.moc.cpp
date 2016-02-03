/****************************************************************************
** Meta object code from reading C++ file 'id_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "id_dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'id_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ID_Dialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      20,   10,   10,   10, 0x08,
      39,   10,   10,   10, 0x08,
      61,   10,   10,   10, 0x08,
      96,   10,   10,   10, 0x08,
     117,   10,   10,   10, 0x08,
     142,   10,   10,   10, 0x08,
     168,   10,   10,   10, 0x08,
     194,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ID_Dialog[] = {
    "ID_Dialog\0\0slotOk()\0slotAddParameter()\0"
    "slotRemoveParameter()\0"
    "slotEditParameter(Q3ListViewItem*)\0"
    "slotToggleShow(bool)\0slotNameChanged(QString)\0"
    "slotValueChanged(QString)\0"
    "slotDescrChanged(QString)\0"
    "slotTypeChanged(QString)\0"
};

void ID_Dialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ID_Dialog *_t = static_cast<ID_Dialog *>(_o);
        switch (_id) {
        case 0: _t->slotOk(); break;
        case 1: _t->slotAddParameter(); break;
        case 2: _t->slotRemoveParameter(); break;
        case 3: _t->slotEditParameter((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 4: _t->slotToggleShow((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->slotNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->slotValueChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->slotDescrChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->slotTypeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ID_Dialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ID_Dialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ID_Dialog,
      qt_meta_data_ID_Dialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ID_Dialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ID_Dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ID_Dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ID_Dialog))
        return static_cast<void*>(const_cast< ID_Dialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ID_Dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
