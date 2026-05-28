#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "databasemanager.h"


class QLabel;
class QPushButton;
class QStackedWidget;
class ReadyDrinksWidget;
class SettingsWidget;
class CustomDrinkWidget;
class QWidget;
class OrderStatusWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget *createSplashPage();
    QWidget *createConnectionPage();

    void showConnectionPage();
    void showReadyDrinksPage();
    void showSettingsPage();
    void showCustomDrinkPage();
    void showOrderStatusPage();


    void checkDatabaseConnection();
    void applyTheme(const QString &theme);
    void loadAppSettings();
    void updateSplashLogo(const QString &theme);
    QString logoPathForTheme(const QString &theme) const;

    QLabel *m_statusLabel;
    QPushButton *m_checkConnectionButton;

    QWidget *m_navigationWidget;
    QPushButton *m_connectionPageButton;
    QPushButton *m_readyDrinksPageButton;
    QPushButton *m_settingsPageButton;
    QPushButton *m_customDrinkPageButton;

    QPushButton *m_orderStatusPageButton;
    OrderStatusWidget *m_orderStatusWidget;

    QStackedWidget *m_stackedWidget;
    QLabel *m_splashLogoLabel;

    QWidget *m_splashPage;
    QWidget *m_connectionPage;
    ReadyDrinksWidget *m_readyDrinksWidget;
    SettingsWidget *m_settingsWidget;
    CustomDrinkWidget *m_customDrinkWidget;




    DatabaseManager m_databaseManager;
};

#endif // MAINWINDOW_H
