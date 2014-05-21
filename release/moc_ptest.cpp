/****************************************************************************
** Meta object code from reading C++ file 'ptest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ptest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ptest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PTest_t {
    QByteArrayData data[19];
    char stringdata[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PTest_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PTest_t qt_meta_stringdata_PTest = {
    {
QT_MOC_LITERAL(0, 0, 5),
QT_MOC_LITERAL(1, 6, 11),
QT_MOC_LITERAL(2, 18, 0),
QT_MOC_LITERAL(3, 19, 7),
QT_MOC_LITERAL(4, 27, 9),
QT_MOC_LITERAL(5, 37, 9),
QT_MOC_LITERAL(6, 47, 12),
QT_MOC_LITERAL(7, 60, 3),
QT_MOC_LITERAL(8, 64, 3),
QT_MOC_LITERAL(9, 68, 5),
QT_MOC_LITERAL(10, 74, 3),
QT_MOC_LITERAL(11, 78, 10),
QT_MOC_LITERAL(12, 89, 11),
QT_MOC_LITERAL(13, 101, 4),
QT_MOC_LITERAL(14, 106, 4),
QT_MOC_LITERAL(15, 111, 4),
QT_MOC_LITERAL(16, 116, 4),
QT_MOC_LITERAL(17, 121, 5),
QT_MOC_LITERAL(18, 127, 10)
    },
    "PTest\0sendCommand\0\0command\0Argument1\0"
    "Argument2\0updateStatus\0apu\0lvs\0spoil\0"
    "oes\0testSystem\0statusVideo\0cam1\0cam2\0"
    "cam3\0cam4\0vizir\0teplovizor\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PTest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x05,
       6,    4,   41,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
      11,    0,   50,    2, 0x0a,
      12,    6,   51,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,    7,    8,    9,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,   13,   14,   15,   16,   17,   18,

       0        // eod
};

void PTest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PTest *_t = static_cast<PTest *>(_o);
        switch (_id) {
        case 0: _t->sendCommand((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->updateStatus((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 2: _t->testSystem(); break;
        case 3: _t->statusVideo((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PTest::*_t)(int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PTest::sendCommand)) {
                *result = 0;
            }
        }
        {
            typedef void (PTest::*_t)(bool , bool , bool , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PTest::updateStatus)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject PTest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PTest.data,
      qt_meta_data_PTest,  qt_static_metacall, 0, 0}
};


const QMetaObject *PTest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PTest::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PTest.stringdata))
        return static_cast<void*>(const_cast< PTest*>(this));
    return QObject::qt_metacast(_clname);
}

int PTest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void PTest::sendCommand(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PTest::updateStatus(bool _t1, bool _t2, bool _t3, bool _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
