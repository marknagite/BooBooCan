#include "orderstatuswidget.h"

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QTimer>
#include <QDateTime>
#include <Qt>

OrderStatusWidget::OrderStatusWidget(DatabaseManager *databaseManager, QWidget *parent)
    : QWidget(parent),
    m_databaseManager(databaseManager),
    m_orders(),
    m_orderId(-1),
    m_order(),
    m_timer(new QTimer(this)),
    m_titleLabel(nullptr),
    m_historyListWidget(nullptr),
    m_orderNumberLabel(nullptr),
    m_drinkNameLabel(nullptr),
    m_statusLabel(nullptr),
    m_remainingTimeLabel(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 32, 40, 32);
    layout->setSpacing(14);

    m_titleLabel = new QLabel("Статус заказа", this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *historyTitleLabel = new QLabel("История заказов", this);
    QFont historyFont = historyTitleLabel->font();
    historyFont.setPointSize(13);
    historyFont.setBold(true);
    historyTitleLabel->setFont(historyFont);

    m_historyListWidget = new QListWidget(this);
    m_historyListWidget->setMinimumHeight(180);

    m_orderNumberLabel = new QLabel("Заказ не выбран", this);
    m_orderNumberLabel->setAlignment(Qt::AlignCenter);

    m_drinkNameLabel = new QLabel(this);
    m_drinkNameLabel->setAlignment(Qt::AlignCenter);
    m_drinkNameLabel->setWordWrap(true);

    m_statusLabel = new QLabel(this);
    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(18);
    statusFont.setBold(true);
    m_statusLabel->setFont(statusFont);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    m_remainingTimeLabel = new QLabel(this);
    m_remainingTimeLabel->setAlignment(Qt::AlignCenter);
    m_remainingTimeLabel->setWordWrap(true);

    layout->addWidget(m_titleLabel);
    layout->addWidget(historyTitleLabel);
    layout->addWidget(m_historyListWidget);
    layout->addSpacing(10);
    layout->addWidget(m_orderNumberLabel);
    layout->addWidget(m_drinkNameLabel);
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_remainingTimeLabel);
    layout->addStretch();

    connect(m_historyListWidget, &QListWidget::currentRowChanged,
            this, [this](int row) {
                if (row < 0 || row >= m_orders.size()) {
                    return;
                }

                m_orderId = m_orders.at(row).id;
                loadOrder();
            });

    connect(m_timer, &QTimer::timeout,
            this, [this]() {
                loadOrdersHistory();
                loadOrder();
            });

    loadOrdersHistory();
    loadOrder();
    m_timer->start(1000);
}

void OrderStatusWidget::setOrderId(int orderId)
{
    m_orderId = orderId;
    loadOrdersHistory();
    loadOrder();

    if (!m_timer->isActive()) {
        m_timer->start(1000);
    }
}

void OrderStatusWidget::loadOrdersHistory()
{
    if (m_databaseManager == nullptr) {
        return;
    }

    m_orders = m_databaseManager->getOrders();

    m_historyListWidget->blockSignals(true);
    m_historyListWidget->clear();

    int selectedRow = -1;

    for (int i = 0; i < m_orders.size(); ++i) {
        const Order &order = m_orders.at(i);

        QString itemText = QString("№%1 • %2 • %3")
                               .arg(order.id)
                               .arg(order.drinkName.isEmpty() ? "Напиток" : order.drinkName)
                               .arg(localizedStatus(order.status));

        if (order.createdAt.isValid()) {
            itemText += QString(" • %1").arg(order.createdAt.toString("dd.MM HH:mm"));
        }

        QListWidgetItem *item = new QListWidgetItem(itemText, m_historyListWidget);
        item->setData(Qt::UserRole, order.id);

        if (order.id == m_orderId) {
            selectedRow = i;
        }
    }

    if (!m_orders.isEmpty()) {
        if (selectedRow < 0) {
            selectedRow = 0;
            m_orderId = m_orders.first().id;
        }

        m_historyListWidget->setCurrentRow(selectedRow);
    } else {
        m_orderId = -1;
    }

    m_historyListWidget->blockSignals(false);
}

void OrderStatusWidget::loadOrder()
{
    if (m_databaseManager == nullptr) {
        m_orderNumberLabel->setText("DatabaseManager не найден");
        m_drinkNameLabel->clear();
        m_statusLabel->clear();
        m_remainingTimeLabel->clear();
        return;
    }

    if (m_orderId <= 0) {
        m_orderNumberLabel->setText("Активных заказов пока нет");
        m_drinkNameLabel->clear();
        m_statusLabel->clear();
        m_remainingTimeLabel->clear();
        return;
    }

    m_order = m_databaseManager->getOrderById(m_orderId);

    if (m_order.id <= 0) {
        m_statusLabel->setText("Не удалось загрузить заказ");
        m_remainingTimeLabel->setText(m_databaseManager->lastError());
        return;
    }

    updateVisualStatus();
}

void OrderStatusWidget::updateVisualStatus()
{
    m_orderNumberLabel->setText(QString("Заказ №%1").arg(m_order.id));
    m_drinkNameLabel->setText(
        m_order.customerName.isEmpty()
            ? m_order.drinkName
            : QString("%1\nКлиент: %2").arg(m_order.drinkName, m_order.customerName)
        );

    if (m_order.status == "cancelled") {
        m_statusLabel->setText("Заказ отменён");
        m_remainingTimeLabel->setText("Этот заказ не будет приготовлен");
        return;
    }

    if (m_order.status == "ready") {
        m_statusLabel->setText("Ваш напиток готов!");
        m_remainingTimeLabel->setText("Можно забирать BooBooCan");
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    qint64 elapsedSeconds = m_order.createdAt.secsTo(now);

    int realSecondsToReady = m_order.estimatedMinutes * 5;
    int remainingSeconds = realSecondsToReady - static_cast<int>(elapsedSeconds);

    if (remainingSeconds <= 0) {
        m_statusLabel->setText("Ваш напиток готов!");
        m_remainingTimeLabel->setText("Можно забирать BooBooCan");

        if (m_order.status != "ready") {
            m_databaseManager->updateOrderStatus(m_order.id, "ready");
            m_order.status = "ready";
            loadOrdersHistory();
        }

        return;
    }

    if (elapsedSeconds < 2) {
        m_statusLabel->setText("Заказ принят");
        m_remainingTimeLabel->setText("Скоро начнём готовить");
        return;
    }

    m_statusLabel->setText("Напиток готовится");
    m_remainingTimeLabel->setText(
        QString("Осталось примерно %1 сек.").arg(remainingSeconds)
        );
}

QString OrderStatusWidget::localizedStatus(const QString &status) const
{
    if (status == "new") {
        return "Принят";
    }

    if (status == "preparing") {
        return "Готовится";
    }

    if (status == "ready") {
        return "Готов";
    }

    if (status == "cancelled") {
        return "Отменён";
    }

    return status;
}
