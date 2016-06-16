#include "GameCanvas.h"
#include "game.h"
#include <QtMath>

const float movement_speed = 0.125;
const int   animation_speed = 444;
const float max_jump = 6;

GameCanvas::GameCanvas(QWidget *parent) : QWidget(parent)
{
    // make sure game is focused
    setFocusPolicy(Qt::StrongFocus);

    // get game logic event loop to work
    startTimer(1000/60);

    // load assets
    this->load_assets();

    // initialize animation timer
    animation_timer = new QTimer(this);
    connect(animation_timer, SIGNAL(timeout()), this, SLOT(change_frame()));
    animation_timer->start(animation_speed);

    app = new game(this);

    connect(app, SIGNAL(add_person(QPoint,QString)), this, SLOT(add_person(QPoint,QString)));
    connect(app, SIGNAL(remove_person(QPoint)), this, SLOT(remove_person(QPoint)));
}

void GameCanvas::updateInteractionIndicator() {

    QString action;

    switch (app->getPossibleInteraction())
    {
     case 0: action="[NIEMOZLIWE]"; break;
     case 1: action="[NAPRAW]"; break;
     case 2: action="[OTWORZ]"; break;
     case 3: action="[ROZMAWIAJ]"; break;
     case 4: action=app->generic_group_label; break;
     default: action="[BULLSHIT]"; break;
    }

    emit update_indicator(action);
}

void GameCanvas::paintEvent(QPaintEvent *)
{
    QPainter app_painter(this);
    QPainter *painter;

    // check if cache is available
    if (this->is_cache_available) {
        // initialize painter
        painter = &app_painter;
        painter->setRenderHint(QPainter::Antialiasing);

        painter->drawImage(0, 0, drawing_cache.toImage());
    } else {
        // clean the cache
        this->drawing_cache = QPixmap(640,480);

        // make new painter
        painter = new QPainter(&drawing_cache);
        painter->setRenderHint(QPainter::Antialiasing);
    }

    // pointers for animation
    QPixmap *av = &assets["pc_av_01"];
    QPixmap *big_av = &assets["pc_big_av_01"];
    QPixmap *broken = &assets["pc_broken_01"];
    QPixmap *fixed = &assets["pc_fixed_01"];
    QPixmap *fixed_alt = &assets["pc_fixed_02"];

    // draw background
    if (!this->is_cache_available)
        for (int y=0; y<49; y++)
            for (int x=0; x<80; x++)
                painter->drawImage(x*8, y*8, assets["background"].toImage());

    // animate map
    switch (this->current_frame) {
    case 1:
        broken = &assets["pc_broken_02"];
        fixed = &assets["pc_fixed_02"];
        av = &assets["pc_av_02"];
        big_av = &assets["pc_big_av_02"];
        break;
    case 2:
        fixed_alt = &assets["pc_working_03"];
        break;
    case 3:
        broken = &assets["pc_broken_02"];
        fixed = &assets["pc_fixed_02"];
        av = &assets["pc_av_02"];
        big_av = &assets["pc_big_av_02"];
        break;
    default: break;
    }

    // render map
    for (int y=0; y<49; y++)
        for (int x=0; x<80; x++) {
            switch (app->map[x][y]) {
                case '#':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, y*8, assets["obstacle"].toImage());
                    break;
                case '@':
                    if (this->is_cache_available)
                        painter->drawImage(x*8, y*8, assets["doors"].toImage());
                    break;
                case '_':
                    if (this->is_cache_available)
                        painter->drawImage(x*8, y*8, assets["force_field"].toImage());
                    break;
                case '"':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, y*8, assets["sky"].toImage());
                    break;
                case '1':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, broken->toImage());
                    break;
                case '2':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, av->toImage());
                    break;
                case '3':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, fixed->toImage());
                    break;
                case '4':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, fixed_alt->toImage());
                    break;
                case '5':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, assets["pc_apple"].toImage());
                    break;
                case '6':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["pc"].toImage());
                    else
                        if (current_frame == 2)
                            painter->drawImage(x*8, (y-1)*8, assets["pc_net5"].toImage());
                        else
                            painter->drawImage(x*8, (y-1)*8, fixed->toImage());
                    break;
                case '8':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["big_pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, assets["pc_big_corrupted"].toImage());
                    break;
                case '9':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["big_pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, big_av->toImage());
                    break;
                case '0':
                    if (!this->is_cache_available)
                        painter->drawImage(x*8, (y-1)*8, assets["pc"].toImage());
                    else
                        painter->drawImage(x*8, (y-1)*8, assets["pc_bsod"].toImage());
                    break;
            }
        }

    // draw all the people
    if (!this->is_cache_available)
        for (int i=0; i<people.count(); i++) {
            Person person = people.at(i);
            painter->drawImage(person.x*8, person.y*8, assets[person.gfx].toImage());
        }

    // draw player
    if (this->is_cache_available)
        painter->drawImage(app->player.x*8, app->player.y*8, assets[app->player.gfx].toImage());

    // draw UI
    if (this->is_cache_available)
        painter->drawImage(0, 0, assets["ui"].toImage());

    if (current_frame == 2)
        if (this->is_cache_available) painter->drawImage(app->story_pointer.x*8, app->story_pointer.y*8, assets["arrow"].toImage());

    //emit update_debuglabel("x: "+QString::number(app->player.x)+"  y: "+QString::number(app->player.y));

    if (!this->is_cache_available) {
        this->is_cache_available = true;
        this->repaint();
    }
}

