#include "BoardConfig.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

namespace Cluedo {

// ── Construction ──────────────────────────────────────────────────────────────

BoardConfig::BoardConfig(int cols, int rows)
    : m_grid(cols, rows)
{}

// ── Rooms ─────────────────────────────────────────────────────────────────────

Room* BoardConfig::roomByName(const QString& name)
{
    for (auto& r : m_rooms)
        if (r.name == name)
            return &r;
    return nullptr;
}

const Room* BoardConfig::roomByName(const QString& name) const
{
    for (const auto& r : m_rooms)
        if (r.name == name)
            return &r;
    return nullptr;
}

bool BoardConfig::addRoom(const Room& room)
{
    if (roomByName(room.name))
        return false;
    m_rooms.append(room);
    return true;
}

bool BoardConfig::removeRoom(const QString& name)
{
    for (int i = 0; i < m_rooms.size(); ++i) {
        if (m_rooms[i].name == name) {
            m_rooms.removeAt(i);
            cleanupRoomReferences(name);
            return true;
        }
    }
    return false;
}

bool BoardConfig::renameRoom(const QString& oldName, const QString& newName)
{
    if (oldName == newName)
        return true;
    if (roomByName(newName))
        return false; // target name already taken

    Room* r = roomByName(oldName);
    if (!r)
        return false;
    r->name = newName;

    // Update grid cells
    m_grid.renameEntity(oldName, newName);

    // Update door connections
    for (auto& d : m_doors)
        if (d.roomName == oldName)
            d.roomName = newName;

    // Update secret passage references
    for (auto& room : m_rooms)
        if (room.secretPassage == oldName)
            room.secretPassage = newName;

    return true;
}

// ── GamePieces ────────────────────────────────────────────────────────────────

GamePiece* BoardConfig::pieceByName(const QString& name)
{
    for (auto& p : m_pieces)
        if (p.name == name)
            return &p;
    return nullptr;
}

const GamePiece* BoardConfig::pieceByName(const QString& name) const
{
    for (const auto& p : m_pieces)
        if (p.name == name)
            return &p;
    return nullptr;
}

bool BoardConfig::addPiece(const GamePiece& piece)
{
    if (pieceByName(piece.name))
        return false;
    m_pieces.append(piece);
    return true;
}

bool BoardConfig::removePiece(const QString& name)
{
    for (int i = 0; i < m_pieces.size(); ++i) {
        if (m_pieces[i].name == name) {
            m_grid.removeEntity(name);
            m_pieces.removeAt(i);
            return true;
        }
    }
    return false;
}

bool BoardConfig::renamePiece(const QString& oldName, const QString& newName)
{
    if (oldName == newName)
        return true;
    if (pieceByName(newName))
        return false;

    GamePiece* p = pieceByName(oldName);
    if (!p)
        return false;
    p->name = newName;
    m_grid.renameEntity(oldName, newName);
    return true;
}

// ── DoorConnections ───────────────────────────────────────────────────────────

DoorConnection* BoardConfig::doorAt(QPoint cell)
{
    for (auto& d : m_doors)
        if (d.cell == cell)
            return &d;
    return nullptr;
}

const DoorConnection* BoardConfig::doorAt(QPoint cell) const
{
    for (const auto& d : m_doors)
        if (d.cell == cell)
            return &d;
    return nullptr;
}

void BoardConfig::upsertDoor(const DoorConnection& door)
{
    for (auto& d : m_doors) {
        if (d.cell == door.cell) {
            d = door;
            return;
        }
    }
    m_doors.append(door);
}

void BoardConfig::removeDoor(QPoint cell)
{
    m_doors.removeIf([&cell](const DoorConnection& d) {
        return d.cell == cell;
    });

    // Remove from owning room's doorCells list
    for (auto& r : m_rooms)
        r.removeDoor(cell);
}

// ── Derived data ──────────────────────────────────────────────────────────────

QList<QPoint> BoardConfig::pathCells() const
{
    QList<QPoint> result;
    for (int row = 0; row < m_grid.rows(); ++row)
        for (int col = 0; col < m_grid.cols(); ++col)
            if (m_grid.cell(col, row).isPath())
                result.append({ col, row });
    return result;
}

// ── Validation ────────────────────────────────────────────────────────────────

QList<ValidationIssue> BoardConfig::validate() const
{
    QList<ValidationIssue> issues;

    auto addError = [&](const QString& type, const QString& name, const QString& msg) {
        issues.append({ IssueSeverity::Error, type, name, msg });
    };
    auto addWarning = [&](const QString& type, const QString& name, const QString& msg) {
        issues.append({ IssueSeverity::Warning, type, name, msg });
    };

    // 1. Rooms without cells
    for (const auto& r : m_rooms)
        if (!r.hasCells())
            addError("Room", r.name, QStringLiteral("Raum hat keine zugeordneten Felder."));

    // 2. Rooms without doors
    for (const auto& r : m_rooms)
        if (!r.hasDoors())
            addError("Room", r.name, QStringLiteral("Raum hat keine Türe."));

    // 3. Pieces without start position
    for (const auto& p : m_pieces)
        if (!p.isPlaced())
            addError("GamePiece", p.name, QStringLiteral("Spielfigur hat keine Startposition."));

    // 4. Duplicate start positions
    QSet<QPair<int,int>> usedStarts;
    for (const auto& p : m_pieces) {
        if (!p.isPlaced()) continue;
        auto key = QPair<int,int>{ p.startPosition.x(), p.startPosition.y() };
        if (usedStarts.contains(key))
            addError("GamePiece", p.name, QStringLiteral("Startposition bereits von einer anderen Figur belegt."));
        else
            usedStarts.insert(key);
    }

    // 5. No path cells at all
    const auto paths = pathCells();
    if (paths.isEmpty())
        addError("Grid", QString{}, QStringLiteral("Das Gitter enthält keine Wege."));

    // 6. Isolated path cells (flood-fill from the first path cell)
    if (!paths.isEmpty()) {
        QSet<QPair<int,int>> visited;
        QList<QPoint>        queue { paths.first() };
        visited.insert({ paths.first().x(), paths.first().y() });

        while (!queue.isEmpty()) {
            QPoint current = queue.takeFirst();
            for (const QPoint& nb : m_grid.neighbours(current)) {
                const auto& c = m_grid.cell(nb);
                if ((c.isPath() || c.isDoor()) ) {
                    auto key = QPair<int,int>{ nb.x(), nb.y() };
                    if (!visited.contains(key)) {
                        visited.insert(key);
                        queue.append(nb);
                    }
                }
            }
        }

        for (const QPoint& p : paths) {
            if (!visited.contains({ p.x(), p.y() }))
                addError("Grid", QString{},
                    QStringLiteral("Isoliertes Feld bei (%1, %2).").arg(p.x()).arg(p.y()));
        }
    }

    // 7. Door without room assignment
    for (const auto& d : m_doors)
        if (!d.isComplete())
            addError("Door",
                QStringLiteral("(%1,%2)").arg(d.cell.x()).arg(d.cell.y()),
                QStringLiteral("Tür hat keine vollständige Zuordnung (Raum oder Wegfeld fehlt)."));

    // 8. Door path-cell is not actually a path cell
    for (const auto& d : m_doors) {
        if (!d.isComplete()) continue;
        if (!m_grid.cell(d.pathCell).isPath())
            addError("Door",
                QStringLiteral("(%1,%2)").arg(d.cell.x()).arg(d.cell.y()),
                QStringLiteral("Das zugeordnete Wegfeld (%1,%2) ist kein Wegfeld.")
                    .arg(d.pathCell.x()).arg(d.pathCell.y()));
    }

    // 9. Secret passage points to non-existing room
    for (const auto& r : m_rooms) {
        if (!r.hasSecretPassage()) continue;
        if (!roomByName(r.secretPassage))
            addError("Room", r.name,
                QStringLiteral("Geheimgang zeigt auf unbekannten Raum \"%1\".").arg(r.secretPassage));
    }

    // 10. Secret passage not mutual (warning only)
    for (const auto& r : m_rooms) {
        if (!r.hasSecretPassage()) continue;
        const Room* target = roomByName(r.secretPassage);
        if (target && target->secretPassage != r.name)
            addWarning("Room", r.name,
                QStringLiteral("Geheimgang zu \"%1\" ist nicht gegenseitig.").arg(r.secretPassage));
    }

    return issues;
}

bool BoardConfig::hasErrors() const
{
    for (const auto& issue : validate())
        if (issue.severity == IssueSeverity::Error)
            return true;
    return false;
}

// ── JSON helpers ──────────────────────────────────────────────────────────────

static QJsonArray pointToArray(QPoint p)
{
    return QJsonArray{ p.x(), p.y() };
}

static QPoint arrayToPoint(const QJsonArray& a)
{
    if (a.size() < 2) return { -1, -1 };
    return { a[0].toInt(), a[1].toInt() };
}

// ── JSON save ─────────────────────────────────────────────────────────────────

bool BoardConfig::saveToFile(const QString& filePath,
                              QList<ValidationIssue>& outIssues) const
{
    outIssues = validate();
    for (const auto& issue : outIssues)
        if (issue.severity == IssueSeverity::Error)
            return false;

    QJsonObject root;

    // grid
    QJsonObject gridObj;
    gridObj["origin"]     = pointToArray(m_grid.origin());
    gridObj["cellSize"]   = pointToArray({ m_grid.cellSize().width(),
                                           m_grid.cellSize().height() });
    gridObj["dimensions"] = pointToArray({ m_grid.cols(), m_grid.rows() });
    root["grid"] = gridObj;

    // maxPlayers
    root["maxPlayers"] = maxPlayers();

    // pieces
    QJsonArray piecesArr;
    for (const auto& p : m_pieces) {
        QJsonObject obj;
        obj["name"]          = p.name;
        obj["imagePath"]     = p.imagePath;
        obj["startPosition"] = pointToArray(p.startPosition);
        piecesArr.append(obj);
    }
    root["pieces"] = piecesArr;

    // rooms
    QJsonArray roomsArr;
    for (const auto& r : m_rooms) {
        QJsonObject obj;
        obj["name"]      = r.name;
        obj["imagePath"] = r.imagePath;

        QJsonArray cells;
        for (const auto& c : r.cells)
            cells.append(pointToArray(c));
        obj["cells"] = cells;

        QJsonArray doors;
        for (const auto& d : r.doorCells)
            doors.append(pointToArray(d));
        obj["doors"] = doors;

        obj["secretPassage"] = r.secretPassage; // empty string if none
        roomsArr.append(obj);
    }
    root["rooms"] = roomsArr;

    // doors (full connection data)
    QJsonArray doorsArr;
    for (const auto& d : m_doors) {
        QJsonObject obj;
        obj["cell"] = pointToArray(d.cell);
        QJsonArray connectsTo;
        connectsTo.append(d.roomName);
        connectsTo.append(pointToArray(d.pathCell));
        obj["connectsTo"] = connectsTo;
        doorsArr.append(obj);
    }
    root["doors"] = doorsArr;

    // paths
    QJsonArray pathsArr;
    for (const auto& p : pathCells())
        pathsArr.append(pointToArray(p));
    root["paths"] = pathsArr;

    // Write file
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

// ── JSON load ─────────────────────────────────────────────────────────────────

bool BoardConfig::loadFromFile(const QString& filePath, QString& outError)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outError = QStringLiteral("Datei konnte nicht geöffnet werden: ") + filePath;
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (doc.isNull()) {
        outError = QStringLiteral("JSON-Fehler: ") + parseError.errorString();
        return false;
    }

    QJsonObject root = doc.object();

    // grid
    if (root.contains("grid")) {
        QJsonObject g = root["grid"].toObject();
        QPoint origin   = arrayToPoint(g["origin"].toArray());
        QPoint cellSize = arrayToPoint(g["cellSize"].toArray());
        QPoint dims     = arrayToPoint(g["dimensions"].toArray());

        m_grid = GridModel(dims.x(), dims.y());
        m_grid.setOrigin(origin);
        m_grid.setCellSize({ cellSize.x(), cellSize.y() });
    }

    m_rooms.clear();
    m_pieces.clear();
    m_doors.clear();

    // pieces
    for (const auto& pVal : root["pieces"].toArray()) {
        QJsonObject o = pVal.toObject();
        GamePiece p;
        p.name          = o["name"].toString();
        p.imagePath     = o["imagePath"].toString();
        p.startPosition = arrayToPoint(o["startPosition"].toArray());
        m_pieces.append(p);

        // Restore grid cell
        if (p.isPlaced())
            m_grid.setCell(p.startPosition,
                           { CellType::PieceStart, p.name });
    }

    // rooms
    for (const auto& rVal : root["rooms"].toArray()) {
        QJsonObject o = rVal.toObject();
        Room r;
        r.name          = o["name"].toString();
        r.imagePath     = o["imagePath"].toString();
        r.secretPassage = o["secretPassage"].toString();

        for (const auto& cVal : o["cells"].toArray()) {
            QPoint pos = arrayToPoint(cVal.toArray());
            r.cells.append(pos);
            m_grid.setCell(pos, { CellType::Room, r.name });
        }
        for (const auto& dVal : o["doors"].toArray()) {
            QPoint pos = arrayToPoint(dVal.toArray());
            r.doorCells.append(pos);
            m_grid.setCell(pos, { CellType::Door, r.name });
        }
        m_rooms.append(r);
    }

    // doors
    for (const auto& dVal : root["doors"].toArray()) {
        QJsonObject o = dVal.toObject();
        DoorConnection dc;
        dc.cell      = arrayToPoint(o["cell"].toArray());
        QJsonArray ct = o["connectsTo"].toArray();
        if (ct.size() == 2) {
            dc.roomName = ct[0].toString();
            dc.pathCell = arrayToPoint(ct[1].toArray());
        }
        m_doors.append(dc);
    }

    // paths – restore grid cells
    for (const auto& pVal : root["paths"].toArray()) {
        QPoint pos = arrayToPoint(pVal.toArray());
        m_grid.setCell(pos, { CellType::Path, {} });
    }

    return true;
}

// ── Internal helpers ──────────────────────────────────────────────────────────

void BoardConfig::cleanupRoomReferences(const QString& roomName)
{
    // Remove grid cells
    m_grid.removeEntity(roomName);

    // Remove door connections that belong to this room
    m_doors.removeIf([&roomName](const DoorConnection& d) {
        return d.roomName == roomName;
    });

    // Clear secret passage references from other rooms
    for (auto& r : m_rooms)
        if (r.secretPassage == roomName)
            r.secretPassage.clear();
}

} // namespace Cluedo
