#pragma once

#include <string>
#include <unordered_map>
#include <exception>
#include <cstring>

#include "LuaArgumentType.h"

static const std::unordered_map<LuaArgumentType, std::string> STRING_TYPE = {
    {LuaArgumentType::LuaTypeNil, "Nil"},
    {LuaArgumentType::LuaTypeBoolean, "Boolean"},
    {LuaArgumentType::LuaTypeLightUserdata, "Light userdata"},
    {LuaArgumentType::LuaTypeNumber, "Number"},
    {LuaArgumentType::LuaTypeString, "String"},
    {LuaArgumentType::LuaTypeUserdata, "Userdata"},
    {LuaArgumentType::LuaTypeInteger, "Integer"},
    {LuaArgumentType::LuaTypeObject, "Object"},
    {LuaArgumentType::LuaTypeTableList, "Table list"},
    {LuaArgumentType::LuaTypeTableMap, "Table map"},
};      ///< Readable type names

/**
 * @brief Base Lua exception
 */
class LuaException: public std::exception
{
    char *message = nullptr;

protected:
    virtual void copy(const LuaException &luaException) noexcept;
    virtual void destroy() noexcept;

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

    virtual const char *getMessageDefault() const
    {
        return nullptr;
    }

    char *getMessage() const
    {
        return message;
    }

    void setMessage(const std::string &newMessage) noexcept
    {
        destroy();
        this->message = new char[newMessage.size()];
        std::strcpy(this->message, newMessage.c_str());
    }

    const char *what() const noexcept override
    {
        if (this->message) {
            return this->message;
        }

        if (this->getMessageDefault()) {
            return this->getMessageDefault();
        }

        return nullptr;
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
private:
    const char *messageDefault = "Bad type";

public:
    using LuaException::LuaException;

    const char *getMessageDefault() const override
    {
        return messageDefault;
    }

    explicit LuaBadType(int typeCode)
    {
        this->setMessage("Bad type. Type code: " + std::to_string(typeCode));
    }

    ~LuaBadType() override = default;
};

/**
 * @brief Expected another type (on parse)
 */
class LuaUnexpectedType: public LuaException
{
private:
    const char *messageDefault = "Unexpected type";

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

    const char *getMessageDefault() const override
    {
        return messageDefault;
    }

    ~LuaUnexpectedType() override = default;
};

/**
 * @brief Expected another type (on push)
 */
class LuaUnexpectedPushType: public LuaException
{
private:
    const char *messageDefault = "Unexpected push type";

public:
    using LuaException::LuaException;

    explicit LuaUnexpectedPushType(LuaArgumentType receivedType)
    {
        this->setMessage(
            "Got unexpected type " + STRING_TYPE.at(receivedType)
        );
    }

    const char *getMessageDefault() const override
    {
        return messageDefault;
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
 * @brief Out of range exception
 */
class LuaOutOfRange: public LuaException
{
private:
    const char *messageDefault = "Out of range";

public:
    using LuaException::LuaException;

    explicit LuaOutOfRange(const std::string &message)
    {
        this->setMessage(message);
    }

    const char *getMessageDefault() const override
    {
        return this->messageDefault;
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
private:
    const char *messageDefault = "Cannot transform to list";

public:
    using LuaArgumentException::LuaArgumentException;

    ~LuaCannotTransformArgumentToList() override = default;

    const char *getMessageDefault() const override
    {
        return messageDefault;
    }
};

