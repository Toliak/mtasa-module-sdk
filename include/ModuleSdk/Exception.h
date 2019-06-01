#pragma once

#include <string>
#include <unordered_map>
#include <exception>

#include "LuaArgumentType.h"


class LuaVmException: public std::exception
{
public:
    const char *what() const noexcept override = 0;

    ~LuaVmException() override = default;
};


class LuaVmBadType: public LuaVmException
{
public:
    const char *what() const noexcept override
    {
        return "Bad argument type";
    }

    ~LuaVmBadType() override = default;
};

class LuaVmUnexpectedType: public LuaVmException
{
public:
    const char *what() const noexcept override
    {
        return "Unexpected argument type";
    }

    ~LuaVmUnexpectedType() override = default;
};


class LuaVmArgumentException: public LuaVmException
{
public:
    const char *what() const noexcept override = 0;

    ~LuaVmArgumentException() override = default;
};


class LuaVmUnexpectedArgumentType: public LuaVmArgumentException
{
public:
    LuaVmUnexpectedArgumentType() = delete;

    explicit LuaVmUnexpectedArgumentType(LuaArgumentType expectedType)
        : expectedType(expectedType)
    {}

    const char *what() const noexcept override
    {
        std::string result = "Expected " + stringify.at(this->expectedType);
        return result.c_str();
    }

    ~LuaVmUnexpectedArgumentType() override = default;

private:
    LuaArgumentType expectedType;

    static const std::unordered_map<LuaArgumentType, std::string> stringify;
};

