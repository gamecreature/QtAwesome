/**
 * MIT Licensed
 *
 * Copyright 2011-2015 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "QtAwesome.h"

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow w;

    QtAwesome* awesome = new QtAwesome(&w);
    awesome->initFontAwesome();

    // a simple beer button
    QPushButton* beerButton = new QPushButton( "Cheers!");


    QVariantMap options;
    options.insert("anim", qVariantFromValue( new QtAwesomeAnimation(beerButton) ) );
    beerButton->setIcon( awesome->icon( fa::beer, options  ) );

    w.setCentralWidget( beerButton );
    w.show();
    
    return app.exec();
}
