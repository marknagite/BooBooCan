#include "readydrinkswidget.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <QPixmap>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QEvent>
#include <QSettings>
#include <Qt>

ReadyDrinksWidget::ReadyDrinksWidget(DatabaseManager *databaseManager, QWidget *parent)
    : QWidget(parent),
    m_databaseManager(databaseManager),
    m_scrollArea(nullptr),
    m_cardsContainer(nullptr),
    m_cardsLayout(nullptr),
    m_detailsPhotoLabel(nullptr),
    m_detailsNameLabel(nullptr),
    m_detailsDescriptionLabel(nullptr),
    m_sizeComboBox(nullptr),
    m_temperatureComboBox(nullptr),
    m_totalPriceLabel(nullptr),
    m_orderButton(nullptr),
    m_selectedIndex(-1),
    m_hasCurrentDrink(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Готовые напитки", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    m_cardsContainer = new QWidget(this);
    m_cardsLayout = new QGridLayout(m_cardsContainer);
    m_cardsLayout->setSpacing(12);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(m_cardsContainer);
    m_scrollArea->setWidgetResizable(true);

    QFrame *detailsFrame = new QFrame(this);
    detailsFrame->setFrameShape(QFrame::StyledPanel);
    detailsFrame->setMinimumWidth(280);

    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsFrame);

    m_detailsPhotoLabel = new QLabel("Фото напитка", detailsFrame);
    m_detailsPhotoLabel->setFixedSize(220, 160);
    m_detailsPhotoLabel->setAlignment(Qt::AlignCenter);

    m_detailsNameLabel = new QLabel("Выберите напиток", detailsFrame);
    QFont nameFont = m_detailsNameLabel->font();
    nameFont.setPointSize(14);
    nameFont.setBold(true);
    m_detailsNameLabel->setFont(nameFont);
    m_detailsNameLabel->setWordWrap(true);

    m_detailsDescriptionLabel = new QLabel("Нажмите на карточку напитка слева", detailsFrame);
    m_detailsDescriptionLabel->setWordWrap(true);

    m_sizeComboBox = new QComboBox(detailsFrame);
    m_sizeComboBox->addItem("Маленький");
    m_sizeComboBox->addItem("Средний");
    m_sizeComboBox->addItem("Большой");

    m_temperatureComboBox = new QComboBox(detailsFrame);
    m_temperatureComboBox->addItem("Холодный");
    m_temperatureComboBox->addItem("Горячий");

    m_totalPriceLabel = new QLabel("Итоговая цена: 0.00 руб", detailsFrame);

    m_orderButton = new QPushButton("Оформить заказ", detailsFrame);
    m_orderButton->setEnabled(false);

    detailsLayout->addWidget(m_detailsPhotoLabel, 0, Qt::AlignCenter);
    detailsLayout->addWidget(m_detailsNameLabel);
    detailsLayout->addWidget(m_detailsDescriptionLabel);
    detailsLayout->addSpacing(8);
    detailsLayout->addWidget(new QLabel("Размер:", detailsFrame));
    detailsLayout->addWidget(m_sizeComboBox);
    detailsLayout->addWidget(new QLabel("Температура:", detailsFrame));
    detailsLayout->addWidget(m_temperatureComboBox);
    detailsLayout->addWidget(m_totalPriceLabel);
    detailsLayout->addWidget(m_orderButton);
    detailsLayout->addStretch();

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->addWidget(m_scrollArea, 3);
    contentLayout->addWidget(detailsFrame, 2);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(contentLayout);

    connect(m_sizeComboBox, &QComboBox::currentTextChanged,
            this, [this]() {
                updateTotalPrice();
            });

    connect(m_temperatureComboBox, &QComboBox::currentTextChanged,
            this, [this]() {
                updateTotalPrice();
            });

    connect(m_orderButton, &QPushButton::clicked,
            this, &ReadyDrinksWidget::showSelectedDrink);
}

