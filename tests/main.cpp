#include <cstring>

#include "lua/ILuaModuleManager.h"
#include "lua/LuaImports.h"
#include "functions.h"

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

    if (!ImportLua()) {
        return false;
    }

    ms_bInitWorked = true;
    return true;
}


EXTERN_C void RegisterFunctions(lua_State *luaVM)
{
    if (!ms_bInitWorked) {
        return;
    }
    if (!(pModuleManager && luaVM)) {
        return;
    }

    pModuleManager->RegisterFunction(luaVM, "test_simple", TestFunction::simple);
    pModuleManager->RegisterFunction(luaVM, "test_simpleList", TestFunction::simpleList);
    pModuleManager->RegisterFunction(luaVM, "test_echo", TestFunction::echo);
    pModuleManager->RegisterFunction(luaVM, "test_isNumber", TestFunction::isNumber);
    pModuleManager->RegisterFunction(luaVM, "test_isString", TestFunction::isString);
    pModuleManager->RegisterFunction(luaVM, "test_echoElement", TestFunction::echoElement);
    pModuleManager->RegisterFunction(luaVM, "test_strictTypes", TestFunction::strictTypes);
    pModuleManager->RegisterFunction(luaVM, "test_simpleTable", TestFunction::simpleTable);
    pModuleManager->RegisterFunction(luaVM, "test_callGetElementPosition", TestFunction::callGetElementPosition);
    pModuleManager->RegisterFunction(luaVM, "test_call", TestFunction::call);
    pModuleManager->RegisterFunction(luaVM, "test_pushFunction", TestFunction::pushFunction);
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
