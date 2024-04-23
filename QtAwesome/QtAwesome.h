/**
 * QtAwesome - use font-awesome (or other font icons) in your c++ / Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2013-2024 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#ifndef QTAWESOME_H
#define QTAWESOME_H

#include <QIcon>
#include <QIconEngine>
#include <QPainter>
#include <QRect>
#include <QVariantMap>

#include "QtAwesomeAnim.h"

/// A list of all icon-names with the codepoint (unicode-value) on the right
/// You can use the names on the page  http://fortawesome.github.io/Font-Awesome/design.html
#include "QtAwesomeEnumGenerated.h"

namespace fa {

/// A list of all style-names
enum fa_styles {
    fa_solid = 0,
    fa_regular = 1,
#ifdef FONT_AWESOME_PRO
    fa_light = 3,
    fa_thin = 4,
    fa_duotone = 5,
    fa_sharp_solid = 6,
    fa_sharp_regular = 7,
    fa_sharp_light = 8,
    fa_sharp_thin = 9,
#endif
    fa_brands = 2
};


struct QtAwesomeNamedIcon {
    const char *name;
    ushort icon;
};

class QtAwesomeIconPainter;


class QtAwesomeFontData
{
public:
    QtAwesomeFontData() : QtAwesomeFontData(QString()) {}
    QtAwesomeFontData(const QString &fontFileName, QFont::Weight fontWeight = QFont::Normal);
    const QString& fontFamily() const;
    void setFontFamily(const QString &family);
    const QString& fontFilename() const;
    int fontId() const;
    void setFontId(int id);
    QFont::Weight fontWeight() const;
    void setFontWeight(QFont::Weight weight);

private:
    QString _fontFamily;
    QString _fontFilename;
    int _fontId;
    QFont::Weight _fontWeight;
};

//---------------------------------------------------------------------------------------

/// The main class for managing icons
/// This class requires a 2-phase construction. You must first create the class and then initialize it via an init* method
class QtAwesome : public QObject
{
Q_OBJECT

public:
    static const QString FA_BRANDS_FONT_FILENAME; // fa-brands
    static const QString FA_REGULAR_FONT_FILENAME; // fa-regular
    static const QString FA_SOLID_FONT_FILENAME; // fa-solid
    static const QFont::Weight FA_BRANDS_FONT_WEIGHT = QFont::Normal;
    static const QFont::Weight FA_REGULAR_FONT_WEIGHT = QFont::Normal;
    static const QFont::Weight FA_SOLID_FONT_WEIGHT = QFont::Black;

#ifdef FONT_AWESOME_PRO
    static const QString FA_LIGHT_FONT_FILENAME; // fa-light
    static const QString FA_DUOTONE_FONT_FILENAME; // fa-duotone
    static const QString FA_THIN_FONT_FILENAME; // fa-thin
    static const QString FA_SHARP_SOLID_FONT_FILENAME; // fa-sharp fa-solid
    static const QString FA_SHARP_REGULAR_FONT_FILENAME; // fa-sharp fa-regular
    static const QString FA_SHARP_LIGHT_FONT_FILENAME; // fa-sharp fa-light
    static const QString FA_SHARP_THIN_FONT_FILENAME; // fa-sharp fa-thin
    static const int DUOTONE_HEX_ICON_VALUE = 0x100000;
    static const QFont::Weight FA_LIGHT_FONT_WEIGHT = QFont::Light;
    static const QFont::Weight FA_THIN_FONT_WEIGHT = QFont::ExtraLight;
    static const QFont::Weight FA_DUOTONE_FONT_WEIGHT = QFont::Black;
    static const QFont::Weight FA_SHARP_SOLID_FONT_WEIGHT = QFont::Black;
    static const QFont::Weight FA_SHARP_REGULAR_FONT_WEIGHT = QFont::Normal;
    static const QFont::Weight FA_SHARP_LIGHT_FONT_WEIGHT = QFont::Light;
    static const QFont::Weight FA_SHARP_THIN_FONT_WEIGHT = QFont::ExtraLight;
#endif

public:

    explicit QtAwesome(QObject* parent = nullptr);
    virtual ~QtAwesome();

    virtual bool initFontAwesome();

    virtual const QHash<QString, int> namedCodePoints(int style) const;

    void setDefaultOption(const QString& name, const QVariant& value);
    QVariant defaultOption(const QString& name);

    QIcon icon(int style, int character, const QVariantMap& options = QVariantMap());
    QIcon icon(const QString& name, const QVariantMap& options = QVariantMap());
    QIcon icon(QtAwesomeIconPainter* painter, const QVariantMap& optionMap = QVariantMap());

    void give(const QString& name, QtAwesomeIconPainter* painter);

    QFont font(int style, int size) const;

    /// Returns the font-name that is used as icon-map
    QString fontName(int style) const;

protected:
    int stringToStyleEnum(const QString style) const;
    const QString styleEnumToString(int style) const;
    void addToNamedCodePoints(int style, const fa::QtAwesomeNamedIcon* faCommonIconArray, int size);

Q_SIGNALS:
    // signal about default options being reset
    void defaultOptionsReset();

public Q_SLOTS:
    // (re)set default options according to current QApplication::palette()
    void resetDefaultOptions();

private:
    QHash<int, QtAwesomeFontData>     _fontDetails;           ///< The fonts name used for each style
    QHash<int, QHash<QString, int>*> _namedCodepointsByStyle; ///< A map with names mapped to code-points for each style
    QList<QHash<QString, int>*>      _namedCodepointsList;    ///< The list of all created named-codepoints

    QHash<QString, QtAwesomeIconPainter*> _painterMap;     ///< A map of custom painters
    QVariantMap _defaultOptions;                           ///< The default icon options
    QtAwesomeIconPainter* _fontIconPainter;                ///< A special painter fo painting codepoints
};

//---------------------------------------------------------------------------------------

/// The QtAwesomeIconPainter is a specialized painter for painting icons
/// your can implement an iconpainter to create custom font-icon code
class QtAwesomeIconPainter
{
public:
    virtual ~QtAwesomeIconPainter();
    virtual void paint(QtAwesome* awesome, QPainter* painter, const QRect& rect,  QIcon::Mode mode, QIcon::State state,
                       const QVariantMap& options) = 0;
};

} // fa

Q_DECLARE_METATYPE(fa::QtAwesomeAnimation*)

#endif // QTAWESOME_H
