#include "spot.h"
#include <QSGRectangleNode>
#include <QQuickWindow>

Spot::Spot(size_t sq, QQuickItem *parent) : QQuickItem(parent)
{
    setWidth(SQUARE_SIZE);
    setHeight(SQUARE_SIZE);

    setY((7 - (sq / 8)) * SQUARE_SIZE);
    setX((sq & 7) * SQUARE_SIZE);
    setOpacity(0.4);

    setEnabled(false);
    setVisible(false);
    setFlag(ItemHasContents, true);
}

QSGNode *Spot::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGRectangleNode *node = static_cast<QSGRectangleNode *>(oldNode);
    if (!node) {
        node = window()->createRectangleNode();
        node->setColor("red");
        node->setRect(boundingRect());
    }
    return node;
}
