#include "GridModel.h"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace Cluedo {

const CellData GridModel::s_emptyCell {};

// ── Construction ──────────────────────────────────────────────────────────────

GridModel::GridModel(int cols, int rows)
    : m_cols(cols)
    , m_rows(rows)
    , m_cells(static_cast<std::size_t>(rows),
              std::vector<CellData>(static_cast<std::size_t>(cols)))
{}

// ── Geometry ──────────────────────────────────────────────────────────────────

void GridModel::setOrigin(QPoint o)
{
    m_origin = o;
}

void GridModel::setCellSize(QSize s)
{
    if (s.width() > 0 && s.height() > 0)
        m_cellSize = s;
}

void GridModel::resize(int cols, int rows)
{
    if (cols <= 0 || rows <= 0)
        return;

    // Resize row count
    m_cells.resize(static_cast<std::size_t>(rows));

    // Resize each row to the new column count
    for (auto& row : m_cells)
        row.resize(static_cast<std::size_t>(cols));

    m_cols = cols;
    m_rows = rows;
}

// ── Cell access ───────────────────────────────────────────────────────────────

bool GridModel::isValid(int col, int row) const
{
    return col >= 0 && row >= 0 && col < m_cols && row < m_rows;
}

const CellData& GridModel::cell(int col, int row) const
{
    if (!isValid(col, row))
        return s_emptyCell;
    return m_cells[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
}

CellData GridModel::setCell(int col, int row, const CellData& data)
{
    if (!isValid(col, row))
        return s_emptyCell;

    auto& target = m_cells[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
    CellData previous = target;
    target = data;
    return previous;
}

void GridModel::clearCell(int col, int row)
{
    if (isValid(col, row))
        m_cells[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = CellData{};
}

// ── Bulk operations ───────────────────────────────────────────────────────────

void GridModel::clear()
{
    for (auto& row : m_cells)
        for (auto& c : row)
            c = CellData{};
}

void GridModel::renameEntity(const QString& oldName, const QString& newName)
{
    for (auto& row : m_cells)
        for (auto& c : row)
            if (c.entityName == oldName)
                c.entityName = newName;
}

void GridModel::removeEntity(const QString& entityName)
{
    for (auto& row : m_cells)
        for (auto& c : row)
            if (c.entityName == entityName)
                c = CellData{};
}

// ── Coordinate helpers ────────────────────────────────────────────────────────

QPoint GridModel::pixelToCell(QPoint pixelPos) const
{
    int relX = pixelPos.x() - m_origin.x();
    int relY = pixelPos.y() - m_origin.y();

    if (relX < 0 || relY < 0)
        return { -1, -1 };

    int col = relX / m_cellSize.width();
    int row = relY / m_cellSize.height();

    if (!isValid(col, row))
        return { -1, -1 };

    return { col, row };
}

QPoint GridModel::cellToPixel(int col, int row) const
{
    return {
        m_origin.x() + col * m_cellSize.width(),
        m_origin.y() + row * m_cellSize.height()
    };
}

QList<QPoint> GridModel::neighbours(int col, int row) const
{
    QList<QPoint> result;
    const std::array<QPoint, 4> offsets = {
        QPoint{ 0, -1 }, QPoint{ 0, 1 },
        QPoint{-1,  0 }, QPoint{ 1, 0 }
    };
    for (const auto& offset : offsets) {
        QPoint n{ col + offset.x(), row + offset.y() };
        if (isValid(n))
            result.append(n);
    }
    return result;
}

} // namespace Cluedo
