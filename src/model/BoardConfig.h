#pragma once

#include "GridModel.h"
#include "Room.h"
#include "GamePiece.h"
#include "DoorConnection.h"

#include <QString>
#include <QList>

namespace Cluedo {

// ── ValidationIssue ───────────────────────────────────────────────────────────

enum class IssueSeverity { Error, Warning };

struct ValidationIssue {
    IssueSeverity severity;
    QString       entityType;   // e.g. "Room", "GamePiece", "Door", "Grid"
    QString       entityName;   // name of the offending object (may be empty)
    QString       message;      // human-readable description
};

// ── BoardConfig ───────────────────────────────────────────────────────────────
// Single owner of all board configuration data.
// Provides CRUD helpers for rooms / pieces / doors and JSON serialisation.
// ─────────────────────────────────────────────────────────────────────────────
class BoardConfig {
public:
    explicit BoardConfig(int cols = 20, int rows = 20);

    // ── Grid ──────────────────────────────────────────────────────────────────
    GridModel&       grid()       { return m_grid; }
    const GridModel& grid() const { return m_grid; }

    // ── Rooms ─────────────────────────────────────────────────────────────────
    QList<Room>&       rooms()       { return m_rooms; }
    const QList<Room>& rooms() const { return m_rooms; }

    // Returns nullptr if not found.
    Room*       roomByName(const QString& name);
    const Room* roomByName(const QString& name) const;

    // Returns true on success; false if a room with that name already exists.
    bool addRoom(const Room& room);

    // Returns true if a room with that name existed and was removed.
    // Also cleans up associated grid cells and door connections.
    bool removeRoom(const QString& name);

    // Rename a room (updates grid cells, door connections, secret-passage refs).
    bool renameRoom(const QString& oldName, const QString& newName);

    // ── GamePieces ────────────────────────────────────────────────────────────
    QList<GamePiece>&       pieces()       { return m_pieces; }
    const QList<GamePiece>& pieces() const { return m_pieces; }

    GamePiece*       pieceByName(const QString& name);
    const GamePiece* pieceByName(const QString& name) const;

    bool addPiece(const GamePiece& piece);
    bool removePiece(const QString& name);
    bool renamePiece(const QString& oldName, const QString& newName);

    // ── DoorConnections ───────────────────────────────────────────────────────
    QList<DoorConnection>&       doors()       { return m_doors; }
    const QList<DoorConnection>& doors() const { return m_doors; }

    // Returns nullptr if not found.
    DoorConnection*       doorAt(QPoint cell);
    const DoorConnection* doorAt(QPoint cell) const;

    // Adds or replaces a door at door.cell.
    void upsertDoor(const DoorConnection& door);

    // Removes the door at the given cell position.
    void removeDoor(QPoint cell);

    // ── Derived data ──────────────────────────────────────────────────────────

    // maxPlayers is derived from the number of pieces.
    int maxPlayers() const { return m_pieces.size(); }

    // Collect all path cell positions from the grid.
    QList<QPoint> pathCells() const;

    // ── Validation ────────────────────────────────────────────────────────────
    // Returns all issues. If the list contains any Error-severity items,
    // the config must not be saved.
    QList<ValidationIssue> validate() const;

    bool hasErrors() const;

    // ── JSON I/O ──────────────────────────────────────────────────────────────

    // Serialises to the given file path. Runs validation first;
    // returns false and populates outIssues if there are errors.
    bool saveToFile(const QString& filePath,
                    QList<ValidationIssue>& outIssues) const;

    // Deserialises from a JSON file. Returns false on parse error.
    bool loadFromFile(const QString& filePath, QString& outError);

private:
    GridModel            m_grid;
    QList<Room>          m_rooms;
    QList<GamePiece>     m_pieces;
    QList<DoorConnection>m_doors;

    // Internal helpers
    void cleanupRoomReferences(const QString& roomName);
};

} // namespace Cluedo
