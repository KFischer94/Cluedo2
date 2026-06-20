#include "PaintCellCommand.h"
namespace Cluedo { PaintCellCommand::PaintCellCommand(QUndoCommand* parent) : QUndoCommand(parent) {} void PaintCellCommand::undo() {} void PaintCellCommand::redo() {} }
