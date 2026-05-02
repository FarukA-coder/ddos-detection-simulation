cmake_minimum_required(VERSION 3.14)

project(DDoSDetectionSimulation
    VERSION 1.0.0
    DESCRIPTION "Professional DDoS Detection Simulation Tool"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── MSVC specific settings ───
if(MSVC)
    add_compile_options(/W3 /utf-8 /EHsc)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
endif()

# ─── Fetch Dependencies ───
include(FetchContent)

# GLFW (Window management)
FetchContent_Declare(glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/3.3.8.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Dear ImGui
FetchContent_Declare(imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.89.9.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw opengl32)

# ImPlot (Charts & Graphs)
FetchContent_Declare(implot
    URL https://github.com/epezent/implot/archive/refs/tags/v0.16.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(implot)
if(NOT implot_POPULATED)
    FetchContent_Populate(implot)
endif()

add_library(implot STATIC
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)

# ─── Source files ───
set(CORE_SOURCES
    src/core/Packet.cpp
    src/core/TrafficWindow.cpp
)
set(NETWORK_SOURCES
    src/network/ClientNode.cpp
    src/network/AttackerNode.cpp
    src/network/ServerNode.cpp
)
set(DETECTION_SOURCES
    src/detection/ThresholdDetector.cpp
    src/detection/EntropyDetector.cpp
    src/detection/RateLimitDetector.cpp
)
set(SECURITY_SOURCES  src/security/Firewall.cpp)
set(SIMULATION_SOURCES  src/simulation/NetworkSimulator.cpp)
set(LOGGING_SOURCES  src/logging/Logger.cpp)
set(UI_SOURCES  src/ui/Dashboard.cpp)

# ─── Executable (WIN32 = no console window) ───
add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    ${CORE_SOURCES}
    ${NETWORK_SOURCES}
    ${DETECTION_SOURCES}
    ${SECURITY_SOURCES}
    ${SIMULATION_SOURCES}
    ${LOGGING_SOURCES}
    ${UI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    implot
)

# ─── Install rules ───
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES README.md DESTINATION doc)

# ─── CPack ───
set(CPACK_PACKAGE_NAME "DDoS Detection Simulation")
set(CPACK_PACKAGE_VENDOR "Faruk - OOP Final Project")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
include(CPack)
