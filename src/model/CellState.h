#pragma once

#include <QString>

namespace Cluedo {

// ── Cell type ─────────────────────────────────────────────────────────────────
enum class CellType {
    Empty,       // Not part of the playable board
    Path,        // Walkable corridor
    Room,        // Belongs to a named room
    Door,        // Connects a room to a path cell
    PieceStart   // Starting position of a game piece
};

// ── Per-cell metadata ─────────────────────────────────────────────────────────
// For Room and Door cells the entityName holds the room name.
// For PieceStart cells the entityName holds the piece name.
// For Path/Empty cells entityName is empty.
struct CellData {
    CellType    type       { CellType::Empty };
    QString     entityName { };               // room name or piece name

    bool isEmpty()      const { return type == CellType::Empty;      }
    bool isPath()       const { return type == CellType::Path;        }
    bool isRoom()       const { return type == CellType::Room;        }
    bool isDoor()       const { return type == CellType::Door;        }
    bool isPieceStart() const { return type == CellType::PieceStart;  }
};

} // namespace Cluedo
