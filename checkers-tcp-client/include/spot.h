#pragma once

#include "misc.h"

#include <QQuickItem>

/**
 * @brief The Spot class represents a spot on the checkers board.
 * 
 * This class inherits from QQuickItem and provides functionality to represent a spot on the checkers board.
 * It is used by the checkers TCP client to display the spots on the board.
 */
class Spot : public QQuickItem {
    Q_OBJECT
public:
    /**
     * @brief Constructs a Spot object with the specified square number and parent item.
     * 
     * @param sq The square number of the spot.
     * @param parent The parent item of the spot.
     */
    Spot(size_t sq, QQuickItem *parent = nullptr);

protected:
    /**
     * @brief Updates the paint node for the spot.
     * 
     * This function is called when the spot needs to update its paint node.
     * It creates or updates the QSGNode that represents the spot's appearance.
     * 
     * @param oldNode The old paint node.
     * @param data The update paint node data.
     * @return The updated paint node.
     */
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
};
