#include "enddialog.h"
#include "ui_enddialog.h"

EndDialog::EndDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EndDialog)
{
    ui->setupUi(this);
}

EndDialog::~EndDialog()
{
    delete ui;
}

void EndDialog::set_text(QString text) {
    ui->title->setText(text);
}
