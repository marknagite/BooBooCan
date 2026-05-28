#include "mainwindow.h"

#include "readydrinkswidget.h"
#include "settingswidget.h"
#include "customdrinkwidget.h"
#include "orderstatuswidget.h"

#include <QApplication>
#include <QSettings>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QStackedWidget>
#include <QTimer>
#include <QPixmap>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <Qt>

static QString findAssetPath(const QString &relativePath)
{
    if (relativePath.isEmpty()) {
        return "";
    }

    QFileInfo directInfo(relativePath);

    if (directInfo.exists()) {
        return directInfo.absoluteFilePath();
    }

    QDir dir(QCoreApplication::applicationDirPath());

    for (int i = 0; i < 8; ++i) {
        const QString candidate = dir.filePath(relativePath);

        if (QFileInfo::exists(candidate)) {
            return candidate;
        }

        dir.cdUp();
    }

    return "";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_splashLogoLabel(nullptr),
    m_statusLabel(nullptr),
    m_checkConnectionButton(nullptr),
    m_navigationWidget(nullptr),
    m_connectionPageButton(nullptr),
    m_readyDrinksPageButton(nullptr),
    m_settingsPageButton(nullptr),
    m_customDrinkPageButton(nullptr),
    m_orderStatusPageButton(nullptr),
    m_stackedWidget(nullptr),
    m_splashPage(nullptr),
    m_connectionPage(nullptr),
    m_readyDrinksWidget(nullptr),
    m_settingsWidget(nullptr),
    m_customDrinkWidget(nullptr),
    m_orderStatusWidget(nullptr)
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("mainCentralWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_connectionPageButton = new QPushButton("Подключение", centralWidget);
    m_readyDrinksPageButton = new QPushButton("Готовые напитки", centralWidget);
    m_settingsPageButton = new QPushButton("Настройки", centralWidget);
    m_customDrinkPageButton = new QPushButton("Собрать свой напиток", centralWidget);
    m_orderStatusPageButton = new QPushButton("Статус заказа", centralWidget);

    m_connectionPageButton->setMinimumHeight(46);
    m_readyDrinksPageButton->setMinimumHeight(46);
    m_customDrinkPageButton->setMinimumHeight(46);
    m_orderStatusPageButton->setMinimumHeight(46);
    m_settingsPageButton->setMinimumHeight(46);

    m_navigationWidget = new QWidget(centralWidget);
    QHBoxLayout *navigationLayout = new QHBoxLayout(m_navigationWidget);
    navigationLayout->setContentsMargins(20, 12, 20, 12);
    navigationLayout->setSpacing(8);

    navigationLayout->addWidget(m_connectionPageButton);
    navigationLayout->addWidget(m_readyDrinksPageButton);
    navigationLayout->addWidget(m_customDrinkPageButton);
    navigationLayout->addWidget(m_orderStatusPageButton);
    navigationLayout->addStretch();
    navigationLayout->addWidget(m_settingsPageButton);

    m_navigationWidget->hide();

    m_stackedWidget = new QStackedWidget(centralWidget);

    m_splashPage = createSplashPage();
    m_connectionPage = createConnectionPage();
    m_readyDrinksWidget = new ReadyDrinksWidget(&m_databaseManager, centralWidget);
    m_settingsWidget = new SettingsWidget(centralWidget);
    m_customDrinkWidget = new CustomDrinkWidget(&m_databaseManager, centralWidget);
    m_orderStatusWidget = new OrderStatusWidget(&m_databaseManager, centralWidget);

    m_stackedWidget->addWidget(m_splashPage);
    m_stackedWidget->addWidget(m_connectionPage);
    m_stackedWidget->addWidget(m_readyDrinksWidget);
    m_stackedWidget->addWidget(m_customDrinkWidget);
    m_stackedWidget->addWidget(m_orderStatusWidget);
    m_stackedWidget->addWidget(m_settingsWidget);

    mainLayout->addWidget(m_navigationWidget);
    mainLayout->addWidget(m_stackedWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("BooBooCan");
    resize(1000, 650);

    connect(m_connectionPageButton, &QPushButton::clicked,
            this, &MainWindow::showConnectionPage);

    connect(m_readyDrinksPageButton, &QPushButton::clicked,
            this, &MainWindow::showReadyDrinksPage);

    connect(m_settingsPageButton, &QPushButton::clicked,
            this, &MainWindow::showSettingsPage);

    connect(m_customDrinkPageButton, &QPushButton::clicked,
            this, &MainWindow::showCustomDrinkPage);

    connect(m_orderStatusPageButton, &QPushButton::clicked,
            this, &MainWindow::showOrderStatusPage);

    connect(m_readyDrinksWidget, &ReadyDrinksWidget::orderCreated,
            this, [this](int orderId) {
                m_orderStatusWidget->setOrderId(orderId);
                showOrderStatusPage();
            });

    connect(m_customDrinkWidget, &CustomDrinkWidget::orderCreated,
            this, [this](int orderId) {
                m_orderStatusWidget->setOrderId(orderId);
                showOrderStatusPage();
            });

    connect(m_settingsWidget, &SettingsWidget::themeChanged,
            this, &MainWindow::applyTheme);

    loadAppSettings();

    m_stackedWidget->setCurrentWidget(m_splashPage);

    QTimer::singleShot(2500, this, &MainWindow::showConnectionPage);
}

QWidget *MainWindow::createSplashPage()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);

    m_splashLogoLabel = new QLabel("BooBooCan", page);
    m_splashLogoLabel->setAlignment(Qt::AlignCenter);

    QFont logoFont = m_splashLogoLabel->font();
    logoFont.setPointSize(38);
    logoFont.setBold(true);
    m_splashLogoLabel->setFont(logoFont);

    QLabel *sloganLabel = new QLabel("Прекрасного напитка стакан", page);
    sloganLabel->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(m_splashLogoLabel);
    layout->addWidget(sloganLabel);
    layout->addStretch();

    return page;
}

