#include "settingswidget.h"

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QPixmap>
#include <Qt>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent),
    m_themeComboBox(nullptr),
    m_currencyComboBox(nullptr),
    m_cityComboBox(nullptr),
    m_nameLineEdit(nullptr),
    m_photoPreviewLabel(nullptr),
    m_loadPhotoButton(nullptr),
    m_saveButton(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Настройки", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QGroupBox *appearanceGroup = new QGroupBox("Внешний вид", this);
    QFormLayout *appearanceLayout = new QFormLayout(appearanceGroup);

    m_themeComboBox = new QComboBox(appearanceGroup);
    m_themeComboBox->addItem("Светлая");
    m_themeComboBox->addItem("Тёмная");
    appearanceLayout->addRow(new QLabel("Тема", appearanceGroup), m_themeComboBox);

    QGroupBox *currencyGroup = new QGroupBox("Валюта", this);
    QFormLayout *currencyLayout = new QFormLayout(currencyGroup);

    m_currencyComboBox = new QComboBox(currencyGroup);
    m_currencyComboBox->addItem("RUB");
    m_currencyComboBox->addItem("USD");
    m_currencyComboBox->addItem("EUR");
    currencyLayout->addRow(new QLabel("Валюта", currencyGroup), m_currencyComboBox);

    QGroupBox *cityGroup = new QGroupBox("Город точки BooBooCan", this);
    QFormLayout *cityLayout = new QFormLayout(cityGroup);

    m_cityComboBox = new QComboBox(cityGroup);
    m_cityComboBox->addItem("Москва");
    m_cityComboBox->addItem("Санкт-Петербург");
    m_cityComboBox->addItem("Казань");
    m_cityComboBox->addItem("Екатеринбург");
    m_cityComboBox->addItem("Новосибирск");
    m_cityComboBox->addItem("Таллин");
    cityLayout->addRow(new QLabel("Город", cityGroup), m_cityComboBox);

    QGroupBox *profileGroup = new QGroupBox("Профиль клиента", this);
    QVBoxLayout *profileLayout = new QVBoxLayout(profileGroup);

    m_nameLineEdit = new QLineEdit(profileGroup);

    m_photoPreviewLabel = new QLabel("Фото не выбрано", profileGroup);
    m_photoPreviewLabel->setFixedSize(120, 120);
    m_photoPreviewLabel->setAlignment(Qt::AlignCenter);
    m_photoPreviewLabel->setStyleSheet("border: 1px solid #AAAAAA;");

    m_loadPhotoButton = new QPushButton("Загрузить фото", profileGroup);
    m_saveButton = new QPushButton("Сохранить настройки", this);

    profileLayout->addWidget(new QLabel("Имя клиента", profileGroup));
    profileLayout->addWidget(m_nameLineEdit);
    profileLayout->addWidget(m_photoPreviewLabel);
    profileLayout->addWidget(m_loadPhotoButton);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(appearanceGroup);
    mainLayout->addWidget(currencyGroup);
    mainLayout->addWidget(cityGroup);
    mainLayout->addWidget(profileGroup);
    mainLayout->addWidget(m_saveButton);
    mainLayout->addStretch();

    connect(m_loadPhotoButton, &QPushButton::clicked,
            this, &SettingsWidget::loadPhoto);

    connect(m_saveButton, &QPushButton::clicked,
            this, &SettingsWidget::saveSettings);

    loadSettings();
}

void SettingsWidget::loadSettings()
{
    QSettings settings("BooBooCan", "BooBooCanApp");

    m_themeComboBox->setCurrentText(settings.value("appearance/theme", "Светлая").toString());
    m_currencyComboBox->setCurrentText(settings.value("app/currency", "RUB").toString());
    m_cityComboBox->setCurrentText(settings.value("app/city", "Москва").toString());
    m_nameLineEdit->setText(settings.value("profile/name", "").toString());

    m_photoPath = settings.value("profile/photoPath", "").toString();
    updatePhotoPreview();
}

void SettingsWidget::saveSettings()
{
    QSettings settings("BooBooCan", "BooBooCanApp");

    const QString theme = m_themeComboBox->currentText();
    const QString currency = m_currencyComboBox->currentText();
    const QString city = m_cityComboBox->currentText();
    const QString name = m_nameLineEdit->text();

    settings.setValue("appearance/theme", theme);
    settings.setValue("app/currency", currency);
    settings.setValue("app/city", city);
    settings.setValue("profile/name", name);
    settings.setValue("profile/photoPath", m_photoPath);

    emit themeChanged(theme);
    emit currencyChanged(currency);
    emit profileChanged(name, m_photoPath);

    QMessageBox::information(this, "Настройки", "Настройки сохранены");
}

void SettingsWidget::loadPhoto()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        "Загрузить фото",
        QString(),
        "Изображения (*.png *.jpg *.jpeg)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    m_photoPath = fileName;
    updatePhotoPreview();
}

void SettingsWidget::updatePhotoPreview()
{
    if (m_photoPath.isEmpty()) {
        m_photoPreviewLabel->setPixmap(QPixmap());
        m_photoPreviewLabel->setText("Фото не выбрано");
        return;
    }

    QPixmap pixmap(m_photoPath);

    if (pixmap.isNull()) {
        m_photoPreviewLabel->setPixmap(QPixmap());
        m_photoPreviewLabel->setText("Не удалось загрузить фото");
        return;
    }

    m_photoPreviewLabel->setText("");
    m_photoPreviewLabel->setPixmap(
        pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
}
