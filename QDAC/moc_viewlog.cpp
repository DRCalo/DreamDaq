/****************************************************************************
** Meta object code from reading C++ file 'viewlog.h'
**
** Created: Wed Oct 21 19:43:40 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "viewlog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewlog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewLog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      26,   21,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ViewLog[] = {
    "ViewLog\0\0updateLog()\0path\0changePath(QString)\0"
};

const QMetaObject ViewLog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ViewLog,
      qt_meta_data_ViewLog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewLog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewLog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewLog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewLog))
        return static_cast<void*>(const_cast< ViewLog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ViewLog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateLog(); break;
        case 1: changePath((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
