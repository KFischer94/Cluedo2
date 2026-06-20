#pragma once

#include <QString>
#include <QPoint>
#include <QList>

namespace Cluedo {

// ── Room ──────────────────────────────────────────────────────────────────────
// Stores the metadata for one room. Grid-cell positions are tracked here
// as well so the model has a single source of truth; the GridModel stores
// per-cell references back to the room name for fast pixel-level queries.
// ─────────────────────────────────────────────────────────────────────────────
struct Room {
    // Unique display name (used as key throughout the config).
    QString         name;

    // Path to a room-card image. Defaults to "/Raum/<name>.png".
    QString         imagePath;

    // Grid positions (col, row) that belong to this room.
    QList<QPoint>   cells;

    // Door cells (col, row) attached to this room.
    // The full DoorConnection objects live in BoardConfig::doors;
    // this list is a convenience index.
    QList<QPoint>   doorCells;

    // Name of the room reachable via a secret passage, or empty if none.
    QString         secretPassage;

    // ── Helpers ───────────────────────────────────────────────────────────────

    bool hasCells()         const { return !cells.isEmpty();     }
    bool hasDoors()         const { return !doorCells.isEmpty(); }
    bool hasSecretPassage() const { return !secretPassage.isEmpty(); }

    void addCell(QPoint pos)     { if (!cells.contains(pos))     cells.append(pos);     }
    void removeCell(QPoint pos)  { cells.removeAll(pos);                                }
    void addDoor(QPoint pos)     { if (!doorCells.contains(pos)) doorCells.append(pos); }
    void removeDoor(QPoint pos)  { doorCells.removeAll(pos);                            }

    // Suggest a default image path from the room name.
    static QString defaultImagePath(const QString& roomName)
    {
        return QStringLiteral("/Raum/") + roomName + QStringLiteral(".png");
    }
};

} // namespace Cluedo
