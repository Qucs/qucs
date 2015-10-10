/****************************************************************************
** Meta object code from reading C++ file 'componentdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "componentdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'componentdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ComponentDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      30,   16,   16,   16, 0x08,
      52,   47,   16,   16, 0x08,
      88,   16,   16,   16, 0x08,
     111,  105,   16,   16, 0x08,
     131,   16,   16,   16, 0x08,
     148,   16,   16,   16, 0x08,
     168,  163,   16,   16, 0x08,
     193,   16,   16,   16, 0x08,
     213,   16,   16,   16, 0x08,
     233,   16,   16,   16, 0x08,
     247,   16,   16,   16, 0x08,
     261,   16,   16,   16, 0x08,
     284,   16,   16,   16, 0x08,
     311,   16,   16,   16, 0x08,
     336,   16,   16,   16, 0x08,
     355,   16,   16,   16, 0x08,
     375,   16,   16,   16, 0x08,
     395,   16,   16,   16, 0x08,
     414,   16,   16,   16, 0x08,
     432,   16,   16,   16, 0x08,
     450,   16,   16,   16, 0x08,
     470,   16,   16,   16, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ComponentDialog[] = {
    "ComponentDialog\0\0slotButtOK()\0"
    "slotButtCancel()\0item\0"
    "slotSelectProperty(Q3ListViewItem*)\0"
    "slotApplyInput()\0State\0slotApplyState(int)\0"
    "slotBrowseFile()\0slotEditFile()\0Text\0"
    "slotApplyChange(QString)\0slotApplyProperty()\0"
    "slotApplyPropName()\0slotButtAdd()\0"
    "slotButtRem()\0slotSimTypeChange(int)\0"
    "slotNumberChanged(QString)\0"
    "slotStepChanged(QString)\0slotParamEntered()\0"
    "slotSimEntered(int)\0slotValuesEntered()\0"
    "slotStartEntered()\0slotStopEntered()\0"
    "slotStepEntered()\0slotNumberEntered()\0"
    "reject()\0"
};

void ComponentDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ComponentDialog *_t = static_cast<ComponentDialog *>(_o);
        switch (_id) {
        case 0: _t->slotButtOK(); break;
        case 1: _t->slotButtCancel(); break;
        case 2: _t->slotSelectProperty((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 3: _t->slotApplyInput(); break;
        case 4: _t->slotApplyState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->slotBrowseFile(); break;
        case 6: _t->slotEditFile(); break;
        case 7: _t->slotApplyChange((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->slotApplyProperty(); break;
        case 9: _t->slotApplyPropName(); break;
        case 10: _t->slotButtAdd(); break;
        case 11: _t->slotButtRem(); break;
        case 12: _t->slotSimTypeChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->slotNumberChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->slotStepChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->slotParamEntered(); break;
        case 16: _t->slotSimEntered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->slotValuesEntered(); break;
        case 18: _t->slotStartEntered(); break;
        case 19: _t->slotStopEntered(); break;
        case 20: _t->slotStepEntered(); break;
        case 21: _t->slotNumberEntered(); break;
        case 22: _t->reject(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ComponentDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ComponentDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ComponentDialog,
      qt_meta_data_ComponentDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ComponentDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ComponentDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ComponentDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ComponentDialog))
        return static_cast<void*>(const_cast< ComponentDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ComponentDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
