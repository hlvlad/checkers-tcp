#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "qpiece.h"
#include "spot.h"
#include "checkers_engine.h"
#include "networkgamesession.h"


namespace checkers_enums_ns {
Q_NAMESPACE
enum Status { GOING = GOING, WIN = WIN, DRAW = DRAW, STOPPED = STOPPED };
enum Color  { WHITE = WHITE, BLACK = BLACK, BOTH = BOTH };
Q_ENUM_NS(Status)
Q_ENUM_NS(Color)
}

class Game : public QQuickItem {
    Q_OBJECT
public:
    explicit Game(QQuickItem *parent = nullptr);

    Q_PROPERTY(checkers_enums_ns::Status status READ get_status NOTIFY statusChanged)

    Q_INVOKABLE void start(QQuickItem* parent);
    Q_INVOKABLE void make_move(quint8 from, quint8 to, quint8 type);
    Q_INVOKABLE void end();
    Q_INVOKABLE void stop();
private:
    void create_pieces();
    void destroy_pieces();

    void show_spots();
    void hide_spots();

    void make_move();
    void graphic_move();

    void disable_selection();
    bool spot_active(size_t sq) const;

    checkers_enums_ns::Status get_status() const { return checkers_enums_ns::Status(status); }

    Spot        *spots[64] = {};	// graphical squares for move selection
    QPiece      *pieces[64] = {};   // graphical pieces
    QPiece      *selected;

    Move        active_move;
    MoveList    moves;

    bool        is_selection = false;

    checkers_engine      engine;
    GameStatus      status = STOPPED;
protected:
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void statusChanged(Color turn);
    void move(quint8 from, quint8 to, quint8 type);
public slots:
    void animate(QPiece *p, int from_y, int from_x, int to_y, int to_x);
};

#endif // GAME_H
