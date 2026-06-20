#pragma once
#include <QObject>
namespace Cluedo { class GridController : public QObject { Q_OBJECT public: explicit GridController(QObject* parent = nullptr); }; }
