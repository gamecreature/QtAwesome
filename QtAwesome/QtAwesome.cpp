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


Q_INIT_RESOURCE_EXTERN(QtAwesome)

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
        QFile res(":/fonts/fontawesome.ttf");
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
    m.insert( "glass",           icon_glass );
    m.insert( "music",           icon_music );
    m.insert( "search",          icon_search );
    m.insert( "envelope",        icon_envelope );
    m.insert( "heart",           icon_heart );
    m.insert( "star",            icon_star );
    m.insert( "star-empty",      icon_star_empty );
    m.insert( "user",            icon_user );
    m.insert( "film",            icon_film );
    m.insert( "th-large",        icon_th_large );
    m.insert( "th",              icon_th );
    m.insert( "th-list",         icon_th_list );
    m.insert( "ok",              icon_ok );
    m.insert( "remove",          icon_remove );
    m.insert( "zoom-in",         icon_zoom_in );

    m.insert( "zoom-out",        icon_zoom_out );
    m.insert( "off",             icon_off );
    m.insert( "signal",          icon_signal );
    m.insert( "cog",             icon_cog );
    m.insert( "gear",            icon_gear );
    m.insert( "trash",           icon_trash );
    m.insert( "home",            icon_home );
    m.insert( "file_alt",        icon_file_alt );
    m.insert( "time",            icon_time );
    m.insert( "road",            icon_road );
    m.insert( "download-alt",    icon_download_alt );
    m.insert( "download",        icon_download );
    m.insert( "upload",          icon_upload );
    m.insert( "inbox",           icon_inbox );
    m.insert( "play-circle",     icon_play_circle );
    m.insert( "repeat",          icon_repeat );

    /* \f020 doesn't work in Safari. all shifted one down */


    m.insert( "refresh",             icon_refresh );
    m.insert( "list-alt",            icon_list_alt );
    m.insert( "lock",                icon_lock );
    m.insert( "flag",                icon_flag );
    m.insert( "headphones",          icon_headphones );
    m.insert( "volume-off",          icon_volume_off );
    m.insert( "volume-down",         icon_volume_down );
    m.insert( "volume-up",           icon_volume_up );
    m.insert( "qrcode",              icon_qrcode );
    m.insert( "barcode",             icon_barcode );
    m.insert( "tag",                 icon_tag );
    m.insert( "tags",                icon_tags );
    m.insert( "book",                icon_book );
    m.insert( "bookmark",            icon_bookmark );
    m.insert( "print",               icon_print );

    m.insert( "camera",              icon_camera );
    m.insert( "font",                icon_font );
    m.insert( "bold",                icon_bold );
    m.insert( "italic",              icon_italic );
    m.insert( "text-height",         icon_text_height );
    m.insert( "text-width",          icon_text_width );
    m.insert( "align-left",          icon_align_left );
    m.insert( "align-center",        icon_align_center );
    m.insert( "align-right",         icon_align_right );
    m.insert( "align-justify",       icon_align_justify );
    m.insert( "list",                icon_list );
    m.insert( "indent-left",         icon_indent_left );
    m.insert( "indent-right",        icon_indent_right );
    m.insert( "facetime-video",      icon_facetime_video );
    m.insert( "picture",             icon_picture );

    m.insert( "pencil",               icon_pencil );
    m.insert( "map-marker",           icon_map_marker );
    m.insert( "adjust",               icon_adjust );
    m.insert( "tint",                 icon_tint );
    m.insert( "edit",                 icon_edit );
    m.insert( "share",                icon_share );
    m.insert( "check",                icon_check );
    m.insert( "move",                 icon_move );
    m.insert( "step-backward",        icon_step_backward );
    m.insert( "fast-backward",        icon_fast_backward );
    m.insert( "backward",             icon_backward );
    m.insert( "play",                 icon_play );
    m.insert( "pause",                icon_pause );
    m.insert( "stop",                 icon_stop );
    m.insert( "forward",              icon_forward );

    m.insert( "fast-forward",         icon_fast_forward );
    m.insert( "step-forward",         icon_step_forward );
    m.insert( "eject",                icon_eject );
    m.insert( "chevron-left",         icon_chevron_left );
    m.insert( "chevron-right",        icon_chevron_right );
    m.insert( "plus-sign",            icon_plus_sign );
    m.insert( "minus-sign",           icon_minus_sign );
    m.insert( "remove-sign",          icon_remove_sign );
    m.insert( "ok-sign",              icon_ok_sign );
    m.insert( "question-sign",        icon_question_sign );
    m.insert( "info-sign",            icon_info_sign );
    m.insert( "screenshot",           icon_screenshot );
    m.insert( "remove-circle",        icon_remove_circle );
    m.insert( "ok-circle",            icon_ok_circle );
    m.insert( "ban-circle",           icon_ban_circle );

    m.insert( "arrow-left",           icon_arrow_left );
    m.insert( "arrow-right",          icon_arrow_right );
    m.insert( "arrow-up",             icon_arrow_up );
    m.insert( "arrow-down",           icon_arrow_down );
    m.insert( "share-alt",            icon_share_alt );
    m.insert( "resize-full",          icon_resize_full );
    m.insert( "resize-small",         icon_resize_small );
    m.insert( "plus",                 icon_plus );
    m.insert( "minus",                icon_minus );
    m.insert( "asterisk",             icon_asterisk );
    m.insert( "exclamation-sign",     icon_exclamation_sign );
    m.insert( "gift",                 icon_gift );
    m.insert( "leaf",                 icon_leaf );
    m.insert( "fire",                 icon_fire );
    m.insert( "eye-open",             icon_eye_open );

    m.insert( "eye-close",            icon_eye_close );
    m.insert( "warning-sign",         icon_warning_sign );
    m.insert( "plane",                icon_plane );
    m.insert( "calendar",             icon_calendar );
    m.insert( "random",               icon_random );
    m.insert( "comment",              icon_comment );
    m.insert( "magnet",               icon_magnet );
    m.insert( "chevron-up",           icon_chevron_up );
    m.insert( "chevron-down",         icon_chevron_down );
    m.insert( "retweet",              icon_retweet );
    m.insert( "shopping-cart",        icon_shopping_cart );
    m.insert( "folder-close",         icon_folder_close );
    m.insert( "folder-open",          icon_folder_open );
    m.insert( "resize-vertical",      icon_resize_vertical );
    m.insert( "resize-horizontal",    icon_resize_horizontal );

    m.insert( "bar-chart",            icon_bar_chart );
    m.insert( "twitter-sign",         icon_twitter_sign );
    m.insert( "facebook-sign",        icon_facebook_sign );
    m.insert( "camera-retro",         icon_camera_retro );
    m.insert( "key",                  icon_key );
    m.insert( "cogs",                 icon_cogs );
    m.insert( "gears",                icon_gears );
    m.insert( "comments",             icon_comments );
    m.insert( "thumbs-up-alt",        icon_thumbs_up_alt );
    m.insert( "thumbs-down-alt",      icon_thumbs_down_alt );
    m.insert( "star-half",            icon_star_half );
    m.insert( "heart-empty",          icon_heart_empty );
    m.insert( "signout",              icon_signout );
    m.insert( "linkedin-sign",        icon_linkedin_sign );
    m.insert( "pushpin",              icon_pushpin );
    m.insert( "external-link",        icon_external_link );

    m.insert( "signin",               icon_signin );
    m.insert( "trophy",               icon_trophy );
    m.insert( "github-sign",          icon_github_sign );
    m.insert( "upload-alt",           icon_upload_alt );
    m.insert( "lemon",                icon_lemon );
    m.insert( "phone",                icon_phone );
    m.insert( "check-empty",          icon_check_empty );
    m.insert( "bookmark-empty",       icon_bookmark_empty );
    m.insert( "phone-sign",           icon_phone_sign );
    m.insert( "twitter",              icon_twitter );
    m.insert( "facebook",             icon_facebook );
    m.insert( "github",               icon_github );
    m.insert( "unlock",               icon_unlock );
    m.insert( "credit-card",          icon_credit_card );
    m.insert( "rss",                  icon_rss );

    m.insert( "hdd",                  icon_hdd );
    m.insert( "bullhorn",             icon_bullhorn );
    m.insert( "bell",                 icon_bell );
    m.insert( "certificate",          icon_certificate );
    m.insert( "hand-right",           icon_hand_right );
    m.insert( "hand-left",            icon_hand_left );
    m.insert( "hand-up",              icon_hand_up );
    m.insert( "hand-down",            icon_hand_down );
    m.insert( "circle-arrow-left",    icon_circle_arrow_left );
    m.insert( "circle-arrow-right",   icon_circle_arrow_right );
    m.insert( "circle-arrow-up",      icon_circle_arrow_up );
    m.insert( "circle-arrow-down",    icon_circle_arrow_down );
    m.insert( "globe",                icon_globe );
    m.insert( "wrench",               icon_wrench );
    m.insert( "tasks",                icon_tasks );

    m.insert( "filter",               icon_filter );
    m.insert( "briefcase",            icon_briefcase );
    m.insert( "fullscreen",           icon_fullscreen );

    m.insert( "group",                icon_group );
    m.insert( "link",                 icon_link );
    m.insert( "cloud",                icon_cloud );
    m.insert( "beaker",               icon_beaker );
    m.insert( "cut",                  icon_cut );
    m.insert( "copy",                 icon_copy );
    m.insert( "paper-clip",           icon_paper_clip );
    m.insert( "save",                 icon_save );
    m.insert( "sign-blank",           icon_sign_blank );
    m.insert( "reorder",              icon_reorder );
    m.insert( "list-ul",              icon_list_ul );
    m.insert( "list-ol",              icon_list_ol );
    m.insert( "strikethrough",        icon_strikethrough );
    m.insert( "underline",            icon_underline );
    m.insert( "table",                icon_table );

    m.insert( "magic",                icon_magic );
    m.insert( "truck",                icon_truck );
    m.insert( "pinterest",            icon_pinterest );
    m.insert( "pinterest-sign",       icon_pinterest_sign );
    m.insert( "google-plus-sign",     icon_google_plus_sign );
    m.insert( "google-plus",          icon_google_plus );
    m.insert( "money",                icon_money );
    m.insert( "caret-down",           icon_caret_down );
    m.insert( "caret-up",             icon_caret_up );
    m.insert( "caret-left",           icon_caret_left );
    m.insert( "caret-right",          icon_caret_right );
    m.insert( "columns",              icon_columns );
    m.insert( "sort",                 icon_sort );
    m.insert( "sort-down",            icon_sort_down );
    m.insert( "sort-up",              icon_sort_up );

    m.insert( "envelope-alt",         icon_envelope_alt );
    m.insert( "linkedin",             icon_linkedin );
    m.insert( "undo",                 icon_undo );
    m.insert( "legal",                icon_legal );
    m.insert( "dashboard",            icon_dashboard );
    m.insert( "comment-alt",          icon_comment_alt );
    m.insert( "comments-alt",         icon_comments_alt );
    m.insert( "bolt",                 icon_bolt );
    m.insert( "sitemap",              icon_sitemap );
    m.insert( "umbrella",             icon_umbrella );
    m.insert( "paste",                icon_paste );
    m.insert( "lightbulb",            icon_lightbulb );
    m.insert( "exchange",             icon_exchange );
    m.insert( "cloud-download",       icon_cloud_download );
    m.insert( "cloud-upload",         icon_cloud_upload );

    m.insert( "user-md",              icon_user_md );
    m.insert( "stethoscope",          icon_stethoscope );
    m.insert( "suitcase",             icon_suitcase );
    m.insert( "bell-alt",             icon_bell_alt );
    m.insert( "coffee",               icon_coffee );
    m.insert( "food",                 icon_food );
    m.insert( "file-text-alt",        icon_file_text_alt );
    m.insert( "building",             icon_building );
    m.insert( "hospital",             icon_hospital );
    m.insert( "ambulance",            icon_ambulance );
    m.insert( "medkit",               icon_medkit );
    m.insert( "fighter-jet",          icon_fighter_jet );
    m.insert( "beer",                 icon_beer );
    m.insert( "h-sign",               icon_h_sign );
    m.insert( "plus-sign-alt",        icon_plus_sign_alt );

    m.insert( "double-angle-left",    icon_double_angle_left );
    m.insert( "double-angle-right",   icon_double_angle_right );
    m.insert( "double-angle-up",      icon_double_angle_up );
    m.insert( "double-angle-down",    icon_double_angle_down );
    m.insert( "angle-left",           icon_angle_left );
    m.insert( "angle-right",          icon_angle_right );
    m.insert( "angle-up",             icon_angle_up );
    m.insert( "angle-down",           icon_angle_down );
    m.insert( "desktop",              icon_desktop );
    m.insert( "laptop",               icon_laptop );
    m.insert( "tablet",               icon_tablet );
    m.insert( "mobile-phone",         icon_mobile_phone );
    m.insert( "circle-blank",         icon_circle_blank );
    m.insert( "quote-left",           icon_quote_left );
    m.insert( "quote-right",          icon_quote_right );

    m.insert( "spinner",              icon_spinner );
    m.insert( "circle",               icon_circle );
    m.insert( "reply",                icon_reply );
    m.insert( "mail_reply",           icon_mail_reply );

    m.insert( "github-alt",           icon_github_alt );
    m.insert( "folder-close-alt",     icon_folder_close_alt );
    m.insert( "folder-open-alt",      icon_folder_open_alt );

    m.insert( "expand_alt",      icon_expand_alt );
    m.insert( "collapse_alt",    icon_collapse_alt );
    m.insert( "smile",           icon_smile );
    m.insert( "frown",           icon_frown );
    m.insert( "meh",             icon_meh );
    m.insert( "gamepad",         icon_gamepad );
    m.insert( "keyboard",        icon_keyboard );
    m.insert( "flag_alt",        icon_flag_alt );
    m.insert( "flag_checkered",  icon_flag_checkered );

    m.insert( "terminal",        icon_terminal );
    m.insert( "code",            icon_code );
    m.insert( "reply_all",       icon_reply_all );
    m.insert( "mail_reply_all",  icon_mail_reply_all );
    m.insert( "star_half_full",  icon_star_half_full );
    m.insert( "star_half_empty", icon_star_half_empty );
    m.insert( "location_arrow",  icon_location_arrow );
    m.insert( "crop",            icon_crop );
    m.insert( "code_fork",       icon_code_fork );
    m.insert( "unlink",          icon_unlink );
    m.insert( "question",        icon_question );
    m.insert( "info",            icon_info );
    m.insert( "exclamation",     icon_exclamation );
    m.insert( "superscript",     icon_superscript );
    m.insert( "subscript",       icon_subscript );
    m.insert( "eraser",          icon_eraser );
    m.insert( "puzzle_piece",    icon_puzzle_piece );

    m.insert( "microphone",         icon_microphone );
    m.insert( "microphone_off",     icon_microphone_off );
    m.insert( "shield",             icon_shield );
    m.insert( "calendar_empty",     icon_calendar_empty );
    m.insert( "fire_extinguisher",  icon_fire_extinguisher );
    m.insert( "rocket",             icon_rocket );
    m.insert( "maxcdn",             icon_maxcdn );
    m.insert( "chevron_sign_left",  icon_chevron_sign_left );
    m.insert( "chevron_sign_right", icon_chevron_sign_right );
    m.insert( "chevron_sign_up",    icon_chevron_sign_up );
    m.insert( "chevron_sign_down",  icon_chevron_sign_down );
    m.insert( "html5",              icon_html5 );
    m.insert( "css3",               icon_css3 );
    m.insert( "anchor",             icon_anchor );
    m.insert( "unlock_alt",         icon_unlock_alt );

    m.insert( "bullseye",            icon_bullseye );
    m.insert( "ellipsis_horizontal", icon_ellipsis_horizontal );
    m.insert( "ellipsis_vertical",   icon_ellipsis_vertical );
    m.insert( "rss_sign",            icon_rss_sign );
    m.insert( "play_sign",           icon_play_sign );
    m.insert( "ticket",              icon_ticket );
    m.insert( "minus_sign_alt",      icon_minus_sign_alt );
    m.insert( "check_minus",         icon_check_minus );
    m.insert( "level_up",            icon_level_up );
    m.insert( "level_down",          icon_level_down );
    m.insert( "check_sign",          icon_check_sign );
    m.insert( "edit_sign",           icon_edit_sign );
    m.insert( "external_link_sign",  icon_external_link_sign );
    m.insert( "share_sign",          icon_share_sign );

    // v3.2.0
    m.insert( "compass",    icon_compass );

    m.insert( "collapse",               icon_collapse );
    m.insert( "collapse_top",           icon_collapse_top );
    m.insert( "expand",                 icon_expand );
    m.insert( "euro",                   icon_euro );
    m.insert( "eur",                    icon_eur );
    m.insert( "gbp",                    icon_gbp );
    m.insert( "dollar",                 icon_dollar );
    m.insert( "usd",                    icon_usd );
    m.insert( "rupee",                  icon_rupee );
    m.insert( "inr",                    icon_inr );
    m.insert( "yen",                    icon_yen );
    m.insert( "jpy",                    icon_jpy );
    m.insert( "renminbi",               icon_renminbi );
    m.insert( "cny",                    icon_cny );
    m.insert( "won",                    icon_won );
    m.insert( "krw",                    icon_krw );
    m.insert( "bitcoin",                icon_bitcoin );
    m.insert( "btc",                    icon_btc );
    m.insert( "file",                   icon_file );
    m.insert( "file_text",              icon_file_text );
    m.insert( "sort_by_alphabet",       icon_sort_by_alphabet );
    m.insert( "sort_by_alphabet_alt",   icon_sort_by_alphabet_alt );
    m.insert( "sort_by_attributes",     icon_sort_by_attributes );
    m.insert( "sort_by_attributes_alt", icon_sort_by_attributes_alt );

    m.insert( "sort_by_order",     icon_sort_by_order );
    m.insert( "sort_by_order_alt", icon_sort_by_order_alt );
    m.insert( "thumbs_up",         icon_thumbs_up );
    m.insert( "thumbs_down",       icon_thumbs_down );
    m.insert( "youtube_sign",      icon_youtube_sign );
    m.insert( "youtube",           icon_youtube );
    m.insert( "xing",              icon_xing );
    m.insert( "xing_sign",         icon_xing_sign );
    m.insert( "youtube_play",      icon_youtube_play );
    m.insert( "dropbox",           icon_dropbox );
    m.insert( "stackexchange",     icon_stackexchange );
    m.insert( "instagram",         icon_instagram );
    m.insert( "flickr",            icon_flickr );

    m.insert( "adn",              icon_adn );
    m.insert( "bitbucket",        icon_bitbucket );
    m.insert( "bitbucket_sign",   icon_bitbucket_sign );
    m.insert( "tumblr",           icon_tumblr );
    m.insert( "tumblr_sign",      icon_tumblr_sign );
    m.insert( "long_arrow_down",  icon_long_arrow_down );
    m.insert( "long_arrow_up",    icon_long_arrow_up );
    m.insert( "long_arrow_left",  icon_long_arrow_left );
    m.insert( "long_arrow_right", icon_long_arrow_right );
    m.insert( "apple",            icon_apple );
    m.insert( "windows",          icon_windows );
    m.insert( "android",          icon_android );
    m.insert( "linux",            icon_linux );
    m.insert( "dribble",          icon_dribble );
    m.insert( "skype",            icon_skype );

    m.insert( "foursquare", icon_foursquare );
    m.insert( "trello",     icon_trello );
    m.insert( "female",     icon_female );
    m.insert( "male",       icon_male );
    m.insert( "gittip",     icon_gittip );
    m.insert( "sun",        icon_sun );
    m.insert( "moon",       icon_moon );
    m.insert( "archive",    icon_archive );
    m.insert( "bug",        icon_bug );
    m.insert( "vk",         icon_vk );
    m.insert( "weibo",      icon_weibo );
    m.insert( "renren",     icon_renren );

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
    optionMap.insert("text", QString( QChar(character) ) );

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
