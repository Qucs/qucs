/****************************************************************************
** Meta object code from reading C++ file 'optimizedialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "optimizedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'optimizedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OptimizeDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      25,   15,   15,   15, 0x08,
      37,   15,   15,   15, 0x08,
      50,   15,   15,   15, 0x08,
      68,   15,   15,   15, 0x08,
      89,   15,   15,   15, 0x08,
     103,   15,   15,   15, 0x08,
     120,   15,   15,   15, 0x08,
     150,   15,   15,   15, 0x08,
     184,   15,   15,   15, 0x08,
     211,   15,   15,   15, 0x08,
     237,   15,   15,   15, 0x08,
     264,   15,   15,   15, 0x08,
     290,   15,   15,   15, 0x08,
     316,   15,   15,   15, 0x08,
     343,   15,   15,   15, 0x08,
     371,   15,   15,   15, 0x08,
     398,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_OptimizeDialog[] = {
    "OptimizeDialog\0\0slotOK()\0slotApply()\0"
    "slotCancel()\0slotAddVariable()\0"
    "slotDeleteVariable()\0slotAddGoal()\0"
    "slotDeleteGoal()\0slotEditGoal(Q3ListViewItem*)\0"
    "slotEditVariable(Q3ListViewItem*)\0"
    "slotChangeVarName(QString)\0"
    "slotChangeVarActive(bool)\0"
    "slotChangeVarInit(QString)\0"
    "slotChangeVarMin(QString)\0"
    "slotChangeVarMax(QString)\0"
    "slotChangeVarType(QString)\0"
    "slotChangeGoalName(QString)\0"
    "slotChangeGoalNum(QString)\0"
    "slotChangeGoalType(QString)\0"
};

void OptimizeDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        OptimizeDialog *_t = static_cast<OptimizeDialog *>(_o);
        switch (_id) {
        case 0: _t->slotOK(); break;
        case 1: _t->slotApply(); break;
        case 2: _t->slotCancel(); break;
        case 3: _t->slotAddVariable(); break;
        case 4: _t->slotDeleteVariable(); break;
        case 5: _t->slotAddGoal(); break;
        case 6: _t->slotDeleteGoal(); break;
        case 7: _t->slotEditGoal((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 8: _t->slotEditVariable((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 9: _t->slotChangeVarName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->slotChangeVarActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->slotChangeVarInit((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->slotChangeVarMin((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->slotChangeVarMax((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->slotChangeVarType((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->slotChangeGoalName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->slotChangeGoalNum((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->slotChangeGoalType((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData OptimizeDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject OptimizeDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_OptimizeDialog,
      qt_meta_data_OptimizeDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OptimizeDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OptimizeDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OptimizeDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OptimizeDialog))
        return static_cast<void*>(const_cast< OptimizeDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int OptimizeDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
