/****************************************************************************
** Meta object code from reading C++ file 'glsleditor.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "glsleditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glsleditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GLSLTextEdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_GLSLTextEdit[] = {
    "GLSLTextEdit\0"
};

void GLSLTextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData GLSLTextEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GLSLTextEdit::staticMetaObject = {
    { &CodeEditor::staticMetaObject, qt_meta_stringdata_GLSLTextEdit,
      qt_meta_data_GLSLTextEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GLSLTextEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GLSLTextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GLSLTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GLSLTextEdit))
        return static_cast<void*>(const_cast< GLSLTextEdit*>(this));
    return CodeEditor::qt_metacast(_clname);
}

int GLSLTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CodeEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_GLSLProgramEditorWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x09,
      35,   24,   24,   24, 0x09,
      51,   24,   24,   24, 0x09,
      70,   24,   24,   24, 0x09,
      89,   24,   24,   24, 0x09,
     108,   24,   24,   24, 0x09,
     127,   24,   24,   24, 0x09,
     137,   24,   24,   24, 0x09,
     147,   24,   24,   24, 0x09,
     160,   24,   24,   24, 0x09,
     168,   24,   24,   24, 0x09,
     180,   24,   24,   24, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GLSLProgramEditorWindow[] = {
    "GLSLProgramEditorWindow\0\0loadObj()\0"
    "reloadShaders()\0vtxstatus(QString)\0"
    "frgstatus(QString)\0lnkstatus(QString)\0"
    "wrnstatus(QString)\0vtxLoad()\0frgLoad()\0"
    "background()\0ztest()\0wireframe()\0"
    "help()\0"
};

void GLSLProgramEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GLSLProgramEditorWindow *_t = static_cast<GLSLProgramEditorWindow *>(_o);
        switch (_id) {
        case 0: _t->loadObj(); break;
        case 1: _t->reloadShaders(); break;
        case 2: _t->vtxstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->frgstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->lnkstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->wrnstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->vtxLoad(); break;
        case 7: _t->frgLoad(); break;
        case 8: _t->background(); break;
        case 9: _t->ztest(); break;
        case 10: _t->wireframe(); break;
        case 11: _t->help(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GLSLProgramEditorWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GLSLProgramEditorWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GLSLProgramEditorWindow,
      qt_meta_data_GLSLProgramEditorWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GLSLProgramEditorWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GLSLProgramEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GLSLProgramEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GLSLProgramEditorWindow))
        return static_cast<void*>(const_cast< GLSLProgramEditorWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int GLSLProgramEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
