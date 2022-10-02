/****************************************************************************
** Meta object code from reading C++ file 'arbeditor.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "arbeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'arbeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ARBTextEdit[] = {

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

static const char qt_meta_stringdata_ARBTextEdit[] = {
    "ARBTextEdit\0"
};

void ARBTextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ARBTextEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ARBTextEdit::staticMetaObject = {
    { &CodeEditor::staticMetaObject, qt_meta_stringdata_ARBTextEdit,
      qt_meta_data_ARBTextEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ARBTextEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ARBTextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ARBTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ARBTextEdit))
        return static_cast<void*>(const_cast< ARBTextEdit*>(this));
    return CodeEditor::qt_metacast(_clname);
}

int ARBTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CodeEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ARBProgramEditorWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x09,
      34,   23,   23,   23, 0x09,
      46,   23,   23,   23, 0x09,
      62,   23,   23,   23, 0x09,
      81,   23,   23,   23, 0x09,
     100,   23,   23,   23, 0x09,
     110,   23,   23,   23, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_ARBProgramEditorWindow[] = {
    "ARBProgramEditorWindow\0\0gfxCube()\0"
    "gfxTeapot()\0reloadShaders()\0"
    "vtxstatus(QString)\0frgstatus(QString)\0"
    "vtxLoad()\0frgLoad()\0"
};

void ARBProgramEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ARBProgramEditorWindow *_t = static_cast<ARBProgramEditorWindow *>(_o);
        switch (_id) {
        case 0: _t->gfxCube(); break;
        case 1: _t->gfxTeapot(); break;
        case 2: _t->reloadShaders(); break;
        case 3: _t->vtxstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->frgstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->vtxLoad(); break;
        case 6: _t->frgLoad(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ARBProgramEditorWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ARBProgramEditorWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ARBProgramEditorWindow,
      qt_meta_data_ARBProgramEditorWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ARBProgramEditorWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ARBProgramEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ARBProgramEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ARBProgramEditorWindow))
        return static_cast<void*>(const_cast< ARBProgramEditorWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int ARBProgramEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
