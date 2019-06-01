#pragma once

#include <list>
#include <vector>
#include <unordered_map>

#include "lua/lua.h"
#include "lua/ILuaModuleManager.h"
#include "LuaArgument.h"

/**
 * @brief Extends lua_State functional
 */
class LuaVmExtended
{
public:
    explicit LuaVmExtended(lua_State *luaVm)
        : luaVm(luaVm)
    {}

    const std::vector<LuaArgument> &getArguments()
    {
        if (!isArgumentsCaptured) {
            captureArguments();
        }

        return arguments;
    }

    /*template<class ...Args>
    const std::vector<LuaArgument> &getArguments(Args... args)
    {
        std::list<LuaArgumentType> typesList{args...};

        return arguments;
    }*/

    template<typename IT>
    int pushArguments(IT begin, IT end)
    {
        for (IT it = begin; it != end; it++) {
            pushArgument(*it);
        }
        return 1;
    }

    void pushArgument(const LuaArgument &argument)
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
        }
    }

private:
    void captureArguments()
    {
        if (!arguments.empty()) {
            arguments.clear();
        }

        for (int index = 1; lua_type(luaVm, index) != LUA_TNONE; index++) {
            arguments.push_back(parseArgument(index));
        }
    }

    /**
     * @brief Parse argument
     * @param index Argument index
     * @throws LuaVmBadType Unexpected type has been captured
     * @return Argument object
     */
    LuaArgument parseArgument(int index)
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

        throw LuaVmBadType();
    }

    LuaArgument parseArgument(int index, LuaArgumentType type, bool force = false)
    {
        static const std::unordered_map<LuaArgumentType, int (*)(lua_State *, int)> TYPE_CHECKER = {
            {LuaArgumentType::INTEGER, lua_isnumber},
            {LuaArgumentType::NUMBER, lua_isnumber},
            {LuaArgumentType::STRING, lua_isstring},
            {LuaArgumentType::USERDATA, lua_isuserdata},

            {LuaArgumentType::BOOLEAN, [](lua_State *vm, int index) -> int {
                return lua_isboolean(vm, index);
            }},
            {LuaArgumentType::NIL, [](lua_State *vm, int index) -> int {
                return lua_isnil(vm, index);
            }},
        };

        if (!force) {
            int (*checker)(lua_State *, int);
            try {
                checker = TYPE_CHECKER.at(type);
            } catch (std::out_of_range &) {
                throw LuaVmBadType();
            }

            if (!checker(luaVm, index)) {
                throw LuaVmUnexpectedType();
            }
        }

        if (type == LuaArgumentType::BOOLEAN) {
            return LuaArgument(static_cast<bool>(lua_toboolean(luaVm, index)));
        } else if (type == LuaArgumentType::NUMBER) {
            return LuaArgument(static_cast<double>(lua_tonumber(luaVm, index)));
        } else if (type == LuaArgumentType::STRING) {
            return LuaArgument(std::string(lua_tostring(luaVm, index)));
        } else if (type == LuaArgumentType::USERDATA) {
            return LuaArgument(static_cast<void *>(lua_touserdata(luaVm, index)));
        }

        return LuaArgument();
    }

    lua_State *luaVm;                               /// Original VM
    bool isArgumentsCaptured = false;               /// Arguments capture flag
    std::vector<LuaArgument> arguments;         /// Captured arguments
};