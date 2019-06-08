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
    int pushArguments(IT begin, IT end)
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
    void pushArgument(const LuaArgument &argument);

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

    virtual ~LuaVmExtended() = default;

private:

    /**
     * @author https://github.com/multitheftauto/mtasa-blue/blob/master/Server/mods/deathmatch/logic/lua/LuaCommon.cpp
     */
    void pushObject(const LuaObject &object);

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
