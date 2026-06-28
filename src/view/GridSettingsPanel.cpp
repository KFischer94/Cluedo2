#include "GridSettingsPanel.h"

#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QScrollArea>

namespace Cluedo {

GridSettingsPanel::GridSettingsPanel(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
    connectSignals();
}

void GridSettingsPanel::buildUi()
{
    auto makeSpinBox = [](int min, int max, int value) -> QSpinBox* {
        auto* sb = new QSpinBox;
        sb->setRange(min, max);
        sb->setValue(value);
        sb->setFixedWidth(80);
        return sb;
    };

    // ── Origin ────────────────────────────────────────────────────────────────
    m_originX = makeSpinBox(-9999, 9999, 0);
    m_originY = makeSpinBox(-9999, 9999, 0);

    auto* originGroup  = new QGroupBox(QStringLiteral("Raster-Startpunkt"));
    auto* originLayout = new QFormLayout(originGroup);
    originLayout->addRow(QStringLiteral("X:"), m_originX);
    originLayout->addRow(QStringLiteral("Y:"), m_originY);

    // ── Cell size ─────────────────────────────────────────────────────────────
    m_cellW = makeSpinBox(4, 1000, 100);
    m_cellH = makeSpinBox(4, 1000, 100);

    auto* cellGroup  = new QGroupBox(QStringLiteral("Feldgröße (Pixel)"));
    auto* cellLayout = new QFormLayout(cellGroup);
    cellLayout->addRow(QStringLiteral("Breite:"),  m_cellW);
    cellLayout->addRow(QStringLiteral("Höhe:"),    m_cellH);

    // ── Dimensions ────────────────────────────────────────────────────────────
    m_cols = makeSpinBox(1, 500, 20);
    m_rows = makeSpinBox(1, 500, 20);

    auto* dimGroup  = new QGroupBox(QStringLiteral("Rastergröße (Felder)"));
    auto* dimLayout = new QFormLayout(dimGroup);
    dimLayout->addRow(QStringLiteral("Spalten:"), m_cols);
    dimLayout->addRow(QStringLiteral("Zeilen:"),  m_rows);

    // ── Hint label ────────────────────────────────────────────────────────────
    auto* hint = new QLabel(
        QStringLiteral("<small><i>Strg+Mausrad: Ansichtszoom<br>"
                       "Mittlere Maustaste: Verschieben</i></small>"));
    hint->setWordWrap(true);
    hint->setAlignment(Qt::AlignCenter);

    // ── Assemble ──────────────────────────────────────────────────────────────
    auto* inner  = new QWidget;
    auto* layout = new QVBoxLayout(inner);
    layout->setSpacing(12);
    layout->addWidget(originGroup);
    layout->addWidget(cellGroup);
    layout->addWidget(dimGroup);
    layout->addWidget(hint);
    layout->addStretch();

    auto* scroll = new QScrollArea(this);
    scroll->setWidget(inner);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);

    setMinimumWidth(200);
}

void GridSettingsPanel::connectSignals()
{
    auto emitOrigin = [this]() {
        emit originChanged(m_originX->value(), m_originY->value());
    };
    connect(m_originX, &QSpinBox::valueChanged, this, emitOrigin);
    connect(m_originY, &QSpinBox::valueChanged, this, emitOrigin);

    auto emitCell = [this]() {
        emit cellSizeChanged(m_cellW->value(), m_cellH->value());
    };
    connect(m_cellW, &QSpinBox::valueChanged, this, emitCell);
    connect(m_cellH, &QSpinBox::valueChanged, this, emitCell);

    auto emitDim = [this]() {
        emit dimensionsChanged(m_cols->value(), m_rows->value());
    };
    connect(m_cols, &QSpinBox::valueChanged, this, emitDim);
    connect(m_rows, &QSpinBox::valueChanged, this, emitDim);
}

void GridSettingsPanel::setValues(int originX, int originY,
                                   int cellW,   int cellH,
                                   int cols,    int rows)
{
    // Block signals while setting to avoid cascading updates
    const QList<QSpinBox*> all = {
        m_originX, m_originY, m_cellW, m_cellH, m_cols, m_rows };
    for (auto* sb : all) sb->blockSignals(true);

    m_originX->setValue(originX);
    m_originY->setValue(originY);
    m_cellW->setValue(cellW);
    m_cellH->setValue(cellH);
    m_cols->setValue(cols);
    m_rows->setValue(rows);

    for (auto* sb : all) sb->blockSignals(false);
}

} // namespace Cluedo
#include "moc_GridSettingsPanel.cpp"