void ReadyDrinksWidget::loadDrinks()
{
    while (QLayoutItem *item = m_cardsLayout->takeAt(0)) {
        if (item->widget() != nullptr) {
            delete item->widget();
        }

        delete item;
    }

    m_drinks.clear();
    m_selectedIndex = -1;
    m_hasCurrentDrink = false;

    m_detailsNameLabel->setText("Выберите напиток");
    m_detailsDescriptionLabel->setText("Нажмите на карточку напитка слева");
    m_totalPriceLabel->setText("Итоговая цена: 0.00 руб");
    m_orderButton->setEnabled(false);
    setDrinkPhoto(m_detailsPhotoLabel, "", QSize(220, 160));

    if (m_databaseManager == nullptr) {
        m_detailsDescriptionLabel->setText("Ошибка: DatabaseManager не найден");
        return;
    }

    m_drinks = m_databaseManager->getReadyDrinks();

    if (m_drinks.isEmpty()) {
        const QString errorText = m_databaseManager->lastError();

        if (!errorText.isEmpty()) {
            m_detailsDescriptionLabel->setText(errorText);
        } else {
            m_detailsDescriptionLabel->setText("В базе пока нет готовых напитков");
        }

        return;
    }

    const int columns = 3;

    for (int i = 0; i < m_drinks.size(); ++i) {
        QFrame *card = createDrinkCard(m_drinks.at(i), i);
        m_cardsLayout->addWidget(card, i / columns, i % columns);
    }

    selectDrink(0);
}

QFrame *ReadyDrinksWidget::createDrinkCard(const Drink &drink, int index)
{
    QFrame *card = new QFrame(m_cardsContainer);
    card->setObjectName("drinkCard");
    card->setFrameShape(QFrame::StyledPanel);
    card->setCursor(Qt::PointingHandCursor);
    card->setProperty("drinkIndex", index);
    card->installEventFilter(this);
    card->setStyleSheet(
        "#drinkCard { "
        "border: 1px solid #DDDDDD; "
        "border-radius: 8px; "
        "background-color: #FFFFFF; "
        "}"
        );

    QVBoxLayout *layout = new QVBoxLayout(card);

    QLabel *photoLabel = new QLabel(card);
    photoLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    setDrinkPhoto(photoLabel, drink.imagePath, QSize(150, 105));

    QLabel *nameLabel = new QLabel(drink.name, card);
    nameLabel->setWordWrap(true);
    nameLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    nameLabel->setStyleSheet("border: none; background: transparent; color: #222222;");

    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    QLabel *categoryLabel = new QLabel(drink.category, card);
    categoryLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    categoryLabel->setStyleSheet("border: none; background: transparent; color: #666666;");

    QLabel *priceLabel = new QLabel(QString::number(drink.basePrice, 'f', 2) + " руб", card);
    priceLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    priceLabel->setStyleSheet("border: none; background: transparent; color: #222222; font-weight: bold;");

    layout->addWidget(photoLabel, 0, Qt::AlignCenter);
    layout->addWidget(nameLabel);
    layout->addWidget(categoryLabel);
    layout->addWidget(priceLabel);

    return card;
}

