/****************************************************************************
** Meta object code from reading C++ file 'qglglslwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qglglslwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qglglslwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGLGLSLWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      34,   14,   14,   14, 0x05,
      53,   14,   14,   14, 0x05,
      72,   14,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QGLGLSLWindow[] = {
    "QGLGLSLWindow\0\0vtxstatus(QString)\0"
    "frgstatus(QString)\0lnkstatus(QString)\0"
    "wrnstatus(QString)\0"
};

void QGLGLSLWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QGLGLSLWindow *_t = static_cast<QGLGLSLWindow *>(_o);
        switch (_id) {
        case 0: _t->vtxstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->frgstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->lnkstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->wrnstatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QGLGLSLWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QGLGLSLWindow::staticMetaObject = {
    { &lgl_Qt_GLWidget::staticMetaObject, qt_meta_stringdata_QGLGLSLWindow,
      qt_meta_data_QGLGLSLWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QGLGLSLWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QGLGLSLWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QGLGLSLWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGLGLSLWindow))
        return static_cast<void*>(const_cast< QGLGLSLWindow*>(this));
    return lgl_Qt_GLWidget::qt_metacast(_clname);
}

int QGLGLSLWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = lgl_Qt_GLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QGLGLSLWindow::vtxstatus(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QGLGLSLWindow::frgstatus(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QGLGLSLWindow::lnkstatus(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QGLGLSLWindow::wrnstatus(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
