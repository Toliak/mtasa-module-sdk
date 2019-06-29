#pragma once

#include <stdexcept>
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

    /**
     * @brief Auto parse all arguments from lua VM
     * @return LuaArgument vector
     */
    const std::vector<LuaArgument> &getArguments()
    {
        if (!isArgumentsCaptured) {
            captureArguments();
        }

        return arguments;
    }

    /**
     * @brief Parse arguments from lua VM
     * @param types Strictly assigned types list
     * @return LuaArgument vector
     */
    std::vector<LuaArgument> getArguments(const std::list<LuaArgumentType> &types) const;

    /**
     * @brief Push arguments to lua VM
     * @tparam IT Iterator type
     * @param begin Begin iterator
     * @param end End iterator
     * @return Size of pushed list
     */
    template<typename IT>
    int pushArguments(IT begin, IT end) const
    {
        int size = 0;
        for (IT it = begin; it != end; it++) {
            pushArgument(*it);
            size++;
        }
        return size;
    }

    /**
     * @brief Push single argument to lua VM
     */
    void pushArgument(const LuaArgument &argument) const;

    /**
    * @brief Parse argument from lua VM (auto type detecting)
    * @param index Argument index
    * @throws LuaBadType Bad type has been captured
    * @return Argument object
    */
    LuaArgument parseArgument(int index) const;

    /**
     * @brief Parse argument from lua VM (manual type detecting)
     * @param index Argument index
     * @param type Argument type
     * @param force Check type compatibility
     * @throws LuaBadType Bad type has been captured
     * @throws LuaUnexpectedType Unexpected type has been captured
     * @return Argument Object
     */
    LuaArgument parseArgument(int index, LuaArgumentType type, bool force = false) const;

    void clearStack() const
    {
        lua_settop(luaVm, 0);
    }

    std::vector<LuaArgument>
    call(const std::string &function, const std::list<LuaArgument> &functionArgs, int returnSize = 0) const
    {
        callFunction(function, functionArgs, returnSize);
        return getCallReturn(returnSize);
    }

    virtual ~LuaVmExtended() = default;

private:
    void callFunction(const std::string &function, const std::list<LuaArgument> &functionArgs, int returnSize) const
    {
        lua_pushstring(luaVm, function.c_str());
        lua_gettable(luaVm, LUA_GLOBALSINDEX);
        this->pushArguments(functionArgs.cbegin(), functionArgs.cend());

        int state = lua_pcall(luaVm, functionArgs.size(), returnSize, 0);

        if (state == LUA_ERRRUN || state == LUA_ERRMEM) {
            std::string message;
            try {
                message = getCallReturn(std::list<LuaArgumentType>{LuaArgumentType::STRING}).front().toString();
            } catch (const LuaException &e) {
                message = "Cannot get error message: ";
                message += e.what();
            }
            throw LuaCallException(state, message);
        }
    }

    std::vector<LuaArgument> getCallReturn(int amount = 0) const
    {
        if (amount <= 0) {
            return {};
        }

        std::vector<LuaArgument> result(amount);
        for (int i = 0; i < amount; i++) {
            int luaIndex = -amount + i;
            result[i] = parseArgument(luaIndex);
        }
        return result;
    }

    std::vector<LuaArgument> getCallReturn(const std::list<LuaArgumentType> &types) const
    {
        std::vector<LuaArgument> result(types.size());
        int index = -types.size();
        auto listIterator = types.cbegin();
        auto resultIterator = result.begin();
        for (; lua_type(luaVm, index) != LUA_TNONE && listIterator != types.cend(); index++, listIterator++) {
            *(resultIterator++) = parseArgument(index, *listIterator);
        }
        return result;
    }

    /**
     * @author https://github.com/multitheftauto/mtasa-blue/blob/master/Server/mods/deathmatch/logic/lua/LuaCommon.cpp
     */
    void pushObject(const LuaObject &object) const;

    void captureArguments()
    {
        if (!arguments.empty()) {
            arguments.clear();
        }

        for (int index = 1; lua_type(luaVm, index) != LUA_TNONE; index++) {
            arguments.push_back(parseArgument(index));
        }
    }

    lua_State *luaVm;                               /// Original VM
    bool isArgumentsCaptured = false;               /// Arguments capture flag
    std::vector<LuaArgument> arguments;         /// Captured arguments
};
