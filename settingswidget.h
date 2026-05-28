#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QString>

class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);

signals:
    void themeChanged(const QString &theme);
    void currencyChanged(const QString &currency);
    void profileChanged(const QString &name, const QString &photoPath);

private:
    void loadSettings();
    void saveSettings();
    void loadPhoto();
    void updatePhotoPreview();

    QComboBox *m_themeComboBox;
    QComboBox *m_currencyComboBox;
    QComboBox *m_cityComboBox;

    QLineEdit *m_nameLineEdit;
    QLabel *m_photoPreviewLabel;
    QPushButton *m_loadPhotoButton;
    QPushButton *m_saveButton;

    QString m_photoPath;
};

#endif // SETTINGSWIDGET_H
