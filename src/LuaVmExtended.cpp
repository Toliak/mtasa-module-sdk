#include "ModuleSdk/LuaVmExtended.h"

std::vector<LuaArgument> LuaVmExtended::getArguments(const std::list<LuaArgumentType> &types) const
{
    std::vector<LuaArgument> result(types.size());

    int index = 1;                          ///< Stack index
    auto listIterator = types.cbegin();     ///< Type iterator
    for (; lua_type(luaVm, index) != LUA_TNONE && listIterator != types.cend(); index++, listIterator++) {
        // While we have value on stack and type lists not ended
        try {
            result[index - 1] = parseArgument(index, *listIterator);
        } catch (LuaException &) {
            throw LuaUnexpectedType(
                *listIterator,
                static_cast<LuaArgumentType>(lua_type(luaVm, index)),
                index
            );
        }
        // TODO: extend LuaUnexpectedType and re-throw LuaBadType
    }

    if (index - 1 != types.size()) {
        throw std::out_of_range("Not enough arguments");                //TODO: LuaOutOfRange
    }

    return result;
}

void LuaVmExtended::pushArgument(const LuaArgument &argument) const
{
    if (argument.getType() == LuaArgumentType::LueTypeNil) {
        lua_pushnil(luaVm);
    } else if (argument.getType() == LuaArgumentType::LuaTypeNumber) {
        lua_pushnumber(luaVm, argument.toNumber());
    } else if (argument.getType() == LuaArgumentType::LuaTypeInteger) {
        lua_pushinteger(luaVm, argument.toInteger());
    } else if (argument.getType() == LuaArgumentType::LuaTypeString) {
        lua_pushstring(luaVm, argument.toString().c_str());
    } else if (argument.getType() == LuaArgumentType::LuaTypeBoolean) {
        lua_pushboolean(luaVm, argument.toBool());
    } else if (argument.getType() == LuaArgumentType::LuaTypeLightUserdata) {
        lua_pushlightuserdata(luaVm, argument.toPointer());
    } else if (argument.getType() == LuaArgumentType::LuaTypeUserdata) {
        lua_pushlightuserdata(luaVm, argument.toPointer());
    } else if (argument.getType() == LuaArgumentType::LuaTypeObject) {
        this->pushObject(argument.toObject());
    } else if (argument.getType() == LuaArgumentType::LuaTypeTableList) {
        this->pushTableList(argument);
    } else if (argument.getType() == LuaArgumentType::LuaTypeTableMap) {
        this->pushTableMap(argument);
    } else {
        throw LuaUnexpectedPushType(argument.getType());
    }
}

LuaArgument LuaVmExtended::parseArgument(int index) const
{
    if (lua_isboolean(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::LuaTypeBoolean, true);
    } else if (lua_isnumber(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::LuaTypeNumber, true);
    } else if (lua_isstring(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::LuaTypeString, true);
    } else if (lua_isuserdata(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::LuaTypeUserdata, true);
    } else if (lua_isnil(luaVm, index)) {
        return parseArgument(index, LuaArgumentType::LueTypeNil, true);
    } else if (lua_istable(luaVm, index)) {
        // Cannot autodetect list
        return parseArgument(index, LuaArgumentType::LuaTypeTableMap, true);
    }

    throw LuaBadType(lua_type(luaVm, index));
}

LuaArgument LuaVmExtended::parseArgument(int index, LuaArgumentType type, bool force) const
{
    static const std::unordered_map<LuaArgumentType, int (*)(lua_State *, int)> TYPE_CHECKER = {
        {LuaArgumentType::LuaTypeInteger, lua_isnumber},
        {LuaArgumentType::LuaTypeNumber, lua_isnumber},
        {LuaArgumentType::LuaTypeString, lua_isstring},
        {LuaArgumentType::LuaTypeUserdata, lua_isuserdata},

        {
            LuaArgumentType::LuaTypeBoolean,
            [](lua_State *vm, int index) -> int
            {
                return lua_isboolean(vm, index);
            }
        },
        {
            LuaArgumentType::LueTypeNil,
            [](lua_State *vm, int index) -> int
            {
                return lua_isnil(vm, index);
            }
        },
        {
            LuaArgumentType::LuaTypeTableMap,
            [](lua_State *vm, int index) -> int
            {
                return lua_istable(vm, index);
            }
        },
    };      ///< Checker function dictionary

    if (!force) {               // No need to check type, if force
        int (*checker)(lua_State *, int);
        try {
            checker = TYPE_CHECKER.at(type);
        } catch (std::out_of_range &) {
            throw LuaBadType(lua_type(luaVm, index));
        }

        if (!checker(luaVm, index)) {
            throw LuaUnexpectedType(type);
        }
    }

    if (type == LuaArgumentType::LuaTypeBoolean) {
        return LuaArgument(static_cast<bool>(lua_toboolean(luaVm, index)));
    } else if (type == LuaArgumentType::LuaTypeNumber) {
        return LuaArgument(static_cast<double>(lua_tonumber(luaVm, index)));
    } else if (type == LuaArgumentType::LuaTypeString) {
        return LuaArgument(std::string(lua_tostring(luaVm, index)));
    } else if (type == LuaArgumentType::LuaTypeUserdata) {
        LuaArgument result(lua_touserdata(luaVm, index));
        result.extractObject();                     // TODO: remove autoextract
        return result;
    } else if (type == LuaArgumentType::LuaTypeInteger) {
        return LuaArgument(static_cast<int>(lua_tointeger(luaVm, index)));
    } else if (type == LuaArgumentType::LuaTypeTableMap) {
        LuaArgument::TableMapType result;
        lua_pushnil(luaVm);         // Current key is nil

        while (lua_next(luaVm, index) != 0) {
            // Cannot user negative index in case of nested tables
            LuaArgument key = parseArgument(lua_gettop(luaVm) - 2 + 1);       // Parse -2 (but index from start)
            LuaArgument value = parseArgument(lua_gettop(luaVm) - 1 + 1);     // Parse -1 (but index from start)

            result[key] = value;
            lua_pop(luaVm, 1);
        }

        return LuaArgument(result);
    }

    return LuaArgument();
}

void LuaVmExtended::pushObject(const LuaObject &object) const
{
    auto *pointer = reinterpret_cast<void *>(object.getObjectId().id);

    // Code from official MTASA repository

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
