#include <list>

#include "functions.h"


#define CREATE_TEST_FUNCTION(x) \
class _test_function_class_##x  \
{                               \
    static int function(lua_State *);  \
                                \
    static AddType _add;        \
};                              \
AddType _test_function_class_##x::_add                                 \
    = allFunctions.insert(std::pair<std::string, Type>(#x, _test_function_class_##x::function));    \
int _test_function_class_##x::function(lua_State *luaVm)


namespace TestFunction
{

using AddType = std::pair<decltype(allFunctions)::iterator, bool>;

std::unordered_map<std::string, Type> allFunctions = {};

std::string stackDump(lua_State *luaVm)
{
    std::string result;
    int i;
    int top = lua_gettop(luaVm);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(luaVm, i);
        switch (t) {

            case LUA_TSTRING:  /* strings */
                result += lua_tostring(luaVm, i);
                break;

            case LUA_TBOOLEAN:  /* booleans */
                result += (lua_toboolean(luaVm, i) ? "true" : "false");
                break;

            case LUA_TNUMBER:  /* numbers */
                result += std::to_string(lua_tonumber(luaVm, i));
                break;

            default:  /* other values */
                result += lua_typename(luaVm, t);
                break;

        }
        result += "  ";  /* put a separator */
    }
    return result;
}

CREATE_TEST_FUNCTION(simple)
{
    LuaVmExtended lua(luaVm);
    lua.pushArgument(LuaArgument(std::string("Yes!")));
    return 1;
}

CREATE_TEST_FUNCTION(simpleList)
{
    LuaVmExtended lua(luaVm);
    std::list<LuaArgument> list = {
        LuaArgument(std::string("Sample string")),
        LuaArgument(-543),
        LuaArgument(true),
        LuaArgument(5.4),
    };

    return lua.pushArguments(list.cbegin(), list.cend());
}

CREATE_TEST_FUNCTION(echo)
{
    LuaVmExtended lua(luaVm);
    auto vector = lua.getArguments();

    return lua.pushArguments(vector.cbegin(), vector.cend());
}

CREATE_TEST_FUNCTION(isNumber)
{
    LuaVmExtended lua(luaVm);
    try {
        lua.parseArgument(1, LuaTypeNumber);
    } catch (const LuaUnexpectedType &) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }
    lua.pushArgument(LuaArgument(true));
    return 1;
}

CREATE_TEST_FUNCTION(isString)
{
    LuaVmExtended lua(luaVm);
    try {
        lua.parseArgument(1, LuaTypeString);
    } catch (const LuaUnexpectedType &) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }
    lua.pushArgument(LuaArgument(true));
    return 1;
}

CREATE_TEST_FUNCTION(echoElement)
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

CREATE_TEST_FUNCTION(strictTypes)
{
    LuaVmExtended lua(luaVm);
    try {
        lua.getArguments({LuaTypeBoolean, LuaTypeString, LuaTypeInteger});
    } catch (const LuaUnexpectedType &e) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }
    lua.pushArgument(LuaArgument(true));
    return 1;
}

CREATE_TEST_FUNCTION(simpleTable)
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

CREATE_TEST_FUNCTION(callGetElementPosition)
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

    return lua.pushArguments(callReturn.cbegin(), callReturn.cend());
}

CREATE_TEST_FUNCTION(callElementGetDimensionMethod)
{
    LuaVmExtended lua(luaVm);
    LuaArgument element = lua.parseArgument(1, LuaTypeUserdata);
    element.extractObject();

    lua_getglobal(luaVm, "Element");
    lua_getfield(luaVm, -1, "getDimension");
    lua.pushArgument(element);

    lua_pcall(luaVm, 1, 1, 0);
    lua.pushArgument(lua.parseArgument(-1, LuaTypeInteger));
    return 1;
}

CREATE_TEST_FUNCTION(pushFunction)
{
    LuaVmExtended lua(luaVm);

    int ref = luaL_ref(luaVm, LUA_REGISTRYINDEX);

    lua_rawgeti(luaVm, LUA_REGISTRYINDEX, ref);
    luaL_unref(luaVm, LUA_REGISTRYINDEX, ref);

    lua_pcall(luaVm, 0, 1, 0);
    lua.pushArgument(lua.parseArgument(-1, LuaTypeInteger));
    return 1;
}

