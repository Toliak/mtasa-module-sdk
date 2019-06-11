#include <list>

#include "functions.h"

namespace TestFunction
{

int simple(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    lua.pushArgument(LuaArgument(std::string("Yes!")));
    return 1;
}

int simpleList(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    std::list<LuaArgument> list = {
        LuaArgument(std::string("Sample string")),
        LuaArgument(-543),
        LuaArgument(true),
        LuaArgument(5.4),
    };

    lua.pushArguments(list.cbegin(), list.cend());
    return list.size();
}

int echo(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    auto vector = lua.getArguments();
    lua.pushArguments(vector.cbegin(), vector.cend());
    return vector.size();
}

int isNumber(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    try {
        lua.parseArgument(1, LuaArgumentType::NUMBER);
    } catch (const LuaUnexpectedType&) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }
    lua.pushArgument(LuaArgument(true));
    return 1;
}

int isString(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    try {
        lua.parseArgument(1, LuaArgumentType::STRING);
    } catch (const LuaUnexpectedType&) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }
    lua.pushArgument(LuaArgument(true));
    return 1;
}

int echoElement(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    LuaArgument arg;
    try {
        arg = lua.parseArgument(1);
        arg.extractObject();
    } catch (const LuaUnexpectedType&) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }
    lua.pushArgument(arg);
    return 1;
}

}

