#pragma once

#include "lua/lua.h"
#include "ModuleSdk/LuaVmExtended.h"

namespace TestFunction
{
    int simple(lua_State *);
    int simpleList(lua_State *);
    int echo(lua_State *);
    int isNumber(lua_State *);
    int isString(lua_State *);
    int echoElement(lua_State *);
    int strictTypes(lua_State *);
    int simpleTable(lua_State *);
    int callGetElementPosition(lua_State *);
    int call(lua_State *);
    int pushFunction(lua_State *);
    int advancedTable(lua_State *);
    int tableToList(lua_State *);
    int listToMap(lua_State *);
}