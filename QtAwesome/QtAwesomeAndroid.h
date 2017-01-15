#ifndef QTAWESOMEANDROID_H
#define QTAWESOMEANDROID_H

#include <QObject>
#include "QtAwesome.h"

/// A list of all icon sizes
namespace dpiset {
enum icon {
    mdpi    = 0,
    hdpi    = 1,
    xhdpi   = 2,
    xxhdpi  = 3,
    xxxhdpi = 4
};
enum type {
    launcher    = 10,
    actionbar    = 11,
    contextual   = 12,
    notification  = 13
};
}

class QtAwesomeAndroid : public QtAwesome
{
    Q_OBJECT
public:
    QtAwesomeAndroid(QObject *parent = 0);

    QSize iconSize(int dpi = dpiset::hdpi, int type = dpiset::actionbar);
    Q_INVOKABLE QString iconLink( int character, const QVariantMap& options = QVariantMap(), int dpi = dpiset::xhdpi, int type = dpiset::actionbar );
    Q_INVOKABLE QString iconLink( const QString& name, const QVariantMap& options = QVariantMap(), int dpi = dpiset::xhdpi, int type = dpiset::actionbar );
};

#endif // QTAWESOMEANDROID_H
