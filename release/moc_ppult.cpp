/****************************************************************************
** Meta object code from reading C++ file 'ppult.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ppult.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ppult.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PPult_t {
    QByteArrayData data[25];
    char stringdata[203];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PPult_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PPult_t qt_meta_stringdata_PPult = {
    {
QT_MOC_LITERAL(0, 0, 5),
QT_MOC_LITERAL(1, 6, 13),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 12),
QT_MOC_LITERAL(4, 34, 6),
QT_MOC_LITERAL(5, 41, 3),
QT_MOC_LITERAL(6, 45, 5),
QT_MOC_LITERAL(7, 51, 7),
QT_MOC_LITERAL(8, 59, 6),
QT_MOC_LITERAL(9, 66, 5),
QT_MOC_LITERAL(10, 72, 11),
QT_MOC_LITERAL(11, 84, 7),
QT_MOC_LITERAL(12, 92, 9),
QT_MOC_LITERAL(13, 102, 9),
QT_MOC_LITERAL(14, 112, 5),
QT_MOC_LITERAL(15, 118, 11),
QT_MOC_LITERAL(16, 130, 14),
QT_MOC_LITERAL(17, 145, 8),
QT_MOC_LITERAL(18, 154, 12),
QT_MOC_LITERAL(19, 167, 3),
QT_MOC_LITERAL(20, 171, 3),
QT_MOC_LITERAL(21, 175, 5),
QT_MOC_LITERAL(22, 181, 3),
QT_MOC_LITERAL(23, 185, 8),
QT_MOC_LITERAL(24, 194, 7)
    },
    "PPult\0dalnomerStart\0\0dalnomerStop\0"
    "newOut\0num\0state\0setIndS\0UINT64\0value\0"
    "sendCommand\0command\0Argument1\0Argument2\0"
    "newIn\0sendAbonent\0flashTimerFunc\0"
    "newState\0updateStatus\0apu\0lvs\0spoil\0"
    "oes\0newPolog\0timeDal\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PPult[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x05,
       3,    0,   75,    2, 0x05,
       4,    2,   76,    2, 0x05,
       7,    1,   81,    2, 0x05,
      10,    3,   84,    2, 0x05,
      14,    2,   91,    2, 0x05,
      15,    2,   96,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
      16,    0,  101,    2, 0x0a,
      17,    2,  102,    2, 0x0a,
      18,    4,  107,    2, 0x0a,
      23,    1,  116,    2, 0x0a,
      24,    0,  119,    2, 0x0a,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   11,   12,   13,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    5,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,   19,   20,   21,   22,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,

       0        // eod
};

void PPult::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PPult *_t = static_cast<PPult *>(_o);
        switch (_id) {
        case 0: _t->dalnomerStart(); break;
        case 1: _t->dalnomerStop(); break;
        case 2: _t->newOut((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->setIndS((*reinterpret_cast< UINT64(*)>(_a[1]))); break;
        case 4: _t->sendCommand((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->newIn((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->sendAbonent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->flashTimerFunc(); break;
        case 8: _t->newState((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->updateStatus((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 10: _t->newPolog((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->timeDal(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPult::dalnomerStart)) {
                *result = 0;
            }
        }
        {
            typedef void (PPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPult::dalnomerStop)) {
                *result = 1;
            }
        }
        {
            typedef void (PPult::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPult::newOut)) {
                *result = 2;
            }
        }
        {
            typedef void (PPult::*_t)(UINT64 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPult::setIndS)) {
                *result = 3;
            }
        }
        {
            typedef void (PPult::*_t)(int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPult::sendCommand)) {
                *result = 4;
            }
        }
        {
            typedef void (PPult::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPult::newIn)) {
                *result = 5;
            }
        }
        {
            typedef void (PPult::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPult::sendAbonent)) {
                *result = 6;
            }
        }
    }
}

const QMetaObject PPult::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PPult.data,
      qt_meta_data_PPult,  qt_static_metacall, 0, 0}
};


const QMetaObject *PPult::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PPult::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PPult.stringdata))
        return static_cast<void*>(const_cast< PPult*>(this));
    return QObject::qt_metacast(_clname);
}

int PPult::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void PPult::dalnomerStart()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void PPult::dalnomerStop()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void PPult::newOut(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PPult::setIndS(UINT64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PPult::sendCommand(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void PPult::newIn(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void PPult::sendAbonent(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
