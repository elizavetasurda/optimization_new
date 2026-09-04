set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.13)
project(optimization)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# Исходники
# ============================================
file(GLOB_RECURSE SOURCES "./*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*/build/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*CMakeCXXCompilerId.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/support_by_make/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/plugins/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/gui/.*")
add_executable(${PROJECT_NAME} ${SOURCES})

# ============================================
# Поиск библиотеки PFM (только для Linux)
# ============================================
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    
    pkg_check_modules(PFM libpfm4)
    if(PFM_FOUND)
        message(STATUS "PFM found via pkg-config")
        target_include_directories(${PROJECT_NAME} PRIVATE ${PFM_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${PFM_LIBRARIES})
        target_compile_options(${PROJECT_NAME} PRIVATE ${PFM_CFLAGS_OTHER})
    else()
        find_library(PFM_LIBRARY NAMES pfm)
        find_path(PFM_INCLUDE_DIR perfmon/pfmlib.h)
        
        if(PFM_LIBRARY AND PFM_INCLUDE_DIR)
            message(STATUS "PFM found manually")
            target_include_directories(${PROJECT_NAME} PRIVATE ${PFM_INCLUDE_DIR})
            target_link_libraries(${PROJECT_NAME} PRIVATE ${PFM_LIBRARY})
        else()
            message(FATAL_ERROR "PFM library not found! Install libpfm4-dev")
        endif()
    endif()
    
    target_compile_definitions(${PROJECT_NAME} PRIVATE __linux__)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
else()
    message(STATUS "Skipping PFM (not Linux)")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NO_PFM)
endif()

# ============================================
# Флаги компиляции
# ============================================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -Wall -Wextra)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O2 -Wall)
endif()

# ============================================
# Копирование исполняемого файла
# ============================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_SOURCE_DIR}/
    COMMENT "Copying ${PROJECT_NAME} to main directory"
)

message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "========================================")
# ============================================
# GUI (Qt)
# ============================================
option(BUILD_GUI "Build Qt GUI wrapper" ON)
if(BUILD_GUI)
    # Сначала пробуем найти Qt6 (у вас как раз он через qt6-base-dev)
    find_package(Qt6 COMPONENTS Widgets QUIET)
    if(NOT Qt6_FOUND)
        # Если Qt6 нет — пробуем Qt5, чтобы код работал и на других машинах
        find_package(Qt5 5.15 COMPONENTS Widgets REQUIRED)
    endif()

    # AUTOMOC говорит CMake автоматически прогонять каждый .h файл с Q_OBJECT
    # через moc (Meta-Object Compiler) — это генератор, который "оживляет"
    # сигналы/слоты. Без этой строки Q_OBJECT просто не заработает.
    set(CMAKE_AUTOMOC ON)

    add_executable(optimization_gui
        gui/main_gui.cpp
        gui/MainWindow.cpp
        gui/ListEditor.cpp
        gui/OptimizationRunner.cpp
    )

    # Линкуем нужную версию Qt Widgets, какая нашлась
    target_link_libraries(optimization_gui PRIVATE
        $<IF:$<TARGET_EXISTS:Qt6::Widgets>,Qt6::Widgets,Qt5::Widgets>
    )

    # Копируем готовый бинарник в корень репозитория (как и для optimization)
    add_custom_command(TARGET optimization_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:optimization_gui>
            ${CMAKE_SOURCE_DIR}/
    )
endif()
