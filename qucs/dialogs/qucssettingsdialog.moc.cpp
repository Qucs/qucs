/****************************************************************************
** Meta object code from reading C++ file 'qucssettingsdialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qucssettingsdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qucssettingsdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QucsSettingsDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      29,   19,   19,   19, 0x08,
      41,   19,   19,   19, 0x08,
      58,   19,   19,   19, 0x08,
      78,   19,   19,   19, 0x08,
      98,   19,   19,   19, 0x08,
     116,   19,   19,   19, 0x08,
     137,   19,   19,   19, 0x08,
     156,   19,   19,   19, 0x08,
     174,   19,   19,   19, 0x08,
     193,   19,   19,   19, 0x08,
     209,   19,   19,   19, 0x08,
     230,   19,   19,   19, 0x08,
     250,   19,   19,   19, 0x08,
     271,   19,   19,   19, 0x08,
     292,   19,   19,   19, 0x08,
     310,  308,   19,   19, 0x08,
     336,  308,   19,   19, 0x08,
     366,   19,   19,   19, 0x08,
     386,   19,   19,   19, 0x08,
     409,   19,   19,   19, 0x08,
     429,   19,   19,   19, 0x08,
     451,   19,   19,   19, 0x08,
     465,   19,   19,   19, 0x08,
     493,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QucsSettingsDialog[] = {
    "QucsSettingsDialog\0\0slotOK()\0slotApply()\0"
    "slotFontDialog()\0slotBGColorDialog()\0"
    "slotDefaultValues()\0slotAddFileType()\0"
    "slotRemoveFileType()\0slotColorComment()\0"
    "slotColorString()\0slotColorInteger()\0"
    "slotColorReal()\0slotColorCharacter()\0"
    "slotColorDataType()\0slotColorAttribute()\0"
    "slotColorDirective()\0slotColorTask()\0"
    ",\0slotTableClicked(int,int)\0"
    "slotPathTableClicked(int,int)\0"
    "slotHomeDirBrowse()\0slotAdmsXmlDirBrowse()\0"
    "slotAscoDirBrowse()\0slotOctaveDirBrowse()\0"
    "slotAddPath()\0slotAddPathWithSubFolders()\0"
    "slotRemovePath()\0"
};

void QucsSettingsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QucsSettingsDialog *_t = static_cast<QucsSettingsDialog *>(_o);
        switch (_id) {
        case 0: _t->slotOK(); break;
        case 1: _t->slotApply(); break;
        case 2: _t->slotFontDialog(); break;
        case 3: _t->slotBGColorDialog(); break;
        case 4: _t->slotDefaultValues(); break;
        case 5: _t->slotAddFileType(); break;
        case 6: _t->slotRemoveFileType(); break;
        case 7: _t->slotColorComment(); break;
        case 8: _t->slotColorString(); break;
        case 9: _t->slotColorInteger(); break;
        case 10: _t->slotColorReal(); break;
        case 11: _t->slotColorCharacter(); break;
        case 12: _t->slotColorDataType(); break;
        case 13: _t->slotColorAttribute(); break;
        case 14: _t->slotColorDirective(); break;
        case 15: _t->slotColorTask(); break;
        case 16: _t->slotTableClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->slotPathTableClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 18: _t->slotHomeDirBrowse(); break;
        case 19: _t->slotAdmsXmlDirBrowse(); break;
        case 20: _t->slotAscoDirBrowse(); break;
        case 21: _t->slotOctaveDirBrowse(); break;
        case 22: _t->slotAddPath(); break;
        case 23: _t->slotAddPathWithSubFolders(); break;
        case 24: _t->slotRemovePath(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QucsSettingsDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QucsSettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QucsSettingsDialog,
      qt_meta_data_QucsSettingsDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QucsSettingsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QucsSettingsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QucsSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QucsSettingsDialog))
        return static_cast<void*>(const_cast< QucsSettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QucsSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
