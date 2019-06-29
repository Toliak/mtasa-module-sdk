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
    } catch (const LuaUnexpectedType &) {
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
    } catch (const LuaUnexpectedType &) {
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
    } catch (const LuaUnexpectedType &) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    } catch (const LuaUnexpectedArgumentType &) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    } catch (const LuaException &e) {
        lua.pushArgument(LuaArgument(std::string(e.what())));
        return 1;
    }
    lua.pushArgument(arg);
    return 1;
}

int strictTypes(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    try {
        lua.getArguments({LuaArgumentType::BOOLEAN, LuaArgumentType::STRING, LuaArgumentType::INTEGER});
    } catch (const LuaUnexpectedType &e) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }
    lua.pushArgument(LuaArgument(true));
    return 1;
}

int simpleTable(lua_State *luaVm)
{
    lua_createtable(luaVm, 1, 0);
    lua_createtable(luaVm, 0, 2); /* creates and pushes new table on top of Lua stack */

    lua_pushstring(luaVm, "name"); /* Pushes table value on top of Lua stack */
    lua_setfield(luaVm, -2, "name");  /* table["name"] = row->name. Pops key value */

    lua_pushstring(luaVm, "surname");
    lua_setfield(luaVm, -2, "surname");

    lua_rawseti(luaVm, -2, 1);

    /* Returning one table which is already on top of Lua stack. */

    return 1;
}

std::string stackDump(lua_State *L)
{
    std::string result;
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {

            case LUA_TSTRING:  /* strings */
                result += lua_tostring(L, i);
                break;

            case LUA_TBOOLEAN:  /* booleans */
                result += (lua_toboolean(L, i) ? "true" : "false");
                break;

            case LUA_TNUMBER:  /* numbers */
                result += std::to_string(lua_tonumber(L, i));
                break;

            default:  /* other values */
                result += lua_typename(L, t);
                break;

        }
        result += "  ";  /* put a separator */
    }
    return result;
}

int callGetElementPosition(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);

    std::vector<LuaArgument> elements;
    try {
        elements = lua.getArguments();
        for (LuaArgument &element : elements) {
            element.extractObject();
        }
    } catch (const LuaException &) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }

    std::list<LuaArgument> callReturn;
    try {
        for (LuaArgument &element : elements) {
            std::vector<LuaArgument> returns = lua.call("getElementPosition", {element}, 3);
            for (const LuaArgument &v : returns) {
                callReturn.push_back(v);
            }
        }
    } catch (const LuaException &e) {
        lua.pushArgument(LuaArgument(std::string(e.what())));
        return 1;
    }

    lua.pushArguments(callReturn.cbegin(), callReturn.cend());
    return callReturn.size();
}

int call(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    LuaArgument element = lua.parseArgument(1, LuaArgumentType::USERDATA);
    element.extractObject();

    lua_getglobal(luaVm, "Element");
    lua_getfield(luaVm, -1, "getDimension");
    lua.pushArgument(element);

    lua_pcall(luaVm, 1, 1, 0);
    lua.pushArgument(lua.parseArgument(-1, LuaArgumentType::INTEGER));
    return 1;
}

int pushFunction(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);

    int ref = luaL_ref(luaVm, LUA_REGISTRYINDEX);

//    lua.pushArgument(function);
    lua_rawgeti(luaVm, LUA_REGISTRYINDEX, ref);
    luaL_unref(luaVm, LUA_REGISTRYINDEX, ref);

    lua_pcall(luaVm, 0, 1, 0);
    lua.pushArgument(lua.parseArgument(-1, LuaArgumentType::INTEGER));
    return 1;
}

}

