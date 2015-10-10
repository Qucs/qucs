/****************************************************************************
** Meta object code from reading C++ file 'exportdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "exportdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'exportdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExportDialog[] = {

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
      14,   13,   13,   13, 0x08,
      28,   13,   13,   13, 0x08,
      40,   13,   13,   13, 0x08,
      53,   13,   13,   13, 0x08,
      67,   13,   13,   13, 0x08,
      98,   89,   13,   13, 0x08,
     114,   13,   13,   13, 0x08,
     130,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ExportDialog[] = {
    "ExportDialog\0\0setFileName()\0calcWidth()\0"
    "calcHeight()\0recalcRatio()\0"
    "restoreOriginalWtoH()\0filename\0"
    "setSvg(QString)\0setSelectedWH()\0"
    "recalcScale()\0"
};

void ExportDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ExportDialog *_t = static_cast<ExportDialog *>(_o);
        switch (_id) {
        case 0: _t->setFileName(); break;
        case 1: _t->calcWidth(); break;
        case 2: _t->calcHeight(); break;
        case 3: _t->recalcRatio(); break;
        case 4: _t->restoreOriginalWtoH(); break;
        case 5: _t->setSvg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->setSelectedWH(); break;
        case 7: _t->recalcScale(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ExportDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ExportDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ExportDialog,
      qt_meta_data_ExportDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ExportDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ExportDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ExportDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExportDialog))
        return static_cast<void*>(const_cast< ExportDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ExportDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
