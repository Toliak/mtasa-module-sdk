#pragma once

#include "LuaArgument.h"
#include "lua/lua.h"
#include <list>
#include <unordered_map>
#include <vector>


/**
 * @brief Extends lua_State functional
 */
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
     * @return LuaArgument vector
     */
    std::vector<LuaArgument> getArguments();

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
    call(const std::string &function, const std::list<LuaArgument> &functionArgs, int returnSize = 0) const;

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
    void callFunction(const std::string &function, const std::list<LuaArgument> &functionArgs, int returnSize) const;

    /**
     * @brief Get called function return values
     * @param amount Values amount
     * @throws LuaBadType Bad type has been captured
     * @return Result values vector
     */
    std::vector<LuaArgument> getCallReturn(int amount = 0) const;

    /**
     * @brief Get called function return values ()
     * @param types List of argument types
     * @throws LuaUnexpectedType Unexpected type has been captured
     * @throws LuaBadType Bad type has been captured
     * @return Result values vector
     */
    std::vector<LuaArgument> getCallReturn(const std::list<LuaArgumentType> &types) const;

    /**
     * @brief Push MTASA object to stack
     * @author https://github.com/multitheftauto/mtasa-blue/blob/master/Server/mods/deathmatch/logic/lua/LuaCommon.cpp
     */
    void pushObject(const LuaObject &object) const;

    /**
     * @brief Push transformable to table-list LuaArgument
     */
    void pushTableList(const LuaArgument &argument) const;

    /**
     * @brief Push transformable to table-list LuaArgument
     */
    void pushTableMap(const LuaArgument &argument) const;

    lua_State *luaVm;                               ///< Original VM
};
