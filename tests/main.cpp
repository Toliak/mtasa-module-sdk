#include "functions.h"
#include "lua/ILuaModuleManager.h"
#include "lua/LuaImports.h"
#include <cstring>
#include <numeric>

#define MODULE_NAME "ModuleSdkTest"
#define MODULE_AUTHOR "Toliak"
#define MODULE_VERSION 1.0f

ILuaModuleManager10 *pModuleManager = nullptr;          ///< Global module manager

bool ms_bInitWorked = false;                            ///< Initialization flag

// Initialisation function (module entrypoint)
EXTERN_C bool InitModule(ILuaModuleManager10 *pManager, char *szModuleName, char *szAuthor, float *fVersion)
{
    pModuleManager = pManager;

    // Set the module info
    strncpy(szModuleName, MODULE_NAME, MAX_INFO_LENGTH);
    strncpy(szAuthor, MODULE_AUTHOR, MAX_INFO_LENGTH);
    (*fVersion) = MODULE_VERSION;

#ifndef WIN32
    if (!ImportLua()) {
        return false;
    }
#endif

    ms_bInitWorked = true;
    return true;
}


EXTERN_C void RegisterFunctions(lua_State *luaVm)
{
    if (!ms_bInitWorked) {
        return;
    }
    if (!(pModuleManager && luaVm)) {
        return;
    }

    pModuleManager->RegisterFunction(
        luaVm,
        "test_dev_status",
        [](lua_State *luaVm) -> int
        {
            std::string result;
            result += "Function amount: " + std::to_string(TestFunction::allFunctions.size()) + "\n";

            result += std::accumulate(
                TestFunction::allFunctions.cbegin(),
                TestFunction::allFunctions.cend(),
                std::string("Functions: \n"),
                [](std::string old, const auto &pair)
                {
                    return std::move(old) + "\t" + pair.first + "\n";
                }
            );

            lua_pushstring(luaVm, result.c_str());
            return 1;
        }
    );

    for (const auto &pair : TestFunction::allFunctions) {
        pModuleManager->RegisterFunction(
            luaVm,
            ("test_" + pair.first).c_str(),
            pair.second
        );
    }
}

EXTERN_C bool DoPulse()
{
    // TODO: Understand, what is it
    return true;
}


EXTERN_C void ResourceStopped(lua_State *)
{

}


EXTERN_C bool ShutdownModule(void)
{
    // Global destructor

    return true;
}
