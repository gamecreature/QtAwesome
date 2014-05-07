/**
 * QtAwesome - use font-awesome (or other font icons) in your c++ / Qt Application
 *
 * Copyright 2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "QtAwesome.h"

#include <QDebug>
#include <QFile>
#include <QFontDatabase>



/// The font-awesome icon painter
class QtAwesomeCharIconPainter: public QtAwesomeIconPainter
{
public:
    virtual void paint( QtAwesome* awesome, QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state, const QVariantMap& options  )
    {
        Q_UNUSED(mode);
        Q_UNUSED(state);
        Q_UNUSED(options);

        painter->save();

        // set the correct color
        QColor color = options.value("color").value<QColor>();
        QString text = options.value("text").toString();

        if( mode == QIcon::Disabled ) {
            color = options.value("color-disabled").value<QColor>();
            QVariant alt = options.value("text-disabled");
            if( alt.isValid() ) {
                text = alt.toString();
            }
        } else if( mode == QIcon::Active ) {
            color = options.value("color-active").value<QColor>();
            QVariant alt = options.value("text-active");
            if( alt.isValid() ) {
                text = alt.toString();
            }
        } else if( mode == QIcon::Selected ) {
            color = options.value("color-selected").value<QColor>();
            QVariant alt = options.value("text-selected");
            if( alt.isValid() ) {
                text = alt.toString();
            }
        }
        painter->setPen(color);

        // add some 'padding' around the icon
        int drawSize = qRound(rect.height()*options.value("scale-factor").toFloat());

        painter->setFont( awesome->font(drawSize) );
        painter->drawText( rect, text, QTextOption( Qt::AlignCenter|Qt::AlignVCenter ) );
        painter->restore();
    }

};


//---------------------------------------------------------------------------------------


/// The painter icon engine.
class QtAwesomeIconPainterIconEngine : public QIconEngine
{

public:

    QtAwesomeIconPainterIconEngine( QtAwesome* awesome, QtAwesomeIconPainter* painter, const QVariantMap& options  )
        : awesomeRef_(awesome)
        , iconPainterRef_(painter)
        , options_(options)
    {
    }

    virtual ~QtAwesomeIconPainterIconEngine() {}

    QtAwesomeIconPainterIconEngine* clone() const
    {
        return new QtAwesomeIconPainterIconEngine( awesomeRef_, iconPainterRef_, options_ );
    }

    virtual void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
    {
        Q_UNUSED( mode );
        Q_UNUSED( state );
        iconPainterRef_->paint( awesomeRef_, painter, rect, mode, state, options_ );
    }

    virtual QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
    {
        QPixmap pm(size);
        pm.fill( Qt::transparent ); // we need transparency
        {
            QPainter p(&pm);
            paint(&p, QRect(QPoint(0,0),size), mode, state);
        }
        return pm;
    }

private:

    QtAwesome* awesomeRef_;                  ///< a reference to the QtAwesome instance
    QtAwesomeIconPainter* iconPainterRef_;   ///< a reference to the icon painter
    QVariantMap options_;                    ///< the options for this icon painter
};


//---------------------------------------------------------------------------------------

/// The default icon colors
QtAwesome::QtAwesome( QObject* parent )
    : QObject( parent )
    , namedCodepoints_()
{
    // initialize the default options
    setDefaultOption( "color", QColor(50,50,50) );
    setDefaultOption( "color-disabled", QColor(70,70,70,60));
    setDefaultOption( "color-active", QColor(10,10,10));
    setDefaultOption( "color-selected", QColor(10,10,10));
    setDefaultOption( "scale-factor", 0.9 );

    setDefaultOption( "text", QVariant() );
    setDefaultOption( "text-disabled", QVariant() );
    setDefaultOption( "text-active", QVariant() );
    setDefaultOption( "text-selected", QVariant() );

    fontIconPainter_ = new QtAwesomeCharIconPainter();

}


QtAwesome::~QtAwesome()
{
    delete fontIconPainter_;
//    delete errorIconPainter_;
    qDeleteAll(painterMap_);
}

/// initializes the QtAwesome icon factory with the given fontname
void QtAwesome::init(const QString& fontname)
{
    fontName_ = fontname;
}


/// a specialized init function so font-awesome is loaded and initialized
/// this method return true on success, it will return false if the fnot cannot be initialized
/// To initialize QtAwesome with font-awesome you need to call this method
bool QtAwesome::initFontAwesome( )
{
    static int fontAwesomeFontId = -1;
    
    // only load font-awesome once
    if( fontAwesomeFontId < 0 ) {

        // The macro below internally calls "qInitResources_QtAwesome()". this initializes
        // the resource system. For a .pri project this isn't required, but when building and using a
        // static library the resource need to initialized first.
        ///
        // I've checked th qInitResource_* code and calling this method mutliple times shouldn't be any problem
        // (More info about this subject:  http://qt-project.org/wiki/QtResources)
        Q_INIT_RESOURCE(QtAwesome);

        // load the font file
        QFile res(":/fonts/fontawesome-4.0.3.ttf");
        if(!res.open(QIODevice::ReadOnly)) {
            qDebug() << "Font awesome font could not be loaded!";
            return false;
        }
        QByteArray fontData( res.readAll() );
        res.close();

        // fetch the given font
        fontAwesomeFontId = QFontDatabase::addApplicationFontFromData(fontData);
    }

    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(fontAwesomeFontId);
    if( !loadedFontFamilies.empty() ) {
        fontName_= loadedFontFamilies.at(0);
    } else {
        qDebug() << "Font awesome font is empty?!";
        fontAwesomeFontId = -1; // restore the font-awesome id
        return false;
    }

    // intialize the map
    QHash<QString, fa>& m = namedCodepoints_;
    m.insert("glass", 					fa::glass);
    m.insert("music", 					fa::music);
    m.insert("search", 					fa::search);
    m.insert("envelopeo", 				fa::envelopeo);
    m.insert("heart", 					fa::heart);
    m.insert("star", 					fa::star);
    m.insert("staro", 					fa::staro);
    m.insert("user", 					fa::user);
    m.insert("film", 					fa::film);
    m.insert("thlarge", 				fa::thlarge);
    m.insert("th", 						fa::th);
    m.insert("thlist", 					fa::thlist);
    m.insert("check", 					fa::check);
    m.insert("times", 					fa::times);
    m.insert("searchplus", 				fa::searchplus);
    m.insert("searchminus", 			fa::searchminus);
    m.insert("poweroff", 				fa::poweroff);
    m.insert("signal", 					fa::signal);
    m.insert("cog", 					fa::cog);
    m.insert("trasho", 					fa::trasho);
    m.insert("home", 					fa::home);
    m.insert("fileo", 					fa::fileo);
    m.insert("clocko", 					fa::clocko);
    m.insert("road", 					fa::road);
    m.insert("download", 				fa::download);
    m.insert("arrowcircleodown", 		fa::arrowcircleodown);
    m.insert("arrowcircleoup", 			fa::arrowcircleoup);
    m.insert("inbox", 					fa::inbox);
    m.insert("playcircleo", 			fa::playcircleo);
    m.insert("repeat", 					fa::repeat);
    m.insert("refresh", 				fa::refresh);
    m.insert("listalt", 				fa::listalt);
    m.insert("lock", 					fa::lock);
    m.insert("flag", 					fa::flag);
    m.insert("headphones", 				fa::headphones);
    m.insert("volumeoff", 				fa::volumeoff);
    m.insert("volumedown", 				fa::volumedown);
    m.insert("volumeup", 				fa::volumeup);
    m.insert("qrcode", 					fa::qrcode);
    m.insert("barcode", 				fa::barcode);
    m.insert("tag", 					fa::tag);
    m.insert("tags", 					fa::tags);
    m.insert("book", 					fa::book);
    m.insert("bookmark", 				fa::bookmark);
    m.insert("print", 					fa::print);
    m.insert("camera", 					fa::camera);
    m.insert("font", 					fa::font);
    m.insert("bold", 					fa::bold);
    m.insert("italic", 					fa::italic);
    m.insert("textheight", 				fa::textheight);
    m.insert("textwidth", 				fa::textwidth);
    m.insert("alignleft", 				fa::alignleft);
    m.insert("aligncenter", 			fa::aligncenter);
    m.insert("alignright", 				fa::alignright);
    m.insert("alignjustify", 			fa::alignjustify);
    m.insert("list", 					fa::list);
    m.insert("outdent", 				fa::outdent);
    m.insert("indent", 					fa::indent);
    m.insert("videocamera", 			fa::videocamera);
    m.insert("pictureo", 				fa::pictureo);
    m.insert("pencil", 					fa::pencil);
    m.insert("mapmarker", 				fa::mapmarker);
    m.insert("adjust", 					fa::adjust);
    m.insert("tint", 					fa::tint);
    m.insert("pencilsquareo", 			fa::pencilsquareo);
    m.insert("sharesquareo", 			fa::sharesquareo);
    m.insert("checksquareo", 			fa::checksquareo);
    m.insert("arrows", 					fa::arrows);
    m.insert("stepbackward", 			fa::stepbackward);
    m.insert("fastbackward", 			fa::fastbackward);
    m.insert("backward", 				fa::backward);
    m.insert("play", 					fa::play);
    m.insert("pause", 					fa::pause);
    m.insert("stop", 					fa::stop);
    m.insert("forward", 				fa::forward);
    m.insert("fastforward", 			fa::fastforward);
    m.insert("stepforward", 			fa::stepforward);
    m.insert("eject", 					fa::eject);
    m.insert("chevronleft", 			fa::chevronleft);
    m.insert("chevronright", 			fa::chevronright);
    m.insert("pluscircle", 				fa::pluscircle);
    m.insert("minuscircle", 			fa::minuscircle);
    m.insert("timescircle", 			fa::timescircle);
    m.insert("checkcircle", 			fa::checkcircle);
    m.insert("questioncircle", 			fa::questioncircle);
    m.insert("infocircle", 				fa::infocircle);
    m.insert("crosshairs", 				fa::crosshairs);
    m.insert("timescircleo", 			fa::timescircleo);
    m.insert("checkcircleo", 			fa::checkcircleo);
    m.insert("ban", 					fa::ban);
    m.insert("arrowleft", 				fa::arrowleft);
    m.insert("arrowright", 				fa::arrowright);
    m.insert("arrowup", 				fa::arrowup);
    m.insert("arrowdown", 				fa::arrowdown);
    m.insert("share", 					fa::share);
    m.insert("expand", 					fa::expand);
    m.insert("compress", 				fa::compress);
    m.insert("plus", 					fa::plus);
    m.insert("minus", 					fa::minus);
    m.insert("asterisk", 				fa::asterisk);
    m.insert("exclamationcircle", 		fa::exclamationcircle);
    m.insert("gift", 					fa::gift);
    m.insert("leaf", 					fa::leaf);
    m.insert("fire", 					fa::fire);
    m.insert("eye", 					fa::eye);
    m.insert("eyeslash", 				fa::eyeslash);
    m.insert("exclamationtriangle", 	fa::exclamationtriangle);
    m.insert("plane", 					fa::plane);
    m.insert("calendar", 				fa::calendar);
    m.insert("random", 					fa::random);
    m.insert("comment", 				fa::comment);
    m.insert("magnet", 					fa::magnet);
    m.insert("chevronup", 				fa::chevronup);
    m.insert("chevrondown", 			fa::chevrondown);
    m.insert("retweet", 				fa::retweet);
    m.insert("shoppingcart", 			fa::shoppingcart);
    m.insert("folder", 					fa::folder);
    m.insert("folderopen", 				fa::folderopen);
    m.insert("arrowsv", 				fa::arrowsv);
    m.insert("arrowsh", 				fa::arrowsh);
    m.insert("barcharto", 				fa::barcharto);
    m.insert("twittersquare", 			fa::twittersquare);
    m.insert("facebooksquare", 			fa::facebooksquare);
    m.insert("cameraretro", 			fa::cameraretro);
    m.insert("key", 					fa::key);
    m.insert("cogs", 					fa::cogs);
    m.insert("comments", 				fa::comments);
    m.insert("thumbsoup", 				fa::thumbsoup);
    m.insert("thumbsodown", 			fa::thumbsodown);
    m.insert("starhalf", 				fa::starhalf);
    m.insert("hearto", 					fa::hearto);
    m.insert("signout", 				fa::signout);
    m.insert("linkedinsquare", 			fa::linkedinsquare);
    m.insert("thumbtack", 				fa::thumbtack);
    m.insert("externallink", 			fa::externallink);
    m.insert("signin", 					fa::signin);
    m.insert("trophy", 					fa::trophy);
    m.insert("githubsquare", 			fa::githubsquare);
    m.insert("upload", 					fa::upload);
    m.insert("lemono", 					fa::lemono);
    m.insert("phone", 					fa::phone);
    m.insert("squareo", 				fa::squareo);
    m.insert("bookmarko", 				fa::bookmarko);
    m.insert("phonesquare", 			fa::phonesquare);
    m.insert("twitter", 				fa::twitter);
    m.insert("facebook", 				fa::facebook);
    m.insert("github", 					fa::github);
    m.insert("unlock", 					fa::unlock);
    m.insert("creditcard", 				fa::creditcard);
    m.insert("rss", 					fa::rss);
    m.insert("hddo", 					fa::hddo);
    m.insert("bullhorn", 				fa::bullhorn);
    m.insert("bell", 					fa::bell);
    m.insert("certificate", 			fa::certificate);
    m.insert("handoright", 				fa::handoright);
    m.insert("handoleft", 				fa::handoleft);
    m.insert("handoup", 				fa::handoup);
    m.insert("handodown", 				fa::handodown);
    m.insert("arrowcircleleft", 		fa::arrowcircleleft);
    m.insert("arrowcircleright", 		fa::arrowcircleright);
    m.insert("arrowcircleup", 			fa::arrowcircleup);
    m.insert("arrowcircledown", 		fa::arrowcircledown);
    m.insert("globe", 					fa::globe);
    m.insert("wrench", 					fa::wrench);
    m.insert("tasks", 					fa::tasks);
    m.insert("filter", 					fa::filter);
    m.insert("briefcase", 				fa::briefcase);
    m.insert("arrowsalt", 				fa::arrowsalt);
    m.insert("users", 					fa::users);
    m.insert("link", 					fa::link);
    m.insert("cloud", 					fa::cloud);
    m.insert("flask", 					fa::flask);
    m.insert("scissors", 				fa::scissors);
    m.insert("fileso", 					fa::fileso);
    m.insert("paperclip", 				fa::paperclip);
    m.insert("floppyo", 				fa::floppyo);
    m.insert("square", 					fa::square);
    m.insert("bars", 					fa::bars);
    m.insert("listul", 					fa::listul);
    m.insert("listol", 					fa::listol);
    m.insert("strikethrough", 			fa::strikethrough);
    m.insert("underline", 				fa::underline);
    m.insert("table", 					fa::table);
    m.insert("magic", 					fa::magic);
    m.insert("truck", 					fa::truck);
    m.insert("pinterest", 				fa::pinterest);
    m.insert("pinterestsquare", 		fa::pinterestsquare);
    m.insert("googleplussquare", 		fa::googleplussquare);
    m.insert("googleplus", 				fa::googleplus);
    m.insert("money", 					fa::money);
    m.insert("caretdown", 				fa::caretdown);
    m.insert("caretup", 				fa::caretup);
    m.insert("caretleft", 				fa::caretleft);
    m.insert("caretright", 				fa::caretright);
    m.insert("columns", 				fa::columns);
    m.insert("sort", 					fa::sort);
    m.insert("sortasc", 				fa::sortasc);
    m.insert("sortdesc", 				fa::sortdesc);
    m.insert("envelope", 				fa::envelope);
    m.insert("linkedin", 				fa::linkedin);
    m.insert("undo", 					fa::undo);
    m.insert("gavel", 					fa::gavel);
    m.insert("tachometer", 				fa::tachometer);
    m.insert("commento", 				fa::commento);
    m.insert("commentso", 				fa::commentso);
    m.insert("bolt", 					fa::bolt);
    m.insert("sitemap", 				fa::sitemap);
    m.insert("umbrella", 				fa::umbrella);
    m.insert("clipboard", 				fa::clipboard);
    m.insert("lightbulbo", 				fa::lightbulbo);
    m.insert("exchange", 				fa::exchange);
    m.insert("clouddownload", 			fa::clouddownload);
    m.insert("cloudupload", 			fa::cloudupload);
    m.insert("usermd", 					fa::usermd);
    m.insert("stethoscope", 			fa::stethoscope);
    m.insert("suitcase", 				fa::suitcase);
    m.insert("bello", 					fa::bello);
    m.insert("coffee", 					fa::coffee);
    m.insert("cutlery", 				fa::cutlery);
    m.insert("filetexto", 				fa::filetexto);
    m.insert("buildingo", 				fa::buildingo);
    m.insert("hospitalo", 				fa::hospitalo);
    m.insert("ambulance", 				fa::ambulance);
    m.insert("medkit", 					fa::medkit);
    m.insert("fighterjet", 				fa::fighterjet);
    m.insert("beer", 					fa::beer);
    m.insert("hsquare", 				fa::hsquare);
    m.insert("plussquare", 				fa::plussquare);
    m.insert("angledoubleleft", 		fa::angledoubleleft);
    m.insert("angledoubleright", 		fa::angledoubleright);
    m.insert("angledoubleup", 			fa::angledoubleup);
    m.insert("angledoubledown", 		fa::angledoubledown);
    m.insert("angleleft", 				fa::angleleft);
    m.insert("angleright", 				fa::angleright);
    m.insert("angleup", 				fa::angleup);
    m.insert("angledown", 				fa::angledown);
    m.insert("desktop", 				fa::desktop);
    m.insert("laptop", 					fa::laptop);
    m.insert("tablet", 					fa::tablet);
    m.insert("mobile", 					fa::mobile);
    m.insert("circleo", 				fa::circleo);
    m.insert("quoteleft", 				fa::quoteleft);
    m.insert("quoteright", 				fa::quoteright);
    m.insert("spinner", 				fa::spinner);
    m.insert("circle", 					fa::circle);
    m.insert("reply", 					fa::reply);
    m.insert("githubalt", 				fa::githubalt);
    m.insert("foldero", 				fa::foldero);
    m.insert("folderopeno", 			fa::folderopeno);
    m.insert("smileo", 					fa::smileo);
    m.insert("frowno", 					fa::frowno);
    m.insert("meho", 					fa::meho);
    m.insert("gamepad", 				fa::gamepad);
    m.insert("keyboardo", 				fa::keyboardo);
    m.insert("flago", 					fa::flago);
    m.insert("flagcheckered", 			fa::flagcheckered);
    m.insert("terminal", 				fa::terminal);
    m.insert("code", 					fa::code);
    m.insert("replyall", 				fa::replyall);
    m.insert("mailreplyall", 			fa::mailreplyall);
    m.insert("starhalfo", 				fa::starhalfo);
    m.insert("locationarrow", 			fa::locationarrow);
    m.insert("crop", 					fa::crop);
    m.insert("codefork", 				fa::codefork);
    m.insert("chainbroken", 			fa::chainbroken);
    m.insert("question", 				fa::question);
    m.insert("info", 					fa::info);
    m.insert("exclamation", 			fa::exclamation);
    m.insert("superscript", 			fa::superscript);
    m.insert("subscript", 				fa::subscript);
    m.insert("eraser", 					fa::eraser);
    m.insert("puzzlepiece", 			fa::puzzlepiece);
    m.insert("microphone", 				fa::microphone);
    m.insert("microphoneslash", 		fa::microphoneslash);
    m.insert("shield", 					fa::shield);
    m.insert("calendaro", 				fa::calendaro);
    m.insert("fireextinguisher", 		fa::fireextinguisher);
    m.insert("rocket", 					fa::rocket);
    m.insert("maxcdn", 					fa::maxcdn);
    m.insert("chevroncircleleft", 		fa::chevroncircleleft);
    m.insert("chevroncircleright", 		fa::chevroncircleright);
    m.insert("chevroncircleup", 		fa::chevroncircleup);
    m.insert("chevroncircledown", 		fa::chevroncircledown);
    m.insert("html5", 					fa::html5);
    m.insert("css3", 					fa::css3);
    m.insert("anchor", 					fa::anchor);
    m.insert("unlockalt", 				fa::unlockalt);
    m.insert("bullseye", 				fa::bullseye);
    m.insert("ellipsish", 				fa::ellipsish);
    m.insert("ellipsisv", 				fa::ellipsisv);
    m.insert("rsssquare", 				fa::rsssquare);
    m.insert("playcircle", 				fa::playcircle);
    m.insert("ticket", 					fa::ticket);
    m.insert("minussquare", 			fa::minussquare);
    m.insert("minussquareo", 			fa::minussquareo);
    m.insert("levelup", 				fa::levelup);
    m.insert("leveldown", 				fa::leveldown);
    m.insert("checksquare", 			fa::checksquare);
    m.insert("pencilsquare", 			fa::pencilsquare);
    m.insert("externallinksquare", 		fa::externallinksquare);
    m.insert("sharesquare", 			fa::sharesquare);
    m.insert("compass", 				fa::compass);
    m.insert("caretsquareodown", 		fa::caretsquareodown);
    m.insert("caretsquareoup", 			fa::caretsquareoup);
    m.insert("caretsquareoright", 		fa::caretsquareoright);
    m.insert("eur", 					fa::eur);
    m.insert("gbp", 					fa::gbp);
    m.insert("usd", 					fa::usd);
    m.insert("inr", 					fa::inr);
    m.insert("jpy", 					fa::jpy);
    m.insert("rub", 					fa::rub);
    m.insert("krw", 					fa::krw);
    m.insert("btc", 					fa::btc);
    m.insert("file", 					fa::file);
    m.insert("filetext", 				fa::filetext);
    m.insert("sortalphaasc", 			fa::sortalphaasc);
    m.insert("sortalphadesc", 			fa::sortalphadesc);
    m.insert("sortamountasc", 			fa::sortamountasc);
    m.insert("sortamountdesc", 			fa::sortamountdesc);
    m.insert("sortnumericasc", 			fa::sortnumericasc);
    m.insert("sortnumericdesc", 		fa::sortnumericdesc);
    m.insert("thumbsup", 				fa::thumbsup);
    m.insert("thumbsdown", 				fa::thumbsdown);
    m.insert("youtubesquare", 			fa::youtubesquare);
    m.insert("youtube", 				fa::youtube);
    m.insert("xing", 					fa::xing);
    m.insert("xingsquare", 				fa::xingsquare);
    m.insert("youtubeplay", 			fa::youtubeplay);
    m.insert("dropbox", 				fa::dropbox);
    m.insert("stackoverflow", 			fa::stackoverflow);
    m.insert("instagram", 				fa::instagram);
    m.insert("flickr", 					fa::flickr);
    m.insert("adn", 					fa::adn);
    m.insert("bitbucket", 				fa::bitbucket);
    m.insert("bitbucketsquare", 		fa::bitbucketsquare);
    m.insert("tumblr", 					fa::tumblr);
    m.insert("tumblrsquare", 			fa::tumblrsquare);
    m.insert("longarrowdown", 			fa::longarrowdown);
    m.insert("longarrowup", 			fa::longarrowup);
    m.insert("longarrowleft", 			fa::longarrowleft);
    m.insert("longarrowright", 			fa::longarrowright);
    m.insert("appleicon", 				fa::appleicon);
    m.insert("windowsicon", 			fa::windowsicon);
    m.insert("androidicon", 			fa::androidicon);
    m.insert("linuxicon", 				fa::linuxicon);
    m.insert("dribbble", 				fa::dribbble);
    m.insert("skype", 					fa::skype);
    m.insert("foursquare", 				fa::foursquare);
    m.insert("trello", 					fa::trello);
    m.insert("female", 					fa::female);
    m.insert("male", 					fa::male);
    m.insert("gittip", 					fa::gittip);
    m.insert("suno", 					fa::suno);
    m.insert("moono", 					fa::moono);
    m.insert("archive", 				fa::archive);
    m.insert("bug", 					fa::bug);
    m.insert("vk", 						fa::vk);
    m.insert("weibo", 					fa::weibo);
    m.insert("renren", 					fa::renren);
    m.insert("pagelines", 				fa::pagelines);
    m.insert("stackexchange", 			fa::stackexchange);
    m.insert("arrowcircleoright", 		fa::arrowcircleoright);
    m.insert("arrowcircleoleft", 		fa::arrowcircleoleft);
    m.insert("caretsquareoleft", 		fa::caretsquareoleft);
    m.insert("dotcircleo", 				fa::dotcircleo);
    m.insert("wheelchair", 				fa::wheelchair);
    m.insert("vimeosquare", 			fa::vimeosquare);
    m.insert("tryicon", 				fa::tryicon);
    m.insert("plussquareo", 			fa::plussquareo);

    return true;
}

void QtAwesome::addNamedCodepoint( const QString& name, fa codePoint)
{
    namedCodepoints_.insert( name, codePoint);
}


/// Sets a default option. These options are passed on to the icon painters
void QtAwesome::setDefaultOption(const QString& name, const QVariant& value)
{
    defaultOptions_.insert( name, value );
}


/// Returns the default option for the given name
QVariant QtAwesome::defaultOption(const QString& name)
{
    return defaultOptions_.value( name );
}


// internal helper method to merge to option amps
static QVariantMap mergeOptions( const QVariantMap& defaults, const QVariantMap& override )
{
    QVariantMap result= defaults;
    if( !override.isEmpty() ) {
        QMapIterator<QString,QVariant> itr(override);
        while( itr.hasNext() ) {
            itr.next();
            result.insert( itr.key(), itr.value() );
        }
    }
    return result;
}


/// Creates an icon with the given code-point
/// <code>
///     awesome->icon( icon_group )
/// </code>
QIcon QtAwesome::icon(fa character, const QVariantMap &options)
{
    // create a merged QVariantMap to have default options and icon-specific options
    QVariantMap optionMap = mergeOptions( defaultOptions_, options );
    optionMap.insert("text", QString( QChar(static_cast<int>(character)) ) );

    return icon( fontIconPainter_, optionMap );
}



/// Creates an icon with the given name
///
/// You can use the icon names as defined on http://fortawesome.github.io/Font-Awesome/design.html  withour the 'icon-' prefix
/// @param name the name of the icon
/// @param options extra option to pass to the icon renderer
QIcon QtAwesome::icon(const QString& name, const QVariantMap& options)
{
    // when it's a named codepoint
    if( namedCodepoints_.count(name) ) {
        return icon( namedCodepoints_.value(name), options );
    }


    // create a merged QVariantMap to have default options and icon-specific options
    QVariantMap optionMap = mergeOptions( defaultOptions_, options );

    // this method first tries to retrieve the icon
    QtAwesomeIconPainter* painter = painterMap_.value(name);
    if( !painter ) {
        return QIcon();
    }

    return icon( painter, optionMap );
}

/// Create a dynamic icon by simlpy supplying a painter object
/// The ownership of the painter is NOT transfered.
/// @param painter a dynamic painter that is going to paint the icon
/// @param optionmap the options to pass to the painter
QIcon QtAwesome::icon(QtAwesomeIconPainter* painter, const QVariantMap& optionMap )
{
    // Warning, when you use memoryleak detection. You should turn it of for the next call
    // QIcon's placed in gui items are often cached and not deleted when my memory-leak detection checks for leaks.
    // I'm not sure if it's a Qt bug or something I do wrong
    QtAwesomeIconPainterIconEngine* engine = new QtAwesomeIconPainterIconEngine( this, painter, optionMap  );
    return QIcon( engine );
}

/// Adds a named icon-painter to the QtAwesome icon map
/// As the name applies the ownership is passed over to QtAwesome
///
/// @param name the name of the icon
/// @param painter the icon painter to add for this name
void QtAwesome::give(const QString& name, QtAwesomeIconPainter* painter)
{
    delete painterMap_.value( name );   // delete the old one
    painterMap_.insert( name, painter );
}

/// Creates/Gets the icon font with a given size in pixels. This can be usefull to use a label for displaying icons
/// Example:
///
///    QLabel* label = new QLabel( QChar( icon_group ) );
///    label->setFont( awesome->font(16) )
QFont QtAwesome::font( int size )
{
    QFont font( fontName_);
    font.setPixelSize(size);
    return font;
}
