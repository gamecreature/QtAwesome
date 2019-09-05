#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtAwesome.h"
#include <QStandardItemModel>
#include <QMap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    awesome = new QtAwesome(this);
    awesome->initFontAwesome();

    for(int st=style::stfas; st<=style::stfab; st++){
        switch(st){
        case style::stfab:
            ui->comboBox->insertItem(style::stfab, "Brands", style::stfab);
            break;
        case style::stfar:
            ui->comboBox->insertItem(style::stfar, "Regular", style::stfar);
            break;
        case style::stfas:
            ui->comboBox->insertItem(style::stfas, "Solid", style::stfas);
            break;
#ifdef FONT_AWESOME_PRO
        case style::stfal:
            ui->comboBox->insertItem(style::stfal, "Light", style::stfal);
            break;
        case style::stfad:
            ui->comboBox->insertItem(style::stfad, "Duotone", style::stfad);
            break;
#endif
        }
    }

    // a simple beer button
    //=====================
    {
        QPushButton* beerButton = ui->beerButton;

        QVariantMap options;
        options.insert("anim", qVariantFromValue( new QtAwesomeAnimation(beerButton) ) );
        beerButton->setIcon( awesome->icon( "fas beer", options  ) );
    }

    // a simple beer checkbox button
    //==============================
    {
        QPushButton* toggleButton = ui->toggleButton;
        toggleButton->setCheckable(true);

        QVariantMap options;
        options.insert("color", QColor(Qt::green) );
        options.insert("text-off", QString(fa::square) );
        options.insert("color-off", QColor(Qt::red) );
        toggleButton->setIcon( awesome->icon( "far check-square", options ));
    }

    QStandardItemModel *model = new QStandardItemModel(this);
    ui->listView->setModel(model);

    connect(ui->comboBox, SIGNAL(activated(int)), this, SLOT(styleChanged(int)));
    ui->comboBox->setCurrentIndex(style::stfab);
    styleChanged(style::stfab);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::styleChanged(int index)
{

    QHash<QString, int> iconset = awesome->namedCodePoints(static_cast<style::styles>(index));

    QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(ui->listView->model());
    model->clear();

    for(QHash<QString, int>::iterator i=iconset.begin();i!=iconset.end();++i)
    {
        QString name = i.key();
        int ic = i.value();

        model->appendRow(new QStandardItem(awesome->icon(index, ic), name));
    }
    model->sort(0);
}
