#pragma once
#include <QObject>
namespace Cluedo { class ConfigExporter : public QObject { Q_OBJECT public: explicit ConfigExporter(QObject* parent = nullptr); }; }
