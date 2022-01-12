#include "game.h"
#include <QPropertyAnimation>

#define LEAVE_CORPSES false

Game::Game(QQuickItem *parent) : QQuickItem(parent)
{
    for (size_t sq = 0; sq < SPOTS_NUMBER; ++sq)
        spots[sq] = new Spot(sq, this);

    setWidth(SQUARE_SIZE * 8);
    setHeight(SQUARE_SIZE * 8);
    setEnabled(false);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void Game::mousePressEvent(QMouseEvent *e)
{
    const auto pos = e->pos();

    int y = pos.y() / SQUARE_SIZE;
    int x = pos.x() / SQUARE_SIZE;

    size_t square_index = square(7 - y, x);
    QPiece *p = pieces[square_index];

    if (is_selection) {

        active_move.to = square_index;

        if (spot_active(active_move.to)) {
            for (const auto m : moves) {
                if (m.from == active_move.from && m.to == active_move.to) {
                    active_move = m;
                    break;
                }
            }
            make_move();
            emit move(active_move.from, active_move.to, active_move.type);
        }

        disable_selection();

    } else if (p && p->type != DEAD && p->color == engine.turn) {

        active_move.from = square_index;
        selected = p;

        is_selection = true;
        show_spots();
        selected->setOpacity(0.4);
    }
}

void Game::start(QQuickItem* parent)
{
    setParentItem(parent);
    engine.reset();
    moves = engine.valid_moves();

    destroy_pieces();
    create_pieces();

    setEnabled(true);

    status = GOING;
    emit statusChanged(engine.turn);
}

void Game::make_move(quint8 from, quint8 to, quint8 type) {
    active_move.from = from;
    active_move.to = to;
    active_move.type = MoveType(type);
    make_move();
}

void Game::end()
{
    setEnabled(false);
    disable_selection();

    emit statusChanged(engine.turn);
}

void Game::stop()
{
    status = STOPPED;
    end();
}

void Game::make_move()
{
    engine.act(active_move);
    graphic_move();

    moves = engine.valid_moves();

    if (!moves.size()) {
        status = WIN;
        end();
    }
    emit statusChanged(engine.turn);
}

void Game::graphic_move()
{
    int from = active_move.from;
    int to = active_move.to;

    if (active_move.type & MoveType::PROMOTION) {
        pieces[from]->type = KING;
        pieces[from]->update();
    }

    if (active_move.type & CAPTURE) {

        size_t trg_sq = 64;

        switch (to - from) {
            case NORTH_EAST * 2: trg_sq = from + NORTH_EAST; break;
            case NORTH_WEST * 2: trg_sq = from + NORTH_WEST; break;
            case SOUTH_EAST * 2: trg_sq = to + NORTH_WEST; break;
            case SOUTH_WEST * 2: trg_sq = to + NORTH_EAST; break;
        }
        if (is_valid_index(trg_sq) && pieces[trg_sq]) {
#if LEAVE_CORPSES == true
            pieces[trg_sq]->type = DEAD;
            pieces[trg_sq]->setOpacity(0.4);
            pieces[trg_sq]->update();
#else
            delete pieces[trg_sq];
            pieces[trg_sq] = nullptr;
#endif
        }
    }
#if LEAVE_CORPSES == true
    if (pieces[to])
        delete pieces[to];
#endif
    pieces[to] = pieces[from];
    pieces[from] = nullptr;

    int from_y = 7 - (from / 8);
    int from_x = from & 7;

    int to_y = 7 - (to / 8);
    int to_x = to & 7;

    animate(pieces[to], from_y, from_x, to_y, to_x);
}

void Game::create_pieces()
{
    for (const auto &[p, sq] : engine.board())
        pieces[sq] = new QPiece(sq, p.type, p.color, this);
}

void Game::destroy_pieces()
{
    for (size_t sq = 0; sq < SPOTS_NUMBER; ++sq) {
        if (pieces[sq]) {
            delete pieces[sq];
            pieces[sq] = nullptr;
        }
    }
}

void Game::show_spots()
{
    for (const auto &move : moves)
        if (move.from == active_move.from)
            spots[move.to]->setVisible(true);
}

void Game::hide_spots()
{
    for (auto &spot : spots)
        spot->setVisible(false);
}

void Game::disable_selection()
{
    if (is_selection) {
        is_selection = false;
        hide_spots();
        selected->setOpacity(1);
    }
}

bool Game::spot_active(size_t sq) const
{
    return spots[sq]->isVisible();
}

void Game::animate(QPiece *p, int from_y, int from_x, int to_y, int to_x)
{
    p->setZ(1);

    auto animX = new QPropertyAnimation(p, "x", this);
    animX->setEasingCurve(QEasingCurve::OutBounce);
    animX->setDuration(500);
    animX->setStartValue(int(from_x * SQUARE_SIZE));
    animX->setEndValue(int(to_x * SQUARE_SIZE));

    auto animY = new QPropertyAnimation(p, "y", this);
    animY->setEasingCurve(QEasingCurve::OutBounce);
    animY->setDuration(500);
    animY->setStartValue(int(from_y * SQUARE_SIZE));
    animY->setEndValue(int(to_y * SQUARE_SIZE));

    animX->start(QAbstractAnimation::DeleteWhenStopped);
    animY->start(QAbstractAnimation::DeleteWhenStopped);

    p->setZ(0);
}
