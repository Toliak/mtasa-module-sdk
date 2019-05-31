cmake_minimum_required(VERSION 3.4)

project(ModuleSdk)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/MtaLua)

set(
        ${PROJECT_NAME}_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories(${${PROJECT_NAME}_INCLUDE_DIR})

set(
        ${PROJECT_NAME}_INCLUDE_FILES

)

set(
        ${PROJECT_NAME}_SCR_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp
)

add_library(
        ${PROJECT_NAME}
        ${${PROJECT_NAME}_INCLUDE_FILES}
        ${${PROJECT_NAME}_SCR_FILES}
)

set(MTA_LUA MtaLua)
target_link_libraries(${PROJECT_NAME} ${MTA_LUA})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE
        "$<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>")