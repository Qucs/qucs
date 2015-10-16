/****************************************************************************
** Meta object code from reading C++ file 'vtabwidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vtabwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vtabwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VTabWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      31,   11,   11,   11, 0x05,
      53,   51,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      79,   77,   11,   11, 0x0a,
     108,  105,   11,   11, 0x0a,
     128,   51,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VTabWidget[] = {
    "VTabWidget\0\0widgetStackShown()\0"
    "widgetStackHidden()\0w\0activatedPage(QWidget*)\0"
    "p\0setPosition(VTabPosition)\0id\0"
    "setCurrentPage(int)\0setCurrentPage(QWidget*)\0"
};

void VTabWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VTabWidget *_t = static_cast<VTabWidget *>(_o);
        switch (_id) {
        case 0: _t->widgetStackShown(); break;
        case 1: _t->widgetStackHidden(); break;
        case 2: _t->activatedPage((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 3: _t->setPosition((*reinterpret_cast< VTabPosition(*)>(_a[1]))); break;
        case 4: _t->setCurrentPage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setCurrentPage((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VTabWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VTabWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VTabWidget,
      qt_meta_data_VTabWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VTabWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VTabWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VTabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VTabWidget))
        return static_cast<void*>(const_cast< VTabWidget*>(this));
    if (!strcmp(_clname, "VTabInterface"))
        return static_cast< VTabInterface*>(const_cast< VTabWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int VTabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void VTabWidget::widgetStackShown()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void VTabWidget::widgetStackHidden()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void VTabWidget::activatedPage(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
