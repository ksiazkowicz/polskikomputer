#ifndef MINIGAMEDIALOG_H
#define MINIGAMEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include "ui_minigamedialog.h"

namespace Ui {
class Dialog;
}

class MiniGameDialog : public QDialog
{
    Q_OBJECT
public:
    MiniGameDialog(QWidget *parent);
    ~MiniGameDialog();

    void init_minigame();
    QString generate_line(QString l1, QString c1, QString l2, QString c2);

    QString get_element(int i, QStringList list);

    void check_input();

public slots:
    void update_prompt();
    void change_frame();

    void build_console_ui();

private slots:
    void on_SeqField_textChanged();

signals:
    void fixed();
    void failed();

private:
    Ui::Dialog *ui;
    QTimer *animation_timer;
    QTimer *close_timer;
    int anim_frame;
    QString sequence;

    QString title;
    QString prompt;
    QString remembered_sequence;

    QMap<int, bool> keys;
    QMap<QString, QStringList> code_files;

    QStringList possible_filenames;
    QStringList codes;
    QStringList entered_codes;
    QStringList random_sequence;
    QStringList letters;

    QString last_buffer;

    bool is_finished;
    QString finished_text;
};

#endif // MINIGAMEDIALOG_H
