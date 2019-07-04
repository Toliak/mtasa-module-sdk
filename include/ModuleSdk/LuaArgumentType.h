#pragma once

#include "lua/lua.h"

/// LuaArgument types description
enum LuaArgumentType
{
    // TODO: Refactor names

    // Lua types
    NIL = LUA_TNIL,
    BOOLEAN = LUA_TBOOLEAN,
    LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
    NUMBER = LUA_TNUMBER,
    STRING = LUA_TSTRING,
    USERDATA = LUA_TUSERDATA,
    TABLE_MAP = LUA_TTABLE,

    // Custom types
    INTEGER = 1001,
    OBJECT = 1002,
    TABLE_LIST = 1003,
};