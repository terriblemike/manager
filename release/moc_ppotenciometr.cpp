/****************************************************************************
** Meta object code from reading C++ file 'ppotenciometr.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ppotenciometr.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ppotenciometr.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PPotenciometr_t {
    QByteArrayData data[8];
    char stringdata[75];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_PPotenciometr_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_PPotenciometr_t qt_meta_stringdata_PPotenciometr = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 9),
QT_MOC_LITERAL(2, 24, 0),
QT_MOC_LITERAL(3, 25, 10),
QT_MOC_LITERAL(4, 36, 11),
QT_MOC_LITERAL(5, 48, 7),
QT_MOC_LITERAL(6, 56, 5),
QT_MOC_LITERAL(7, 62, 11)
    },
    "PPotenciometr\0targetPos\0\0alrLeftPos\0"
    "alrRightPos\0newData\0value\0requestData\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PPotenciometr[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x05,
       3,    0,   40,    2, 0x05,
       4,    0,   41,    2, 0x05,
       5,    1,   42,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       7,    0,   45,    2, 0x08,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void PPotenciometr::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PPotenciometr *_t = static_cast<PPotenciometr *>(_o);
        switch (_id) {
        case 0: _t->targetPos(); break;
        case 1: _t->alrLeftPos(); break;
        case 2: _t->alrRightPos(); break;
        case 3: _t->newData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->requestData(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PPotenciometr::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPotenciometr::targetPos)) {
                *result = 0;
            }
        }
        {
            typedef void (PPotenciometr::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPotenciometr::alrLeftPos)) {
                *result = 1;
            }
        }
        {
            typedef void (PPotenciometr::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPotenciometr::alrRightPos)) {
                *result = 2;
            }
        }
        {
            typedef void (PPotenciometr::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PPotenciometr::newData)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject PPotenciometr::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PPotenciometr.data,
      qt_meta_data_PPotenciometr,  qt_static_metacall, 0, 0}
};


const QMetaObject *PPotenciometr::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PPotenciometr::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PPotenciometr.stringdata))
        return static_cast<void*>(const_cast< PPotenciometr*>(this));
    return QObject::qt_metacast(_clname);
}

int PPotenciometr::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void PPotenciometr::targetPos()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void PPotenciometr::alrLeftPos()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void PPotenciometr::alrRightPos()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void PPotenciometr::newData(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
