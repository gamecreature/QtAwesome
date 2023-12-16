# QtAwesome - Font Awesome for Qt Applications

QtAwesome is a library to add [Font Awesome](https://fontawesome.com)
icons to your [Qt application](http://qt-project.org/).

## Table of Contents

- [Latest Release 6.5.1](#latest-release-651)
- [Font Awesome 6 Release](#font-awesome-6-release)
- [Installation Free Version](#installation-free-version)
- [Installation Pro version](#installation-pro-version)
- [Basic Usage](#basic-usage)
- [Examples](#examples)
- [Example Custom Painter](#example-custom-painter)
- [Default options](#default-options)
- [Known Issues And Workarounds](#known-issues-and-workarounds)
- [Summary of Changes](#summary-of-changes)
- [Thanks](#thanks)
- [Contact](#contact)
- [License](#license)

## Latest Release 6.5.1

Pro Sharp Thin font support.

[View changelog](CHANGES.md)

## Font Awesome 6 Release

This is the Font Awesome 6 release. It replaces the main branch, which still was a Font Awesome 4 version.
(There's also a Font Awesome 5 branch, but was never merged to the main/master branch.)

This release is **not** completely backwards compatible with the 4 and 5 releases.
The decision was made for a new clean version which better suited for the future.
(A compatibility layer is in development).

Previous versions used a hand-crafted icon list, this version has a generated list.

Having troubles with this new release?

- You can find the previous `master` branch in the [fontawesome-4](https://github.com/gamecreature/QtAwesome/tree/fontawesome-4) branch. (`master` is dropped in favour of `main`)
- The [fontawesome-5](https://github.com/gamecreature/QtAwesome/tree/fontawesome-5) branch contains the Font Awesome 5 version.
- The new [main](https://github.com/gamecreature/QtAwesome/) branch contains the latest Font Awesome 6 version.
- Open a github issue if you'v found a bug or have a suggestion

## Installation Free Version

The easiest way to include QtAweome in your project is to copy the QtAwesome directory to your
project tree and add the following `include()` to your Qt project file:

```bash
CONFIG+=fontAwesomeFree
include(QtAwesome/QtAwesome.pri)
```

Now you are good to go! The free fonts are included in this project.

## Installation Pro version

To activate the pro version, `fontAwesomePro` config should be defined.

```bash
CONFIG+=fontAwesomePro
include(QtAwesome/QtAwesome.pri)
```

And the pro font files need to be copied to the `QtAwesome/fonts/pro` folder.
(ex, Font Awesome 6 Brands-Regular-400.otf, etc... )

## Basic Usage

You probably want to create a single QtAwesome object for your whole application.

```c++
fa::QtAwesome* awesome = new fa::QtAwesome(qApp)
awesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map
```

- Add an accessor to this object (i.e. a global function, member of your application object, or whatever you like).
- Use an icon name from the [Font Awesome Library](https://fontawesome.com/icons).

## Examples

Next the icons can be accessed via the `awesome->icon` method.

```c++
// The most performant operation the get an icon
QPushButton* btn = new QPushButton(awesome->icon(fa::fa_solid, fa::fa_wine_glass), "Cheers!");

// You can also use 'string' names to access the icons.
QPushButton* btn = new QPushButton(awesome->icon("fa-solid fa-coffee" ), "Black please!");

// The string items passed to the icon method  can be used without the 'fa-' prefix
QPushButton* btn = new QPushButton(awesome->icon("solid coffee" ), "Black please!");

// The style is also optional and will fallback to the 'solid' style
QPushButton* btn = new QPushButton(awesome->icon("coffee" ), "Black please!");
```

For shorter syntax (more Font Aweseome like) is possible to bring the fa namespace into the curren scope:

```c++
using namespace fa;
QPushButton* btn = new QPushButton(awesome->icon(fa_solid, fa_wine_glass), "Cheers!");
```

It is possible to create some extra options for the icons.
The available options can be found in the [Default options list](#default-options)

```c++
QVariantMap options;
options.insert("color" , QColor(255, 0 ,0));
QPushButton* musicButton = new QPushButton(awesome->icon(fa::fa_solid, fa::music, options), "Music");
```

The defaults option can also be adjusted via the `setDefaultOption` method.\
For example having green disabled icons, it is possible to call:

```c++
awesome->setDefaultOption("color-disabled", QColor(0, 255, 0));
```

It also possible to render a label directly with this font

```c++
QLabel* label = new QLabel(QChar(fa::fa_github));
label->setFont(awesome->font(fa::fa_brands, 16));
```

## Example Custom Painter

This example registers a custom painter for supporting an custom icon named 'duplicate'
It simply draws 2 "plus marks".

```c++
class DuplicateIconPainter : public QtAwesomeIconPainter
{
public:
    virtual void paint(QtAwesome* awesome, QPainter* painter, const QRect& rectIn, QIcon::Mode mode, QIcon::State state, const QVariantMap& options)
    {
        int drawSize = qRound(rectIn.height() * 0.5);
        int offset = rectIn.height() / 4;
        QChar chr = QChar(static_cast<int>(fa::plus));
        int st = fa::fa_solid;

        painter->setFont(st, awesome->font(drawSize));

        painter->setPen(QColor(100,100,100));
        painter->drawText(QRect(QPoint(offset * 2, offset * 2),
                          QSize(drawSize, drawSize)), chr ,
                          QTextOption(Qt::AlignCenter|Qt::AlignVCenter));

        painter->setPen(QColor(50,50,50));
        painter->drawText(QRect(QPoint(rectIn.width() - drawSize-offset, rectIn.height() - drawSize - offset),
                                QSize(drawSize, drawSize) ), chr ,
                                QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    }
};

awesome->give("duplicate", new DuplicateIconPainter());
```

After this, this icon can be used with the given string name:

```c++
awesome->icon("duplicate")
```

## Default options

The following options are the defaults in the QtAwesome class.

```c++
setDefaultOption("color", QApplication::palette().color(QPalette::Normal, QPalette::Text));
setDefaultOption("color-disabled", QApplication::palette().color(QPalette::Disabled, QPalette::Text));
setDefaultOption("color-active", QApplication::palette().color(QPalette::Active, QPalette::Text));
setDefaultOption("color-selected", QApplication::palette().color(QPalette::Active, QPalette::Text));

setDefaultOption("text", QString()); // internal option
setDefaultOption("text-disabled", QString());
setDefaultOption("text-active", QString());
setDefaultOption("text-selected", QString());

setDefaultOption("scale-factor", 0.9);
```

Extra items for the pro version

```c++
setDefaultOption("duotone-color", QApplication::palette().color(QPalette::Normal, QPalette::BrightText));
setDefaultOption("duotone-color-disabled", QApplication::palette().color(QPalette::Disabled, QPalette::BrightText));
setDefaultOption("duotone-color-active", QApplication::palette().color(QPalette::Active, QPalette::BrightText));
setDefaultOption("duotone-color-selected", QApplication::palette().color(QPalette::Active, QPalette::BrightText));
```

When creating an icon, it first populates the options-map with the default options from the QtAwesome object.
After that the options are expanded/overwritten by the options supplied to the icon.

It is possible to use another glyph per icon-state. For example to make an icon-unlock symbol switch to locked when selected,
you could supply the following option:

```c++
options.insert("text-selected", QString(fa::fa_lock));
```

Color and text options have the following structure:
`keyname-iconmode-iconstate`

When iconmode normal is empty\
And iconstate on is blank

So the list of items used is:

- color
- color-disabled
- color-active
- color-selected
- color-off
- color-disabled-off
- color-active-off
- color-selected-off
- duotone-color (pro)
- duotone-color-disabled (pro)
- duotone-color-active (pro)
- duotone-color-selected (pro)
- duotone-color-off (pro)
- duotone-color-disabled-off (pro)
- duotone-color-active-off (pro)
- duotone-color-selected-off (pro)
- text
- text-disabled
- text-active
- text-selected
- text-off
- text-disabled-off
- text-active-off
- text-selected-off
- style
- style-disabled
- style-active
- style-selected
- style-off
- style-disabled-off
- style-active-off
- style-selected-off

## Known Issues And Workarounds

On Mac OS X, placing an qtAwesome icon in QMainWindow menu, doesn't work directly.
See the following issue: [https://github.com/gamecreature/QtAwesome/issues/10]

A workaround for this problem is converting it to a Pixmap icon:

```c++
QAction* menuAction = new QAction("test");
menuAction->setIcon(awesome->icon(fa::fa_heart).pixmap(32,32));
```

## Summary of Changes

- The complete icons set is renewed and is generated
- Everything is namespaced in the `fa` namespace
- Icon name enumerations are changed so the full Font Aweomse name is used: `fa::user`  => `fa::fa_user`.
  With the dashes replaced by underscores.
- Font Awesome 6 full style names, like `fa::fa_regular`, `fa::fa_solid`
- This release has been tested with Qt 5 and Qt 6.

## Thanks

Thanks go to the contributors of this project!

Many thanks go to Dave Gandy an the other Font Awesome contributors!! [https://github.com/FortAwesome/Font-Awesome](https://github.com/FortAwesome/Font-Awesome)
And of course to the Qt team/contributors for supplying this great cross-platform c++ library.

Contributions are welcome! Feel free to fork and send a pull request through Github.

<a href="https://github.com/gamecreature/qtawesome/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=gamecreature/qtawesome" />
</a>

<small>*Contribution list made with [contrib.rocks](https://contrib.rocks).*</small>

## Contact

- email: <rick@blommersit.nl>
- mastedon: [https://ruby.social/@rick](https://ruby.social/@rick)
- twitter: [https://twitter.com/gamecreature](https://twitter.com/gamecreature)
- website: [https://gamecreatures.com](https://gamecreatures.com)
- github: [https://github.com/gamecreature/QtAwesome](https://github.com/gamecreature/QtAwesome)

## License

MIT License. Copyright 2013-2022 - Reliable Bits Software by Blommers IT. [https://blommersit.nl/](https://blommersit.nl)

The Font Awesome font is licensed under the SIL Open Font License - [https://scripts.sil.org/OFL](http://scripts.sil.org/OFL)
The Font Awesome pictograms are licensed under the CC BY 3.0 License - [https://creativecommons.org/licenses/by/3.0/](http://creativecommons.org/licenses/by/3.0/)
"Font Awesome by Dave Gandy - https://github.com/FortAwesome/Font-Awesome"
