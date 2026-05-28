#include "databasemanager.h"

#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

static QString sqlText(const QString &value)
{
    QString safe = value;
    safe.replace("'", "''");
    return "'" + safe + "'";
}

DatabaseManager::DatabaseManager()
{
    m_database = QSqlDatabase::addDatabase("QPSQL");
}

bool DatabaseManager::connectToDatabase(const QString &host,
                                        int port,
                                        const QString &databaseName,
                                        const QString &userName,
                                        const QString &password)
{
    Q_UNUSED(host);
    Q_UNUSED(port);
    Q_UNUSED(databaseName);
    Q_UNUSED(userName);
    Q_UNUSED(password);

    if (m_database.isOpen()) {
        m_database.close();
    }

    m_database.setHostName("localhost");
    m_database.setPort(5432);
    m_database.setDatabaseName("BooBooCan");
    m_database.setUserName("postgres");
    m_database.setPassword("unstoppable");

    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        return false;
    }

    QSqlQuery query(m_database);
    if (!query.exec("SELECT COUNT(*) FROM public.ready_drinks")) {
        m_lastError = query.lastError().text();
        m_database.close();
        return false;
    }

    m_lastError.clear();
    return true;
}

QList<Drink> DatabaseManager::getReadyDrinks()
{
    QList<Drink> drinks;

    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return drinks;
    }

    QSqlQuery query(m_database);

    if (!query.exec(
            "SELECT id, name, category, description, base_price, image_path "
            "FROM public.ready_drinks "
            "ORDER BY id")) {
        m_lastError = query.lastError().text();
        return drinks;
    }

    while (query.next()) {
        Drink drink;
        drink.id = query.value(0).toInt();
        drink.name = query.value(1).toString();
        drink.category = query.value(2).toString();
        drink.description = query.value(3).toString();
        drink.basePrice = query.value(4).toDouble();

        if (query.value(5).isNull()) {
            drink.imagePath = "";
        } else {
            drink.imagePath = query.value(5).toString();
        }

        drinks.append(drink);
    }

    m_lastError.clear();
    return drinks;
}

bool DatabaseManager::createReadyDrinkOrder(int readyDrinkId,
                                            const QString &drinkName,
                                            const QString &size,
                                            const QString &temperature,
                                            double totalPrice)
{
    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return false;
    }

    QString safeDrinkName = drinkName;
    QString safeSize = size;
    QString safeTemperature = temperature;

    safeDrinkName.replace("'", "''");
    safeSize.replace("'", "''");
    safeTemperature.replace("'", "''");

    const QString sql = QString(
                            "INSERT INTO public.orders "
                            "(order_type, ready_drink_id, drink_name, composition, size, temperature, total_price, status) "
                            "VALUES "
                            "('ready', %1, '%2', NULL, '%3', '%4', %5, 'new')"
                            )
                            .arg(readyDrinkId)
                            .arg(safeDrinkName)
                            .arg(safeSize)
                            .arg(safeTemperature)
                            .arg(QString::number(totalPrice, 'f', 2));

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}

QList<Order> DatabaseManager::getOrders()
{
    QList<Order> orders;

    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return orders;
    }

    QSqlQuery query(m_database);

    if (!query.exec(
            "SELECT id, order_type, drink_name, composition, size, temperature, "
            "total_price, status, created_at, estimated_minutes, customer_name "
            "FROM public.orders "
            "ORDER BY created_at DESC")) {
        m_lastError = query.lastError().text();
        return orders;
    }

    while (query.next()) {
        Order order;
        order.id = query.value(0).toInt();
        order.orderType = query.value(1).toString();
        order.drinkName = query.value(2).toString();
        order.composition = query.value(3).toString();
        order.size = query.value(4).toString();
        order.temperature = query.value(5).toString();
        order.totalPrice = query.value(6).toDouble();
        order.status = query.value(7).toString();
        order.createdAt = query.value(8).toDateTime();
        order.estimatedMinutes = query.value(9).toInt();
        order.customerName = query.value(10).toString();

        orders.append(order);
    }

    m_lastError.clear();
    return orders;
}

