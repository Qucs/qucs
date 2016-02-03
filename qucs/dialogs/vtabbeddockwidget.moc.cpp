/****************************************************************************
** Meta object code from reading C++ file 'vtabbeddockwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vtabbeddockwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vtabbeddockwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VTabbedDockWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x08,
      36,   18,   18,   18, 0x08,
      56,   54,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VTabbedDockWidget[] = {
    "VTabbedDockWidget\0\0slotStackShown()\0"
    "slotStackHidden()\0p\0"
    "updatePosition(Q3DockWindow::Place)\0"
};

void VTabbedDockWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VTabbedDockWidget *_t = static_cast<VTabbedDockWidget *>(_o);
        switch (_id) {
        case 0: _t->slotStackShown(); break;
        case 1: _t->slotStackHidden(); break;
        case 2: _t->updatePosition((*reinterpret_cast< Q3DockWindow::Place(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VTabbedDockWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VTabbedDockWidget::staticMetaObject = {
    { &Q3DockWindow::staticMetaObject, qt_meta_stringdata_VTabbedDockWidget,
      qt_meta_data_VTabbedDockWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VTabbedDockWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VTabbedDockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VTabbedDockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VTabbedDockWidget))
        return static_cast<void*>(const_cast< VTabbedDockWidget*>(this));
    if (!strcmp(_clname, "VTabInterface"))
        return static_cast< VTabInterface*>(const_cast< VTabbedDockWidget*>(this));
    return Q3DockWindow::qt_metacast(_clname);
}

int VTabbedDockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3DockWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
