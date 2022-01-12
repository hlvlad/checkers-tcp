#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <checkersmodel.h>
#include <networksession.h>
#include "game.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

//    auto game = std::make_unique<Game>();
    auto network_session = std::make_unique<NetworkSession>();

    qmlRegisterType<CheckersModel>("checkersmodel", 1, 0, "CheckersModel");
//    qmlRegisterSingletonInstance("game", 1, 0, "Game", game.get());
    qmlRegisterSingletonInstance("network", 1, 0, "NetworkSession", network_session.get());

//    qmlRegisterUncreatableMetaObject(
//      checkers_enums_ns::staticMetaObject,   // static meta object
//      "enums",                  // import statement (can be any string)
//      1, 0,                     // major and minor version of the import
//      "Enums",                  // name in QML (does not have to match C++ name)
//      "Error: only enums"       // error in case someone tries to create a MyNamespace object
//    );

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
//    QQuickItem *board = root->findChild<QQuickItem*>("board");
//    game->setParentItem(board);

    return app.exec();
}
