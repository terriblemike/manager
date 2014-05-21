/****************************************************************************
** Meta object code from reading C++ file 'pjoystick.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../pjoystick.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pjoystick.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PJoystick_t {
    QByteArrayData data[8];
    char stringdata[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PJoystick_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PJoystick_t qt_meta_stringdata_PJoystick = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 8),
QT_MOC_LITERAL(2, 19, 0),
QT_MOC_LITERAL(3, 20, 5),
QT_MOC_LITERAL(4, 26, 7),
QT_MOC_LITERAL(5, 34, 1),
QT_MOC_LITERAL(6, 36, 1),
QT_MOC_LITERAL(7, 38, 10)
    },
    "PJoystick\0btnPress\0\0state\0newData\0x\0"
    "y\0updateData\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PJoystick[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x05,
       4,    2,   32,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       7,    0,   37,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void PJoystick::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PJoystick *_t = static_cast<PJoystick *>(_o);
        switch (_id) {
        case 0: _t->btnPress((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->newData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->updateData(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PJoystick::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PJoystick::btnPress)) {
                *result = 0;
            }
        }
        {
            typedef void (PJoystick::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PJoystick::newData)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject PJoystick::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PJoystick.data,
      qt_meta_data_PJoystick,  qt_static_metacall, 0, 0}
};


const QMetaObject *PJoystick::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PJoystick::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PJoystick.stringdata))
        return static_cast<void*>(const_cast< PJoystick*>(this));
    return QObject::qt_metacast(_clname);
}

int PJoystick::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void PJoystick::btnPress(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PJoystick::newData(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
