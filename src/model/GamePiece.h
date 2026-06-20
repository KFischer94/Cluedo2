#pragma once

#include <QString>
#include <QPoint>

namespace Cluedo {

// ── GamePiece ─────────────────────────────────────────────────────────────────
// Represents a player token / suspect figure.
// ─────────────────────────────────────────────────────────────────────────────
struct GamePiece {
    // Unique display name (e.g. "Scarlett").
    QString name;

    // Path to a figure/card image. Defaults to "/Figur/<name>.png".
    QString imagePath;

    // Starting grid position (col, row).
    // {-1, -1} means "not yet placed".
    QPoint  startPosition { -1, -1 };

    // ── Helpers ───────────────────────────────────────────────────────────────

    bool isPlaced() const
    {
        return startPosition.x() >= 0 && startPosition.y() >= 0;
    }

    static QString defaultImagePath(const QString& pieceName)
    {
        return QStringLiteral("/Figur/") + pieceName + QStringLiteral(".png");
    }
};

} // namespace Cluedo
