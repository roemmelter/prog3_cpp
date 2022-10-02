/****************************************************************************
** Meta object code from reading C++ file 'glvertex_qt_codeeditor.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "glvertex_qt_codeeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glvertex_qt_codeeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_lgl_Qt_CodeEditor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      33,   19,   18,   18, 0x08,
      72,   64,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_lgl_Qt_CodeEditor[] = {
    "lgl_Qt_CodeEditor\0\0newBlockCount\0"
    "updateLineNumberAreaWidth(int)\0rect,dy\0"
    "updateLineNumberArea(QRect,int)\0"
};

void lgl_Qt_CodeEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        lgl_Qt_CodeEditor *_t = static_cast<lgl_Qt_CodeEditor *>(_o);
        switch (_id) {
        case 0: _t->updateLineNumberAreaWidth((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateLineNumberArea((*reinterpret_cast< const QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData lgl_Qt_CodeEditor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject lgl_Qt_CodeEditor::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_lgl_Qt_CodeEditor,
      qt_meta_data_lgl_Qt_CodeEditor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &lgl_Qt_CodeEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *lgl_Qt_CodeEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *lgl_Qt_CodeEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_lgl_Qt_CodeEditor))
        return static_cast<void*>(const_cast< lgl_Qt_CodeEditor*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int lgl_Qt_CodeEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
