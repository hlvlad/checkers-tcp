/**
 * @file main.cpp
 * @brief Contains the main function for the Checkers TCP client application.
 */
#include "checkers_model.h"
#include "network_session.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

/**
 * @brief The main entry point for the Checkers TCP client application.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return The exit code of the application.
 */
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    auto network_session = std::make_unique<NetworkSession>();

    qmlRegisterType<CheckersModel>("checkersmodel", 1, 0, "CheckersModel");
    qmlRegisterSingletonInstance("network", 1, 0, "NetworkSession", network_session.get());

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QObject *root = engine.rootObjects().at(0);

    network_session->setParent(root);

    return app.exec();
}
