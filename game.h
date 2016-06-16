#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QScriptEngine>
#include <QtMath>
#include <QMap>
#include <QPoint>
#include "minigamedialog.h"
#include "enddialog.h"

#include "pointer.h"
#include "player.h"
#include "progress.h"

class game : public QObject
{
    Q_OBJECT
public:
    game(QObject *parent=0);
    bool   isMenuVisible;
    bool   isDebugEnabled;

    char   map[80][49];

    void readMap(QString path);

    void initStory(QString path);
    void loadChapter(QString chapter);
    pointer story_pointer;
    int story_part;
    QStringList strings;
    QStringList scripts_strings;

    QString generic_group;
    QString generic_group_label;

    int getPossibleInteraction();

    bool isObstacle(int x, int y);
    int isNextToDoor();
    void openNextDoor();
    bool isFixPossible();

    QString continue_story();
    QString run_generic_group();

    Player player;

    void interact();
    void doScore();

public slots:
    void handle_fixed();
    void handle_failed();
    Q_INVOKABLE void update_mapchar(int x, int y, char character);
    Q_INVOKABLE void showEndingDialog();
    Q_INVOKABLE void rage(int a) { game_progress.rage += a; }
    Q_INVOKABLE int get_achievment_var(QString type);

signals:
    void update_dialogs(QString label);
    void update_score(QString label);
    void force_refresh();
    void add_person(QPoint pos, QString gfx);
    void remove_person(QPoint pos);

private:
    QSettings *story;
    MiniGameDialog *dialog;
    EndDialog *end;
    QScriptEngine engine;
    QMap<QString,int> generic_group_tracker;
    QMap<QString,QStringList> achievments;
    int temp_generic_group_tracker;

    progress game_progress;

    void populate_achievments();
};

#endif // GAME_H
