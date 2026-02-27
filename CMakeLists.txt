cmake_minimum_required(VERSION 3.16)

project(Qtdon VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Network QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(QtdonApp
    main.cpp
    oauth2.h oauth2.cpp
    oauthmastodon.h oauthmastodon.cpp
    qmastodonnetbase.h qmastodonnetbase.cpp
    qmastodonpoststatus.h qmastodonpoststatus.cpp
)

set_target_properties(QtdonApp PROPERTIES
    OUTPUT_NAME Qtdon
    MACOSX_BUNDLE TRUE
)

qt_add_qml_module(QtdonApp
    URI Qtdon
    VERSION 1.0
    QML_FILES
        Main.qml
        content/AuthWindow.qml
    SOURCES
        mastodonclient.h mastodonclient.cpp
)

target_link_libraries(QtdonApp PRIVATE
    Qt6::Quick
    Qt6::Network
    Qt6::QuickControls2
)
