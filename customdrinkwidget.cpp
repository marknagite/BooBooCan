#include "customdrinkwidget.h"

#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFrame>
#include <QFont>
#include <QMessageBox>
#include <QPixmap>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <Qt>

CustomDrinkWidget::CustomDrinkWidget(DatabaseManager *databaseManager, QWidget *parent)
    : QWidget(parent),
    m_databaseManager(databaseManager),
    m_sizeComboBox(nullptr),
    m_baseComboBox(nullptr),
    m_syrupComboBox(nullptr),
    m_toppingComboBox(nullptr),
    m_cheeseFoamCheckBox(nullptr),
    m_previewLabel(nullptr),
    m_syrupIconLabel(nullptr),
    m_toppingIconLabel(nullptr),
    m_foamIconLabel(nullptr),
    m_compositionLabel(nullptr),
    m_priceLabel(nullptr),
    m_createOrderButton(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Собери свой BooBooCan", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(22);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QLabel *subtitleLabel = new QLabel("Создай напиток из любимых ингредиентов", this);

    QHBoxLayout *contentLayout = new QHBoxLayout;

    QGroupBox *choiceGroup = new QGroupBox("Выбор ингредиентов", this);
    QVBoxLayout *choiceBoxLayout = new QVBoxLayout(choiceGroup);
    QFormLayout *choiceLayout = new QFormLayout;

    m_sizeComboBox = new QComboBox(choiceGroup);
    m_sizeComboBox->addItem("Маленький");
    m_sizeComboBox->addItem("Средний");
    m_sizeComboBox->addItem("Большой");

    m_baseComboBox = new QComboBox(choiceGroup);
    m_baseComboBox->addItem("Чёрный чай");
    m_baseComboBox->addItem("Зелёный чай");
    m_baseComboBox->addItem("Молоко");
    m_baseComboBox->addItem("Чай + молоко");

    m_syrupComboBox = new QComboBox(choiceGroup);
    m_syrupComboBox->addItem("Без сиропа");
    m_syrupComboBox->addItem("Клубника");
    m_syrupComboBox->addItem("Манго");
    m_syrupComboBox->addItem("Карамель");
    m_syrupComboBox->addItem("Маракуйя");

    m_toppingComboBox = new QComboBox(choiceGroup);
    m_toppingComboBox->addItem("Без топпинга");
    m_toppingComboBox->addItem("Тапиока");
    m_toppingComboBox->addItem("Джус-болы виноград");
    m_toppingComboBox->addItem("Джус-болы клубника");
    m_toppingComboBox->addItem("Джус-болы личи");

    m_cheeseFoamCheckBox = new QCheckBox("Добавить сырную пенку", choiceGroup);

    choiceLayout->addRow("Размер:", m_sizeComboBox);
    choiceLayout->addRow("Основа:", m_baseComboBox);
    choiceLayout->addRow("Сироп:", m_syrupComboBox);
    choiceLayout->addRow("Топпинг:", m_toppingComboBox);
    choiceLayout->addRow("Сырная пенка:", m_cheeseFoamCheckBox);

    choiceBoxLayout->addLayout(choiceLayout);

    QFrame *tipsCard = new QFrame(choiceGroup);
    tipsCard->setObjectName("constructorTipsCard");
    tipsCard->setStyleSheet(
        "#constructorTipsCard {"
        "border: 1px solid rgba(255, 122, 200, 0.45);"
        "border-radius: 12px;"
        "background-color: rgba(255, 122, 200, 0.08);"
        "}"
        );

    QVBoxLayout *tipsLayout = new QVBoxLayout(tipsCard);

    QLabel *tipsTitleLabel = new QLabel("Памятка по цене", tipsCard);
    QFont tipsTitleFont = tipsTitleLabel->font();
    tipsTitleFont.setBold(true);
    tipsTitleLabel->setFont(tipsTitleFont);

    QLabel *tipsTextLabel = new QLabel(
        "База: 180 руб\n"
        "Размер: +0 / +40 / +80\n"
        "Сироп: +30\n"
        "Топпинг: +45\n"
        "Сырная пенка: +50\n\n"
        "Демо-время:\n"
        "1 минута = 5 секунд",
        tipsCard
        );
    tipsTextLabel->setWordWrap(true);

    tipsLayout->addWidget(tipsTitleLabel);
    tipsLayout->addWidget(tipsTextLabel);

    choiceBoxLayout->addSpacing(12);
    choiceBoxLayout->addWidget(tipsCard);
    choiceBoxLayout->addStretch();

    QGroupBox *previewGroup = new QGroupBox("Предпросмотр", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);

    m_previewLabel = new QLabel("Предпросмотр напитка", previewGroup);
    m_previewLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *iconsLayout = new QHBoxLayout;

    m_syrupIconLabel = new QLabel(previewGroup);
    m_toppingIconLabel = new QLabel(previewGroup);
    m_foamIconLabel = new QLabel(previewGroup);

    iconsLayout->addWidget(m_syrupIconLabel);
    iconsLayout->addWidget(m_toppingIconLabel);
    iconsLayout->addWidget(m_foamIconLabel);
    iconsLayout->addStretch();

    m_compositionLabel = new QLabel(previewGroup);
    m_compositionLabel->setWordWrap(true);

    m_priceLabel = new QLabel(previewGroup);
    QFont priceFont = m_priceLabel->font();
    priceFont.setPointSize(14);
    priceFont.setBold(true);
    m_priceLabel->setFont(priceFont);

    m_createOrderButton = new QPushButton("Создать заказ", previewGroup);

    previewLayout->addWidget(m_previewLabel, 0, Qt::AlignCenter);
    previewLayout->addLayout(iconsLayout);
    previewLayout->addWidget(m_compositionLabel);
    previewLayout->addWidget(m_priceLabel);
    previewLayout->addWidget(m_createOrderButton);
    previewLayout->addStretch();

    contentLayout->addWidget(choiceGroup, 2);
    contentLayout->addWidget(previewGroup, 3);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addLayout(contentLayout);

    connect(m_sizeComboBox, &QComboBox::currentIndexChanged,
            this, [this](int) { updateConstructor(); });

    connect(m_baseComboBox, &QComboBox::currentIndexChanged,
            this, [this](int) { updateConstructor(); });

    connect(m_syrupComboBox, &QComboBox::currentIndexChanged,
            this, [this](int) { updateConstructor(); });

    connect(m_toppingComboBox, &QComboBox::currentIndexChanged,
            this, [this](int) { updateConstructor(); });

    connect(m_cheeseFoamCheckBox, &QCheckBox::toggled,
            this, [this](bool) { updateConstructor(); });

    connect(m_createOrderButton, &QPushButton::clicked,
            this, &CustomDrinkWidget::showOrderMessage);

    updateConstructor();
}

