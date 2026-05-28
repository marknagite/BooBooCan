#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QList>
#include "order.h"
#include "drink.h"

class DatabaseManager
{
public:
    DatabaseManager();

    bool connectToDatabase(const QString &host,
                           int port,
                           const QString &databaseName,
                           const QString &userName,
                           const QString &password);

    QList<Drink> getReadyDrinks();

    bool createReadyDrinkOrder(int readyDrinkId,
                               const QString &drinkName,
                               const QString &size,
                               const QString &temperature,
                               double totalPrice);

    bool createCustomDrinkOrder(const QString &customerName,
                                const QString &composition,
                                const QString &size,
                                const QString &customBase,
                                const QString &customSyrup,
                                const QString &customTopping,
                                bool cheeseFoam,
                                double totalPrice);

    QString lastError() const;
    bool isConnected() const;

    bool updateOrderEstimatedMinutes(int orderId, int minutes);

    int createReadyDrinkOrderAndReturnId(int readyDrinkId,
                                         const QString &drinkName,
                                         const QString &size,
                                         const QString &temperature,
                                         double totalPrice,
                                         int estimatedMinutes,
                                         const QString &customerName = QString());

    int createCustomDrinkOrderAndReturnId(const QString &size,
                                          const QString &base,
                                          const QString &syrup,
                                          const QString &topping,
                                          bool cheeseFoam,
                                          double totalPrice,
                                          int estimatedMinutes,
                                          const QString &customerName = QString());

    Order getOrderById(int orderId);

    QList<Order> getOrders();
    bool updateOrderStatus(int orderId, const QString &status);
private:
    QSqlDatabase m_database;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H
