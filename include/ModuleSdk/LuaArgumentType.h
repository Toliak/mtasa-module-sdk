#pragma once

#include "lua/lua.h"

enum LuaArgumentType
{
    NIL = LUA_TNIL,
    BOOLEAN = LUA_TBOOLEAN,
    LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
    NUMBER = LUA_TNUMBER,
    STRING = LUA_TSTRING,
    USERDATA = LUA_TUSERDATA,
    INTEGER = 1001,
    OBJECT = 1002,
};