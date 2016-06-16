#include "game.h"
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QRegExp>
#include "GameCanvas.h"

#define DOOR_LEFT   1
#define DOOR_RIGHT  2
#define DOOR_BOTTOM 3

#define FIX             1
#define OPEN_DOOR       2
#define CONTINUE_STORY  3
#define GENERIC_GROUP   4

game::game(QObject *parent): QObject(parent)
{
    // initialize default x and y
    player.x = 6;
    player.y = 12;

    // add player to Scripting Engine
    QScriptValue objectValue = engine.newQObject(&this->player);
    engine.globalObject().setProperty("player", objectValue);

    // add game object to Scripting Engine
    QScriptValue gameValue = engine.newQObject(this);
    engine.globalObject().setProperty("game", gameValue);

    // load map and story
    readMap("maps\\map01.map");
    initStory("maps\\map01.gin");

    dialog = new MiniGameDialog(0);
    end = new EndDialog(0);
    connect(dialog,SIGNAL(fixed()),this,SLOT(handle_fixed()));
    connect(dialog, SIGNAL(failed()), this, SLOT(handle_failed()));

    populate_achievments();
}

void game::readMap(QString path) {
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    int length=0;

    for (int i=0;i<49;i++) {
        QByteArray line = file.readLine();
        length=line.size();
        if (length>79) length=79;
        for (int a=0;a<=length;a++) {
            this->map[a][i]=line[a];
        }
    }

    file.close();
}

void game::loadChapter(QString chapter) {
    //this->story_part_name = chapter;

    story->beginGroup(chapter);
    qDebug() << story->group();
    qDebug() << story->allKeys();
    story_pointer.x = story->value("position_x").toInt();
    story_pointer.y = story->value("position_y").toInt();

    if (!strings.isEmpty())
        strings.clear();

    int count = story->value("count").toInt();

    for (int i=0; i<count; i++) {
        strings.append(story->value(QString::number(i)).toString());
        scripts_strings.append(story->value(QString::number(i)+ "_script").toString());
    }

    qDebug() << strings;
    qDebug() << scripts_strings;

    story->endGroup();
    qDebug() << "Loaded chapter" << chapter;
    qDebug() << "Lines:" << count;
    qDebug() << "Pointer X:" << story_pointer.x << "Pointer Y:" << story_pointer.y;
}

void game::initStory(QString path) {
    story = new QSettings(path, QSettings::IniFormat);


    story->beginGroup("Story");
    int story_parts = story->value("count").toInt();
    qDebug() << "Story loaded. Parts" << story_parts;
    for (int i=0; i<story_parts; i++) {
        qDebug() << "Part" << i << "-" << story->value(QString::number(i)).toString();
    }

    QString chapter_name = story->value("0").toString();
    story->endGroup();

    this->loadChapter(chapter_name);
    story_part = 0;

    // add all the people
    QStringList keys = story->allKeys();
    qDebug() << keys;

    for (int i=0; i<keys.count(); i++) {
        // check if matches _x_/gfx
        if (QRegExp("[0-9]+x[0-9]+\/gfx").exactMatch(keys[i])) {
            QString position = keys[i].split("/")[0];
            int x = position.split("x")[0].toInt();
            int y = position.split("x")[1].toInt();

            QPoint point = QPoint(x,y);

            story->beginGroup(position);
            QString gfx = story->value("gfx").toString();

            GameCanvas* parent = (GameCanvas*)this->parent();
            parent->add_person(point, gfx);
            story->endGroup();
        }
    }
}

int game::getPossibleInteraction() {
    int x = ceil(player.x);
    int y = ceil(player.y);

    // check if we could continue story
    if ((x==story_pointer.x+1 || x==story_pointer.x) && y==story_pointer.y+2) {
        return CONTINUE_STORY;
    }

    // check if fix possible
    if (this->isFixPossible()) {
        return FIX;
    }

    // check if we could open the door
    if (this->isNextToDoor()) {
        return OPEN_DOOR;
    }

    // check if non-story related interaction available
    story->beginGroup(QString::number(x)+"x"+QString::number(y));
    generic_group = story->value("group").toString();
    story->endGroup();
    if (!generic_group.isEmpty()) {
        story->beginGroup(generic_group);
        int do_not_track = story->value("do_not_track").toInt();
        int count = story->value("count").toInt();

        // try to load tracker
        if (!do_not_track) {
            if (generic_group_tracker.keys().contains(generic_group))
                temp_generic_group_tracker = generic_group_tracker[generic_group];
            else
                generic_group_tracker[generic_group] = 0;
        }

        generic_group_label = story->value("label").toString();
        story->endGroup();

        if (temp_generic_group_tracker<count)
            return GENERIC_GROUP;
    } else {
        temp_generic_group_tracker = 0;
    }

    return 0;
}

