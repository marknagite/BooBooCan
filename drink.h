#ifndef DRINK_H
#define DRINK_H

#include <QString>

struct Drink {
    int id;
    QString name;
    QString category;
    QString description;
    double basePrice;
    QString imagePath;
};

#endif // DRINK_H
