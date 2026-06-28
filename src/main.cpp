#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>

#include "view/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("CluedoBoardConfigurator"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Cluedo Spielbrettkonfigurator"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption imageOpt(
        QStringList{ QStringLiteral("i"), QStringLiteral("image") },
        QStringLiteral("Pfad zum Spielfeldbild."),
        QStringLiteral("Bildpfad"));
    parser.addOption(imageOpt);

    QCommandLineOption outputOpt(
        QStringList{ QStringLiteral("o"), QStringLiteral("output") },
        QStringLiteral("Ausgabepfad für die JSON-Konfigurationsdatei."),
        QStringLiteral("Ausgabepfad"));
    parser.addOption(outputOpt);

    parser.process(app);

    const QString imagePath  = parser.value(imageOpt);
    const QString outputPath = parser.value(outputOpt);

    Cluedo::MainWindow window(imagePath, outputPath);
    window.show();

    return app.exec();
}