QWidget *MainWindow::createConnectionPage()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);

    QLabel *titleLabel = new QLabel("Подключение к базе данных", page);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);

    m_checkConnectionButton = new QPushButton("Подключиться к БД", page);

    m_statusLabel = new QLabel("Нажмите кнопку, чтобы подключиться к базе данных", page);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(m_checkConnectionButton);
    layout->addWidget(m_statusLabel);
    layout->addStretch();

    connect(m_checkConnectionButton, &QPushButton::clicked,
            this, &MainWindow::checkDatabaseConnection);

    return page;
}

void MainWindow::showConnectionPage()
{
    m_navigationWidget->show();
    m_stackedWidget->setCurrentWidget(m_connectionPage);
}

void MainWindow::showReadyDrinksPage()
{
    m_navigationWidget->show();
    m_readyDrinksWidget->loadDrinks();
    m_stackedWidget->setCurrentWidget(m_readyDrinksWidget);
}

void MainWindow::showSettingsPage()
{
    m_navigationWidget->show();
    m_stackedWidget->setCurrentWidget(m_settingsWidget);
}

void MainWindow::showCustomDrinkPage()
{
    m_navigationWidget->show();
    m_stackedWidget->setCurrentWidget(m_customDrinkWidget);
}

void MainWindow::showOrderStatusPage()
{
    m_navigationWidget->show();
    m_stackedWidget->setCurrentWidget(m_orderStatusWidget);
}

void MainWindow::checkDatabaseConnection()
{
    const bool connected = m_databaseManager.connectToDatabase(
        "localhost",
        5432,
        "BooBooCan",
        "postgres",
        "unstoppable"
        );

    if (connected) {
        m_statusLabel->setText("База данных подключена");
    } else {
        m_statusLabel->setText(m_databaseManager.lastError());
    }
}

void MainWindow::loadAppSettings()
{
    QSettings settings("BooBooCan", "BooBooCanApp");
    const QString theme = settings.value("appearance/theme", "Светлая").toString();

    applyTheme(theme);
}

