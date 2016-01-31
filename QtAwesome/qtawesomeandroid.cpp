#include "QtAwesomeAndroid.h"
#include <QTemporaryFile>
#include <QDebug>

QtAwesomeAndroid::QtAwesomeAndroid(QObject *parent) : QtAwesome(parent)
{

}

QSize QtAwesomeAndroid::iconSize(int dpi, int type)
{
    QSize iconSize;
    switch (dpi) {
    case dpiset::mdpi:
        switch (type) {
        case dpiset::launcher:
            iconSize = QSize(48, 48);
            break;
        case dpiset::actionbar:
            iconSize = QSize(24, 24);
            break;
        case dpiset::contextual:
            iconSize = QSize(16, 16);
            break;
        case dpiset::notification:
            iconSize = QSize(22, 22);
            break;
        default:
            break;
        }
        break;
    case dpiset::hdpi:
        switch (type) {
        case dpiset::launcher:
            iconSize = QSize(72, 72);
            break;
        case dpiset::actionbar:
            iconSize = QSize(36, 36);
            break;
        case dpiset::contextual:
            iconSize = QSize(24, 24);
            break;
        case dpiset::notification:
            iconSize = QSize(33, 33);
            break;
        default:
            break;
        }
        break;
    case dpiset::xhdpi:
        switch (type) {
        case dpiset::launcher:
            iconSize = QSize(96, 96);
            break;
        case dpiset::actionbar:
            iconSize = QSize(48, 48);
            break;
        case dpiset::contextual:
            iconSize = QSize(32, 32);
            break;
        case dpiset::notification:
            iconSize = QSize(44, 44);
            break;
        default:
            break;
        }
        break;
    case dpiset::xxhdpi:
        switch (type) {
        case dpiset::launcher:
            iconSize = QSize(144, 144);
            break;
        case dpiset::actionbar:
            iconSize = QSize(72, 72);
            break;
        case dpiset::contextual:
            iconSize = QSize(48, 48);
            break;
        case dpiset::notification:
            iconSize = QSize(66, 66);
            break;
        default:
            break;
        }
        break;
    case dpiset::xxxhdpi:
        switch (type) {
        case dpiset::launcher:
            iconSize = QSize(192, 192);
            break;
        case dpiset::actionbar:
            iconSize = QSize(96, 96);
            break;
        case dpiset::contextual:
            iconSize = QSize(64, 64);
            break;
        case dpiset::notification:
            iconSize = QSize(88, 88);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return iconSize;
}

/// Creates a file with icon and return link to this file with the given code-point
/// It may use with QML Type QToolButton for example.
/// <code>
///     awesome->iconLink( fa::beer );
/// </code>
QString QtAwesomeAndroid::iconLink(int character, const QVariantMap &options, int dpi, int type)
{
    QIcon icon = this->icon(character, options);

    QSize iconSize = this->iconSize(dpi, type);

    QPixmap pixmap = icon.pixmap(iconSize);
    QTemporaryFile file;
    file.setAutoRemove(false);

    bool res = false;
    if (file.open()) {
        res = pixmap.save(&file, "PNG");
        if (!res) {
            qDebug() << "Icon generation and save failed:" << "file:///"+file.fileName();
        }
    } else {
        qDebug() << file.errorString() << pixmap.save(&file, "PNG");
    }

    return "file:///"+file.fileName();
}

/// Creates a file with icon and return link to this file with the given code-point
/// It may use with QML Type QToolButton for example.
/// <code>
///     awesome->iconLink( "beer" );
/// </code>
QString QtAwesomeAndroid::iconLink(const QString& name, const QVariantMap &options, int dpi, int type)
{
    if( namedCodepoints_.count(name) ) {
        return iconLink( namedCodepoints_.value(name), options, dpi, type );
    }
}
