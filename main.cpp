#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQuickStyle>
#include "binding.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQuickStyle::setStyle("Default");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Binding binding;
    engine.rootContext()->setContextProperty("mastodon", &binding);

    return app.exec();
}
