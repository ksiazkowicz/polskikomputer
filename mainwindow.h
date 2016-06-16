#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void update_dialog(QString label);
    void update_indicator(QString label);
    void update_score(QString label);
    void update_debug(QString label);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
