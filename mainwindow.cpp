#include "mainwindow.h"
#include "game.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game *app = ui->widget->app;

    ui->label->setText("[POLSKI KOMPUTER]                     \nDostajesz niezliczona ilosc powiadomien, chyba cos powaznie padlo.");
    connect(app, SIGNAL(update_dialogs(QString)), this, SLOT(update_dialog(QString)));
    connect(ui->widget, SIGNAL(update_indicator(QString)), this, SLOT(update_indicator(QString)));
    connect(app, SIGNAL(update_score(QString)), this, SLOT(update_score(QString)));
    connect(ui->widget, SIGNAL(update_debuglabel(QString)), this, SLOT(update_debug(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_dialog(QString label) {
    ui->label->setText(label);
}

void MainWindow::update_indicator(QString label) {
    ui->interaction_indicator->setText(label);
}

void MainWindow::update_score(QString label) {
    ui->score_indicator->setText(label);
}

void MainWindow::update_debug(QString label) {
    ui->debug_label->setText(label);
}