bool DatabaseManager::updateOrderStatus(int orderId, const QString &status)
{
    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return false;
    }

    if (status != "new" &&
        status != "preparing" &&
        status != "ready" &&
        status != "cancelled") {
        m_lastError = "Недопустимый статус заказа";
        return false;
    }

    QString safeStatus = status;
    safeStatus.replace("'", "''");

    const QString sql = QString(
                            "UPDATE public.orders "
                            "SET status = '%1' "
                            "WHERE id = %2"
                            )
                            .arg(safeStatus)
                            .arg(orderId);

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        m_lastError = "Заказ не найден";
        return false;
    }

    m_lastError.clear();
    return true;
}

bool DatabaseManager::createCustomDrinkOrder(const QString &customerName,
                                             const QString &composition,
                                             const QString &size,
                                             const QString &customBase,
                                             const QString &customSyrup,
                                             const QString &customTopping,
                                             bool cheeseFoam,
                                             double totalPrice)
{
    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return false;
    }

    QString safeCustomerName = customerName;
    QString safeComposition = composition;
    QString safeSize = size;
    QString safeBase = customBase;
    QString safeSyrup = customSyrup;
    QString safeTopping = customTopping;

    safeCustomerName.replace("'", "''");
    safeComposition.replace("'", "''");
    safeSize.replace("'", "''");
    safeBase.replace("'", "''");
    safeSyrup.replace("'", "''");
    safeTopping.replace("'", "''");

    const QString customerNameSql = safeCustomerName.isEmpty()
                                        ? "NULL"
                                        : QString("'%1'").arg(safeCustomerName);

    const QString sql = QString(
                            "INSERT INTO public.orders "
                            "(order_type, ready_drink_id, drink_name, composition, size, temperature, "
                            "total_price, status, estimated_minutes, customer_name, custom_base, "
                            "custom_syrup, custom_topping, cheese_foam) "
                            "VALUES "
                            "('custom', NULL, 'Собранный напиток', '%1', '%2', 'Не указано', "
                            "%3, 'new', 7, %4, '%5', '%6', '%7', %8)"
                            )
                            .arg(safeComposition)
                            .arg(safeSize)
                            .arg(QString::number(totalPrice, 'f', 2))
                            .arg(customerNameSql)
                            .arg(safeBase)
                            .arg(safeSyrup)
                            .arg(safeTopping)
                            .arg(cheeseFoam ? "true" : "false");

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::isConnected() const
{
    return m_database.isOpen();
}

bool DatabaseManager::updateOrderEstimatedMinutes(int orderId, int minutes)
{
    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return false;
    }

    if (minutes < 0) {
        minutes = 0;
    }

    const QString sql = QString(
                            "UPDATE public.orders "
                            "SET estimated_minutes = %1 "
                            "WHERE id = %2"
                            )
                            .arg(minutes)
                            .arg(orderId);

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        m_lastError = "Заказ не найден";
        return false;
    }

    m_lastError.clear();
    return true;
}

int DatabaseManager::createReadyDrinkOrderAndReturnId(int readyDrinkId,
                                                      const QString &drinkName,
                                                      const QString &size,
                                                      const QString &temperature,
                                                      double totalPrice,
                                                      int estimatedMinutes,
                                                      const QString &customerName)
{
    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return -1;
    }

    const QString customerSql = customerName.isEmpty() ? "NULL" : sqlText(customerName);

    const QString sql = QString(
                            "INSERT INTO public.orders "
                            "(order_type, ready_drink_id, drink_name, composition, size, temperature, "
                            "total_price, status, estimated_minutes, customer_name) "
                            "VALUES "
                            "('ready', %1, %2, '', %3, %4, %5, 'new', %6, %7) "
                            "RETURNING id"
                            )
                            .arg(readyDrinkId)
                            .arg(sqlText(drinkName))
                            .arg(sqlText(size))
                            .arg(sqlText(temperature))
                            .arg(QString::number(totalPrice, 'f', 2))
                            .arg(estimatedMinutes)
                            .arg(customerSql);

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return -1;
    }

    if (!query.next()) {
        m_lastError = "Не удалось получить ID созданного заказа";
        return -1;
    }

    m_lastError.clear();
    return query.value(0).toInt();
}

