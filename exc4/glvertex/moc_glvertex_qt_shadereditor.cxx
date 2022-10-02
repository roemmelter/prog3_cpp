/****************************************************************************
** Meta object code from reading C++ file 'glvertex_qt_shadereditor.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "glvertex_qt_shadereditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glvertex_qt_shadereditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_lgl_Qt_ShaderEditor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x09,
      28,   20,   20,   20, 0x09,
      35,   20,   20,   20, 0x09,
      42,   20,   20,   20, 0x09,
      51,   20,   20,   20, 0x09,
      65,   20,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_lgl_Qt_ShaderEditor[] = {
    "lgl_Qt_ShaderEditor\0\0quit()\0load()\0"
    "save()\0revert()\0textChanged()\0help()\0"
};

void lgl_Qt_ShaderEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        lgl_Qt_ShaderEditor *_t = static_cast<lgl_Qt_ShaderEditor *>(_o);
        switch (_id) {
        case 0: _t->quit(); break;
        case 1: _t->load(); break;
        case 2: _t->save(); break;
        case 3: _t->revert(); break;
        case 4: _t->textChanged(); break;
        case 5: _t->help(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData lgl_Qt_ShaderEditor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject lgl_Qt_ShaderEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_lgl_Qt_ShaderEditor,
      qt_meta_data_lgl_Qt_ShaderEditor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &lgl_Qt_ShaderEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *lgl_Qt_ShaderEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *lgl_Qt_ShaderEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_lgl_Qt_ShaderEditor))
        return static_cast<void*>(const_cast< lgl_Qt_ShaderEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int lgl_Qt_ShaderEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
