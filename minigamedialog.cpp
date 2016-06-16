#include "minigamedialog.h"

#include "mainwindow.h"
#include "game.h"
#include "ui_mainwindow.h"
#include <time.h>

MiniGameDialog::MiniGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    animation_timer = new QTimer(this);
    close_timer = new QTimer(this);
    connect(animation_timer, SIGNAL(timeout()), (MiniGameDialog*)this, SLOT(change_frame()));
    animation_timer->start(300);

    // set possible filenames
    possible_filenames << "CANCER.CPP" << "MAIN.CPP" << "TOAST.CPP" << "FIBONACCI.CPP" << "COFFEE.CPP" << "LIFE.CPP";

    // randomize life
    srand(time(NULL));

    // letters
    letters << "A" << "B" << "C" << "D";

    // hardcoded, UGH
    code_files["FIBONACCI.CPP"]  = QStringList() << "if (n==0) return 0;" << "else if (n == 1) return 1;" << "else return fibonacci(n-1)" << "+ fibonacci(n-2);";
    code_files["MAIN.CPP"]       = QStringList() << "int main() {" << "int a = 2+2;" << "cout << a;" << "return 0;";
    code_files["CANCER.CPP"]     = QStringList() << "feel_bad();" << "Diag d = get_diagnosis();" << "if (d == S_CANCER)" << "die();";
    code_files["TOAST.CPP"]      = QStringList() << "Bread b = get_bread();" << "b.add(F_BUTTER);" << "b.toast();" << "return b;";
    //code_files["GAME.CPP"]       = QStringList() << "int a = rand()*10; int b;" << "cout<<\"1-10?\n\"; cin << b;" << "if (a==b)" << "cout << \"gratulacje\";";
    code_files["LIFE.CPP"]       = QStringList() << "eat();" << "sleep();" << "code();" << "die();";
    code_files["COFFEE.CPP"]     = QStringList() << "while (0) {" << "coffee = refill();" << "while (coffee)" << "drink(coffee); }";
}

MiniGameDialog::~MiniGameDialog()
{
    delete ui;
}

void MiniGameDialog::init_minigame() {
    // initialize title
    QString filename = possible_filenames.at(rand() % possible_filenames.size());
    title = "CORRUPTED CODE EDITOR - " + filename;

    // clean sequence field
    sequence = "";
    ui->SeqField->setText("");

    // make new codes
    codes.clear();
    entered_codes.clear();
    codes = code_files[filename];

    // make list of all possibilities
    QStringList possible_sequence;
    possible_sequence << "0" << "1" << "2" << "3";

    random_sequence.clear();

    // choose at random
    while (possible_sequence.count() > 0) {
        // choose random
        int i = rand() % possible_sequence.size();

        // append to random_sequence
        random_sequence << possible_sequence.at(i);

        // remove that element
        possible_sequence.removeAt(i);
    }

    // we don't want game to think that we're finished
    is_finished = false;

    // run the game
    this->open();
    build_console_ui();
}

QString MiniGameDialog::generate_line(QString l1, QString c1, QString l2, QString c2) {
    QString buffer = "";
    buffer += "ş łÚÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄżłłÚÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄżł ş˛\n";

    buffer += "ş ";
    buffer += "łł "+l1+" ł "+c1.right(30);

    for (int i=0; i<30-c1.count(); i++)
        buffer += " ";

    buffer +=" łł";
    buffer += "łł "+l2+" ł "+c2.right(29);

    for (int i=0; i<29-c2.count(); i++)
        buffer += " ";

    buffer +=" łł";
    buffer += " ş˛\n";

    buffer += "ş łŔÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄŮłłŔÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄŮł ş˛\n";

    return buffer;
}

QString MiniGameDialog::get_element(int i, QStringList list) {
    if (i < list.count())
        return list.at(i);
    else return "";
}

void MiniGameDialog::change_frame() {
    anim_frame++;
    update_prompt();
}

