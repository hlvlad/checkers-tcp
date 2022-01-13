cmake_minimum_required(VERSION 3.2)

project(CheckersTcpClient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/checkers_engine.cpp
    src/main.cpp
    src/message.cpp
    src/networkgamesession.cpp
    src/networksession.cpp
    src/pack.cpp
    src/checkersmodel.cpp
    )

set(HEADERS
    include/checkers_engine.h
    include/error.h
    include/message.h
    include/new_misc.h
    include/networkgamesession.h
    include/networksession.h
    include/pack.h
    include/checkersmodel.h
    )

set(QML_FILES
    qml/CheckerBoard.qml
    qml/ConnectingPane.qml
    qml/CreatedLobbyPane.qml
    qml/ErrorDialog.qml
    qml/GamePane.qml
    qml/MainMenu.qml
    qml/MenuButton.qml
    qml/Settings.qml
    qml/SettingsDialog.qml
    qml/main.qml
    )

include_directories(qml include)

find_package(QT NAMES Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

add_executable(CheckersTcpClient ${SOURCES} ${HEADERS} qml.qrc)

target_compile_definitions(CheckersTcpClient
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(CheckersTcpClient
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
