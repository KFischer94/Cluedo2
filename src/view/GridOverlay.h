#pragma once

#include <QGraphicsItem>

namespace Cluedo {

class GridModel;

// ── GridOverlay ───────────────────────────────────────────────────────────────
// QGraphicsItem that draws the grid on top of the board image.
// It reads origin, cellSize and dimensions directly from GridModel.
//
// Cell colours per CellType:
//   Empty      → transparent (only border)
//   Path       → semi-transparent blue
//   Room       → semi-transparent green  (hue varies per room name hash)
//   Door       → semi-transparent orange
//   PieceStart → semi-transparent yellow
// ─────────────────────────────────────────────────────────────────────────────
class GridOverlay : public QGraphicsItem {
public:
    explicit GridOverlay(QGraphicsItem* parent = nullptr);

    void setGridModel(GridModel* model);

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void   paint(QPainter* painter,
                 const QStyleOptionGraphicsItem* option,
                 QWidget* widget) override;

private:
    GridModel* m_gridModel { nullptr };

    static QColor colorForType(int cellTypeInt, const QString& entityName);
};

} // namespace Cluedo
