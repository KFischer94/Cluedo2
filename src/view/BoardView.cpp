#include "BoardView.h"
#include "GridOverlay.h"
#include "../model/GridModel.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>

namespace Cluedo {

BoardView::BoardView(QWidget* parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    // Smooth rendering
    setRenderHint(QPainter::Antialiasing, false);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    // No scroll bars – we do manual pan
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    setDragMode(QGraphicsView::NoDrag);
    setBackgroundBrush(QColor(45, 45, 45));

    m_overlay = new GridOverlay();
    m_scene->addItem(m_overlay);
    m_overlay->setZValue(1);
}

void BoardView::setImage(const QPixmap& pixmap)
{
    // Remove old pixmap item
    if (m_pixItem) {
        m_scene->removeItem(m_pixItem);
        delete m_pixItem;
        m_pixItem = nullptr;
    }

    m_pixItem = m_scene->addPixmap(pixmap);
    m_pixItem->setZValue(0);

    // Fit in view initially
    m_scene->setSceneRect(m_pixItem->boundingRect());
    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);

    refreshGrid();
}

void BoardView::setGridModel(GridModel* model)
{
    m_gridModel = model;
    m_overlay->setGridModel(model);
    refreshGrid();
}

void BoardView::refreshGrid()
{
    if (m_overlay)
        m_overlay->update();
}

// ── Zoom (Ctrl+Wheel) ─────────────────────────────────────────────────────────
void BoardView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        const double factor = (event->angleDelta().y() > 0) ? 1.15 : (1.0 / 1.15);
        scale(factor, factor);
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

// ── Pan (Middle-button or Ctrl+Left) ─────────────────────────────────────────
void BoardView::mousePressEvent(QMouseEvent* event)
{
    const bool panButton = (event->button() == Qt::MiddleButton) ||
                           (event->button() == Qt::LeftButton && m_ctrlHeld);
    if (panButton) {
        m_panning  = true;
        m_panStart = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void BoardView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning) {
        const QPoint delta = event->pos() - m_panStart;
        m_panStart = event->pos();
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void BoardView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void BoardView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
        m_ctrlHeld = true;
    QGraphicsView::keyPressEvent(event);
}

void BoardView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
        m_ctrlHeld = false;
    QGraphicsView::keyReleaseEvent(event);
}

} // namespace Cluedo
#include "moc_BoardView.cpp"
