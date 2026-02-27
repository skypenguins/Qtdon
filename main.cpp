#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle(QStringLiteral("Default"));

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app,    []() { QCoreApplication::exit(1); },
        Qt::QueuedConnection);

    engine.loadFromModule("Qtdon", "Main");

    return app.exec();
}
