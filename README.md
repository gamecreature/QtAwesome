QtAwesome - FontAwesome support for Qt applications
===================================================

Description
-----------

QtAwesome is a simple library that can be used to add font-awesome icons to your Qt application.
Though the name is QtAwesome and currently it's very FontAwesome based, you can use every other 
iconfont you want.

The class can also be used to manage your own dynamic code-drawn icons, by adding named icon-painters.


Installation
------------

The easiest way to include QtAweome in your project is to copy the QtAwesome directory to your
project tree and add the the following code include to your Qt project file: 

    include(QtAwesome/QtAwesome.pri)

Now your good to go!!


Usage
-----

You probably want to create a single QtAwesome object for your whole application. 

    QtAwesome* awesome = new QtAwesome( qApp )
    awesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map

Add an accessor to this object. (a global function, membor of your application object whatever you like).


Example
--------
  
    // You should create a single object of QtAwesome.
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();

    // Next create your icon with the help of the icon-enumeration: (all dashes are replaced by underscores)
    QPushButton* beerButton new QPushButton( awesome->icon( icon_beer ), "Cheers!" );

    // You can also use 'string' names to access the icons. (The string version omits the 'icon-' prefix )
    QPushButton* coffeeButton new QPushButton( awesome->icon( "coffee" ), "Black please!" );

    // When you create an icon you can supply some options for your icons:
    // The available options can be found at the "Default options"-section

    QVariantMap options;
    options.insert( "color" , QColor(255,0,0) );
    QPushButton* musicButton = new QPushButton( awesome->icon( icon_music ), "Music" ); 

    // You can also change the default options. 
    // for example if you always would like to have green icons you could call)
    awesome->setDefaultOption( "color-disabled", QColor(0,255,0) );



Default options:
----------------
  
  The following options are default in the QtAwesome class. 

    setDefaultOption( "color", QColor(50,50,50) );
    setDefaultOption( "color-disabled", QColor(70,70,70,60));
    setDefaultOption( "color-active", QColor(10,10,10));
    setDefaultOption( "color-selected", QColor(10,10,10));
    setDefaultOption( "scale-factor", 0.9 );

  When creating an icon, it first populates the options-map with the default options from the QtAwesome object.
  After that the options are expanded/overwritten by the options supplied to the icon.


License
-------

MIT License. Copyright 2013 - Reliable Bits Software by Blommers IT. [http://blommersit.nl/](http://blommersit.nl)

The Font Awesome font is licensed under the SIL Open Font License - [http://scripts.sil.org/OFL](http://scripts.sil.org/OFL)
The Font Awesome pictograms are licensed under the CC BY 3.0 License - [http://creativecommons.org/licenses/by/3.0/](http://creativecommons.org/licenses/by/3.0/)
"Font Awesome by Dave Gandy - http://fortawesome.github.com/Font-Awesome"

Contact
-------

* email: <rick@blommersit.nl>
* twitter: [https://twitter.com/gamecreature](https://twitter.com/gamecreature)
* website: [http://blommersit.nl](http://blommersit.nl)  (warning Dutch content ahead)
* github: [https://github.com/gamecreature/QtAwesome](https://github.com/gamecreature/QtAwesome)

Remarks
-------

I've created this project because I needed some nice icons for my own Qt project. After doing a lot of 
css/html5 work and being spoiled by the ease of twitter bootstrap with FontAwesome, 
I thought it would be nice to be able to use these icons for my Qt project.

I've slightly changed the code from the original, added some more documentation, but it's still
a work in progress. So feel free to drop me an e-mail for your suggestions and improvements! 

There are still some things todo, like:

  * document the usage of a custom icon painter
  * document the usage of another icon font
  * add some tests
  * do some code cleanup
  
And of course last but not least, 

Many thanks go to Dave Gandy an the other FontAwesome contributors!! [http://fortawesome.github.com/Font-Awesome](http://fortawesome.github.com/Font-Awesome)  
And of course to the Qt team/contributors for supplying this great cross-platform c++ library.


Contributions are welcome! Feel free to fork and send a pull request through Github.