void CustomDrinkWidget::updateConstructor()
{
    setImageOrText(m_previewLabel, previewImagePath(), "Предпросмотр напитка", QSize(260, 260));
    setImageOrText(m_syrupIconLabel, syrupIconPath(), "", QSize(64, 64));
    setImageOrText(m_toppingIconLabel, toppingIconPath(), "", QSize(64, 64));
    setImageOrText(m_foamIconLabel, foamIconPath(), "", QSize(64, 64));

    m_compositionLabel->setText(compositionText());
    m_priceLabel->setText("Итоговая цена: " + QString::number(currentTotalPrice(), 'f', 2) + " руб");
}

double CustomDrinkWidget::currentTotalPrice() const
{
    double price = 180.0;

    if (m_sizeComboBox->currentText() == "Средний") {
        price += 40.0;
    } else if (m_sizeComboBox->currentText() == "Большой") {
        price += 80.0;
    }

    if (m_syrupComboBox->currentText() != "Без сиропа") {
        price += 30.0;
    }

    if (m_toppingComboBox->currentText() != "Без топпинга") {
        price += 45.0;
    }

    if (m_cheeseFoamCheckBox->isChecked()) {
        price += 50.0;
    }

    return price;
}

QString CustomDrinkWidget::compositionText() const
{
    return QString(
               "Размер: %1\n"
               "Основа: %2\n"
               "Сироп: %3\n"
               "Топпинг: %4\n"
               "Сырная пенка: %5"
               )
        .arg(m_sizeComboBox->currentText())
        .arg(m_baseComboBox->currentText())
        .arg(m_syrupComboBox->currentText())
        .arg(m_toppingComboBox->currentText())
        .arg(m_cheeseFoamCheckBox->isChecked() ? "Добавлена" : "Нет");
}

void CustomDrinkWidget::showOrderMessage()
{
    if (m_databaseManager == nullptr) {
        QMessageBox::critical(this, "Создание заказа", "DatabaseManager не найден");
        return;
    }

    QSettings settings("BooBooCan", "BooBooCanApp");
    const QString customerName = settings.value("profile/name", "").toString();

    const int orderId = m_databaseManager->createCustomDrinkOrderAndReturnId(
        m_sizeComboBox->currentText(),
        m_baseComboBox->currentText(),
        m_syrupComboBox->currentText(),
        m_toppingComboBox->currentText(),
        m_cheeseFoamCheckBox->isChecked(),
        currentTotalPrice(),
        4,
        customerName
        );

    if (orderId > 0) {
        QMessageBox::information(this, "Создание заказа", "Ваш заказ создан");
        emit orderCreated(orderId);
    } else {
        QMessageBox::critical(this, "Ошибка создания заказа", m_databaseManager->lastError());
    }
}

