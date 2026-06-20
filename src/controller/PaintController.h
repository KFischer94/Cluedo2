#pragma once
#include <QObject>
namespace Cluedo { class PaintController : public QObject { Q_OBJECT public: explicit PaintController(QObject* parent = nullptr); }; }
