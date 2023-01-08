#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtAwesome.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void styleChanged(int index);

private:
    Ui::MainWindow *ui;
    fa::QtAwesome* awesome;
};

#endif // MAINWINDOW_H
