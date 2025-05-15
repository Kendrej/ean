cmake_minimum_required(VERSION 3.16)
project(EAN_GUI_Solver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Automatyczna obsługa Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Dodaj wszystkie pliki źródłowe i nagłówki
set(SOURCES
    main.cpp
    MainWindow.cpp
    # MatrixInputWidget.cpp
    Parser.cpp
    Solver.cpp
)

set(HEADERS
    MainWindow.h
    # MatrixInputWidget.h
    Parser.h
    Solver.h
)


# Ścieżki do bibliotek
include_directories(
    ${CMAKE_SOURCE_DIR}  # dla mpreal.h i innych lokalnych
    "C:/Users/Dell/Desktop/Studia/ean/boost/boost_1_88_0"  # Boost Interval
)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Linkowanie MPFR + GMP (wymagane przez mpreal)
link_directories("C:/msys64/mingw64/lib")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    mpfr
    gmp
)
