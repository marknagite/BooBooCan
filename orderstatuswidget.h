#ifndef ORDERSTATUSWIDGET_H
#define ORDERSTATUSWIDGET_H

#include <QWidget>
#include <QList>

#include "databasemanager.h"
#include "order.h"

class QLabel;
class QListWidget;
class QTimer;

class OrderStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OrderStatusWidget(DatabaseManager *databaseManager, QWidget *parent = nullptr);

    void setOrderId(int orderId);

private:
    void loadOrdersHistory();
    void loadOrder();
    void updateVisualStatus();
    QString localizedStatus(const QString &status) const;

    DatabaseManager *m_databaseManager;
    QList<Order> m_orders;
    int m_orderId;
    Order m_order;
    QTimer *m_timer;

    QLabel *m_titleLabel;
    QListWidget *m_historyListWidget;
    QLabel *m_orderNumberLabel;
    QLabel *m_drinkNameLabel;
    QLabel *m_statusLabel;
    QLabel *m_remainingTimeLabel;
};

#endif // ORDERSTATUSWIDGET_H
