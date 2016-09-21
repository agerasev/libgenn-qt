cmake_minimum_required(VERSION 2.8.11)
project(libgenn-qt)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

include_directories(${LIBLA} ${QT5_INCLUDE} ${QT5_INCLUDE}/QtWidgets ${QT5_INCLUDE}/QtCore ${QT5_INCLUDE}/QtGui)

add_subdirectory(libgenn)
include_directories(${LIBGENN})

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBGENN})
set(LIBGENN_QT ${INCLUDE} PARENT_SCOPE)
include_directories(${INCLUDE} ${LIBLA})

set(SOURCE source/netview.cpp)

add_library(genn-qt STATIC ${SOURCE})
target_link_libraries(genn ${LIBS})