QString game::continue_story() {
    QString text = strings.first();
    strings.removeFirst();

    qDebug() << engine.evaluate(scripts_strings.first()).toString();
    scripts_strings.removeFirst();

    doScore();

    if (strings.empty()) {
        story_part++;

        // go to next chapter
        story->beginGroup("Story");
        int story_parts = story->value("count").toInt();

        if (story_part < story_parts) {
            QString chapter_name = story->value(QString::number(story_part)).toString();
            story->endGroup();
            this->loadChapter(chapter_name);
        } else {
            qDebug() << "Game finished I guess";
            story_pointer.x = -1;
            story_pointer.y = -1;
        }
    }

    return text;
}

QString game::run_generic_group() {
    story->beginGroup(generic_group);

    int do_not_track = story->value("do_not_track").toInt();
    QString label;

    int count = story->value("count").toInt();
    if (temp_generic_group_tracker < count) {
        label = story->value(QString::number(temp_generic_group_tracker)).toString();
    } else {
        label = "To bug jak cos ale nie mow nikomu";
    }

    // update tacker
    temp_generic_group_tracker++;
    if (!do_not_track)
        generic_group_tracker[generic_group] = temp_generic_group_tracker;

    // close group
    story->endGroup();

    return label;
}

bool game::isObstacle(int x, int y) {
    char mapChar = map[x][y];
    if (mapChar=='#' || mapChar=='@' || mapChar=='_') return 1; else return 0;
}

int game::isNextToDoor() {
    int x = ceil(player.x); int y = ceil(player.y);

    // test bottom
    char mapChar = map[x][y+2];
    if (mapChar == '@') return DOOR_BOTTOM;

    // test left
    mapChar = map[x-1][y];
    if (mapChar == '@') return DOOR_LEFT;

    // test right
    mapChar = map[x+1][y];
    if (mapChar == '@') return DOOR_RIGHT;

    return false;
}

void game::openNextDoor() {
    int x = ceil(player.x); int y = ceil(player.y);

    int direction = this->isNextToDoor();

    switch (direction) {
        case DOOR_LEFT:
            for (int i = y-3; i<y+3; i++) {
                if (map[x-1][i] == '@')
                    map[x-1][i] = ' ';
            }
        break;
        case DOOR_RIGHT:
        for (int i = y-3; i<y+3; i++) {
            if (map[x+1][i] == '@')
                map[x+1][i] = ' ';
        }
        break;
        case DOOR_BOTTOM:
            for (int i = x-3; i<x+3; i++) {
                if (map[i][y+2] == '@')
                    map[i][y+2] = ' ';
            }
        break;
    }

    // force refresh cache
    GameCanvas* parent = (GameCanvas*)this->parent();
    parent->reset_cache();
}

void game::handle_fixed() {
    player.setCharacterLocked(false);
    map[int(player.x)][int(player.y+1)] = '3';

    player.score+=15;
    emit update_score(QString::number(player.score));

    game_progress.fixed++;
}

void game::handle_failed() {
    player.setCharacterLocked(false);
    player.score-=10;
    emit update_score(QString::number(player.score));
    game_progress.rage += 10;
    game_progress.failed++;
    player.setCharacterLocked(false);
}

void game::interact() {
    QString label;

     if (isFixPossible())
     {
         dialog->init_minigame();
         player.setCharacterLocked(true);
     } else {
            switch (getPossibleInteraction())
            {
             case OPEN_DOOR: openNextDoor(); break;
             case CONTINUE_STORY:
                label = continue_story();
                emit update_dialogs(label);
                break;
             case GENERIC_GROUP:
                label = run_generic_group();
                emit update_dialogs(label);
                break;
            }
    }
}

