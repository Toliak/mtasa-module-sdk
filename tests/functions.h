#pragma once

#include "ModuleSdk/LuaVmExtended.h"
#include <unordered_map>


namespace TestFunction
{

using Type = int (*)(lua_State *);

extern std::unordered_map<std::string, Type> allFunctions;

}