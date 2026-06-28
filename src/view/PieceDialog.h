#pragma once

#include <QDialog>
#include "../model/GamePiece.h"

class QLineEdit;
class QPushButton;

namespace Cluedo {

// ── PieceDialog ───────────────────────────────────────────────────────────────
// Modal dialog for creating or editing a GamePiece.
// Mirrors RoomDialog: name + imagePath, auto-suggest on name change.
// ─────────────────────────────────────────────────────────────────────────────
class PieceDialog : public QDialog {
    Q_OBJECT

public:
    explicit PieceDialog(QWidget* parent = nullptr);

    void setPiece(const GamePiece& piece);
    GamePiece piece() const;

private slots:
    void onNameChanged(const QString& name);
    void onBrowseImage();

private:
    void buildUi();
    bool validate();

    QLineEdit*   m_nameEdit      { nullptr };
    QLineEdit*   m_imagePathEdit { nullptr };
    QPushButton* m_okButton      { nullptr };
    bool         m_imagePathUserEdited { false };
};

} // namespace Cluedo
