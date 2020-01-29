/****************************************************************************
** Meta object code from reading C++ file 'LFViewWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Simple_Render/LFViewWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LFViewWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LFViewerWindow_t {
    QByteArrayData data[8];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LFViewerWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LFViewerWindow_t qt_meta_stringdata_LFViewerWindow = {
    {
QT_MOC_LITERAL(0, 0, 14), // "LFViewerWindow"
QT_MOC_LITERAL(1, 15, 11), // "KposChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 7), // "newKPos"
QT_MOC_LITERAL(4, 36, 17), // "renderTimeUpdated"
QT_MOC_LITERAL(5, 54, 10), // "renderTime"
QT_MOC_LITERAL(6, 65, 10), // "fovUpdated"
QT_MOC_LITERAL(7, 76, 3) // "fov"

    },
    "LFViewerWindow\0KposChanged\0\0newKPos\0"
    "renderTimeUpdated\0renderTime\0fovUpdated\0"
    "fov"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LFViewerWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       4,    1,   32,    2, 0x0a /* Public */,
       6,    1,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QVector3D,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Double,    7,

       0        // eod
};

void LFViewerWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LFViewerWindow *_t = static_cast<LFViewerWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->KposChanged((*reinterpret_cast< QVector3D(*)>(_a[1]))); break;
        case 1: _t->renderTimeUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->fovUpdated((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject LFViewerWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_LFViewerWindow.data,
      qt_meta_data_LFViewerWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *LFViewerWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LFViewerWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LFViewerWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int LFViewerWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
