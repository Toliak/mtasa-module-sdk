#include "ModuleSdk/Exception.h"

void LuaException::copy(const LuaException &luaException) noexcept
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

const char *LuaUnexpectedType::what() const noexcept
{
    const char *message = LuaException::what();

    if (!message) {
        return "Unexpected argument type";
    }
    return message;
}
