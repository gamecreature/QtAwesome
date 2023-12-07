#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtAwesome.h"
#include <QStandardItemModel>
#include <QMap>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    awesome = new fa::QtAwesome(this);
    awesome->initFontAwesome();

    ui->comboBox->addItem("Solid", fa::fa_solid);
    ui->comboBox->addItem("Brands", fa::fa_brands);
    ui->comboBox->addItem("Regular", fa::fa_regular);
#ifdef FONT_AWESOME_PRO
    ui->comboBox->addItem("Light", fa::fa_light);
    ui->comboBox->addItem("Thin", fa::fa_thin);
    ui->comboBox->addItem("Duotone", fa::fa_duotone);
    ui->comboBox->addItem("Sharp Solid", fa::fa_sharp_solid);
    ui->comboBox->addItem("Sharp Regular", fa::fa_sharp_regular);
    ui->comboBox->addItem("Sharp Light", fa::fa_sharp_light);
    ui->comboBox->addItem("Sharp Thin", fa::fa_sharp_thin);
#endif

    // a simple beer button
    //=====================
    {
        QPushButton* beerButton = ui->beerButton;

        QVariantMap options;
        options.insert("anim", QVariant::fromValue(new fa::QtAwesomeAnimation(beerButton)));

        // below are the possible variation to show thi icon
         beerButton->setIcon(awesome->icon(fa::fa_solid, fa::fa_beer_mug_empty, options));
        // beerButton->setIcon(awesome->icon("fa-solid fa-beer-mug-empty", options));
        // beerButton->setIcon(awesome->icon("beer-mug-empty", options));
        // beerButton->setIcon(awesome->icon("solid beer-mug-empty", options));
    }

    // a simple checkbox button
    //=========================
    {
        QPushButton* toggleButton = ui->toggleButton;
        toggleButton->setCheckable(true);

        QVariantMap options;
        options.insert("color", QColor(Qt::yellow));
        options.insert("text-off", QString(fa::fa_square));
        options.insert("color-off", QColor(Qt::darkBlue));
        toggleButton->setIcon( awesome->icon("fa_solid square-check", options));
    }

    QStandardItemModel* model = new QStandardItemModel(this);
    ui->listView->setModel(model);

    connect(ui->comboBox, SIGNAL(activated(int)), this, SLOT(styleChanged(int)));
    styleChanged(fa::fa_solid);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::styleChanged(int index)
{
    Q_UNUSED(index)
    QVariant styleValue = ui->comboBox->currentData();
    int style = styleValue.toInt();

    QHash<QString, int> iconset = awesome->namedCodePoints(style);

    QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(ui->listView->model());
    model->clear();

    for (QHash<QString, int>::iterator i=iconset.begin(); i != iconset.end(); ++i) {
        QString name = i.key();
        int ic = i.value();
        model->appendRow(new QStandardItem(awesome->icon(style, ic), name));
    }
    model->sort(0);
}
