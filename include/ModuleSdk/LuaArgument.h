#pragma once

#include <string>

#include "lua/lua.h"

#include "Exception.h"
#include "LuaArgumentType.h"
#include "LuaObject.h"

#define LUA_VM_ARGUMENT_GET_FUNCTION(templateType, check, typeName) \
templateType to##typeName() const    \
{                       \
    if (this->type != (check)) {  \
        throw LuaUnexpectedArgumentType(check, this->type); \
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

    explicit LuaArgument(LuaObject value)
        : value(new LuaObject(std::move(value))), type(LuaArgumentType::OBJECT)
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

    LUA_VM_ARGUMENT_GET_FUNCTION(LuaObject, LuaArgumentType::OBJECT, Object)
        return *reinterpret_cast<LuaObject *>(value);
    }

    /**
     * @brief Getter for userdata or lightuserdata
     * @return void pointer
     */
    void *toPointer() const
    {
        if (!(this->type == LuaArgumentType::LIGHTUSERDATA || this->type == LuaArgumentType::USERDATA)) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LIGHTUSERDATA, this->type);
        }

        return value;
    }

    bool isNil() const
    {
        return this->type == LuaArgumentType::NIL;
    }

    LuaObject &extractObject(const std::string &stringClass = "")
    {
        if (!(this->type == LuaArgumentType::USERDATA || this->type == LuaArgumentType::LIGHTUSERDATA)) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LIGHTUSERDATA, this->type);
        }

        this->value = new LuaObject(
            ObjectId(*reinterpret_cast<unsigned long *>(this->value)),
            stringClass
        );
        this->type = LuaArgumentType::OBJECT;

        return *reinterpret_cast<LuaObject *>(this->value);
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
    virtual void copy(const LuaArgument &argument);

    virtual void destroy() noexcept;

    void *value;
    LuaArgumentType type;
};