bool game::isFixPossible() {
    char mapChar = map[int(player.x)][int(player.y+1)];
    return mapChar == '1';
}

void game::doScore() {
    player.score+=5;
    emit update_score(QString::number(player.score));
}

void game::update_mapchar(int x, int y, char character) {
    qDebug()<< map[x][y];
    map[x][y] = character;

    // force refresh cache
    GameCanvas* parent = (GameCanvas*)this->parent();
    parent->reset_cache();
}

void game::showEndingDialog() {
    // craft game ending dialog
    QString screen = "";

    screen += "ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ POLSKI KOMPUTER ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» \n";
    screen += "ş ÚÄÄ PODSUMOWANIE ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄż ş˛\n";
    screen += "ş ł                                                          ł ş˛\n";
    screen += "ş ł  Zdobyte punkty:         ";
    QString score = QString::number(player.score);
    for (int x=0; x<30-score.count(); x++) screen += " ";
    screen += score + "  ł ş˛\n";
    screen += "ş ł  Naprawione komputery:   ";
    QString fixed = QString::number(game_progress.fixed);
    for (int x=0; x<30-fixed.count(); x++) screen += " ";
    screen += fixed + "  ł ş˛\n";
    screen += "ş ł  Poboczne zadania:       ";
    QString social = QString::number(generic_group_tracker.count());
    for (int x=0; x<30-social.count(); x++) screen += " ";
    screen += social + "  ł ş˛\n";
    screen += "ş ł  Punkty nienawisci:      ";
    QString rage = QString::number(game_progress.rage);
    for (int x=0; x<30-rage.count(); x++) screen += " ";
    screen += rage + "  ł ş˛\n";
    screen += "ş ł  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ                       ł ş˛\n";
    screen += "ş ł  Osiagniecia:                                            ł ş˛\n";

    int unlock_count = 0;
    QMap<QString, QStringList>::iterator i;
    for (i=achievments.begin(); i!=achievments.end(); ++i) {
        int unlocked = engine.evaluate(i.key()).toInteger();
        if (unlocked) {
            unlock_count++;
            QString title = i.value().at(0);
            QString desc =  i.value().at(1);
            screen += "ş ł     - " + title;
            for (int x=0; x<49-title.count(); x++) screen += " ";
            screen += "  ł ş˛\n";
            screen += "ş ł     " + desc;
            for (int x=0; x<51-desc.count(); x++) screen += " ";
            screen += "  ł ş˛\n";
        }
    }

    if (unlock_count == 0) {
        screen += "ş ł     - Byc dobrym zulem                                   ł ş˛\n";
        screen += "ş ł     Nie osiagnij ani jednej rzeczy                       ł ş˛\n";
    }

    screen += "ş ł                                                          ł ş˛\n";
    screen += "ş ł  u bro tanks for playin. im sad ur sofware enginer so u  ł ş˛\n";
    screen += "ş ł  can made bro                                            ł ş˛\n";
    screen += "ş ŔÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄŮ ş˛\n";
    screen += "ş Informatyka Stosowana i Systemy Pomiarowe 2016. Polecam.     ş˛\n";
    screen += "ČÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍĽ˛\n";
    screen += "˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛˛\n";

    end->set_text(screen);

    end->open();
}

void game::populate_achievments() {
    achievments.insert("function a() { if (game.get_achievment_var('social')==0) return 1; } a();", QStringList() << "Aspolecznosc" << "Nie porozmawiaj z ani jedna postacia");
    achievments.insert("function a() { if (game.get_achievment_var('rage')==0) return 1; } a();", QStringList() << "Moment zen" << "Nie zdobadz ani jednego punktu nienawisci");
    achievments.insert("function a() { if (game.get_achievment_var('fixed')==0) return 1; } a();", QStringList() << "Szlachta nie pracuje" << "Nie napraw ani jednego komputera");
}

int game::get_achievment_var(QString type) {
    if (type == "rage")
        return game_progress.rage;
    if (type == "fixed")
        return game_progress.fixed;
    if (type == "social")
        return this->generic_group_tracker.count();
    if (type == "score")
        return player.score;
    if (type == "failed")
        return game_progress.failed;
    return 0;
}
