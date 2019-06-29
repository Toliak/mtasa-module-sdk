#include "ModuleSdk/LuaVmExtended.h"

std::vector<LuaArgument> LuaVmExtended::getArguments(const std::list<LuaArgumentType> &types) const
{
    std::vector<LuaArgument> result(types.size());

    int index = 1;
    auto listIterator = types.cbegin();
    for (; lua_type(luaVm, index) != LUA_TNONE && listIterator != types.cend(); index++, listIterator++) {
        try {
            result[index - 1] = parseArgument(index, *listIterator);
        } catch (LuaException &) {
            throw LuaUnexpectedType(
                *listIterator,
                static_cast<LuaArgumentType>(lua_type(luaVm, index)),
                index
            );
        }
    }

    if (index - 1 != types.size()) {
        throw std::out_of_range("Not enough arguments");                //TODO: LuaOutOfRange
    }


    return result;
}

void LuaVmExtended::pushArgument(const LuaArgument &argument) const
{
    if (argument.getType() == LuaArgumentType::NIL) {
        lua_pushnil(luaVm);
    } else if (argument.getType() == LuaArgumentType::NUMBER) {
        lua_pushnumber(luaVm, argument.toNumber());
    } else if (argument.getType() == LuaArgumentType::INTEGER) {
        lua_pushinteger(luaVm, argument.toInteger());
    } else if (argument.getType() == LuaArgumentType::STRING) {
        lua_pushstring(luaVm, argument.toString().c_str());
    } else if (argument.getType() == LuaArgumentType::BOOLEAN) {
        lua_pushboolean(luaVm, argument.toBool());
    } else if (argument.getType() == LuaArgumentType::LIGHTUSERDATA) {
        lua_pushlightuserdata(luaVm, argument.toPointer());
    } else if (argument.getType() == LuaArgumentType::USERDATA) {
        lua_pushlightuserdata(luaVm, argument.toPointer());
    } else if (argument.getType() == LuaArgumentType::OBJECT) {
        this->pushObject(argument.toObject());
    } else {
        throw LuaUnexpectedPushType(argument.getType());
    }
}

LuaArgument LuaVmExtended::parseArgument(int index) const
{
    if (lua_isboolean(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::BOOLEAN, true);
    } else if (lua_isnumber(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::NUMBER, true);
    } else if (lua_isstring(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::STRING, true);
    } else if (lua_isuserdata(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::USERDATA, true);
    } else if (lua_isnil(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::NIL, true);
    }

    throw LuaBadType();
}

LuaArgument LuaVmExtended::parseArgument(int index, LuaArgumentType type, bool force) const
{
    static const std::unordered_map<LuaArgumentType, int (*)(lua_State *, int)> TYPE_CHECKER = {
        {LuaArgumentType::INTEGER, lua_isnumber},
        {LuaArgumentType::NUMBER, lua_isnumber},
        {LuaArgumentType::STRING, lua_isstring},
        {LuaArgumentType::USERDATA, lua_isuserdata},

        {
            LuaArgumentType::BOOLEAN,
            [](lua_State *vm, int index) -> int
            {
                return lua_isboolean(vm, index);
            }
        },
        {
            LuaArgumentType::NIL,
            [](lua_State *vm, int index) -> int
            {
                return lua_isnil(vm, index);
            }
        },
    };

    if (!force) {
        int (*checker)(lua_State *, int);
        try {
            checker = TYPE_CHECKER.at(type);
        } catch (std::out_of_range &) {
            throw LuaBadType();
        }

        if (!checker(luaVm, index)) {
            throw LuaUnexpectedType(type);
        }
    }

    if (type == LuaArgumentType::BOOLEAN) {
        return LuaArgument(static_cast<bool>(lua_toboolean(luaVm, index)));
    } else if (type == LuaArgumentType::NUMBER) {
        return LuaArgument(static_cast<double>(lua_tonumber(luaVm, index)));
    } else if (type == LuaArgumentType::STRING) {
        return LuaArgument(std::string(lua_tostring(luaVm, index)));
    } else if (type == LuaArgumentType::USERDATA) {
        return LuaArgument(lua_touserdata(luaVm, index));
    } else if (type == LuaArgumentType::INTEGER) {
        return LuaArgument(static_cast<int>(lua_tointeger(luaVm, index)));
    }

    return LuaArgument();
}

void LuaVmExtended::pushObject(const LuaObject &object) const
{
    auto *pointer = reinterpret_cast<void *>(object.getObjectId().id);

    lua_pushstring(luaVm, "ud");
    lua_rawget(luaVm, LUA_REGISTRYINDEX);

    // First we want to check if we have a userdata for this already
    lua_pushlightuserdata(luaVm, pointer);
    lua_rawget(luaVm, -2);

    if (lua_isnil(luaVm, -1)) {
        lua_pop(luaVm, 1);

        // we don't have it, create it
        *(void **) lua_newuserdata(luaVm, sizeof(void *)) = pointer;

        // save in ud table
        lua_pushlightuserdata(luaVm, pointer);
        lua_pushvalue(luaVm, -2);
        lua_rawset(luaVm, -4);
    }

    // userdata is already on the stack, just remove the table
    lua_remove(luaVm, -2);

    // Get class
    lua_pushstring(luaVm, "mt");                     // "mt"
    lua_rawget(luaVm, LUA_REGISTRYINDEX);            // mt=
    lua_pushstring(luaVm, object.getStringClass().c_str());  // mt, class name
    lua_rawget(luaVm, -2);                    // mt, class
    lua_remove(luaVm, -2);            // class

    // Assign the class metatable
    lua_setmetatable(luaVm, -2);            // element
}