void GameCanvas::keyPressEvent(QKeyEvent *e)
{
    keys[e->key()] = true;
    QWidget::keyPressEvent(e);
}

void GameCanvas::keyReleaseEvent(QKeyEvent *e)
{
    keys[e->key()] = false;
    QWidget::keyReleaseEvent(e);
}

void GameCanvas::timerEvent(QTimerEvent *)
{
    // game logic

    bool need_to_repaint = false;

    // disable movement when jumping
    if (!app->player.isJumping && !app->player.isFalling && !app->player.isCharacterLocked) {
        // support key presses
        if (keys[Qt::Key_Up]) {
            jump_speed = movement_speed*(max_jump-2);
            jump_state = 0;
            app->player.isJumping = true;
            app->player.isFalling = false;
        }
    }
    if (keys[Qt::Key_Left])
    {
        // move player to the left
        if (!app->isObstacle(ceil(app->player.x)-1, app->player.y+1) && !app->player.isCharacterLocked)
            app->player.x-=movement_speed;
        need_to_repaint = true;
    }

    if (keys[Qt::Key_Right])
    {
        // move player to the right
        if (!app->isObstacle(ceil(app->player.x)+1, app->player.y+1) && !app->player.isCharacterLocked)
            app->player.x+=movement_speed;
        need_to_repaint = true;
    }
    if (keys[Qt::Key_Return]) {
        app->interact();
        keys[Qt::Key_Return] = false;
        updateInteractionIndicator();
    }

    if (app->player.isJumping && !app->player.isFalling){
        if (jump_state >= max_jump || app->isObstacle(ceil(app->player.x), ceil(app->player.y-1.5-movement_speed))) {
            app->player.isJumping = false;
            app->player.isFalling = true;
            app->player.y = (int)app->player.y;
        } else {
            app->player.y -= jump_speed;
            jump_state += jump_speed;
            if (jump_speed > movement_speed) {
                jump_speed -= movement_speed;
            }
        }

        need_to_repaint = true;
    }

    if (!app->player.isJumping && !app->isObstacle(ceil(app->player.x), ceil(app->player.y+1+movement_speed))) {
        app->player.y += jump_speed;
        need_to_repaint = true;
    } else if (!app->player.isJumping) {
        app->player.y = (int)app->player.y;
        app->player.isFalling = false;
    }

    if (need_to_repaint)
    {
        updateInteractionIndicator();
        // update the screen
        this->repaint();
    }
}

QPixmap GameCanvas::load_asset(QString path) {
    QPixmap pixmap = QPixmap();
    pixmap.load(path);
    return pixmap;
}

void GameCanvas::add_person(QPoint pos, QString gfx) {
    Person person = Person();
    person.x = pos.x();
    person.y = pos.y();
    person.gfx = gfx;

    people.append(person);

    // reset cache
    this->is_cache_available = false;

    this->repaint();
}

void GameCanvas::remove_person(QPoint pos) {
    for (int i = 0; i < people.count(); i++) {
        if (people[i].x == pos.x() && people[i].y == pos.y())
            people.removeAt(i);
    }

    // reset cache
    this->is_cache_available = false;

    this->repaint();
}

void GameCanvas::load_assets()
{
    assets["arrow"] = load_asset("://ui/szczalka");
    assets["obstacle"] = load_asset("://map/wall");
    assets["background"] = load_asset("://map/back");
    assets["doors"] = load_asset("://map/door");
    assets["force_field"] = load_asset("://map/force_field");
    assets["ui"] = load_asset("://ui/background");
    assets["sky"] = load_asset("://map/sky");
    assets["pc"] = load_asset("://map/pc");
    assets["big_pc"] = load_asset("://map/big_pc");

    assets["ui/labels"] = load_asset("://ui/labels");

    // Generate lists to iterate through
    QStringList hairtypes, states;
    hairtypes << "blonde" << "brown" << "gray" << "red";
    states << "m" << "m_dead" << "f" << "f_dead" << "f_boss";

    // iterate through hairtypes and states
    for (int i=0; i<hairtypes.count(); i++) {
        for (int j=0; j<states.count(); j++) {
            // create path
            QString path = "people/" + hairtypes.at(i) + "/" + states.at(j);

            // load asset from qrc
            assets[path] = load_asset("://"+path);
        }
    }

    // load all the pcs
    QStringList types;
    types << "bsod" << "av_01" << "av_02" << "broken_01" << "broken_02" << "broken_03" << "fixed_01" << "fixed_02" << "working_03" << "apple" << "net5" << "big_av_01" << "big_av_02" << "big_corrupted";

    // iterate through types
    for (int i=0; i<types.count(); i++) {
         // load asset from qrc
         assets["pc_"+types.at(i)] = load_asset("://map/pc_states/"+types.at(i));
    }

    qDebug() << "Loaded" << assets.count() << "assets:"<< assets.keys();
}

void GameCanvas::reset_cache() {
    // reset cache
    this->is_cache_available = false;

    // force repaint
    this->repaint();
}
