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
// TODO: split to cpp
class LuaVmExtended
{
public:
    /**
     * @brief Constructor
     * @param luaVm Lua VM pointer
     */
    explicit LuaVmExtended(lua_State *luaVm)
        : luaVm(luaVm)
    {}

    /**
     * @brief Parse all arguments from lua VM (types autodetect)
     * @details Caches captured arguments
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
     * @throws LuaUnexpectedType Type mismatch
     * @throws LuaBadType Captured type is not supported
     * @throws LuaOutOfRange Arguments amount less than expected
     * @return LuaArgument vector
     */
    std::vector<LuaArgument> getArguments(const std::list<LuaArgumentType> &types) const;

    /**
     * @brief Push arguments to lua VM
     * @tparam IT Iterator type
     * @param begin Begin iterator
     * @param end End iterator
     * @throws LuaUnexpectedPushType Passed argument type is not supported
     * @return Size of pushed list
     */
    template<
        typename IT,
        typename = std::enable_if<
            std::is_same<LuaArgument, typename std::iterator_traits<IT>::value_type>::value
        >
    >
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
     * @throws LuaUnexpectedPushType Passed argument type is not supported
     */
    void pushArgument(const LuaArgument &argument) const;

    /**
    * @brief Parse argument from lua VM (auto type detecting)
    * @param index Argument index
    * @throws LuaBadType Bad type has been captured
    * @return Parsed argument
    */
    LuaArgument parseArgument(int index) const;

    /**
     * @brief Parse argument from lua VM (manual type detecting)
     * @param index Argument index
     * @param type Argument type
     * @param force Check type compatibility
     * @throws LuaBadType Bad type has been captured
     * @throws LuaUnexpectedType Unexpected type has been captured
     * @return Parsed argument
     */
    LuaArgument parseArgument(int index, LuaArgumentType type, bool force = false) const;

    /**
     * @brief Clears lua VM stack
     */
    void clearStack() const
    {
        lua_settop(luaVm, 0);
    }

    /**
     * @brief Call lua function and capture return values
     * @param function Function name
     * @param functionArgs Arguments
     * @param returnSize Return values amount
     * @throws LuaUnexpectedPushType Passed argument type is not supported
     * @throws LuaCallException Error during function execution
     * @throws LuaBadType Bad type has been captured
     * @return Function result
     */
    // TODO: Make functionArgs param iterators
    // TODO: Make return values expected types
    std::vector<LuaArgument>
    call(const std::string &function, const std::list<LuaArgument> &functionArgs, int returnSize = 0) const
    {
        // TODO: Check lua local function calling
        callFunction(function, functionArgs, returnSize);
        return getCallReturn(returnSize);
    }

    virtual ~LuaVmExtended() = default;

private:

    /**
     * @brief Call lua function
     * @throws LuaUnexpectedPushType Passed argument type is not supported
     * @throws LuaCallException Error during function execution
     * @param function Function name
     * @param functionArgs Function arguments
     * @param returnSize Return values amount
     */
    void callFunction(const std::string &function, const std::list<LuaArgument> &functionArgs, int returnSize) const
    {
        lua_pushstring(luaVm, function.c_str());
        lua_gettable(luaVm, LUA_GLOBALSINDEX);
        this->pushArguments(functionArgs.cbegin(), functionArgs.cend());

        int state = lua_pcall(
            luaVm,
            static_cast<int>(functionArgs.size()),
            returnSize,
            0
        );

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

    /**
     * @brief Get called function return values
     * @param amount Values amount
     * @throws LuaBadType Bad type has been captured
     * @return Result values vector
     */
    std::vector<LuaArgument> getCallReturn(int amount = 0) const
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

    /**
     * @brief Get called function return values ()
     * @param types List of argument types
     * @throws LuaUnexpectedType Unexpected type has been captured
     * @throws LuaBadType Bad type has been captured
     * @return Result values vector
     */
    std::vector<LuaArgument> getCallReturn(const std::list<LuaArgumentType> &types) const
    {
        std::vector<LuaArgument> result(types.size());
        auto index = -static_cast<int>(types.size());
        auto listIterator = types.cbegin();
        auto resultIterator = result.begin();
        for (; lua_type(luaVm, index) != LUA_TNONE && listIterator != types.cend(); index++, listIterator++) {
            *(resultIterator++) = parseArgument(index, *listIterator);
        }
        return result;
    }

    /**
     * @brief Push MTASA object
     * @author https://github.com/multitheftauto/mtasa-blue/blob/master/Server/mods/deathmatch/logic/lua/LuaCommon.cpp
     */
    void pushObject(const LuaObject &object) const;

    /**
     * @brief Push transformable to table-list LuaArgument
     */
    void pushTableList(const LuaArgument &argument) const
    {
        const std::vector<LuaArgument> &list = argument.toList();

        lua_createtable(luaVm, list.size(), 0);
        for (int i = 0; i < list.size(); i++) {
            this->pushArgument(list[i]);
            lua_rawseti(luaVm, -2, i + 1);
        }
    }

    /**
     * @brief Push transformable to table-list LuaArgument
     */
    void pushTableMap(const LuaArgument &argument) const
    {
        const auto &map = argument.toMap();

        lua_createtable(luaVm, 0, map.size());
        for (const auto &pair : map) {
            this->pushArgument(pair.first);
            this->pushArgument(pair.second);
            lua_rawset(luaVm, -3);
        }
    }

    /**
     * Capture arguments (type autodetect)
     */
    void captureArguments()
    {
        if (!arguments.empty()) {
            arguments.clear();
        }

        for (int index = 1; lua_type(luaVm, index) != LUA_TNONE; index++) {
            arguments.push_back(parseArgument(index));
        }
    }

    lua_State *luaVm;                               ///< Original VM
    bool isArgumentsCaptured = false;               ///< Arguments capture flag
    std::vector<LuaArgument> arguments;             ///< Captured arguments
};
