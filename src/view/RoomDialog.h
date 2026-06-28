#pragma once

#include <QDialog>
#include "../model/Room.h"

class QLineEdit;
class QPushButton;

namespace Cluedo {

// ── RoomDialog ────────────────────────────────────────────────────────────────
// Modal dialog for creating or editing a Room.
// Shows name + imagePath fields; imagePath auto-suggests on name change.
// ─────────────────────────────────────────────────────────────────────────────
class RoomDialog : public QDialog {
    Q_OBJECT

public:
    explicit RoomDialog(QWidget* parent = nullptr);

    // Pre-fill fields for editing an existing room.
    void setRoom(const Room& room);

    // Returns the room as configured by the user (only valid after Accepted).
    Room room() const;

private slots:
    void onNameChanged(const QString& name);
    void onBrowseImage();

private:
    void buildUi();
    bool validate();

    QLineEdit*   m_nameEdit      { nullptr };
    QLineEdit*   m_imagePathEdit { nullptr };
    QPushButton* m_okButton      { nullptr };

    // Track whether the user has manually edited the image path
    bool m_imagePathUserEdited { false };
};

} // namespace Cluedo
