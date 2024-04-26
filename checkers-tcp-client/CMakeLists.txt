cmake_minimum_required(VERSION 3.25)

project(CheckersTcpClient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/message_handler.cpp
    src/network_session.cpp
    src/checkers_model.cpp
    )

set(HEADERS
    include/error.h
    include/message_handler.h
    include/network_session.h
    include/checkers_model.h
    )

find_package(QT NAMES Qt6 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

add_executable(CheckersTcpClient ${SOURCES} ${HEADERS} qml.qrc)

target_include_directories(CheckersTcpClient PRIVATE include qml)
target_compile_definitions(CheckersTcpClient
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(CheckersTcpClient
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick CheckersTcpCore PackUnpack)
