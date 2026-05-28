#ifndef READYDRINKSWIDGET_H
#define READYDRINKSWIDGET_H

#include <QWidget>
#include <QList>
#include <QSize>

#include "drink.h"
#include "databasemanager.h"

class QScrollArea;
class QGridLayout;
class QFrame;
class QLabel;
class QComboBox;
class QPushButton;
class QEvent;

class ReadyDrinksWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReadyDrinksWidget(DatabaseManager *databaseManager, QWidget *parent = nullptr);

    void loadDrinks();

signals:
    void orderCreated(int orderId);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QFrame *createDrinkCard(const Drink &drink, int index);

    void selectDrink(int index);
    void updateDetails();
    void updateTotalPrice();
    void showSelectedDrink();

    double currentTotalPrice() const;

    void setDrinkPhoto(QLabel *label, const QString &imagePath, const QSize &size);
    QString resolvedImagePath(const QString &imagePath) const;

    DatabaseManager *m_databaseManager;
    QList<Drink> m_drinks;

    QScrollArea *m_scrollArea;
    QWidget *m_cardsContainer;
    QGridLayout *m_cardsLayout;

    QLabel *m_detailsPhotoLabel;
    QLabel *m_detailsNameLabel;
    QLabel *m_detailsDescriptionLabel;
    QComboBox *m_sizeComboBox;
    QComboBox *m_temperatureComboBox;
    QLabel *m_totalPriceLabel;
    QPushButton *m_orderButton;

    Drink m_currentDrink;
    int m_selectedIndex;
    bool m_hasCurrentDrink;
};

#endif // READYDRINKSWIDGET_H
