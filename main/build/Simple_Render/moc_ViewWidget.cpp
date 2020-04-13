/****************************************************************************
** Meta object code from reading C++ file 'ViewWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Simple_Render/ViewWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ViewWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ViewWidget_t {
    QByteArrayData data[20];
    char stringdata0[193];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewWidget_t qt_meta_stringdata_ViewWidget = {
    {
QT_MOC_LITERAL(0, 0, 10), // "ViewWidget"
QT_MOC_LITERAL(1, 11, 11), // "KposChanged"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "newK_pos"
QT_MOC_LITERAL(4, 33, 12), // "focalChanged"
QT_MOC_LITERAL(5, 46, 5), // "focus"
QT_MOC_LITERAL(6, 52, 15), // "apertureChanged"
QT_MOC_LITERAL(7, 68, 8), // "aperture"
QT_MOC_LITERAL(8, 77, 10), // "fovChanged"
QT_MOC_LITERAL(9, 88, 3), // "fov"
QT_MOC_LITERAL(10, 92, 17), // "renderTimeUpdated"
QT_MOC_LITERAL(11, 110, 10), // "renderTime"
QT_MOC_LITERAL(12, 121, 7), // "animate"
QT_MOC_LITERAL(13, 129, 7), // "setKpos"
QT_MOC_LITERAL(14, 137, 8), // "setFocal"
QT_MOC_LITERAL(15, 146, 8), // "newFocus"
QT_MOC_LITERAL(16, 155, 11), // "setAperture"
QT_MOC_LITERAL(17, 167, 11), // "newAperture"
QT_MOC_LITERAL(18, 179, 6), // "setFov"
QT_MOC_LITERAL(19, 186, 6) // "newFov"

    },
    "ViewWidget\0KposChanged\0\0newK_pos\0"
    "focalChanged\0focus\0apertureChanged\0"
    "aperture\0fovChanged\0fov\0renderTimeUpdated\0"
    "renderTime\0animate\0setKpos\0setFocal\0"
    "newFocus\0setAperture\0newAperture\0"
    "setFov\0newFov"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    1,   67,    2, 0x06 /* Public */,
       6,    1,   70,    2, 0x06 /* Public */,
       8,    1,   73,    2, 0x06 /* Public */,
      10,    1,   76,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    0,   79,    2, 0x0a /* Public */,
      13,    1,   80,    2, 0x0a /* Public */,
      14,    1,   83,    2, 0x0a /* Public */,
      16,    1,   86,    2, 0x0a /* Public */,
      18,    1,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QVector3D,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Int,   11,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVector3D,    3,
    QMetaType::Void, QMetaType::Double,   15,
    QMetaType::Void, QMetaType::Double,   17,
    QMetaType::Void, QMetaType::Double,   19,

       0        // eod
};

void ViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ViewWidget *_t = static_cast<ViewWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->KposChanged((*reinterpret_cast< QVector3D(*)>(_a[1]))); break;
        case 1: _t->focalChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->apertureChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->fovChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->renderTimeUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->animate(); break;
        case 6: _t->setKpos((*reinterpret_cast< QVector3D(*)>(_a[1]))); break;
        case 7: _t->setFocal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->setAperture((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->setFov((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ViewWidget::*_t)(QVector3D );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewWidget::KposChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ViewWidget::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewWidget::focalChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ViewWidget::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewWidget::apertureChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ViewWidget::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewWidget::fovChanged)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (ViewWidget::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewWidget::renderTimeUpdated)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject ViewWidget::staticMetaObject = {
    { &QOpenGLWidget::staticMetaObject, qt_meta_stringdata_ViewWidget.data,
      qt_meta_data_ViewWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ViewWidget.stringdata0))
        return static_cast<void*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int ViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void ViewWidget::KposChanged(QVector3D _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ViewWidget::focalChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ViewWidget::apertureChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ViewWidget::fovChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ViewWidget::renderTimeUpdated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
