#pragma once

#include <string>

#include "lua/lua.h"

#include "Exception.h"
#include "LuaArgumentType.h"

#define LUA_VM_ARGUMENT_GET_FUNCTION(templateType, check, typeName) \
templateType to##typeName() const    \
{                       \
    if (this->type != (check)) {  \
        throw LuaUnexpectedArgumentType(check); \
    }


class LuaArgument
{
public:
    explicit LuaArgument()
        : value(nullptr), type(LuaArgumentType::NIL)
    {}

    explicit LuaArgument(bool value)
        : value(new bool(value)), type(LuaArgumentType::BOOLEAN)
    {}

    explicit LuaArgument(double value)
        : value(new double(value)), type(LuaArgumentType::NUMBER)
    {}

    explicit LuaArgument(std::string value)
        : value(new std::string(std::move(value))), type(LuaArgumentType::STRING)
    {}

    explicit LuaArgument(void *value, bool isLight = false)
        :
        value(value),
        type(isLight ? LuaArgumentType::LIGHTUSERDATA : LuaArgumentType::USERDATA)
    {}

    explicit LuaArgument(int value)
        : value(new int(value)), type(LuaArgumentType::INTEGER)
    {}

    LuaArgument(const LuaArgument &argument)
        : type(argument.type), value(nullptr)
    {
        this->copy(argument);
    }

    LuaArgument(LuaArgument &&obj) noexcept
        : value(obj.value), type(obj.type)
    {
        obj.value = nullptr;
        obj.type = LuaArgumentType::NIL;
    }

    LuaArgument &operator=(const LuaArgument &argument)
    {
        this->destroy();            // Destruct old value
        this->copy(argument);       // Copy new value

        return *this;
    }

    LuaArgument &operator=(LuaArgument &&argument) noexcept
    {
        this->value = argument.value;
        this->type = argument.type;

        argument.value = nullptr;
        argument.type = LuaArgumentType::NIL;

        return *this;
    }

    LUA_VM_ARGUMENT_GET_FUNCTION(bool, LuaArgumentType::BOOLEAN, Bool)
        return *reinterpret_cast<bool *>(value);
    }

    LUA_VM_ARGUMENT_GET_FUNCTION(double, LuaArgumentType::NUMBER, Number)
        return *reinterpret_cast<double *>(value);
    }

    LUA_VM_ARGUMENT_GET_FUNCTION(int, LuaArgumentType::INTEGER, Integer)
        return *reinterpret_cast<int *>(value);
    }

    LUA_VM_ARGUMENT_GET_FUNCTION(std::string, LuaArgumentType::STRING, String)
        return *reinterpret_cast<std::string *>(value);
    }

    /**
     * @brief Getter for userdata or lightuserdata
     * @return void pointer
     */
    void *toPointer() const
    {
        if (!(this->type == LuaArgumentType::LIGHTUSERDATA || this->type == LuaArgumentType::USERDATA)) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LIGHTUSERDATA);
        }

        return value;
    }

    bool isNil() const
    {
        return this->type == LuaArgumentType::NIL;
    }

    LuaArgumentType getType() const
    {
        return type;
    }

    ~LuaArgument()
    {
        destroy();
    }

private:
    void copy(const LuaArgument &argument)
    {
        if (!argument.value) {
            this->type = LuaArgumentType::NIL;
            this->value = nullptr;
            return;
        }

        this->type = argument.type;
        if (type == LuaArgumentType::BOOLEAN) {
            this->value = new bool(*reinterpret_cast<bool *>(argument.value));
        } else if (type == LuaArgumentType::NUMBER) {
            this->value = new double(*reinterpret_cast<double *>(argument.value));
        } else if (type == LuaArgumentType::INTEGER) {
            this->value = new int(*reinterpret_cast<int *>(argument.value));
        } else if (type == LuaArgumentType::STRING) {
            this->value = new std::string(*reinterpret_cast<std::string *>(argument.value));
        } else if (type == LuaArgumentType::LIGHTUSERDATA || type == LuaArgumentType::USERDATA) {
            this->value = argument.value;
        } else {
            this->value = nullptr;
        }
    }

    void destroy() noexcept
    {
        if (!this->value) {
            return;
        }

        if (type == LuaArgumentType::BOOLEAN) {
            delete reinterpret_cast<bool *>(value);
        } else if (type == LuaArgumentType::NUMBER) {
            delete reinterpret_cast<double *>(value);
        } else if (type == LuaArgumentType::INTEGER) {
            delete reinterpret_cast<int *>(value);
        } else if (type == LuaArgumentType::STRING) {
            delete reinterpret_cast<std::string *>(value);
        }
    }

    void *value;
    LuaArgumentType type;
};