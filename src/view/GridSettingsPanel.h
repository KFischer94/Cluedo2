#pragma once

#include <QWidget>

class QSpinBox;
class QLabel;

namespace Cluedo {

// ── GridSettingsPanel ─────────────────────────────────────────────────────────
// Right-side panel for configuring the grid geometry.
// Emits signals when values change so MainWindow can update the model.
// ─────────────────────────────────────────────────────────────────────────────
class GridSettingsPanel : public QWidget {
    Q_OBJECT

public:
    explicit GridSettingsPanel(QWidget* parent = nullptr);

    // Programmatic update (e.g. when loading a config)
    void setValues(int originX, int originY,
                   int cellW,   int cellH,
                   int cols,    int rows);

signals:
    void originChanged(int x, int y);
    void cellSizeChanged(int w, int h);
    void dimensionsChanged(int cols, int rows);

private:
    void buildUi();
    void connectSignals();

    QSpinBox* m_originX  { nullptr };
    QSpinBox* m_originY  { nullptr };
    QSpinBox* m_cellW    { nullptr };
    QSpinBox* m_cellH    { nullptr };
    QSpinBox* m_cols     { nullptr };
    QSpinBox* m_rows     { nullptr };
};

} // namespace Cluedo
