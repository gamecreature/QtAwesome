/**
 * QtAwesome - use font-awesome (or other font icons) in your c++ / Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2013-2015 - Reliable Bits Software by Blommers IT. All Rights Reserved.
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
        QFile res(":/fonts/fontawesome-4.4.0.ttf");
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
    QHash<QString, int>& m = namedCodepoints_;
    m.insert("fa_500px", fa::fa_500px );
    m.insert("adjust", fa::adjust );
    m.insert("adn", fa::adn );
    m.insert("aligncenter", fa::aligncenter );
    m.insert("alignjustify", fa::alignjustify );
    m.insert("alignleft", fa::alignleft );
    m.insert("alignright", fa::alignright );
    m.insert("amazon", fa::amazon );
    m.insert("ambulance", fa::ambulance );
    m.insert("anchor", fa::anchor );
    m.insert("android", fa::android );
    m.insert("angellist", fa::angellist );
    m.insert("angledoubledown", fa::angledoubledown );
    m.insert("angledoubleleft", fa::angledoubleleft );
    m.insert("angledoubleright", fa::angledoubleright );
    m.insert("angledoubleup", fa::angledoubleup );
    m.insert("angledown", fa::angledown );
    m.insert("angleleft", fa::angleleft );
    m.insert("angleright", fa::angleright );
    m.insert("angleup", fa::angleup );
    m.insert("apple", fa::apple );
    m.insert("archive", fa::archive );
    m.insert("areachart", fa::areachart );
    m.insert("arrowcircledown", fa::arrowcircledown );
    m.insert("arrowcircleleft", fa::arrowcircleleft );
    m.insert("arrowcircleodown", fa::arrowcircleodown );
    m.insert("arrowcircleoleft", fa::arrowcircleoleft );
    m.insert("arrowcircleoright", fa::arrowcircleoright );
    m.insert("arrowcircleoup", fa::arrowcircleoup );
    m.insert("arrowcircleright", fa::arrowcircleright );
    m.insert("arrowcircleup", fa::arrowcircleup );
    m.insert("arrowdown", fa::arrowdown );
    m.insert("arrowleft", fa::arrowleft );
    m.insert("arrowright", fa::arrowright );
    m.insert("arrowup", fa::arrowup );
    m.insert("arrows", fa::arrows );
    m.insert("arrowsalt", fa::arrowsalt );
    m.insert("arrowsh", fa::arrowsh );
    m.insert("arrowsv", fa::arrowsv );
    m.insert("asterisk", fa::asterisk );
    m.insert("at", fa::at );
    m.insert("automobile", fa::automobile );
    m.insert("backward", fa::backward );
    m.insert("balancescale", fa::balancescale );
    m.insert("ban", fa::ban );
    m.insert("bank", fa::bank );
    m.insert("barchart", fa::barchart );
    m.insert("barcharto", fa::barcharto );
    m.insert("barcode", fa::barcode );
    m.insert("bars", fa::bars );
    m.insert("battery0", fa::battery0 );
    m.insert("battery1", fa::battery1 );
    m.insert("battery2", fa::battery2 );
    m.insert("battery3", fa::battery3 );
    m.insert("battery4", fa::battery4 );
    m.insert("batteryempty", fa::batteryempty );
    m.insert("batteryfull", fa::batteryfull );
    m.insert("batteryhalf", fa::batteryhalf );
    m.insert("batteryquarter", fa::batteryquarter );
    m.insert("batterythreequarters", fa::batterythreequarters );
    m.insert("bed", fa::bed );
    m.insert("beer", fa::beer );
    m.insert("behance", fa::behance );
    m.insert("behancesquare", fa::behancesquare );
    m.insert("bell", fa::bell );
    m.insert("bello", fa::bello );
    m.insert("bellslash", fa::bellslash );
    m.insert("bellslasho", fa::bellslasho );
    m.insert("bicycle", fa::bicycle );
    m.insert("binoculars", fa::binoculars );
    m.insert("birthdaycake", fa::birthdaycake );
    m.insert("bitbucket", fa::bitbucket );
    m.insert("bitbucketsquare", fa::bitbucketsquare );
    m.insert("bitcoin", fa::bitcoin );
    m.insert("blacktie", fa::blacktie );
    m.insert("bold", fa::bold );
    m.insert("bolt", fa::bolt );
    m.insert("bomb", fa::bomb );
    m.insert("book", fa::book );
    m.insert("bookmark", fa::bookmark );
    m.insert("bookmarko", fa::bookmarko );
    m.insert("briefcase", fa::briefcase );
    m.insert("btc", fa::btc );
    m.insert("bug", fa::bug );
    m.insert("building", fa::building );
    m.insert("buildingo", fa::buildingo );
    m.insert("bullhorn", fa::bullhorn );
    m.insert("bullseye", fa::bullseye );
    m.insert("bus", fa::bus );
    m.insert("buysellads", fa::buysellads );
    m.insert("cab", fa::cab );
    m.insert("calculator", fa::calculator );
    m.insert("calendar", fa::calendar );
    m.insert("calendarchecko", fa::calendarchecko );
    m.insert("calendarminuso", fa::calendarminuso );
    m.insert("calendaro", fa::calendaro );
    m.insert("calendarpluso", fa::calendarpluso );
    m.insert("calendartimeso", fa::calendartimeso );
    m.insert("camera", fa::camera );
    m.insert("cameraretro", fa::cameraretro );
    m.insert("car", fa::car );
    m.insert("caretdown", fa::caretdown );
    m.insert("caretleft", fa::caretleft );
    m.insert("caretright", fa::caretright );
    m.insert("caretsquareodown", fa::caretsquareodown );
    m.insert("caretsquareoleft", fa::caretsquareoleft );
    m.insert("caretsquareoright", fa::caretsquareoright );
    m.insert("caretsquareoup", fa::caretsquareoup );
    m.insert("caretup", fa::caretup );
    m.insert("cartarrowdown", fa::cartarrowdown );
    m.insert("cartplus", fa::cartplus );
    m.insert("cc", fa::cc );
    m.insert("ccamex", fa::ccamex );
    m.insert("ccdinersclub", fa::ccdinersclub );
    m.insert("ccdiscover", fa::ccdiscover );
    m.insert("ccjcb", fa::ccjcb );
    m.insert("ccmastercard", fa::ccmastercard );
    m.insert("ccpaypal", fa::ccpaypal );
    m.insert("ccstripe", fa::ccstripe );
    m.insert("ccvisa", fa::ccvisa );
    m.insert("certificate", fa::certificate );
    m.insert("chain", fa::chain );
    m.insert("chainbroken", fa::chainbroken );
    m.insert("check", fa::check );
    m.insert("checkcircle", fa::checkcircle );
    m.insert("checkcircleo", fa::checkcircleo );
    m.insert("checksquare", fa::checksquare );
    m.insert("checksquareo", fa::checksquareo );
    m.insert("chevroncircledown", fa::chevroncircledown );
    m.insert("chevroncircleleft", fa::chevroncircleleft );
    m.insert("chevroncircleright", fa::chevroncircleright );
    m.insert("chevroncircleup", fa::chevroncircleup );
    m.insert("chevrondown", fa::chevrondown );
    m.insert("chevronleft", fa::chevronleft );
    m.insert("chevronright", fa::chevronright );
    m.insert("chevronup", fa::chevronup );
    m.insert("child", fa::child );
    m.insert("chrome", fa::chrome );
    m.insert("circle", fa::circle );
    m.insert("circleo", fa::circleo );
    m.insert("circleonotch", fa::circleonotch );
    m.insert("circlethin", fa::circlethin );
    m.insert("clipboard", fa::clipboard );
    m.insert("clocko", fa::clocko );
    m.insert("clone", fa::clone );
    m.insert("close", fa::close );
    m.insert("cloud", fa::cloud );
    m.insert("clouddownload", fa::clouddownload );
    m.insert("cloudupload", fa::cloudupload );
    m.insert("cny", fa::cny );
    m.insert("code", fa::code );
    m.insert("codefork", fa::codefork );
    m.insert("codepen", fa::codepen );
    m.insert("coffee", fa::coffee );
    m.insert("cog", fa::cog );
    m.insert("cogs", fa::cogs );
    m.insert("columns", fa::columns );
    m.insert("comment", fa::comment );
    m.insert("commento", fa::commento );
    m.insert("commenting", fa::commenting );
    m.insert("commentingo", fa::commentingo );
    m.insert("comments", fa::comments );
    m.insert("commentso", fa::commentso );
    m.insert("compass", fa::compass );
    m.insert("compress", fa::compress );
    m.insert("connectdevelop", fa::connectdevelop );
    m.insert("contao", fa::contao );
    m.insert("copy", fa::copy );
    m.insert("copyright", fa::copyright );
    m.insert("creativecommons", fa::creativecommons );
    m.insert("creditcard", fa::creditcard );
    m.insert("crop", fa::crop );
    m.insert("crosshairs", fa::crosshairs );
    m.insert("css3", fa::css3 );
    m.insert("cube", fa::cube );
    m.insert("cubes", fa::cubes );
    m.insert("cut", fa::cut );
    m.insert("cutlery", fa::cutlery );
    m.insert("dashboard", fa::dashboard );
    m.insert("dashcube", fa::dashcube );
    m.insert("database", fa::database );
    m.insert("dedent", fa::dedent );
    m.insert("delicious", fa::delicious );
    m.insert("desktop", fa::desktop );
    m.insert("deviantart", fa::deviantart );
    m.insert("diamond", fa::diamond );
    m.insert("digg", fa::digg );
    m.insert("dollar", fa::dollar );
    m.insert("dotcircleo", fa::dotcircleo );
    m.insert("download", fa::download );
    m.insert("dribbble", fa::dribbble );
    m.insert("dropbox", fa::dropbox );
    m.insert("drupal", fa::drupal );
    m.insert("edit", fa::edit );
    m.insert("eject", fa::eject );
    m.insert("ellipsish", fa::ellipsish );
    m.insert("ellipsisv", fa::ellipsisv );
    m.insert("empire", fa::empire );
    m.insert("envelope", fa::envelope );
    m.insert("envelopeo", fa::envelopeo );
    m.insert("envelopesquare", fa::envelopesquare );
    m.insert("eraser", fa::eraser );
    m.insert("eur", fa::eur );
    m.insert("euro", fa::euro );
    m.insert("exchange", fa::exchange );
    m.insert("exclamation", fa::exclamation );
    m.insert("exclamationcircle", fa::exclamationcircle );
    m.insert("exclamationtriangle", fa::exclamationtriangle );
    m.insert("expand", fa::expand );
    m.insert("expeditedssl", fa::expeditedssl );
    m.insert("externallink", fa::externallink );
    m.insert("externallinksquare", fa::externallinksquare );
    m.insert("eye", fa::eye );
    m.insert("eyeslash", fa::eyeslash );
    m.insert("eyedropper", fa::eyedropper );
    m.insert("facebook", fa::facebook );
    m.insert("facebookf", fa::facebookf );
    m.insert("facebookofficial", fa::facebookofficial );
    m.insert("facebooksquare", fa::facebooksquare );
    m.insert("fastbackward", fa::fastbackward );
    m.insert("fastforward", fa::fastforward );
    m.insert("fax", fa::fax );
    m.insert("feed", fa::feed );
    m.insert("female", fa::female );
    m.insert("fighterjet", fa::fighterjet );
    m.insert("file", fa::file );
    m.insert("filearchiveo", fa::filearchiveo );
    m.insert("fileaudioo", fa::fileaudioo );
    m.insert("filecodeo", fa::filecodeo );
    m.insert("fileexcelo", fa::fileexcelo );
    m.insert("fileimageo", fa::fileimageo );
    m.insert("filemovieo", fa::filemovieo );
    m.insert("fileo", fa::fileo );
    m.insert("filepdfo", fa::filepdfo );
    m.insert("filephotoo", fa::filephotoo );
    m.insert("filepictureo", fa::filepictureo );
    m.insert("filepowerpointo", fa::filepowerpointo );
    m.insert("filesoundo", fa::filesoundo );
    m.insert("filetext", fa::filetext );
    m.insert("filetexto", fa::filetexto );
    m.insert("filevideoo", fa::filevideoo );
    m.insert("filewordo", fa::filewordo );
    m.insert("filezipo", fa::filezipo );
    m.insert("fileso", fa::fileso );
    m.insert("film", fa::film );
    m.insert("filter", fa::filter );
    m.insert("fire", fa::fire );
    m.insert("fireextinguisher", fa::fireextinguisher );
    m.insert("firefox", fa::firefox );
    m.insert("flag", fa::flag );
    m.insert("flagcheckered", fa::flagcheckered );
    m.insert("flago", fa::flago );
    m.insert("flash", fa::flash );
    m.insert("flask", fa::flask );
    m.insert("flickr", fa::flickr );
    m.insert("floppyo", fa::floppyo );
    m.insert("folder", fa::folder );
    m.insert("foldero", fa::foldero );
    m.insert("folderopen", fa::folderopen );
    m.insert("folderopeno", fa::folderopeno );
    m.insert("font", fa::font );
    m.insert("fonticons", fa::fonticons );
    m.insert("forumbee", fa::forumbee );
    m.insert("forward", fa::forward );
    m.insert("foursquare", fa::foursquare );
    m.insert("frowno", fa::frowno );
    m.insert("futbolo", fa::futbolo );
    m.insert("gamepad", fa::gamepad );
    m.insert("gavel", fa::gavel );
    m.insert("gbp", fa::gbp );
    m.insert("ge", fa::ge );
    m.insert("gear", fa::gear );
    m.insert("gears", fa::gears );
    m.insert("genderless", fa::genderless );
    m.insert("getpocket", fa::getpocket );
    m.insert("gg", fa::gg );
    m.insert("ggcircle", fa::ggcircle );
    m.insert("gift", fa::gift );
    m.insert("git", fa::git );
    m.insert("gitsquare", fa::gitsquare );
    m.insert("github", fa::github );
    m.insert("githubalt", fa::githubalt );
    m.insert("githubsquare", fa::githubsquare );
    m.insert("gittip", fa::gittip );
    m.insert("glass", fa::glass );
    m.insert("globe", fa::globe );
    m.insert("google", fa::google );
    m.insert("googleplus", fa::googleplus );
    m.insert("googleplussquare", fa::googleplussquare );
    m.insert("googlewallet", fa::googlewallet );
    m.insert("graduationcap", fa::graduationcap );
    m.insert("gratipay", fa::gratipay );
    m.insert("group", fa::group );
    m.insert("hsquare", fa::hsquare );
    m.insert("hackernews", fa::hackernews );
    m.insert("handgrabo", fa::handgrabo );
    m.insert("handlizardo", fa::handlizardo );
    m.insert("handodown", fa::handodown );
    m.insert("handoleft", fa::handoleft );
    m.insert("handoright", fa::handoright );
    m.insert("handoup", fa::handoup );
    m.insert("handpapero", fa::handpapero );
    m.insert("handpeaceo", fa::handpeaceo );
    m.insert("handpointero", fa::handpointero );
    m.insert("handrocko", fa::handrocko );
    m.insert("handscissorso", fa::handscissorso );
    m.insert("handspocko", fa::handspocko );
    m.insert("handstopo", fa::handstopo );
    m.insert("hddo", fa::hddo );
    m.insert("header", fa::header );
    m.insert("headphones", fa::headphones );
    m.insert("heart", fa::heart );
    m.insert("hearto", fa::hearto );
    m.insert("heartbeat", fa::heartbeat );
    m.insert("history", fa::history );
    m.insert("home", fa::home );
    m.insert("hospitalo", fa::hospitalo );
    m.insert("hotel", fa::hotel );
    m.insert("hourglass", fa::hourglass );
    m.insert("hourglass1", fa::hourglass1 );
    m.insert("hourglass2", fa::hourglass2 );
    m.insert("hourglass3", fa::hourglass3 );
    m.insert("hourglassend", fa::hourglassend );
    m.insert("hourglasshalf", fa::hourglasshalf );
    m.insert("hourglasso", fa::hourglasso );
    m.insert("hourglassstart", fa::hourglassstart );
    m.insert("houzz", fa::houzz );
    m.insert("html5", fa::html5 );
    m.insert("icursor", fa::icursor );
    m.insert("ils", fa::ils );
    m.insert("image", fa::image );
    m.insert("inbox", fa::inbox );
    m.insert("indent", fa::indent );
    m.insert("industry", fa::industry );
    m.insert("info", fa::info );
    m.insert("infocircle", fa::infocircle );
    m.insert("inr", fa::inr );
    m.insert("instagram", fa::instagram );
    m.insert("institution", fa::institution );
    m.insert("internetexplorer", fa::internetexplorer );
    m.insert("intersex", fa::intersex );
    m.insert("ioxhost", fa::ioxhost );
    m.insert("italic", fa::italic );
    m.insert("joomla", fa::joomla );
    m.insert("jpy", fa::jpy );
    m.insert("jsfiddle", fa::jsfiddle );
    m.insert("key", fa::key );
    m.insert("keyboardo", fa::keyboardo );
    m.insert("krw", fa::krw );
    m.insert("language", fa::language );
    m.insert("laptop", fa::laptop );
    m.insert("lastfm", fa::lastfm );
    m.insert("lastfmsquare", fa::lastfmsquare );
    m.insert("leaf", fa::leaf );
    m.insert("leanpub", fa::leanpub );
    m.insert("legal", fa::legal );
    m.insert("lemono", fa::lemono );
    m.insert("leveldown", fa::leveldown );
    m.insert("levelup", fa::levelup );
    m.insert("lifebouy", fa::lifebouy );
    m.insert("lifebuoy", fa::lifebuoy );
    m.insert("lifering", fa::lifering );
    m.insert("lifesaver", fa::lifesaver );
    m.insert("lightbulbo", fa::lightbulbo );
    m.insert("linechart", fa::linechart );
    m.insert("link", fa::link );
    m.insert("linkedin", fa::linkedin );
    m.insert("linkedinsquare", fa::linkedinsquare );
    m.insert("linux", fa::linux );
    m.insert("list", fa::list );
    m.insert("listalt", fa::listalt );
    m.insert("listol", fa::listol );
    m.insert("listul", fa::listul );
    m.insert("locationarrow", fa::locationarrow );
    m.insert("lock", fa::lock );
    m.insert("longarrowdown", fa::longarrowdown );
    m.insert("longarrowleft", fa::longarrowleft );
    m.insert("longarrowright", fa::longarrowright );
    m.insert("longarrowup", fa::longarrowup );
    m.insert("magic", fa::magic );
    m.insert("magnet", fa::magnet );
    m.insert("mailforward", fa::mailforward );
    m.insert("mailreply", fa::mailreply );
    m.insert("mailreplyall", fa::mailreplyall );
    m.insert("male", fa::male );
    m.insert("map", fa::map );
    m.insert("mapmarker", fa::mapmarker );
    m.insert("mapo", fa::mapo );
    m.insert("mappin", fa::mappin );
    m.insert("mapsigns", fa::mapsigns );
    m.insert("mars", fa::mars );
    m.insert("marsdouble", fa::marsdouble );
    m.insert("marsstroke", fa::marsstroke );
    m.insert("marsstrokeh", fa::marsstrokeh );
    m.insert("marsstrokev", fa::marsstrokev );
    m.insert("maxcdn", fa::maxcdn );
    m.insert("meanpath", fa::meanpath );
    m.insert("medium", fa::medium );
    m.insert("medkit", fa::medkit );
    m.insert("meho", fa::meho );
    m.insert("mercury", fa::mercury );
    m.insert("microphone", fa::microphone );
    m.insert("microphoneslash", fa::microphoneslash );
    m.insert("minus", fa::minus );
    m.insert("minuscircle", fa::minuscircle );
    m.insert("minussquare", fa::minussquare );
    m.insert("minussquareo", fa::minussquareo );
    m.insert("mobile", fa::mobile );
    m.insert("mobilephone", fa::mobilephone );
    m.insert("money", fa::money );
    m.insert("moono", fa::moono );
    m.insert("mortarboard", fa::mortarboard );
    m.insert("motorcycle", fa::motorcycle );
    m.insert("mousepointer", fa::mousepointer );
    m.insert("music", fa::music );
    m.insert("navicon", fa::navicon );
    m.insert("neuter", fa::neuter );
    m.insert("newspapero", fa::newspapero );
    m.insert("objectgroup", fa::objectgroup );
    m.insert("objectungroup", fa::objectungroup );
    m.insert("odnoklassniki", fa::odnoklassniki );
    m.insert("odnoklassnikisquare", fa::odnoklassnikisquare );
    m.insert("opencart", fa::opencart );
    m.insert("openid", fa::openid );
    m.insert("opera", fa::opera );
    m.insert("optinmonster", fa::optinmonster );
    m.insert("outdent", fa::outdent );
    m.insert("pagelines", fa::pagelines );
    m.insert("paintbrush", fa::paintbrush );
    m.insert("paperplane", fa::paperplane );
    m.insert("paperplaneo", fa::paperplaneo );
    m.insert("paperclip", fa::paperclip );
    m.insert("paragraph", fa::paragraph );
    m.insert("paste", fa::paste );
    m.insert("pause", fa::pause );
    m.insert("paw", fa::paw );
    m.insert("paypal", fa::paypal );
    m.insert("pencil", fa::pencil );
    m.insert("pencilsquare", fa::pencilsquare );
    m.insert("pencilsquareo", fa::pencilsquareo );
    m.insert("phone", fa::phone );
    m.insert("phonesquare", fa::phonesquare );
    m.insert("photo", fa::photo );
    m.insert("pictureo", fa::pictureo );
    m.insert("piechart", fa::piechart );
    m.insert("piedpiper", fa::piedpiper );
    m.insert("piedpiperalt", fa::piedpiperalt );
    m.insert("pinterest", fa::pinterest );
    m.insert("pinterestp", fa::pinterestp );
    m.insert("pinterestsquare", fa::pinterestsquare );
    m.insert("plane", fa::plane );
    m.insert("play", fa::play );
    m.insert("playcircle", fa::playcircle );
    m.insert("playcircleo", fa::playcircleo );
    m.insert("plug", fa::plug );
    m.insert("plus", fa::plus );
    m.insert("pluscircle", fa::pluscircle );
    m.insert("plussquare", fa::plussquare );
    m.insert("plussquareo", fa::plussquareo );
    m.insert("poweroff", fa::poweroff );
    m.insert("print", fa::print );
    m.insert("puzzlepiece", fa::puzzlepiece );
    m.insert("qq", fa::qq );
    m.insert("qrcode", fa::qrcode );
    m.insert("question", fa::question );
    m.insert("questioncircle", fa::questioncircle );
    m.insert("quoteleft", fa::quoteleft );
    m.insert("quoteright", fa::quoteright );
    m.insert("ra", fa::ra );
    m.insert("random", fa::random );
    m.insert("rebel", fa::rebel );
    m.insert("recycle", fa::recycle );
    m.insert("reddit", fa::reddit );
    m.insert("redditsquare", fa::redditsquare );
    m.insert("refresh", fa::refresh );
    m.insert("registered", fa::registered );
    m.insert("remove", fa::remove );
    m.insert("renren", fa::renren );
    m.insert("reorder", fa::reorder );
    m.insert("repeat", fa::repeat );
    m.insert("reply", fa::reply );
    m.insert("replyall", fa::replyall );
    m.insert("retweet", fa::retweet );
    m.insert("rmb", fa::rmb );
    m.insert("road", fa::road );
    m.insert("rocket", fa::rocket );
    m.insert("rotateleft", fa::rotateleft );
    m.insert("rotateright", fa::rotateright );
    m.insert("rouble", fa::rouble );
    m.insert("rss", fa::rss );
    m.insert("rsssquare", fa::rsssquare );
    m.insert("rub", fa::rub );
    m.insert("ruble", fa::ruble );
    m.insert("rupee", fa::rupee );
    m.insert("safari", fa::safari );
    m.insert("save", fa::save );
    m.insert("scissors", fa::scissors );
    m.insert("search", fa::search );
    m.insert("searchminus", fa::searchminus );
    m.insert("searchplus", fa::searchplus );
    m.insert("sellsy", fa::sellsy );
    m.insert("send", fa::send );
    m.insert("sendo", fa::sendo );
    m.insert("server", fa::server );
    m.insert("share", fa::share );
    m.insert("sharealt", fa::sharealt );
    m.insert("sharealtsquare", fa::sharealtsquare );
    m.insert("sharesquare", fa::sharesquare );
    m.insert("sharesquareo", fa::sharesquareo );
    m.insert("shekel", fa::shekel );
    m.insert("sheqel", fa::sheqel );
    m.insert("shield", fa::shield );
    m.insert("ship", fa::ship );
    m.insert("shirtsinbulk", fa::shirtsinbulk );
    m.insert("shoppingcart", fa::shoppingcart );
    m.insert("signin", fa::signin );
    m.insert("signout", fa::signout );
    m.insert("signal", fa::signal );
    m.insert("simplybuilt", fa::simplybuilt );
    m.insert("sitemap", fa::sitemap );
    m.insert("skyatlas", fa::skyatlas );
    m.insert("skype", fa::skype );
    m.insert("slack", fa::slack );
    m.insert("sliders", fa::sliders );
    m.insert("slideshare", fa::slideshare );
    m.insert("smileo", fa::smileo );
    m.insert("soccerballo", fa::soccerballo );
    m.insert("sort", fa::sort );
    m.insert("sortalphaasc", fa::sortalphaasc );
    m.insert("sortalphadesc", fa::sortalphadesc );
    m.insert("sortamountasc", fa::sortamountasc );
    m.insert("sortamountdesc", fa::sortamountdesc );
    m.insert("sortasc", fa::sortasc );
    m.insert("sortdesc", fa::sortdesc );
    m.insert("sortdown", fa::sortdown );
    m.insert("sortnumericasc", fa::sortnumericasc );
    m.insert("sortnumericdesc", fa::sortnumericdesc );
    m.insert("sortup", fa::sortup );
    m.insert("soundcloud", fa::soundcloud );
    m.insert("spaceshuttle", fa::spaceshuttle );
    m.insert("spinner", fa::spinner );
    m.insert("spoon", fa::spoon );
    m.insert("spotify", fa::spotify );
    m.insert("square", fa::square );
    m.insert("squareo", fa::squareo );
    m.insert("stackexchange", fa::stackexchange );
    m.insert("stackoverflow", fa::stackoverflow );
    m.insert("star", fa::star );
    m.insert("starhalf", fa::starhalf );
    m.insert("starhalfempty", fa::starhalfempty );
    m.insert("starhalffull", fa::starhalffull );
    m.insert("starhalfo", fa::starhalfo );
    m.insert("staro", fa::staro );
    m.insert("steam", fa::steam );
    m.insert("steamsquare", fa::steamsquare );
    m.insert("stepbackward", fa::stepbackward );
    m.insert("stepforward", fa::stepforward );
    m.insert("stethoscope", fa::stethoscope );
    m.insert("stickynote", fa::stickynote );
    m.insert("stickynoteo", fa::stickynoteo );
    m.insert("stop", fa::stop );
    m.insert("streetview", fa::streetview );
    m.insert("strikethrough", fa::strikethrough );
    m.insert("stumbleupon", fa::stumbleupon );
    m.insert("stumbleuponcircle", fa::stumbleuponcircle );
    m.insert("subscript", fa::subscript );
    m.insert("subway", fa::subway );
    m.insert("suitcase", fa::suitcase );
    m.insert("suno", fa::suno );
    m.insert("superscript", fa::superscript );
    m.insert("support", fa::support );
    m.insert("table", fa::table );
    m.insert("tablet", fa::tablet );
    m.insert("tachometer", fa::tachometer );
    m.insert("tag", fa::tag );
    m.insert("tags", fa::tags );
    m.insert("tasks", fa::tasks );
    m.insert("taxi", fa::taxi );
    m.insert("television", fa::television );
    m.insert("tencentweibo", fa::tencentweibo );
    m.insert("terminal", fa::terminal );
    m.insert("textheight", fa::textheight );
    m.insert("textwidth", fa::textwidth );
    m.insert("th", fa::th );
    m.insert("thlarge", fa::thlarge );
    m.insert("thlist", fa::thlist );
    m.insert("thumbtack", fa::thumbtack );
    m.insert("thumbsdown", fa::thumbsdown );
    m.insert("thumbsodown", fa::thumbsodown );
    m.insert("thumbsoup", fa::thumbsoup );
    m.insert("thumbsup", fa::thumbsup );
    m.insert("ticket", fa::ticket );
    m.insert("times", fa::times );
    m.insert("timescircle", fa::timescircle );
    m.insert("timescircleo", fa::timescircleo );
    m.insert("tint", fa::tint );
    m.insert("toggledown", fa::toggledown );
    m.insert("toggleleft", fa::toggleleft );
    m.insert("toggleoff", fa::toggleoff );
    m.insert("toggleon", fa::toggleon );
    m.insert("toggleright", fa::toggleright );
    m.insert("toggleup", fa::toggleup );
    m.insert("trademark", fa::trademark );
    m.insert("train", fa::train );
    m.insert("transgender", fa::transgender );
    m.insert("transgenderalt", fa::transgenderalt );
    m.insert("trash", fa::trash );
    m.insert("trasho", fa::trasho );
    m.insert("tree", fa::tree );
    m.insert("trello", fa::trello );
    m.insert("tripadvisor", fa::tripadvisor );
    m.insert("trophy", fa::trophy );
    m.insert("truck", fa::truck );
    m.insert("fa_try", fa::fa_try );
    m.insert("tty", fa::tty );
    m.insert("tumblr", fa::tumblr );
    m.insert("tumblrsquare", fa::tumblrsquare );
    m.insert("turkishlira", fa::turkishlira );
    m.insert("tv", fa::tv );
    m.insert("twitch", fa::twitch );
    m.insert("twitter", fa::twitter );
    m.insert("twittersquare", fa::twittersquare );
    m.insert("umbrella", fa::umbrella );
    m.insert("underline", fa::underline );
    m.insert("undo", fa::undo );
    m.insert("university", fa::university );
    m.insert("unlink", fa::unlink );
    m.insert("unlock", fa::unlock );
    m.insert("unlockalt", fa::unlockalt );
    m.insert("unsorted", fa::unsorted );
    m.insert("upload", fa::upload );
    m.insert("usd", fa::usd );
    m.insert("user", fa::user );
    m.insert("usermd", fa::usermd );
    m.insert("userplus", fa::userplus );
    m.insert("usersecret", fa::usersecret );
    m.insert("usertimes", fa::usertimes );
    m.insert("users", fa::users );
    m.insert("venus", fa::venus );
    m.insert("venusdouble", fa::venusdouble );
    m.insert("venusmars", fa::venusmars );
    m.insert("viacoin", fa::viacoin );
    m.insert("videocamera", fa::videocamera );
    m.insert("vimeo", fa::vimeo );
    m.insert("vimeosquare", fa::vimeosquare );
    m.insert("vine", fa::vine );
    m.insert("vk", fa::vk );
    m.insert("volumedown", fa::volumedown );
    m.insert("volumeoff", fa::volumeoff );
    m.insert("volumeup", fa::volumeup );
    m.insert("warning", fa::warning );
    m.insert("wechat", fa::wechat );
    m.insert("weibo", fa::weibo );
    m.insert("weixin", fa::weixin );
    m.insert("whatsapp", fa::whatsapp );
    m.insert("wheelchair", fa::wheelchair );
    m.insert("wifi", fa::wifi );
    m.insert("wikipediaw", fa::wikipediaw );
    m.insert("windows", fa::windows );
    m.insert("won", fa::won );
    m.insert("wordpress", fa::wordpress );
    m.insert("wrench", fa::wrench );
    m.insert("xing", fa::xing );
    m.insert("xingsquare", fa::xingsquare );
    m.insert("ycombinator", fa::ycombinator );
    m.insert("ycombinatorsquare", fa::ycombinatorsquare );
    m.insert("yahoo", fa::yahoo );
    m.insert("yc", fa::yc );
    m.insert("ycsquare", fa::ycsquare );
    m.insert("yelp", fa::yelp );
    m.insert("yen", fa::yen );
    m.insert("youtube", fa::youtube );
    m.insert("youtubeplay", fa::youtubeplay );
    m.insert("youtubesquare", fa::youtubesquare );

    return true;
}

void QtAwesome::addNamedCodepoint( const QString& name, int codePoint)
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
QIcon QtAwesome::icon(int character, const QVariantMap &options)
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
