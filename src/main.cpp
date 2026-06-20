#include <QApplication>

// Phase 1 smoke test – instantiates the model without UI.
#include "model/BoardConfig.h"
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Cluedo::BoardConfig config(20, 20);

    Cluedo::Room kitchen;
    kitchen.name      = "Küche";
    kitchen.imagePath = Cluedo::Room::defaultImagePath("Küche");
    config.addRoom(kitchen);

    Cluedo::GamePiece scarlett;
    scarlett.name          = "Scarlett";
    scarlett.imagePath     = Cluedo::GamePiece::defaultImagePath("Scarlett");
    scarlett.startPosition = { 1, 0 };
    config.addPiece(scarlett);

    qDebug() << "Rooms:"  << config.rooms().size();
    qDebug() << "Pieces:" << config.pieces().size();
    qDebug() << "maxPlayers:" << config.maxPlayers();

    // Phase 2+ will launch MainWindow here.
    return 0;
}
