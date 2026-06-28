#pragma once

#include <QWidget>
#include "../model/CellState.h"

class QComboBox;
class QPushButton;
class QSlider;
class QLabel;
class QStackedWidget;

namespace Cluedo {

class BoardConfig;

// ── AppMode ───────────────────────────────────────────────────────────────────
enum class AppMode {
    GridSettings,   // Schieberegler links  – Gittereinstellung
    GridAssignment  // Schieberegler rechts – Gitterzuordnung
};

// ── ToolBar ───────────────────────────────────────────────────────────────────
// Left toolbar docked to the MainWindow.
// Contains:
//   • Mode slider (Gittereinstellung ↔ Gitterzuordnung)
//   • "Neuer Raum" button  → emits addRoomRequested()
//   • "Neue Figur" button  → emits addPieceRequested()
//   • Tool selection (only visible in GridAssignment mode):
//       – Radiergummi (Empty)
//       – Weg
//       – Raum  + room selector ComboBox
//       – Tür
//       – Startposition + piece selector ComboBox
// ─────────────────────────────────────────────────────────────────────────────
class ToolBar : public QWidget {
    Q_OBJECT

public:
    explicit ToolBar(QWidget* parent = nullptr);

    // Call after rooms / pieces are added or removed
    void syncWithConfig(const BoardConfig* config);

    AppMode  currentMode()        const;
    CellType currentTool()        const;
    QString  activeRoomName()     const;
    QString  activePieceName()    const;

signals:
    void modeChanged(AppMode mode);
    void toolChanged(CellType tool);
    void activeRoomChanged(const QString& name);
    void activePieceChanged(const QString& name);
    void addRoomRequested();
    void addPieceRequested();

private slots:
    void onSliderValueChanged(int value);
    void onToolButtonClicked();

private:
    void buildUi();
    void updateToolVisibility();

    // Mode slider
    QSlider*      m_modeSlider   { nullptr };
    QLabel*       m_modeLabel    { nullptr };

    // Entity buttons
    QPushButton*  m_addRoomBtn   { nullptr };
    QPushButton*  m_addPieceBtn  { nullptr };

    // Tool buttons (assignment mode)
    QWidget*      m_toolPanel    { nullptr };
    QPushButton*  m_toolEmpty    { nullptr };
    QPushButton*  m_toolPath     { nullptr };
    QPushButton*  m_toolRoom     { nullptr };
    QPushButton*  m_toolDoor     { nullptr };
    QPushButton*  m_toolPiece    { nullptr };

    QComboBox*    m_roomCombo    { nullptr };
    QComboBox*    m_pieceCombo   { nullptr };

    CellType      m_activeTool   { CellType::Path };
    AppMode       m_mode         { AppMode::GridSettings };
};

} // namespace Cluedo