QString CustomDrinkWidget::sizeKey() const
{
    if (m_sizeComboBox->currentText() == "Маленький") {
        return "small";
    }

    if (m_sizeComboBox->currentText() == "Средний") {
        return "average";
    }

    return "big";
}

QString CustomDrinkWidget::baseKey() const
{
    if (m_baseComboBox->currentText() == "Чёрный чай") {
        return "black_tea";
    }

    if (m_baseComboBox->currentText() == "Зелёный чай") {
        return "green_tea";
    }

    if (m_baseComboBox->currentText() == "Молоко") {
        return "milk";
    }

    return "milk_tea";
}

QString CustomDrinkWidget::previewImagePath() const
{
    return QString("assets/custom/preview/%1_%2.jpg")
    .arg(sizeKey())
        .arg(baseKey());
}

QString CustomDrinkWidget::syrupIconPath() const
{
    const QString syrup = m_syrupComboBox->currentText();

    if (syrup == "Без сиропа") {
        return "";
    }

    if (syrup == "Клубника") {
        return "assets/custom/icons/strawberry_syrup.jpg";
    }

    if (syrup == "Манго") {
        return "assets/custom/icons/mango_syrup.jpg";
    }

    if (syrup == "Карамель") {
        return "assets/custom/icons/caramel_syrup.jpg";
    }

    return "assets/custom/icons/passion_fruit_syrup.jpg";
}

QString CustomDrinkWidget::toppingIconPath() const
{
    const QString topping = m_toppingComboBox->currentText();

    if (topping == "Без топпинга") {
        return "";
    }

    if (topping == "Тапиока") {
        return "assets/custom/icons/tapioca.jpg";
    }

    if (topping == "Джус-болы виноград") {
        return "assets/custom/icons/grape_boba.jpg";
    }

    if (topping == "Джус-болы клубника") {
        return "assets/custom/icons/strawberry_boba.jpg";
    }

    return "assets/custom/icons/lychee_boba.jpg";
}

QString CustomDrinkWidget::foamIconPath() const
{
    if (m_cheeseFoamCheckBox->isChecked()) {
        return "assets/custom/icons/cheese_foam.jpg";
    }

    return "";
}

void CustomDrinkWidget::setImageOrText(QLabel *label,
                                       const QString &path,
                                       const QString &fallbackText,
                                       const QSize &size)
{
    label->setFixedSize(size);
    label->setAlignment(Qt::AlignCenter);
    label->setScaledContents(false);
    label->setStyleSheet("background-color: transparent; border: none; padding: 0px;");

    if (path.isEmpty()) {
        label->clear();
        label->update();
        return;
    }

    const QString resolvedPath = resolvedImagePath(path);
    QPixmap pixmap(resolvedPath);

    label->setToolTip(path + "\n" + resolvedPath);

    if (resolvedPath.isEmpty() || pixmap.isNull()) {
        label->setPixmap(QPixmap());
        label->setText(fallbackText);
        label->update();
        return;
    }

    label->setText("");
    label->setPixmap(
        pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    label->update();
}

QString CustomDrinkWidget::resolvedImagePath(const QString &path) const
{
    if (path.isEmpty()) {
        return "";
    }

    QStringList pathsToTry;
    pathsToTry.append(path);

    if (path.endsWith(".jpg", Qt::CaseInsensitive)) {
        QString pngPath = path;
        pngPath.replace(".jpg", ".png", Qt::CaseInsensitive);
        pathsToTry.append(pngPath);

        QString jpegPath = path;
        jpegPath.replace(".jpg", ".jpeg", Qt::CaseInsensitive);
        pathsToTry.append(jpegPath);
    } else if (path.endsWith(".png", Qt::CaseInsensitive)) {
        QString jpgPath = path;
        jpgPath.replace(".png", ".jpg", Qt::CaseInsensitive);
        pathsToTry.append(jpgPath);

        QString jpegPath = path;
        jpegPath.replace(".png", ".jpeg", Qt::CaseInsensitive);
        pathsToTry.append(jpegPath);
    }

    QStringList baseDirs;
    baseDirs.append(QDir::currentPath());
    baseDirs.append(QCoreApplication::applicationDirPath());

    QDir appDir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 8; ++i) {
        baseDirs.append(appDir.absolutePath());
        appDir.cdUp();
    }

    for (const QString &currentPath : pathsToTry) {
        QFileInfo directInfo(currentPath);

        if (directInfo.isAbsolute() && directInfo.exists()) {
            return directInfo.absoluteFilePath();
        }

        if (directInfo.exists()) {
            return directInfo.absoluteFilePath();
        }

        for (const QString &baseDir : baseDirs) {
            const QString candidate = QDir(baseDir).filePath(currentPath);

            if (QFileInfo::exists(candidate)) {
                return candidate;
            }
        }
    }

    return "";
}
