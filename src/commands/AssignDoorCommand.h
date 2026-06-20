#pragma once
#include <QUndoCommand>
namespace Cluedo { class AssignDoorCommand : public QUndoCommand { public: explicit AssignDoorCommand(QUndoCommand* parent = nullptr); void undo() override; void redo() override; }; }
