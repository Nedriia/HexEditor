cmake_minimum_required(VERSION 3.20)
project(HexEditor)

set( PROJECT_DIR    "${CMAKE_CURRENT_SOURCE_DIR}" )
set( SRC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/src" )

add_executable( ${PROJECT_NAME}
                ${SRC_DIR}/main.cpp
                ${SRC_DIR}/Buffer.cpp
                ${SRC_DIR}/Buffer.h
                ${SRC_DIR}/HexEditor.cpp
                ${SRC_DIR}/HexEditor.h
)

option( LEAK_DETECTOR_ENABLE "Enable leak detector" OFF )
if( LEAK_DETECTOR_ENABLE )
    target_compile_definitions( ${PROJECT_NAME} PRIVATE LEAK_DETECTOR )

    set(VLD_PATH "C:/Program Files (x86)/Visual Leak Detector")

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${VLD_PATH}/include
    )

    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            "${VLD_PATH}/lib/Win64/vld.lib"
    )
endif()