Order DatabaseManager::getOrderById(int orderId)
{
    Order order;
    order.id = -1;

    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return order;
    }

    const QString sql = QString(
                            "SELECT id, order_type, drink_name, composition, size, temperature, "
                            "total_price, status, created_at, estimated_minutes, customer_name "
                            "FROM public.orders "
                            "WHERE id = %1"
                            ).arg(orderId);

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return order;
    }

    if (!query.next()) {
        m_lastError = "Заказ не найден";
        return order;
    }

    order.id = query.value(0).toInt();
    order.orderType = query.value(1).toString();
    order.drinkName = query.value(2).toString();
    order.composition = query.value(3).toString();
    order.size = query.value(4).toString();
    order.temperature = query.value(5).toString();
    order.totalPrice = query.value(6).toDouble();
    order.status = query.value(7).toString();
    order.createdAt = query.value(8).toDateTime();
    order.estimatedMinutes = query.value(9).toInt();
    order.customerName = query.value(10).toString();

    m_lastError.clear();
    return order;
}

int DatabaseManager::createCustomDrinkOrderAndReturnId(const QString &size,
                                                       const QString &base,
                                                       const QString &syrup,
                                                       const QString &topping,
                                                       bool cheeseFoam,
                                                       double totalPrice,
                                                       int estimatedMinutes,
                                                       const QString &customerName)
{
    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return -1;
    }

    const QString composition = QString("Основа: %1\nСироп: %2\nТоппинг: %3\nСырная пенка: %4")
                                    .arg(base)
                                    .arg(syrup)
                                    .arg(topping)
                                    .arg(cheeseFoam ? "Да" : "Нет");

    const QString customerSql = customerName.isEmpty() ? "NULL" : sqlText(customerName);

    const QString sql = QString(
                            "INSERT INTO public.orders "
                            "(order_type, ready_drink_id, drink_name, composition, size, temperature, "
                            "total_price, status, estimated_minutes, customer_name) "
                            "VALUES "
                            "('custom', NULL, 'Собранный напиток', %1, %2, 'Не указано', "
                            "%3, 'new', %4, %5) "
                            "RETURNING id"
                            )
                            .arg(sqlText(composition))
                            .arg(sqlText(size))
                            .arg(QString::number(totalPrice, 'f', 2))
                            .arg(estimatedMinutes)
                            .arg(customerSql);

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return -1;
    }

    if (!query.next()) {
        m_lastError = "Не удалось получить ID созданного заказа";
        return -1;
    }

    m_lastError.clear();
    return query.value(0).toInt();
}

int DatabaseManager::repeatOrderAndReturnId(int orderId)
{
    if (!m_database.isOpen()) {
        m_lastError = "База данных не подключена";
        return -1;
    }

    if (orderId <= 0) {
        m_lastError = "Некорректный номер заказа";
        return -1;
    }

    const QString sql = QString(
                            "INSERT INTO public.orders "
                            "(order_type, ready_drink_id, drink_name, composition, size, temperature, "
                            "total_price, status, estimated_minutes, customer_name) "
                            "SELECT "
                            "order_type, ready_drink_id, drink_name, composition, size, temperature, "
                            "total_price, 'new', estimated_minutes, customer_name "
                            "FROM public.orders "
                            "WHERE id = %1 "
                            "RETURNING id"
                            ).arg(orderId);

    QSqlQuery query(m_database);

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        return -1;
    }

    if (!query.next()) {
        m_lastError = "Не удалось повторить заказ";
        return -1;
    }

    m_lastError.clear();
    return query.value(0).toInt();
}
