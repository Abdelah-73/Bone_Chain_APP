cmake_minimum_required(VERSION 3.16)
project(BoneFertilizerApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic compilation tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 (Change to Qt5 if necessary)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Include directories so your GUI can find your Core headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)

# Define your source files
set(PROJECT_SOURCES
    GUI/main.cpp
    GUI/mainwindow.cpp
    GUI/mainwindow.h
    GUI/userdialog.cpp
    GUI/userdialog.h
    GUI/supplierdialog.cpp
    GUI/supplierdialog.h
    GUI/productdialog.cpp
    GUI/productdialog.h
    GUI/articledialog.cpp
    GUI/articledialog.h
    GUI/articlereaderdialog.cpp
    GUI/articlereaderdialog.h
    GUI/customerdialog.cpp
    GUI/customerdialog.h
    GUI/orderstatusdialog.cpp
    GUI/orderstatusdialog.h
    GUI/deliverydialog.cpp
    GUI/deliverydialog.h
    # Note: Because your core logic is entirely in header files (.h),
    # you don't need to add them to PROJECT_SOURCES for compilation.
)

# Create the executable
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})

# Link Qt libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