void MiniGameDialog::check_input() {
    int i = 0;
    while (get_element(i, codes).simplified() == get_element(i, entered_codes).simplified() && i<4)
        i++;

    if (i == 4) {
        finished_text = "CONGRATS! SYSTEM CORRUPTION FIXED.";
        emit fixed();
    } else {
        finished_text = "INCORRECT SEQUENCE ENTERED.       ";
        emit failed();
    }

    is_finished = true;

    close_timer->singleShot(8000,this,SLOT(close()));
    last_buffer = ui->title->text();

    entered_codes.clear();
    build_console_ui();
}

void MiniGameDialog::build_console_ui() {
    if (this->isVisible()) {
        // screen buffer
        QString buffer = "";

        // set screen width
        int col = 85;
        int max_title = col - 1 - 6;

        int lin = 19;

        if (!is_finished) {
            // start drawing window
            buffer += "ÉÍ";

            QString max_border = "";
            // add title
            for (int i=0; i<(max_title-title.count())/2; i++) {
                max_border += "Í";
            }

            buffer += max_border + " " + title + " " + max_border;
            while (buffer.count() < col-3) {
                buffer += "Í";
            }

            buffer += "» \n";
            buffer += "ş ÚÄÄ FOUND CODE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄżÚÄÄ ENTERED CODE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄż ş˛\n";

            for (int i=0; i<4; i++) {
                int seq_c = random_sequence.at(i).toInt();
                buffer += generate_line(letters.at(seq_c),get_element(seq_c,codes),letters.at(i),get_element(i,entered_codes));
            }

            buffer += "ş ŔÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄŮŔÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄŮ ş˛\n";
            buffer += "ĚÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍą˛\n";
            buffer += "ş Enter correct sequence to continue: ";

            int max_field = col-42;
            QString temp_sequence = sequence.right(max_field-1);

            buffer += temp_sequence + prompt;

            for (int i=0; i<max_field-(temp_sequence+prompt).count(); i++)
                buffer += " ";

            buffer += " ş˛\n";
            buffer += "ČÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍĽ˛\n";
            buffer += " ˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛";
        } else {
            QStringList lines = last_buffer.split("\n");

            int vbegin = (lin - 8)/2;
            int hbegin = (col - 45)/2;
            for (int i = 0; i < vbegin; i++) {
                buffer += lines.at(i) + "\n";
            }

            QString line;

            QStringList screen;
            screen << "ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»Ä"
                   << "ş ÚÄÄ INFORMATION ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄż ş˛"
                   << "ş ł                                      ł ş˛"
                   << "ş ł  "+finished_text+"  ł ş˛"
                   << "ş ł                                      ł ş˛"
                   << "ş ŔÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄŮ ş˛"
                   << "ČÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍĽ˛"
                   << " ˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛";

            for (int i=0; i<screen.count(); i++) {
                line = lines.at(vbegin+i);
                buffer += line.left(hbegin);
                buffer += screen.at(i);
                buffer += line.right(col-45-hbegin-1);
                buffer += "\n";
            }

            for (int i=vbegin+8; i<lin; i++) {
                buffer += lines.at(i);
                if (i!=lin-1)
                    buffer += "\n";
            }
        }
        ui->title->setText(buffer);
    }
}

void MiniGameDialog::update_prompt() {
    if (this->isVisible()) {
        prompt = anim_frame % 2 == 0 ? "_" : " ";
        build_console_ui();
    }
}

void MiniGameDialog::on_SeqField_textChanged()
{
    if (this->isVisible()) {
        // set sequence to text field value
        sequence = ui->SeqField->toPlainText();

        // check if last char is newline
        if (sequence.right(1) == "\n") {
            if (is_finished)
                close();

            // cut that char
            entered_codes << ui->SeqField->toPlainText().replace("\n", "");

            // clear field
            ui->SeqField->clear();

            // clear sequence
            sequence = "";

            // check if finished
            if (entered_codes.count() == 4)
                check_input();
        }

        // update UI
        build_console_ui();
    }
}
