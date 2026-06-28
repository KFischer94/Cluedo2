#include "GridOverlay.h"
#include "../model/GridModel.h"
#include "../model/CellState.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Cluedo {

GridOverlay::GridOverlay(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    // Allow the item to be updated without being selectable/movable
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable,    false);
}

void GridOverlay::setGridModel(GridModel* model)
{
    m_gridModel = model;
    update();
}

QRectF GridOverlay::boundingRect() const
{
    if (!m_gridModel)
        return QRectF();

    const QPoint  origin   = m_gridModel->origin();
    const QSize   cellSize = m_gridModel->cellSize();
    const int     cols     = m_gridModel->cols();
    const int     rows     = m_gridModel->rows();

    return QRectF(origin.x(), origin.y(),
                  cols * cellSize.width(),
                  rows * cellSize.height());
}

void GridOverlay::paint(QPainter* painter,
                        const QStyleOptionGraphicsItem* /*option*/,
                        QWidget* /*widget*/)
{
    if (!m_gridModel)
        return;

    const QPoint origin   = m_gridModel->origin();
    const QSize  cellSize = m_gridModel->cellSize();
    const int    cols     = m_gridModel->cols();
    const int    rows     = m_gridModel->rows();
    const int    cw       = cellSize.width();
    const int    ch       = cellSize.height();

    painter->save();

    // ── Fill cells ────────────────────────────────────────────────────────────
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const CellData& cell = m_gridModel->cell(col, row);
            if (cell.type == CellType::Empty)
                continue;

            const QColor fill = colorForType(static_cast<int>(cell.type),
                                             cell.entityName);
            const QRectF rect(origin.x() + col * cw,
                              origin.y() + row * ch,
                              cw, ch);
            painter->fillRect(rect, fill);
        }
    }

    // ── Grid lines ────────────────────────────────────────────────────────────
    QPen linePen(QColor(255, 255, 255, 60));
    linePen.setWidth(1);
    painter->setPen(linePen);

    const int totalW = cols * cw;
    const int totalH = rows * ch;

    for (int col = 0; col <= cols; ++col) {
        const int x = origin.x() + col * cw;
        painter->drawLine(x, origin.y(), x, origin.y() + totalH);
    }
    for (int row = 0; row <= rows; ++row) {
        const int y = origin.y() + row * ch;
        painter->drawLine(origin.x(), y, origin.x() + totalW, y);
    }

    // ── Origin marker ─────────────────────────────────────────────────────────
    painter->setPen(QPen(QColor(255, 80, 80), 2));
    painter->drawRect(QRectF(origin.x(), origin.y(), cw, ch));

    painter->restore();
}

// ── Static colour helper ──────────────────────────────────────────────────────
QColor GridOverlay::colorForType(int typeInt, const QString& entityName)
{
    switch (static_cast<CellType>(typeInt)) {
    case CellType::Path:
        return QColor(80, 140, 240, 100);   // blue
    case CellType::Door:
        return QColor(240, 160, 40, 140);   // orange
    case CellType::PieceStart:
        return QColor(240, 220, 40, 140);   // yellow
    case CellType::Room: {
        // Hash the room name to get a stable hue
        int hash = 0;
        for (const QChar c : entityName)
            hash = hash * 31 + c.unicode();
        const int hue = ((hash % 360) + 360) % 360;
        return QColor::fromHsv(hue, 180, 200, 120);
    }
    default:
        return QColor(0, 0, 0, 0);
    }
}

} // namespace Cluedo
