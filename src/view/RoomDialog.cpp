#include "RoomDialog.h"

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

RoomDialog::RoomDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Raum anlegen"));
    setMinimumWidth(400);
    buildUi();
}

void RoomDialog::buildUi()
{
    m_nameEdit      = new QLineEdit(this);
    m_imagePathEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(QStringLiteral("z. B. Küche"));
    m_imagePathEdit->setPlaceholderText(QStringLiteral("/Raum/Küche.png"));

    auto* browseBtn = new QPushButton(QStringLiteral("…"), this);
    browseBtn->setFixedWidth(32);
    browseBtn->setToolTip(QStringLiteral("Bild auswählen"));

    auto* imageRow = new QHBoxLayout;
    imageRow->addWidget(m_imagePathEdit);
    imageRow->addWidget(browseBtn);

    auto* form = new QFormLayout;
    form->addRow(QStringLiteral("Name:"),       m_nameEdit);
    form->addRow(QStringLiteral("Kartenbild:"), imageRow);

    auto* hint = new QLabel(
        QStringLiteral("<small><i>Das Kartenbild ist optional. "
                       "Leer lassen = Name wird als Dateiname verwendet.</i></small>"));
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
            this, &RoomDialog::onNameChanged);
    connect(m_imagePathEdit, &QLineEdit::textEdited,
            this, [this]() { m_imagePathUserEdited = true; });
    connect(browseBtn, &QPushButton::clicked,
            this, &RoomDialog::onBrowseImage);
    connect(buttons, &QDialogButtonBox::accepted,
            this, [this]() { if (validate()) accept(); });
    connect(buttons, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

void RoomDialog::onNameChanged(const QString& name)
{
    m_okButton->setEnabled(!name.trimmed().isEmpty());

    // Auto-suggest image path only if the user hasn't manually edited it
    if (!m_imagePathUserEdited)
        m_imagePathEdit->setText(Room::defaultImagePath(name.trimmed()));
}

void RoomDialog::onBrowseImage()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Kartenbild auswählen"),
        QString(),
        QStringLiteral("Bilder (*.png *.jpg *.jpeg *.bmp);;Alle Dateien (*)"));
    if (!path.isEmpty()) {
        m_imagePathEdit->setText(path);
        m_imagePathUserEdited = true;
    }
}

bool RoomDialog::validate()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this,
            QStringLiteral("Ungültige Eingabe"),
            QStringLiteral("Bitte einen Namen für den Raum eingeben."));
        m_nameEdit->setFocus();
        return false;
    }
    return true;
}

void RoomDialog::setRoom(const Room& room)
{
    m_nameEdit->setText(room.name);
    m_imagePathEdit->setText(room.imagePath);
    m_imagePathUserEdited = !room.imagePath.isEmpty();
    setWindowTitle(QStringLiteral("Raum bearbeiten"));
}

Room RoomDialog::room() const
{
    Room r;
    r.name      = m_nameEdit->text().trimmed();
    r.imagePath = m_imagePathEdit->text().trimmed();
    if (r.imagePath.isEmpty())
        r.imagePath = Room::defaultImagePath(r.name);
    return r;
}

} // namespace Cluedo
#include "moc_RoomDialog.cpp"
