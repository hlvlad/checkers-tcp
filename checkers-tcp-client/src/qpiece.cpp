#include "qpiece.h"
#include <QSGImageNode>
#include <QQuickWindow>

QPiece::QPiece(size_t sq_, PieceType type_, Color color_, QQuickItem *parent)
    : QQuickItem(parent), sq(sq_), type(type_), color(color_)
{
    setWidth(SQUARE_SIZE);
    setHeight(SQUARE_SIZE);

    setY((7 - (sq / 8)) * SQUARE_SIZE);
    setX((sq & 7) * SQUARE_SIZE);

    setEnabled(false);
    setFlag(ItemHasContents, true);
}

QSGNode *QPiece::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
    QSGImageNode *node = static_cast<QSGImageNode *>(oldNode);
    if (!node)
        node = window()->createImageNode();
    QString str(":/images/");

    switch (type) {
        case DEAD:	str += "dead";  break;
        case MAN:	str += "man";	break;
        case KING:	str += "king";	break;
    }
    if (color == BLACK)
        str += "b";
    else
        str += "w";
    str += ".png";

    QSGTexture *texture = window()->createTextureFromImage(QImage(str));

    node->setTexture(texture);
    node->setOwnsTexture(true);
    node->setRect(boundingRect());

    window()->update();

    return node;
}
