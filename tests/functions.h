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
    int callElementGetDimensionMethod(lua_State *);
    int pushFunction(lua_State *);
    int advancedTable(lua_State *);
    int tableToList(lua_State *);
    int listToMap(lua_State *);
    int constructors(lua_State *);
    int checkGetArgumentsUnexpected(lua_State *);
    int checkGetArgumentsBad(lua_State *);
    int checkGetArgumentsOutOfRange(lua_State *);
    int checkParseArgumentObject(lua_State *);
    int callFunction(lua_State *);
}