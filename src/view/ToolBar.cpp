#include "ToolBar.h"
#include "../model/BoardConfig.h"

#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QFrame>

namespace Cluedo {

ToolBar::ToolBar(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    updateToolVisibility();
}

void ToolBar::buildUi()
{
    setFixedWidth(180);

    // ── Mode slider ───────────────────────────────────────────────────────────
    auto* modeGroup  = new QGroupBox(QStringLiteral("Modus"));
    auto* modeLayout = new QVBoxLayout(modeGroup);

    auto* sliderRow  = new QHBoxLayout;
    auto* labelLeft  = new QLabel(QStringLiteral("Gitter-\neinstellung"));
    auto* labelRight = new QLabel(QStringLiteral("Gitter-\nzuordnung"));
    labelLeft->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    labelRight->setAlignment(Qt::AlignLeft  | Qt::AlignVCenter);
    labelLeft->setWordWrap(true);
    labelRight->setWordWrap(true);

    m_modeSlider = new QSlider(Qt::Horizontal);
    m_modeSlider->setRange(0, 1);
    m_modeSlider->setValue(0);
    m_modeSlider->setFixedWidth(40);
    m_modeSlider->setTickPosition(QSlider::TicksBelow);

    sliderRow->addWidget(labelLeft);
    sliderRow->addWidget(m_modeSlider);
    sliderRow->addWidget(labelRight);

    m_modeLabel = new QLabel(QStringLiteral("Gittereinstellung"));
    m_modeLabel->setAlignment(Qt::AlignCenter);
    QFont boldFont = m_modeLabel->font();
    boldFont.setBold(true);
    m_modeLabel->setFont(boldFont);

    modeLayout->addLayout(sliderRow);
    modeLayout->addWidget(m_modeLabel);

    // ── Entity buttons ────────────────────────────────────────────────────────
    auto* entityGroup  = new QGroupBox(QStringLiteral("Entitäten"));
    auto* entityLayout = new QVBoxLayout(entityGroup);

    m_addRoomBtn  = new QPushButton(QStringLiteral("+ Neuer Raum"));
    m_addPieceBtn = new QPushButton(QStringLiteral("+ Neue Figur"));
    m_addRoomBtn->setToolTip(QStringLiteral("Neuen Raum anlegen"));
    m_addPieceBtn->setToolTip(QStringLiteral("Neue Spielfigur anlegen"));

    entityLayout->addWidget(m_addRoomBtn);
    entityLayout->addWidget(m_addPieceBtn);

    // ── Tool panel (only in GridAssignment mode) ──────────────────────────────
    m_toolPanel        = new QWidget;
    auto* toolLayout   = new QVBoxLayout(m_toolPanel);
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(4);

    auto* toolGroup = new QGroupBox(QStringLiteral("Werkzeug"));
    auto* tgl       = new QVBoxLayout(toolGroup);
    tgl->setSpacing(4);

    auto makeToolBtn = [](const QString& label, const QString& color) {
        auto* btn = new QPushButton(label);
        btn->setCheckable(true);
        btn->setStyleSheet(
            QStringLiteral("QPushButton:checked { background: %1; color: white; font-weight: bold; }")
            .arg(color));
        return btn;
    };

    m_toolEmpty = makeToolBtn(QStringLiteral("✕  Radiergummi"), QStringLiteral("#666"));
    m_toolPath  = makeToolBtn(QStringLiteral("⬜  Weg"),          QStringLiteral("#3a8adf"));
    m_toolRoom  = makeToolBtn(QStringLiteral("🟩  Raum"),         QStringLiteral("#3a9e5a"));
    m_toolDoor  = makeToolBtn(QStringLiteral("🚪  Tür"),          QStringLiteral("#d4860a"));
    m_toolPiece = makeToolBtn(QStringLiteral("🟡  Startposition"),QStringLiteral("#c5aa00"));

    // Room selector
    m_roomCombo = new QComboBox;
    m_roomCombo->setToolTip(QStringLiteral("Aktiver Raum"));
    m_roomCombo->setPlaceholderText(QStringLiteral("– Raum wählen –"));

    // Piece selector
    m_pieceCombo = new QComboBox;
    m_pieceCombo->setToolTip(QStringLiteral("Aktive Spielfigur"));
    m_pieceCombo->setPlaceholderText(QStringLiteral("– Figur wählen –"));

    tgl->addWidget(m_toolEmpty);
    tgl->addWidget(m_toolPath);
    tgl->addWidget(m_toolRoom);
    tgl->addWidget(m_roomCombo);
    tgl->addWidget(m_toolDoor);
    tgl->addWidget(m_toolPiece);
    tgl->addWidget(m_pieceCombo);

    toolLayout->addWidget(toolGroup);

    // Make tool buttons mutually exclusive
    auto* btnGroup = new QButtonGroup(this);
    btnGroup->addButton(m_toolEmpty);
    btnGroup->addButton(m_toolPath);
    btnGroup->addButton(m_toolRoom);
    btnGroup->addButton(m_toolDoor);
    btnGroup->addButton(m_toolPiece);
    btnGroup->setExclusive(true);
    m_toolPath->setChecked(true); // default

    // ── Main layout ───────────────────────────────────────────────────────────
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(8);
    mainLayout->addWidget(modeGroup);
    mainLayout->addWidget(entityGroup);
    mainLayout->addWidget(m_toolPanel);
    mainLayout->addStretch();

    // ── Connections ───────────────────────────────────────────────────────────
    connect(m_modeSlider, &QSlider::valueChanged,
            this, &ToolBar::onSliderValueChanged);

    connect(m_addRoomBtn,  &QPushButton::clicked,
            this, &ToolBar::addRoomRequested);
    connect(m_addPieceBtn, &QPushButton::clicked,
            this, &ToolBar::addPieceRequested);

    auto connectTool = [this](QPushButton* btn, CellType type) {
        connect(btn, &QPushButton::clicked, this, [this, type]() {
            m_activeTool = type;
            updateToolVisibility();
            emit toolChanged(type);
        });
    };
    connectTool(m_toolEmpty, CellType::Empty);
    connectTool(m_toolPath,  CellType::Path);
    connectTool(m_toolRoom,  CellType::Room);
    connectTool(m_toolDoor,  CellType::Door);
    connectTool(m_toolPiece, CellType::PieceStart);

    connect(m_roomCombo,  &QComboBox::currentTextChanged,
            this, &ToolBar::activeRoomChanged);
    connect(m_pieceCombo, &QComboBox::currentTextChanged,
            this, &ToolBar::activePieceChanged);
}

void ToolBar::onSliderValueChanged(int value)
{
    m_mode = (value == 0) ? AppMode::GridSettings : AppMode::GridAssignment;
    m_modeLabel->setText(
        m_mode == AppMode::GridSettings
            ? QStringLiteral("Gittereinstellung")
            : QStringLiteral("Gitterzuordnung"));
    updateToolVisibility();
    emit modeChanged(m_mode);
}

void ToolBar::updateToolVisibility()
{
    m_toolPanel->setVisible(m_mode == AppMode::GridAssignment);
    m_roomCombo->setVisible(m_activeTool == CellType::Room);
    m_pieceCombo->setVisible(m_activeTool == CellType::PieceStart);
}

void ToolBar::syncWithConfig(const BoardConfig* config)
{
    if (!config) return;

    const QString prevRoom  = m_roomCombo->currentText();
    const QString prevPiece = m_pieceCombo->currentText();

    m_roomCombo->blockSignals(true);
    m_pieceCombo->blockSignals(true);

    m_roomCombo->clear();
    for (const auto& r : config->rooms())
        m_roomCombo->addItem(r.name);

    m_pieceCombo->clear();
    for (const auto& p : config->pieces())
        m_pieceCombo->addItem(p.name);

    // Restore previous selection if still available
    int roomIdx = m_roomCombo->findText(prevRoom);
    m_roomCombo->setCurrentIndex(roomIdx >= 0 ? roomIdx : 0);

    int pieceIdx = m_pieceCombo->findText(prevPiece);
    m_pieceCombo->setCurrentIndex(pieceIdx >= 0 ? pieceIdx : 0);

    m_roomCombo->blockSignals(false);
    m_pieceCombo->blockSignals(false);
}

AppMode  ToolBar::currentMode()      const { return m_mode; }
CellType ToolBar::currentTool()      const { return m_activeTool; }
QString  ToolBar::activeRoomName()   const { return m_roomCombo->currentText(); }
QString  ToolBar::activePieceName()  const { return m_pieceCombo->currentText(); }

} // namespace Cluedo
#include "moc_ToolBar.cpp"
