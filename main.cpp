#include "mainwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 启用高DPI缩放支持
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication a(argc, argv);
    MainWidget w;
    w.show();
    return a.exec();
}