bool ReadyDrinksWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *widget = qobject_cast<QWidget *>(watched);

        if (widget != nullptr && widget->property("drinkIndex").isValid()) {
            selectDrink(widget->property("drinkIndex").toInt());
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void ReadyDrinksWidget::selectDrink(int index)
{
    if (index < 0 || index >= m_drinks.size()) {
        return;
    }

    m_selectedIndex = index;
    m_currentDrink = m_drinks.at(index);
    m_hasCurrentDrink = true;

    for (int i = 0; i < m_cardsLayout->count(); ++i) {
        QWidget *widget = m_cardsLayout->itemAt(i)->widget();

        if (widget == nullptr) {
            continue;
        }

        const int cardIndex = widget->property("drinkIndex").toInt();

        if (cardIndex == m_selectedIndex) {
            widget->setStyleSheet(
                "#drinkCard { "
                "border: 2px solid #FF7AC8; "
                "border-radius: 8px; "
                "background-color: #FFF0FA; "
                "}"
                );
        } else {
            widget->setStyleSheet(
                "#drinkCard { "
                "border: 1px solid #DDDDDD; "
                "border-radius: 8px; "
                "background-color: #FFFFFF; "
                "}"
                );
        }
    }

    updateDetails();
}

void ReadyDrinksWidget::updateDetails()
{
    if (!m_hasCurrentDrink) {
        return;
    }

    setDrinkPhoto(m_detailsPhotoLabel, m_currentDrink.imagePath, QSize(220, 160));

    m_detailsNameLabel->setText(m_currentDrink.name);

    const QString description = QString("Категория: %1\n\n%2")
                                    .arg(m_currentDrink.category)
                                    .arg(m_currentDrink.description);

    m_detailsDescriptionLabel->setText(description);
    m_orderButton->setEnabled(true);

    updateTotalPrice();
}

void ReadyDrinksWidget::updateTotalPrice()
{
    const QString priceText = QString::number(currentTotalPrice(), 'f', 2);
    m_totalPriceLabel->setText("Итоговая цена: " + priceText + " руб");
}

double ReadyDrinksWidget::currentTotalPrice() const
{
    if (!m_hasCurrentDrink) {
        return 0.0;
    }

    double price = m_currentDrink.basePrice;
    const QString size = m_sizeComboBox->currentText();

    if (size == "Средний") {
        price += 40.0;
    } else if (size == "Большой") {
        price += 80.0;
    }

    return price;
}

void ReadyDrinksWidget::showSelectedDrink()
{
    if (!m_hasCurrentDrink) {
        QMessageBox::warning(this, "Оформление заказа", "Сначала выберите напиток");
        return;
    }

    if (m_databaseManager == nullptr) {
        QMessageBox::critical(this, "Оформление заказа", "DatabaseManager не найден");
        return;
    }

    int estimatedMinutes = 2;
    const QString category = m_currentDrink.category.toLower();

    if (category == "author") {
        estimatedMinutes = 3;
    } else if (category == "classic") {
        estimatedMinutes = 2;
    }

    QSettings settings("BooBooCan", "BooBooCanApp");
    const QString customerName = settings.value("profile/name", "").toString();

    const int orderId = m_databaseManager->createReadyDrinkOrderAndReturnId(
        m_currentDrink.id,
        m_currentDrink.name,
        m_sizeComboBox->currentText(),
        m_temperatureComboBox->currentText(),
        currentTotalPrice(),
        estimatedMinutes,
        customerName
        );

    if (orderId > 0) {
        QMessageBox::information(this, "Оформление заказа", "Ваш заказ создан");
        emit orderCreated(orderId);
    } else {
        QMessageBox::critical(this,
                              "Ошибка оформления заказа",
                              m_databaseManager->lastError());
    }
}

void ReadyDrinksWidget::setDrinkPhoto(QLabel *label, const QString &imagePath, const QSize &size)
{
    label->setFixedSize(size);
    label->setMinimumSize(size);
    label->setMaximumSize(size);
    label->setAlignment(Qt::AlignCenter);
    label->setScaledContents(false);

    const QString path = resolvedImagePath(imagePath);
    QPixmap pixmap(path);

    label->setStyleSheet(
        "background-color: transparent;"
        "border: none;"
        "padding: 0px;"
        "margin: 0px;"
        );

    if (path.isEmpty() || pixmap.isNull()) {
        label->setPixmap(QPixmap());
        label->setText("Фото напитка");
        return;
    }

    label->setText("");

    const QPixmap scaledPixmap = pixmap.scaled(
        size,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    label->setPixmap(scaledPixmap);
}

QString ReadyDrinksWidget::resolvedImagePath(const QString &imagePath) const
{
    if (imagePath.isEmpty()) {
        return "";
    }

    QFileInfo fileInfo(imagePath);

    if (fileInfo.isAbsolute() && fileInfo.exists()) {
        return imagePath;
    }

    if (fileInfo.exists()) {
        return fileInfo.absoluteFilePath();
    }

    QDir dir(QCoreApplication::applicationDirPath());

    for (int i = 0; i < 8; ++i) {
        const QString candidate = dir.filePath(imagePath);

        if (QFileInfo::exists(candidate)) {
            return candidate;
        }

        dir.cdUp();
    }

    return "";
}
