#include "ModuleSdk/Exception.h"

void LuaException::move(LuaException &&luaException) noexcept
{
    this->message = luaException.message;
    luaException.message = nullptr;
}

void LuaException::copy(const LuaException &luaException)
{
    if (!luaException.what()) {
        return;
    }

    destroy();
    this->message = new char[std::strlen(luaException.message)];
    std::strcpy(this->message, luaException.message);
}

void LuaException::destroy() noexcept
{
    delete[] message;
}
