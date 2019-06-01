#include "ModuleSdk/Exception.h"

const std::unordered_map<LuaArgumentType, std::string> LuaVmUnexpectedArgumentType::stringify = {
    {LuaArgumentType::NIL,              "Nil"},
    {LuaArgumentType::BOOLEAN,          "Boolean"},
    {LuaArgumentType::LIGHTUSERDATA,    "Light userdata"},
    {LuaArgumentType::NUMBER,           "Number"},
    {LuaArgumentType::STRING,           "String"},
    {LuaArgumentType::TABLE,            "Table"},
    {LuaArgumentType::FUNCTION,         "Function"},
    {LuaArgumentType::USERDATA,         "Userdata"},
    {LuaArgumentType::THREAD,           "Thread"},
    {LuaArgumentType::INTEGER,          "Integer"},
};