CREATE_TEST_FUNCTION(advancedTable)
{
    LuaVmExtended lua(luaVm);

    std::list<LuaArgument> arguments{
        LuaArgument(std::string("start")),
        LuaArgument((LuaArgument::TableListType) {
            LuaArgument(-1),
            LuaArgument({
                            {LuaArgument(std::string("keyOne")), LuaArgument(6547)},
                            {LuaArgument(true), LuaArgument(std::string("value"))},
                        }),
            LuaArgument(-3),
        }),
        LuaArgument(7854),
        LuaArgument(std::string("stop")),
    };

    return lua.pushArguments(arguments.cbegin(), arguments.cend());
}

CREATE_TEST_FUNCTION(tableToList)
{
    LuaVmExtended lua(luaVm);

    LuaArgument table = lua.parseArgument(1);
    if (table.getType() != LuaTypeTableMap) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }

    LuaArgument::TableListType list;
    try {
        list = table.toList();
    } catch (LuaException &e) {
        lua.pushArgument(
            LuaArgument(false)
        );
        return 1;
    }

    return lua.pushArguments(list.cbegin(), list.cend());
}

CREATE_TEST_FUNCTION(listToMap)
{
    LuaVmExtended lua(luaVm);

    LuaArgument::TableListType list{
        LuaArgument(53),
        LuaArgument(42),
        LuaArgument(24),
        LuaArgument(74),
        LuaArgument(81),
    };

    LuaArgument::TableMapType map = LuaArgument(list).toMap();
    map[LuaArgument(std::string("key"))] = LuaArgument(876);

    lua.pushArgument(LuaArgument(map));
    return 1;
}

CREATE_TEST_FUNCTION(constructors)
{
    LuaVmExtended lua(luaVm);

    auto args = lua.getArguments();
    LuaArgument argument = args.back();

    // Copy constructor
    LuaArgument copy(argument);

    // Copy assignment
    LuaArgument copyAssignment = argument;

    try {
        copy.extractObject();
        copyAssignment.extractObject();
    } catch (LuaException &) {

    }

    // Move constructor
    LuaArgument move(std::move(copy));

    // Move assignment
    LuaArgument moveAssignment = std::move(copyAssignment);

    std::list<LuaArgument> pushArgs{
        argument,
        move,
        moveAssignment,
    };

    return lua.pushArguments(pushArgs.cbegin(), pushArgs.cend());
}

CREATE_TEST_FUNCTION(checkGetArgumentsUnexpected)
{
    LuaVmExtended lua(luaVm);

    try {
        lua.getArguments({LuaTypeNumber});      // Exception here

        lua.pushArgument(LuaArgument(false));
    } catch (LuaUnexpectedType &e) {
        lua.pushArgument(LuaArgument(true));
    } catch (LuaException &) {
        lua.pushArgument(LuaArgument(false));
    }

    lua.pushArgument(LuaArgument(true));
    return 1;
}

CREATE_TEST_FUNCTION(checkGetArgumentsBad)
{
    LuaVmExtended lua(luaVm);

    try {
        lua.getArguments();

        lua.pushArgument(LuaArgument(false));
    } catch (LuaBadType &e) {
        lua.pushArgument(LuaArgument(true));
    } catch (LuaException &) {
        lua.pushArgument(LuaArgument(false));
    }

    return 1;
}

CREATE_TEST_FUNCTION(checkGetArgumentsOutOfRange)
{
    LuaVmExtended lua(luaVm);

    try {
        lua.getArguments({LuaTypeNumber, LuaTypeNumber});

        lua.pushArgument(LuaArgument(false));
    } catch (LuaOutOfRange &e) {
        lua.pushArgument(LuaArgument(true));
    } catch (LuaException &) {
        lua.pushArgument(LuaArgument(false));
    }

    return 1;
}

CREATE_TEST_FUNCTION(checkParseArgumentObject)
{
    LuaVmExtended lua(luaVm);

    LuaArgument object;
    try {
        object = lua.parseArgument(1, LuaTypeObject);
    } catch (LuaException &) {
        lua.pushArgument(LuaArgument(false));
        return 1;
    }

    lua.pushArgument(LuaArgument(
        object.getType() == LuaTypeObject
    ));
    return 1;
}

CREATE_TEST_FUNCTION(callFunction)
{
    LuaVmExtended lua(luaVm);

    std::string name = lua.parseArgument(1, LuaTypeString).toString();
    auto result = lua.call(name, {}, 1);

    return lua.pushArguments(result.cbegin(), result.cend());
}

}

