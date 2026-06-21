#pragma once

#include "CellState.h"

#include <QList>
#include <QPoint>
#include <QSize>
#include <vector>

namespace Cluedo {

// ── GridModel ─────────────────────────────────────────────────────────────────
// Owns the 2-D array of CellData and the geometric parameters of the grid.
// Row-major storage: cells[row][col]  /  accessors use (col, row) = (x, y).
//
// This is a pure data class – no Qt signals. The controller layers own the
// QUndoStack and notify views after every mutating operation.
// ─────────────────────────────────────────────────────────────────────────────
class GridModel {
public:
    // Construction ────────────────────────────────────────────────────────────

    // Creates a grid filled with CellType::Empty.
    GridModel(int cols, int rows);

    // Geometry ────────────────────────────────────────────────────────────────

    // Origin of the grid in image-pixel coordinates.
    QPoint  origin()    const { return m_origin;   }
    void    setOrigin(QPoint o);

    // Width and height of a single cell in image-pixel coordinates.
    QSize   cellSize()  const { return m_cellSize;  }
    void    setCellSize(QSize s);

    // Number of columns and rows.
    int     cols()      const { return m_cols;      }
    int     rows()      const { return m_rows;      }

    // Resize the grid. Existing cells are preserved where they fit;
    // new cells default to CellType::Empty.
    void    resize(int cols, int rows);

    // Cell access ─────────────────────────────────────────────────────────────

    bool            isValid(int col, int row) const;
    bool            isValid(QPoint pos)       const { return isValid(pos.x(), pos.y()); }

    const CellData& cell(int col, int row)    const;
    const CellData& cell(QPoint pos)          const { return cell(pos.x(), pos.y()); }

    // Sets the cell and returns the previous CellData (needed by undo commands).
    CellData        setCell(int col, int row, const CellData& data);
    CellData        setCell(QPoint pos, const CellData& data) { return setCell(pos.x(), pos.y(), data); }

    // Clears a single cell back to Empty.
    void            clearCell(int col, int row);
    void            clearCell(QPoint pos) { clearCell(pos.x(), pos.y()); }

    // Bulk operations ─────────────────────────────────────────────────────────

    // Fill the entire grid with Empty.
    void            clear();

    // Rename every cell whose entityName matches oldName to newName.
    // Useful when a Room or GamePiece is renamed.
    void            renameEntity(const QString& oldName, const QString& newName);

    // Remove every cell assignment for the given entity name
    // (sets matching cells back to Empty).
    void            removeEntity(const QString& entityName);

    // Coordinate helpers ──────────────────────────────────────────────────────

    // Convert an image-pixel position to grid (col, row).
    // Returns {-1, -1} if the pixel is outside the grid.
    QPoint          pixelToCell(QPoint pixelPos)  const;

    // Convert a grid cell to the top-left pixel corner of that cell.
    QPoint          cellToPixel(int col, int row) const;
    QPoint          cellToPixel(QPoint pos)       const { return cellToPixel(pos.x(), pos.y()); }

    // Returns the 4 orthogonal neighbours that are inside the grid.
    QList<QPoint>   neighbours(int col, int row)  const;
    QList<QPoint>   neighbours(QPoint pos)        const { return neighbours(pos.x(), pos.y()); }

private:
    int                            m_cols    { 0 };
    int                            m_rows    { 0 };
    QPoint                         m_origin  { 0, 0 };
    QSize                          m_cellSize{ 100, 100 };
    std::vector<std::vector<CellData>> m_cells; // [row][col]

    // Sentinel returned for out-of-bounds reads.
    static const CellData s_emptyCell;
};

} // namespace Cluedo
