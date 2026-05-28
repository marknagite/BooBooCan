#ifndef CUSTOMDRINKWIDGET_H
#define CUSTOMDRINKWIDGET_H

#include <QWidget>
#include <QString>
#include <QSize>

#include "databasemanager.h"

class QLabel;
class QComboBox;
class QCheckBox;
class QPushButton;

class CustomDrinkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomDrinkWidget(DatabaseManager *databaseManager, QWidget *parent = nullptr);

signals:
    void orderCreated(int orderId);

private:
    void updateConstructor();
    void showOrderMessage();

    double currentTotalPrice() const;
    QString compositionText() const;

    QString sizeKey() const;
    QString baseKey() const;
    QString previewImagePath() const;
    QString syrupIconPath() const;
    QString toppingIconPath() const;
    QString foamIconPath() const;

    QString resolvedImagePath(const QString &path) const;
    void setImageOrText(QLabel *label,
                        const QString &path,
                        const QString &fallbackText,
                        const QSize &size);

    DatabaseManager *m_databaseManager;

    QComboBox *m_sizeComboBox;
    QComboBox *m_baseComboBox;
    QComboBox *m_syrupComboBox;
    QComboBox *m_toppingComboBox;
    QCheckBox *m_cheeseFoamCheckBox;

    QLabel *m_previewLabel;
    QLabel *m_syrupIconLabel;
    QLabel *m_toppingIconLabel;
    QLabel *m_foamIconLabel;
    QLabel *m_compositionLabel;
    QLabel *m_priceLabel;

    QPushButton *m_createOrderButton;
};

#endif // CUSTOMDRINKWIDGET_H
