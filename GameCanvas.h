#ifndef GAMECANVAS_H
#define GAMECANVAS_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QPoint>
#include "game.h"

class Person {
public:
    int x;
    int y;
    QString gfx;
};

class GameCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit GameCanvas(QWidget *parent = 0);
    void paintEvent(QPaintEvent * event);

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void timerEvent(QTimerEvent *);

    void updateInteractionIndicator();

    game *app;

signals:
    void update_indicator(QString label);
    void update_debuglabel(QString label);

public slots:
    void change_frame() {
        this->current_frame++;
        if (current_frame > 3) {
            current_frame = 1;
        }
        this->repaint();
    }

    void add_person(QPoint pos, QString gfx);
    void remove_person(QPoint pos);
    void reset_cache();

private:
    void load_assets();
    QPixmap load_asset(QString path);

    QMap<QString, QPixmap> assets;
    QMap<int, bool> keys;
    QList<Person> people;

    int jumpStrength;

    QTimer *animation_timer;
    int current_frame;
    float jump_state;
    float jump_speed;

    bool is_cache_available = false;
    QPixmap drawing_cache;
};

#endif // GAMECANVAS_H
