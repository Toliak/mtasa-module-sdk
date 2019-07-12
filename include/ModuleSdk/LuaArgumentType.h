#pragma once

#include "lua/lua.h"

/// LuaArgument types description
enum LuaArgumentType
{
    // Lua types
    LuaTypeNil = LUA_TNIL,
    LuaTypeBoolean = LUA_TBOOLEAN,
    LuaTypeLightUserdata = LUA_TLIGHTUSERDATA,
    LuaTypeNumber = LUA_TNUMBER,
    LuaTypeString = LUA_TSTRING,
    LuaTypeUserdata = LUA_TUSERDATA,
    LuaTypeTableMap = LUA_TTABLE,

    // Custom types
    LuaTypeInteger = 1001,
    LuaTypeObject = 1002,
    LuaTypeTableList = 1003,
};