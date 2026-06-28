#include "MainWindow.h"
#include "BoardView.h"
#include "GridSettingsPanel.h"
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

namespace Cluedo {

MainWindow::MainWindow(const QString& imagePath,
                       const QString& outputPath,
                       QWidget* parent)
    : QMainWindow(parent)
    , m_imagePath(imagePath)
    , m_outputPath(outputPath)
{
    setWindowTitle(QStringLiteral("Cluedo Board Configurator"));
    resize(1280, 800);

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
}

void MainWindow::buildLayout()
{
    m_boardView     = new BoardView(this);
    m_settingsPanel = new GridSettingsPanel(this);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_boardView);
    splitter->addWidget(m_settingsPanel);
    splitter->setStretchFactor(0, 4);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({ 960, 320 });

    setCentralWidget(splitter);

    // Connect settings panel → board view
    connect(m_settingsPanel, &GridSettingsPanel::originChanged,
            this, &MainWindow::onGridOriginChanged);
    connect(m_settingsPanel, &GridSettingsPanel::cellSizeChanged,
            this, &MainWindow::onCellSizeChanged);
    connect(m_settingsPanel, &GridSettingsPanel::dimensionsChanged,
            this, &MainWindow::onGridDimensionsChanged);

    // Pass the model to the board view
    m_boardView->setGridModel(&m_config->grid());
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
        statusBar()->showMessage(
            QStringLiteral("Gespeichert: ") + path);
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

void MainWindow::updateStatusBar()
{
    statusBar()->showMessage(
        QStringLiteral("Bild: ") + m_imagePath);
}

} // namespace Cluedo
#include "moc_MainWindow.cpp"
