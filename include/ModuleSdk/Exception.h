#pragma once

#include <string>
#include <unordered_map>
#include <exception>
#include <cstring>

#include "LuaArgumentType.h"

static const std::unordered_map<LuaArgumentType, std::string> STRING_TYPE = {
    {LuaArgumentType::NIL, "Nil"},
    {LuaArgumentType::BOOLEAN, "Boolean"},
    {LuaArgumentType::LIGHTUSERDATA, "Light userdata"},
    {LuaArgumentType::NUMBER, "Number"},
    {LuaArgumentType::STRING, "String"},
    {LuaArgumentType::USERDATA, "Userdata"},
    {LuaArgumentType::INTEGER, "Integer"},
    {LuaArgumentType::OBJECT, "Object"},
};

class LuaException: public std::exception
{
    char *message = nullptr;

protected:
    virtual void copy(const LuaException &luaException) noexcept
    {
        if (!luaException.what()) {
            return;
        }

        destroy();
        this->message = new char[std::strlen(luaException.message)];
        std::strcpy(this->message, luaException.message);
    }

    virtual void destroy() noexcept
    {
        delete[] message;
    }

public:
    LuaException() = default;

    LuaException(const LuaException &luaException) noexcept
    {
        destroy();
        copy(luaException);
    }

    LuaException(LuaException &&luaException) noexcept
    {
        this->message = luaException.message;
        luaException.message = nullptr;
    }

    LuaException &operator=(const LuaException &luaException)
    {
        destroy();
        copy(luaException);
        return *this;
    }

    LuaException &operator=(LuaException &&luaException) noexcept
    {
        this->message = luaException.message;
        luaException.message = nullptr;
        return *this;
    }

    void setMessage(const std::string &newMessage) noexcept
    {
        destroy();
        this->message = new char[newMessage.size()];
        std::strcpy(this->message, newMessage.c_str());
    }

    const char *what() const noexcept override
    {
        return this->message;
    }

    ~LuaException() override
    {
        destroy();
    }
};

class LuaBadType: public LuaException
{
public:
    const char *what() const noexcept override
    {
        return "Bad argument type";
    }

    ~LuaBadType() override = default;
};

class LuaUnexpectedType: public LuaException
{
public:
    using LuaException::LuaException;

    explicit LuaUnexpectedType(LuaArgumentType expectedType)
    {
        this->setMessage(
            "Expected " + STRING_TYPE.at(expectedType)
        );
    }

    LuaUnexpectedType(LuaArgumentType expectedType, LuaArgumentType receivedType)
    {
        this->setMessage(
            "Expected " + STRING_TYPE.at(expectedType) + ", got " + STRING_TYPE.at(receivedType)
        );
    }

    LuaUnexpectedType(LuaArgumentType expectedType, LuaArgumentType receivedType, int index)
    {
        this->setMessage(
            "Expected " + STRING_TYPE.at(expectedType) + ", got " + STRING_TYPE.at(receivedType)
                + " at argument " + std::to_string(index)
        );
    }

    const char *what() const noexcept override
    {
        const char *message = LuaException::what();

        if (!message) {
            return "Unexpected argument type";
        }
        return message;
    }

    ~LuaUnexpectedType() override = default;
};

class LuaUnexpectedPushType: public LuaException
{
public:
    using LuaException::LuaException;

    explicit LuaUnexpectedPushType(LuaArgumentType receivedType)
    {
        this->setMessage(
            "Got unexpected type " + STRING_TYPE.at(receivedType)
        );
    }

    ~LuaUnexpectedPushType() override = default;
};

class LuaArgumentException: public LuaException
{
public:
    using LuaException::LuaException;

    ~LuaArgumentException() override = default;
};

class LuaUnexpectedArgumentType: public LuaArgumentException
{
public:
    using LuaArgumentException::LuaArgumentException;

    LuaUnexpectedArgumentType() = delete;

    explicit LuaUnexpectedArgumentType(LuaArgumentType expectedType, LuaArgumentType recievedType)
    {
        this->setMessage("Expected " + STRING_TYPE.at(expectedType) + ", got " + STRING_TYPE.at(recievedType));
    }

    ~LuaUnexpectedArgumentType() override = default;
};

class LuaCallException: public LuaException
{
public:
    using LuaException::LuaException;

    explicit LuaCallException(int errorId, const std::string &message = "")
    {
        this->setMessage("Error code: " + std::to_string(errorId) + ". Message: " + message);
    }
};

