#pragma once

#include <QGraphicsView>

namespace Cluedo {

class GridOverlay;
class GridModel;

// ── BoardView ─────────────────────────────────────────────────────────────────
// Central widget: QGraphicsView containing the board image + grid overlay.
//
// Zoom:  Ctrl+Wheel  → view-only zoom (does NOT change GridModel cellSize)
// Pan:   Middle-button drag  OR  Ctrl+Left-drag
// ─────────────────────────────────────────────────────────────────────────────
class BoardView : public QGraphicsView {
    Q_OBJECT

public:
    explicit BoardView(QWidget* parent = nullptr);

    void setImage(const QPixmap& pixmap);
    void setGridModel(GridModel* model);
    void refreshGrid();

signals:
    void cellClicked(int col, int row);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    QGraphicsScene*    m_scene     { nullptr };
    QGraphicsPixmapItem* m_pixItem { nullptr };
    GridOverlay*       m_overlay   { nullptr };
    GridModel*         m_gridModel { nullptr };

    bool   m_panning  { false };
    QPoint m_panStart;
    bool   m_ctrlHeld { false };
};

} // namespace Cluedo
