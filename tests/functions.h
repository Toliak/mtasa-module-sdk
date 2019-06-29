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
    int callElementGetPosition(lua_State *);
}