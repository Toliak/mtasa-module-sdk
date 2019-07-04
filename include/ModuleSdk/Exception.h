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
    {LuaArgumentType::TABLE_LIST, "Table list"},
    {LuaArgumentType::TABLE_MAP, "Table map"},
};      ///< Readable type names

/**
 * @brief Base Lua exception
 */
// TODO: split to cpp
class LuaException: public std::exception
{
    char *message = nullptr;
    // TODO: make default message field

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

    // TODO: make move method

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

/**
 * @brief Unsupported type have been captured
 */
class LuaBadType: public LuaException
{
public:
    explicit LuaBadType()
    {
        this->setMessage("Bad argument type");
    }

    explicit LuaBadType(int typeCode)
    {
        this->setMessage("Bad argument type. Type code: " + std::to_string(typeCode));
    }

    ~LuaBadType() override = default;
};

/**
 * @brief Expected another type (on parse)
 */
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

/**
 * @brief Expected another type (on push)
 */
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

/**
 * @brief Function call failed
 */
class LuaCallException: public LuaException
{
public:
    using LuaException::LuaException;

    explicit LuaCallException(int errorId, const std::string &message = "")
    {
        this->setMessage("Error code: " + std::to_string(errorId) + ". Message: " + message);
    }
};

/**
 * @brief Base exception for LuaArgument
 */
class LuaArgumentException: public LuaException
{
public:
    using LuaException::LuaException;

    ~LuaArgumentException() override = default;
};

/**
 * @brief Expected another type in LuaArgument method
 */
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

/**
 * @brief Cannot transform LuaArgument to table list
 */
class LuaCannotTransformArgumentToList: public LuaArgumentException
{
public:
    using LuaArgumentException::LuaArgumentException;

    const char *what() const noexcept override
    {
        return "Cannot transform to list";
    }

    ~LuaCannotTransformArgumentToList() override = default;
};

