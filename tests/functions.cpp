#include "functions.h"

namespace TestFunction
{

int test1(lua_State *luaVm)
{
    LuaVmExtended lua(luaVm);
    lua.pushArgument(
        LuaArgument(std::string("Yes!"))
    );

    return 1;
}

int test2(lua_State *)
{
    return 0;
}

}

