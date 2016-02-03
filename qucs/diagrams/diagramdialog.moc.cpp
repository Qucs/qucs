/****************************************************************************
** Meta object code from reading C++ file 'diagramdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "diagramdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'diagramdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DiagramDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      38,   33,   14,   14, 0x08,
      69,   14,   14,   14, 0x08,
     103,   14,   14,   14, 0x08,
     118,   14,   14,   14, 0x08,
     136,   14,   14,   14, 0x08,
     145,   14,   14,   14, 0x08,
     157,   14,   14,   14, 0x08,
     170,   14,   14,   14, 0x08,
     185,   14,   14,   14, 0x08,
     204,   14,   14,   14, 0x08,
     229,   14,   14,   14, 0x08,
     251,   14,   14,   14, 0x08,
     271,   14,   14,   14, 0x08,
     291,   14,   14,   14, 0x08,
     314,   14,   14,   14, 0x08,
     332,   14,   14,   14, 0x08,
     349,   14,   14,   14, 0x08,
     366,   14,   14,   14, 0x08,
     383,   14,   14,   14, 0x08,
     407,   14,   14,   14, 0x08,
     424,   14,   14,   14, 0x08,
     441,   14,   14,   14, 0x08,
     458,   14,   14,   14, 0x08,
     480,   14,   14,   14, 0x08,
     502,   14,   14,   14, 0x08,
     524,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_DiagramDialog[] = {
    "DiagramDialog\0\0slotReadVars(int)\0item\0"
    "slotTakeVar(QTableWidgetItem*)\0"
    "slotSelectGraph(QListWidgetItem*)\0"
    "slotNewGraph()\0slotDeleteGraph()\0"
    "slotOK()\0slotApply()\0slotCancel()\0"
    "slotSetColor()\0slotSetGridColor()\0"
    "slotResetToTake(QString)\0slotSetProp2(QString)\0"
    "slotSetNumMode(int)\0slotSetGridBox(int)\0"
    "slotSetGraphStyle(int)\0slotSetYAxis(int)\0"
    "slotManualX(int)\0slotManualY(int)\0"
    "slotManualZ(int)\0slotChangeTab(QWidget*)\0"
    "slotNewRotX(int)\0slotNewRotY(int)\0"
    "slotNewRotZ(int)\0slotEditRotX(QString)\0"
    "slotEditRotY(QString)\0slotEditRotZ(QString)\0"
    "reject()\0"
};

void DiagramDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DiagramDialog *_t = static_cast<DiagramDialog *>(_o);
        switch (_id) {
        case 0: _t->slotReadVars((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->slotTakeVar((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 2: _t->slotSelectGraph((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->slotNewGraph(); break;
        case 4: _t->slotDeleteGraph(); break;
        case 5: _t->slotOK(); break;
        case 6: _t->slotApply(); break;
        case 7: _t->slotCancel(); break;
        case 8: _t->slotSetColor(); break;
        case 9: _t->slotSetGridColor(); break;
        case 10: _t->slotResetToTake((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->slotSetProp2((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->slotSetNumMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->slotSetGridBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->slotSetGraphStyle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->slotSetYAxis((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->slotManualX((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->slotManualY((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->slotManualZ((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->slotChangeTab((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 20: _t->slotNewRotX((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->slotNewRotY((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->slotNewRotZ((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->slotEditRotX((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 24: _t->slotEditRotY((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 25: _t->slotEditRotZ((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 26: _t->reject(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DiagramDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DiagramDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DiagramDialog,
      qt_meta_data_DiagramDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DiagramDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DiagramDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DiagramDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DiagramDialog))
        return static_cast<void*>(const_cast< DiagramDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DiagramDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
