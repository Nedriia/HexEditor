cmake_minimum_required(VERSION 3.20)
project(HexEditor)

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${PROJECT_DIR}/src")
set(LIBS_DIR "${PROJECT_DIR}/Librairies")
set(VENDOR_DIR "${PROJECT_DIR}/vendor")

add_executable(${PROJECT_NAME}
        ${SRC_DIR}/Buffer.cpp
        ${SRC_DIR}/Buffer.h
        ${SRC_DIR}/HexEditor.cpp
        ${SRC_DIR}/HexEditor.h
        ${SRC_DIR}/HexEditor_ImGUI.cpp
        ${SRC_DIR}/HexEditor_ImGUI.h
        ${SRC_DIR}/HexEditor_QT.cpp
        ${SRC_DIR}/HexEditor_QT.h
)