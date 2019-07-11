#pragma once

#include <unordered_map>

#include "lua/lua.h"
#include "ModuleSdk/LuaVmExtended.h"


namespace TestFunction
{

using Type = int (*)(lua_State *);

extern std::unordered_map<std::string, Type> allFunctions;

}