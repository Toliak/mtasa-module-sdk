cmake_minimum_required(VERSION 3.4)

project(MtaLua)

set(
        ${PROJECT_NAME}_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(
        ${PROJECT_NAME}_INCLUDE_FILES
        ${${PROJECT_NAME}_INCLUDE_DIR}/MtaLua/ILuaModuleManager.h
        ${${PROJECT_NAME}_INCLUDE_DIR}/MtaLua/lua.h
        ${${PROJECT_NAME}_INCLUDE_DIR}/MtaLua/luaconf.h
        ${${PROJECT_NAME}_INCLUDE_DIR}/MtaLua/lualib.h
        ${${PROJECT_NAME}_INCLUDE_DIR}/MtaLua/lauxlib.h
        ${${PROJECT_NAME}_INCLUDE_DIR}/MtaLua/utils.h
)
set(
        ${PROJECT_NAME}_SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/LuaImports.cpp
)

add_library(
        ${PROJECT_NAME}
        ${${PROJECT_NAME}_INCLUDE_FILES}
        ${${PROJECT_NAME}_SRC_FILES}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

include_directories(${${PROJECT_NAME}_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE
        "$<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIR}>")