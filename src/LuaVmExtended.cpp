#include "ModuleSdk/LuaVmExtended.h"

std::vector<LuaArgument> LuaVmExtended::getArguments()
{
    std::vector<LuaArgument> result;

    for (int index = 1; lua_type(luaVm, index) != LUA_TNONE; index++) {
        result.push_back(parseArgument(index));
    }

    return result;
}

std::vector<LuaArgument> LuaVmExtended::getArguments(const std::list<LuaArgumentType> &types) const
{
    std::vector<LuaArgument> result(types.size());

    int index = 1;                          ///< Stack index
    auto listIterator = types.cbegin();     ///< Type iterator
    for (; lua_type(luaVm, index) != LUA_TNONE && listIterator != types.cend(); index++, listIterator++) {
        // While we have value on stack and type lists not ended
        try {
            result[index - 1] = parseArgument(index, *listIterator);
        } catch (LuaUnexpectedType &e) {
            // Extends captured exception
            throw LuaUnexpectedType(
                *listIterator,
                static_cast<LuaArgumentType>(lua_type(luaVm, index)),
                index
            );
        }
    }

    if (index - 1 != types.size()) {
        throw LuaOutOfRange("Not enough arguments");
    }

    return result;
}

void LuaVmExtended::pushArgument(const LuaArgument &argument) const
{
    if (argument.getType() == LuaArgumentType::LuaTypeNil) {
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
        return parseArgument(index, LuaArgumentType::LuaTypeNil, true);
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
        {LuaArgumentType::LuaTypeObject, lua_isuserdata},

        {
            LuaArgumentType::LuaTypeBoolean,
            [](lua_State *vm, int index) -> int
            {
                return lua_isboolean(vm, index);
            }
        },
        {
            LuaArgumentType::LuaTypeNil,
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
    }
    if (type == LuaArgumentType::LuaTypeNumber) {
        return LuaArgument(static_cast<double>(lua_tonumber(luaVm, index)));
    }
    if (type == LuaArgumentType::LuaTypeString) {
        return LuaArgument(std::string(lua_tostring(luaVm, index)));
    }
    if (type == LuaArgumentType::LuaTypeUserdata) {
        LuaArgument result(lua_touserdata(luaVm, index));
        return result;
    }
    if (type == LuaArgumentType::LuaTypeInteger) {
        return LuaArgument(static_cast<int>(lua_tointeger(luaVm, index)));
    }
    if (type == LuaArgumentType::LuaTypeTableMap) {
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
    if (type == LuaArgumentType::LuaTypeObject) {
        LuaArgument result(lua_touserdata(luaVm, index));
        result.extractObject();
        return result;
    }

    return LuaArgument();
}

std::vector<LuaArgument> LuaVmExtended::call(const std::string &function,
                                             const std::list<LuaArgument> &functionArgs,
                                             int returnSize) const
{
    callFunction(function, functionArgs, returnSize);
    return getCallReturn(returnSize);
}

void LuaVmExtended::callFunction(const std::string &function,
                                 const std::list<LuaArgument> &functionArgs,
                                 int returnSize) const
{
    lua_pushstring(luaVm, function.c_str());
    lua_gettable(luaVm, LUA_GLOBALSINDEX);              // To get global function
    this->pushArguments(functionArgs.cbegin(), functionArgs.cend());

    int state = lua_pcall(
        luaVm,
        static_cast<int>(functionArgs.size()),
        returnSize,
        0
    );

    if (state == LUA_ERRRUN || state == LUA_ERRMEM) {                   // Error handler
        std::string message;
        try {
            message = getCallReturn(std::list<LuaArgumentType>{LuaArgumentType::LuaTypeString}).front().toString();
        } catch (const LuaException &e) {
            message = "Cannot get error message: ";
            message += e.what();
        }
        throw LuaCallException(state, message);
    }
}

std::vector<LuaArgument> LuaVmExtended::getCallReturn(int amount) const
{
    if (amount <= 0) {
        return {};
    }

    // TODO: check stack size
    // TODO: check LUA_TNONE

    std::vector<LuaArgument> result(static_cast<size_t>(amount));
    for (int i = 0; i < amount; i++) {
        int luaIndex = -amount + i;
        result[i] = parseArgument(luaIndex);
    }
    return result;
}

std::vector<LuaArgument> LuaVmExtended::getCallReturn(const std::list<LuaArgumentType> &types) const
{
    std::vector<LuaArgument> result(types.size());

    auto index = -static_cast<int>(types.size());           ///< Stack index (starts from minimal negative)
    auto listIterator = types.cbegin();                     ///< Type iterator
    auto resultIterator = result.begin();                   ///< Where to place result value
    for (; lua_type(luaVm, index) != LUA_TNONE && listIterator != types.cend(); index++, listIterator++) {
        // While we have value on stack and type lists not ended
        *(resultIterator++) = parseArgument(index, *listIterator);
    }

    return result;
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

void LuaVmExtended::pushTableList(const LuaArgument &argument) const
{
    const std::vector<LuaArgument> &list = argument.toList();

    lua_createtable(luaVm, list.size(), 0);
    for (int i = 0; i < list.size(); i++) {
        this->pushArgument(list[i]);

        lua_rawseti(luaVm, -2, i + 1);
    }
}

void LuaVmExtended::pushTableMap(const LuaArgument &argument) const
{
    const auto &map = argument.toMap();

    lua_createtable(luaVm, 0, map.size());
    for (const auto &pair : map) {
        this->pushArgument(pair.first);             // Set key
        this->pushArgument(pair.second);            // Set value

        lua_rawset(luaVm, -3);                      // Set table row
    }
}
