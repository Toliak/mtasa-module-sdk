#pragma once

#include <string>
#include <unordered_map>
#include <exception>

#include "LuaArgumentType.h"

static const std::unordered_map<LuaArgumentType, std::string> STRING_TYPE = {
    {LuaArgumentType::NIL, "Nil"},
    {LuaArgumentType::BOOLEAN, "Boolean"},
    {LuaArgumentType::LIGHTUSERDATA, "Light userdata"},
    {LuaArgumentType::NUMBER, "Number"},
    {LuaArgumentType::STRING, "String"},
    {LuaArgumentType::TABLE, "Table"},
    {LuaArgumentType::FUNCTION, "Function"},
    {LuaArgumentType::USERDATA, "Userdata"},
    {LuaArgumentType::THREAD, "Thread"},
    {LuaArgumentType::INTEGER, "Integer"},
};

class LuaException: public std::exception
{
public:
    const char *what() const noexcept override = 0;

    ~LuaException() override = default;
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
    LuaUnexpectedType() = default;

    LuaUnexpectedType(LuaArgumentType expectedType, LuaArgumentType receivedType)
        : expectedType(expectedType),
          receivedType(receivedType),
          typesInfo(true)
    {}

    LuaUnexpectedType(LuaArgumentType expectedType, LuaArgumentType receivedType, int index)
        : index(index),
          expectedType(expectedType),
          receivedType(receivedType),
          typesInfo(true),
          indexInfo(true)
    {}

    const char *what() const noexcept override
    {
        if (!typesInfo) {
            return "Unexpected argument type";
        }

        if (!indexInfo) {
            std::string result =
                "Expected " + STRING_TYPE.at(this->expectedType) + ", got " + STRING_TYPE.at(this->receivedType);
            return result.c_str();
        }

        std::string result =
            "Expected " + STRING_TYPE.at(this->expectedType) + ", got " + STRING_TYPE.at(this->receivedType)
                + " at argument " + std::to_string(index);
        return result.c_str();
    }

    ~LuaUnexpectedType() override = default;

private:
    bool typesInfo = false;
    bool indexInfo = false;
    int index = 0;
    LuaArgumentType expectedType = LuaArgumentType::NIL;
    LuaArgumentType receivedType = LuaArgumentType::NIL;
};

class LuaArgumentException: public LuaException
{
public:
    const char *what() const noexcept override = 0;

    ~LuaArgumentException() override = default;
};

class LuaUnexpectedArgumentType: public LuaArgumentException
{
public:
    LuaUnexpectedArgumentType() = delete;

    explicit LuaUnexpectedArgumentType(LuaArgumentType expectedType)
        : expectedType(expectedType)
    {}

    const char *what() const noexcept override
    {
        std::string result = "Expected " + STRING_TYPE.at(this->expectedType);
        return result.c_str();
    }

    ~LuaUnexpectedArgumentType() override = default;

private:
    LuaArgumentType expectedType;
};

