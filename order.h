#ifndef ORDER_H
#define ORDER_H

#include <QString>
#include <QDateTime>

struct Order {
    int id;
    QString orderType;
    QString drinkName;
    QString composition;
    QString size;
    QString temperature;
    double totalPrice;
    QString status;
    QDateTime createdAt;
    int estimatedMinutes;
    QString customerName;
};

#endif // ORDER_H
