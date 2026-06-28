#include "MainWindow.h"
#include "BoardView.h"
#include "GridSettingsPanel.h"
#include "ToolBar.h"
#include "RoomDialog.h"
#include "PieceDialog.h"
#include "../model/BoardConfig.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QMessageBox>
#include <QPixmap>
#include <QDockWidget>

namespace Cluedo {

MainWindow::MainWindow(const QString& imagePath,
                       const QString& outputPath,
                       QWidget* parent)
    : QMainWindow(parent)
    , m_imagePath(imagePath)
    , m_outputPath(outputPath)
{
    setWindowTitle(QStringLiteral("Cluedo Board Configurator"));
    resize(1400, 850);

    m_config = new BoardConfig(20, 20);

    buildMenuBar();
    buildLayout();
    statusBar()->showMessage(QStringLiteral("Bereit."));

    if (!imagePath.isEmpty())
        loadImage(imagePath);
}

MainWindow::~MainWindow() = default;

void MainWindow::buildMenuBar()
{
    QMenu* fileMenu = menuBar()->addMenu(QStringLiteral("&Datei"));

    QAction* openAct = fileMenu->addAction(QStringLiteral("&Bild öffnen…"));
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::onOpenImage);

    fileMenu->addSeparator();

    QAction* saveAct = fileMenu->addAction(QStringLiteral("&Speichern"));
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSave);

    QMenu* editMenu = menuBar()->addMenu(QStringLiteral("&Bearbeiten"));

    QAction* addRoomAct  = editMenu->addAction(QStringLiteral("Neuer &Raum…"));
    QAction* addPieceAct = editMenu->addAction(QStringLiteral("Neue &Spielfigur…"));
    connect(addRoomAct,  &QAction::triggered, this, &MainWindow::onAddRoom);
    connect(addPieceAct, &QAction::triggered, this, &MainWindow::onAddPiece);
}

void MainWindow::buildLayout()
{
    // ── Central: BoardView ────────────────────────────────────────────────────
    m_boardView = new BoardView(this);
    m_boardView->setGridModel(&m_config->grid());

    // ── Right dock: GridSettingsPanel ─────────────────────────────────────────
    m_settingsPanel = new GridSettingsPanel(this);
    auto* rightDock = new QDockWidget(QStringLiteral("Gittereinstellungen"), this);
    rightDock->setWidget(m_settingsPanel);
    rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
    rightDock->setFeatures(QDockWidget::DockWidgetMovable |
                            QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, rightDock);

    // ── Left dock: ToolBar ────────────────────────────────────────────────────
    m_toolBar = new ToolBar(this);
    auto* leftDock = new QDockWidget(QStringLiteral("Werkzeuge"), this);
    leftDock->setWidget(m_toolBar);
    leftDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    leftDock->setFeatures(QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, leftDock);

    setCentralWidget(m_boardView);

    // ── GridSettingsPanel → MainWindow ────────────────────────────────────────
    connect(m_settingsPanel, &GridSettingsPanel::originChanged,
            this, &MainWindow::onGridOriginChanged);
    connect(m_settingsPanel, &GridSettingsPanel::cellSizeChanged,
            this, &MainWindow::onCellSizeChanged);
    connect(m_settingsPanel, &GridSettingsPanel::dimensionsChanged,
            this, &MainWindow::onGridDimensionsChanged);

    // ── ToolBar → MainWindow ──────────────────────────────────────────────────
    connect(m_toolBar, &ToolBar::modeChanged,
            this, &MainWindow::onModeChanged);
    connect(m_toolBar, &ToolBar::addRoomRequested,
            this, &MainWindow::onAddRoom);
    connect(m_toolBar, &ToolBar::addPieceRequested,
            this, &MainWindow::onAddPiece);
}

void MainWindow::loadImage(const QString& path)
{
    QPixmap pix(path);
    if (pix.isNull()) {
        statusBar()->showMessage(
            QStringLiteral("Bild konnte nicht geladen werden: ") + path);
        return;
    }
    m_imagePath = path;
    m_boardView->setImage(pix);
    updateStatusBar();
}

void MainWindow::onOpenImage()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Bild öffnen"),
        QString(),
        QStringLiteral("Bilder (*.png *.jpg *.jpeg *.bmp *.gif);;Alle Dateien (*)"));
    if (!path.isEmpty())
        loadImage(path);
}

void MainWindow::onSave()
{
    QString path = m_outputPath;
    if (path.isEmpty()) {
        path = QFileDialog::getSaveFileName(
            this,
            QStringLiteral("Konfiguration speichern"),
            QString(),
            QStringLiteral("JSON-Dateien (*.json);;Alle Dateien (*)"));
    }
    if (path.isEmpty())
        return;

    QList<ValidationIssue> issues;
    if (!m_config->saveToFile(path, issues)) {
        QString msg = QStringLiteral("Speichern fehlgeschlagen:\n\n");
        for (const auto& issue : issues)
            if (issue.severity == IssueSeverity::Error)
                msg += QStringLiteral("[%1] %2: %3\n")
                    .arg(issue.entityType, issue.entityName, issue.message);
        QMessageBox::critical(this, QStringLiteral("Validierungsfehler"), msg);
    } else {
        statusBar()->showMessage(QStringLiteral("Gespeichert: ") + path);
    }
}

void MainWindow::onGridOriginChanged(int x, int y)
{
    m_config->grid().setOrigin({ x, y });
    m_boardView->refreshGrid();
}

void MainWindow::onCellSizeChanged(int w, int h)
{
    m_config->grid().setCellSize({ w, h });
    m_boardView->refreshGrid();
}

void MainWindow::onGridDimensionsChanged(int cols, int rows)
{
    m_config->grid().resize(cols, rows);
    m_boardView->refreshGrid();
}

void MainWindow::onModeChanged(AppMode mode)
{
    // Show/hide settings panel based on mode
    const bool isSettings = (mode == AppMode::GridSettings);
    // The dock is always visible; just update status bar hint
    statusBar()->showMessage(
        isSettings
            ? QStringLiteral("Modus: Gittereinstellung – Gitter konfigurieren")
            : QStringLiteral("Modus: Gitterzuordnung – Felder einfärben"));
}

void MainWindow::onAddRoom()
{
    RoomDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    Room r = dlg.room();
    if (!m_config->addRoom(r)) {
        QMessageBox::warning(this,
            QStringLiteral("Raum existiert bereits"),
            QStringLiteral("Ein Raum mit dem Namen \"%1\" existiert bereits.")
                .arg(r.name));
        return;
    }

    m_toolBar->syncWithConfig(m_config);
    statusBar()->showMessage(
        QStringLiteral("Raum \"%1\" angelegt.").arg(r.name));
}

void MainWindow::onAddPiece()
{
    PieceDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    GamePiece p = dlg.piece();
    if (!m_config->addPiece(p)) {
        QMessageBox::warning(this,
            QStringLiteral("Figur existiert bereits"),
            QStringLiteral("Eine Spielfigur mit dem Namen \"%1\" existiert bereits.")
                .arg(p.name));
        return;
    }

    m_toolBar->syncWithConfig(m_config);
    statusBar()->showMessage(
        QStringLiteral("Spielfigur \"%1\" angelegt.").arg(p.name));
}

void MainWindow::updateStatusBar()
{
    statusBar()->showMessage(
        QStringLiteral("Bild: ") + m_imagePath);
}

} // namespace Cluedo
#include "moc_MainWindow.cpp"
