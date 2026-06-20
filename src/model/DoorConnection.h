#pragma once

#include <QString>
#include <QPoint>

namespace Cluedo {

// ── DoorConnection ────────────────────────────────────────────────────────────
// A door occupies exactly one grid cell and connects a named room to a
// specific adjacent path cell.
//
//   cell        – grid position (col, row) of the door tile itself.
//   roomName    – the room this door belongs to.
//   pathCell    – the adjacent path cell (col, row) that the door opens onto.
//                 {-1,-1} if not yet assigned.
// ─────────────────────────────────────────────────────────────────────────────
struct DoorConnection {
    QPoint  cell      { -1, -1 };
    QString roomName;
    QPoint  pathCell  { -1, -1 };

    bool isComplete() const
    {
        return !roomName.isEmpty()
            && pathCell.x() >= 0
            && pathCell.y() >= 0;
    }
};

} // namespace Cluedo
