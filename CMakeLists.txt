cmake_minimum_required(VERSION 3.16)

project(BooBooCan VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_standard_project_setup()

qt_add_executable(BooBooCan
    main.cpp

    mainwindow.h
    mainwindow.cpp

    databasemanager.h
    databasemanager.cpp

    readydrinkswidget.h
    readydrinkswidget.cpp

    drink.h
    order.h
    settingswidget.h
    settingswidget.cpp
    customdrinkwidget.h
    customdrinkwidget.cpp
    orderstatuswidget.h orderstatuswidget.cpp

)

target_link_libraries(BooBooCan PRIVATE
    Qt6::Widgets
    Qt6::Sql
)
