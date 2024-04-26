# Checkers GUI game with Multiplayer

## Building from source

### Prerequites

You need to have Qt6 installed on your system. For Ubuntu, you can use following command

`sudo apt update && sudo apt install qt6-base-dev qt6-declarative-dev qml5-module-qtquick qml6-module-qtquick-window qml6-module-qtquick-layouts qml6-module-qtquick-controls qml6-module-qtqml-workerscript qml6-module-qtquick-dialogs qml6-module-qtquick-templates qml6-module-qt5compat-graphicaleffects`



### Building with CMake

To build both server and client apps, use following commands:
```bash
mkdir build
cd build
cmake ..
make -j
```

Or you can use this single liner:

```bash
mkdir build && cd build && cmake .. && make -j
```