#pragma once
#include <QUndoCommand>
namespace Cluedo { class PaintCellCommand : public QUndoCommand { public: explicit PaintCellCommand(QUndoCommand* parent = nullptr); void undo() override; void redo() override; }; }
