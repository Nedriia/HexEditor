cmake_minimum_required(VERSION 3.20)
project(HexEditor)

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SRC_DIR "${PROJECT_DIR}/src")
set(LIBS_DIR "${PROJECT_DIR}/Librairies")
set(VENDOR_DIR "${PROJECT_DIR}/vendor")

option(IMGUI_VISUAL_DISPLAY "Use ImGui" ON)
option(QT_VISUAL_DISPLAY "Use QT" OFF)
option(LEAK_DETECTOR_ENABLE "Enable leak detector" ON)

add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/Buffer.cpp
        ${SRC_DIR}/Buffer.h
        ${SRC_DIR}/HexEditor.cpp
        ${SRC_DIR}/HexEditor.h
)

if(LEAK_DETECTOR_ENABLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LEAK_DETECTOR)
    set(VLD_PATH "C:/Program Files (x86)/Visual Leak Detector")
    target_include_directories(${PROJECT_NAME} PRIVATE ${VLD_PATH}/include)
    target_link_libraries(${PROJECT_NAME} PRIVATE "${VLD_PATH}/lib/Win64/vld.lib")
endif()

if(IMGUI_VISUAL_DISPLAY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_ENABLED)
    set(IMGUI_DIR "${PROJECT_DIR}/imgui")

    target_sources(${PROJECT_NAME} PRIVATE
            ${SRC_DIR}/HexEditor_ImGUI.cpp
            ${SRC_DIR}/HexEditor_ImGUI.h
            ${IMGUI_DIR}/imgui.cpp
            ${IMGUI_DIR}/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_tables.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp
    )

    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${VENDOR_DIR}/glfw )

    set(IMGUI_BACKEND "OPENGL" CACHE STRING "Graphic Backend for ImGui")
    if(IMGUI_BACKEND STREQUAL "OPENGL")
        target_sources(${PROJECT_NAME} PRIVATE
                ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
                ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
                ${VENDOR_DIR}/glad.c
        )
        find_package(OpenGL REQUIRED)

        target_link_libraries(${PROJECT_NAME} PRIVATE
                glfw
                OpenGL::GL
        )
    endif()

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${IMGUI_DIR}
            ${IMGUI_DIR}/backends
            ${LIBS_DIR}/Include
    )
endif()

if(QT_VISUAL_DISPLAY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_ENABLED)
    target_sources(${PROJECT_NAME} PRIVATE
            ${SRC_DIR}/HexEditor_QT.cpp
            ${SRC_DIR}/HexEditor_QT.h
    )
endif()