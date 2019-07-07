cmake_minimum_required(VERSION 3.0)
project(ModuleSdk)
option(BUILD_TEST "Build test mtasa module" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/MtaLua)

set(
        ${PROJECT_NAME}_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(
        ${PROJECT_NAME}_INCLUDE_FILES
        ${${PROJECT_NAME}_INCLUDE_DIR}/ModuleSdk/Exception.h
        ${${PROJECT_NAME}_INCLUDE_DIR}/ModuleSdk/LuaArgument.h
        ${${PROJECT_NAME}_INCLUDE_DIR}/ModuleSdk/LuaVmExtended.h
)
set(
        ${PROJECT_NAME}_SCR_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/LuaVmExtended.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/LuaArgument.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/LuaObject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Exception.cpp
)

add_library(
        ${PROJECT_NAME}
        STATIC
        ${${PROJECT_NAME}_INCLUDE_FILES}
        ${${PROJECT_NAME}_SCR_FILES}
)
include_directories(${${PROJECT_NAME}_INCLUDE_DIR})
set(MTA_LUA MtaLua)

set(
        ${PROJECT_NAME}_LINK
        ${MTA_LUA}
)
if (WIN32)
    set(
            ${PROJECT_NAME}_LINK
            ${${PROJECT_NAME}_LINK}
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/lua5.1.lib
    )
endif ()
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LINK})

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE
        "$<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>")

if (BUILD_TEST)
    set(TEST_NAME ${PROJECT_NAME}Test)
    set(
            ${TEST_NAME}_SCR_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/functions.cpp
    )
    add_library(${TEST_NAME} SHARED ${${TEST_NAME}_SCR_FILES})
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME} ${MTA_LUA})
endif ()