cmake_minimum_required(VERSION 2.8.11)
project(libgenn-qt)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

include_directories(${LIBLA})

add_subdirectory(libgenn)
include_directories(${LIBGENN})

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBGENN})
set(LIBGENN_QT ${INCLUDE} PARENT_SCOPE)
include_directories(${INCLUDE} ${LIBLA})

set(SOURCE source/netview.cpp source/common.cpp source/plot.cpp)

add_library(genn-qt STATIC ${SOURCE})
target_link_libraries(genn ${LIBS})
