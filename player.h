#ifndef PLAYER_H
#define PLAYER_H
#include <QObject.h>

class Player : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float x READ getX WRITE setX)
    Q_PROPERTY(float y READ getY WRITE setY)
    Q_PROPERTY(bool isCharacterLocked READ getCharacterLocked WRITE setCharacterLocked)
    Q_PROPERTY(QString gfx READ getGfx WRITE setGfx)

public:
    Player();
    float     x;
    float     y;

    void setX(float _x) { x = _x; }
    void setY(float _y) { y = _y; }
    float getX() const { return x; }
    float getY() const { return y; }

    int    time;
    int    score = 0;

    int    jumpStrength;
    bool   isJumping = false;
    bool   isFalling = false;

    bool   isFixInProgress;
    bool   isCharacterLocked = false;
    int    fixEffect;

    QString gfx;

    bool getCharacterLocked() const { return isCharacterLocked; }
    void setCharacterLocked(bool value) { isCharacterLocked=value; }
    QString getGfx() const { return gfx; }
    void setGfx(QString value) { gfx=value; }

};

#endif // PLAYER_H
