/****************************************************************************
** Meta object code from reading C++ file 'pstaticdio.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../pstaticdio.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pstaticdio.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PStaticDIO_t {
    QByteArrayData data[11];
    char stringdata[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PStaticDIO_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PStaticDIO_t qt_meta_stringdata_PStaticDIO = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 5),
QT_MOC_LITERAL(2, 17, 0),
QT_MOC_LITERAL(3, 18, 3),
QT_MOC_LITERAL(4, 22, 5),
QT_MOC_LITERAL(5, 28, 6),
QT_MOC_LITERAL(6, 35, 5),
QT_MOC_LITERAL(7, 41, 6),
QT_MOC_LITERAL(8, 48, 7),
QT_MOC_LITERAL(9, 56, 6),
QT_MOC_LITERAL(10, 63, 5)
    },
    "PStaticDIO\0newIn\0\0num\0state\0newOut\0"
    "getIn\0setOut\0setIndS\0UINT64\0value\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PStaticDIO[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x05,
       5,    2,   44,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       6,    0,   49,    2, 0x08,
       7,    2,   50,    2, 0x08,
       8,    1,   55,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    4,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void PStaticDIO::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PStaticDIO *_t = static_cast<PStaticDIO *>(_o);
        switch (_id) {
        case 0: _t->newIn((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->newOut((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->getIn(); break;
        case 3: _t->setOut((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->setIndS((*reinterpret_cast< UINT64(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PStaticDIO::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PStaticDIO::newIn)) {
                *result = 0;
            }
        }
        {
            typedef void (PStaticDIO::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PStaticDIO::newOut)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject PStaticDIO::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PStaticDIO.data,
      qt_meta_data_PStaticDIO,  qt_static_metacall, 0, 0}
};


const QMetaObject *PStaticDIO::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PStaticDIO::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PStaticDIO.stringdata))
        return static_cast<void*>(const_cast< PStaticDIO*>(this));
    return QObject::qt_metacast(_clname);
}

int PStaticDIO::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void PStaticDIO::newIn(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PStaticDIO::newOut(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
