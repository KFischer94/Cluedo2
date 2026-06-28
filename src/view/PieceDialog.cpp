#include "PieceDialog.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>

namespace Cluedo {

PieceDialog::PieceDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Spielfigur anlegen"));
    setMinimumWidth(400);
    buildUi();
}

void PieceDialog::buildUi()
{
    m_nameEdit      = new QLineEdit(this);
    m_imagePathEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(QStringLiteral("z. B. Scarlett"));
    m_imagePathEdit->setPlaceholderText(QStringLiteral("/Figur/Scarlett.png"));

    auto* browseBtn = new QPushButton(QStringLiteral("…"), this);
    browseBtn->setFixedWidth(32);
    browseBtn->setToolTip(QStringLiteral("Bild auswählen"));

    auto* imageRow = new QHBoxLayout;
    imageRow->addWidget(m_imagePathEdit);
    imageRow->addWidget(browseBtn);

    auto* form = new QFormLayout;
    form->addRow(QStringLiteral("Name:"),       m_nameEdit);
    form->addRow(QStringLiteral("Figurenbild:"), imageRow);

    auto* hint = new QLabel(
        QStringLiteral("<small><i>Das Figurenbild ist optional.</i></small>"));
    hint->setWordWrap(true);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttons->button(QDialogButtonBox::Ok);
    m_okButton->setEnabled(false);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(hint);
    layout->addWidget(buttons);

    connect(m_nameEdit, &QLineEdit::textChanged,
            this, &PieceDialog::onNameChanged);
    connect(m_imagePathEdit, &QLineEdit::textEdited,
            this, [this]() { m_imagePathUserEdited = true; });
    connect(browseBtn, &QPushButton::clicked,
            this, &PieceDialog::onBrowseImage);
    connect(buttons, &QDialogButtonBox::accepted,
            this, [this]() { if (validate()) accept(); });
    connect(buttons, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

void PieceDialog::onNameChanged(const QString& name)
{
    m_okButton->setEnabled(!name.trimmed().isEmpty());
    if (!m_imagePathUserEdited)
        m_imagePathEdit->setText(GamePiece::defaultImagePath(name.trimmed()));
}

void PieceDialog::onBrowseImage()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Figurenbild auswählen"),
        QString(),
        QStringLiteral("Bilder (*.png *.jpg *.jpeg *.bmp);;Alle Dateien (*)"));
    if (!path.isEmpty()) {
        m_imagePathEdit->setText(path);
        m_imagePathUserEdited = true;
    }
}

bool PieceDialog::validate()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this,
            QStringLiteral("Ungültige Eingabe"),
            QStringLiteral("Bitte einen Namen für die Spielfigur eingeben."));
        m_nameEdit->setFocus();
        return false;
    }
    return true;
}

void PieceDialog::setPiece(const GamePiece& piece)
{
    m_nameEdit->setText(piece.name);
    m_imagePathEdit->setText(piece.imagePath);
    m_imagePathUserEdited = !piece.imagePath.isEmpty();
    setWindowTitle(QStringLiteral("Spielfigur bearbeiten"));
}

GamePiece PieceDialog::piece() const
{
    GamePiece p;
    p.name      = m_nameEdit->text().trimmed();
    p.imagePath = m_imagePathEdit->text().trimmed();
    if (p.imagePath.isEmpty())
        p.imagePath = GamePiece::defaultImagePath(p.name);
    return p;
}

} // namespace Cluedo
#include "moc_PieceDialog.cpp"
