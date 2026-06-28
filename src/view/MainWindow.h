#pragma once

#include <QMainWindow>
#include <QString>

namespace Cluedo {

class BoardView;
class GridSettingsPanel;
class ToolBar;
class BoardConfig;
enum class AppMode;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString& imagePath,
                        const QString& outputPath,
                        QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onOpenImage();
    void onSave();
    void onGridOriginChanged(int x, int y);
    void onCellSizeChanged(int w, int h);
    void onGridDimensionsChanged(int cols, int rows);
    void onModeChanged(AppMode mode);
    void onAddRoom();
    void onAddPiece();

private:
    void buildMenuBar();
    void buildLayout();
    void loadImage(const QString& path);
    void updateStatusBar();

    BoardView*         m_boardView    { nullptr };
    GridSettingsPanel* m_settingsPanel{ nullptr };
    ToolBar*           m_toolBar      { nullptr };
    BoardConfig*       m_config       { nullptr };

    QString m_imagePath;
    QString m_outputPath;
};

} // namespace Cluedo