void MainWindow::applyTheme(const QString &theme)
{
    const QString darkBg = findAssetPath("assets/backgrounds/bg_dark.png").replace("\\", "/");
    const QString lightBg = findAssetPath("assets/backgrounds/bg_light.png").replace("\\", "/");

    if (theme == "Тёмная") {
        qApp->setStyleSheet(
            QString(
                "QMainWindow { background-color: #121212; }"
                "QWidget#mainCentralWidget { border-image: url(%1) 0 0 0 0 stretch stretch; }"

                "QWidget { background-color: transparent; color: #F5F5F5; }"

                "QGroupBox { "
                "background-color: rgba(18, 18, 18, 210); "
                "border: 1px solid #333333; "
                "border-radius: 8px; "
                "margin-top: 10px; "
                "padding: 10px; "
                "}"

                "QLineEdit, QComboBox, QListWidget, QTableWidget, QScrollArea { "
                "background-color: rgba(30, 30, 30, 235); "
                "color: #F5F5F5; "
                "border: 1px solid #444444; "
                "padding: 6px; "
                "}"

                "QPushButton { "
                "background-color: #FF7AC8; "
                "color: #121212; "
                "border: none; "
                "border-radius: 8px; "
                "padding: 12px 20px; "
                "min-height: 32px; "
                "}"

                "QPushButton:hover { background-color: #FF9BD6; }"

                "QComboBox QAbstractItemView { "
                "background-color: #1E1E1E; "
                "color: #F5F5F5; "
                "selection-background-color: #FF7AC8; "
                "selection-color: #121212; "
                "border: 1px solid #444444; "
                "}"

                "QStackedWidget { background-color: transparent; border: none; }"
                ).arg(darkBg)
            );
    } else {
        qApp->setStyleSheet(
            QString(
                "QMainWindow { background-color: #F8F8F8; }"
                "QWidget#mainCentralWidget { border-image: url(%1) 0 0 0 0 stretch stretch; }"

                "QWidget { background-color: transparent; color: #222222; }"

                "QGroupBox { "
                "background-color: rgba(248, 248, 248, 220); "
                "border: 1px solid #DDDDDD; "
                "border-radius: 8px; "
                "margin-top: 10px; "
                "padding: 10px; "
                "}"

                "QLineEdit, QComboBox, QListWidget, QTableWidget, QScrollArea { "
                "background-color: rgba(255, 255, 255, 240); "
                "color: #222222; "
                "border: 1px solid #CCCCCC; "
                "padding: 6px; "
                "}"

                "QPushButton { "
                "background-color: #FF7AC8; "
                "color: #222222; "
                "border: none; "
                "border-radius: 8px; "
                "padding: 12px 20px; "
                "min-height: 32px; "
                "}"

                "QPushButton:hover { background-color: #FF9BD6; }"

                "QHeaderView::section { "
                "background-color: #FFFFFF; "
                "color: #222222; "
                "border: 1px solid #CCCCCC; "
                "padding: 4px; "
                "}"

                "QStackedWidget { background-color: transparent; border: none; }"
                ).arg(lightBg)
            );
    }

    updateSplashLogo(theme);
}

QString MainWindow::logoPathForTheme(const QString &theme) const
{
    if (theme == "Тёмная") {
        return "assets/logo/logo_dark_theme.png";
    }

    return "assets/logo/logo_light_theme.png";
}

void MainWindow::updateSplashLogo(const QString &theme)
{
    if (m_splashLogoLabel == nullptr) {
        return;
    }

    const QString logoPath = findAssetPath(logoPathForTheme(theme));
    QPixmap logoPixmap(logoPath);

    if (logoPath.isEmpty() || logoPixmap.isNull()) {
        m_splashLogoLabel->setText("BooBooCan");

        QFont logoFont = m_splashLogoLabel->font();
        logoFont.setPointSize(38);
        logoFont.setBold(true);
        m_splashLogoLabel->setFont(logoFont);

        return;
    }

    m_splashLogoLabel->setText("");
    m_splashLogoLabel->setPixmap(
        logoPixmap.scaled(220, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
}
