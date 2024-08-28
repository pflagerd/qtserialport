# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.28)
project(receiver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui SerialPort Widgets)

#qt_standard_project_setup()

add_executable(receiver
    dialog.cpp
    dialog.h
    main.cpp
)

#set_target_properties(receiver PROPERTIES
#    WIN32_EXECUTABLE TRUE
#    MACOSX_BUNDLE TRUE
#)

target_link_libraries(receiver PRIVATE
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5SerialPort_INCLUDE_DIRS})