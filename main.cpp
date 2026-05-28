#include <QApplication>
#include <QIcon>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

#include "mainwindow.h"

static QString findAssetPath(const QString &relativePath)
{
    if (relativePath.isEmpty()) {
        return "";
    }

    QFileInfo directInfo(relativePath);

    if (directInfo.exists()) {
        return directInfo.absoluteFilePath();
    }

    QDir dir(QCoreApplication::applicationDirPath());

    for (int i = 0; i < 8; ++i) {
        const QString candidate = dir.filePath(relativePath);

        if (QFileInfo::exists(candidate)) {
            return candidate;
        }

        dir.cdUp();
    }

    return "";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;

    const QString iconPath = findAssetPath("assets/logo/app_icon.ico");

    if (!iconPath.isEmpty()) {
        app.setWindowIcon(QIcon(iconPath));
        window.setWindowIcon(QIcon(iconPath));
    }

    window.show();

    return app.exec();
}
