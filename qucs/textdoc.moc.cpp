/****************************************************************************
** Meta object code from reading C++ file 'textdoc.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "textdoc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textdoc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextDoc[] = {

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
       9,    8,    8,    8, 0x0a,
      32,    8,    8,    8, 0x0a,
      49,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TextDoc[] = {
    "TextDoc\0\0slotCursorPosChanged()\0"
    "slotSetChanged()\0highlightCurrentLine()\0"
};

void TextDoc::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextDoc *_t = static_cast<TextDoc *>(_o);
        switch (_id) {
        case 0: _t->slotCursorPosChanged(); break;
        case 1: _t->slotSetChanged(); break;
        case 2: _t->highlightCurrentLine(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TextDoc::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextDoc::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_TextDoc,
      qt_meta_data_TextDoc, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextDoc::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextDoc::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextDoc::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextDoc))
        return static_cast<void*>(const_cast< TextDoc*>(this));
    if (!strcmp(_clname, "QucsDoc"))
        return static_cast< QucsDoc*>(const_cast< TextDoc*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int TextDoc::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
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
