#pragma once

#include <vector>
#include <string>
#include <unordered_map>

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


class LuaArgument;

class LuaArgumentHash
{
public:
    size_t operator()(const LuaArgument &k) const;
};

class LuaArgument
{
    friend LuaArgumentHash;
    friend bool operator==(const LuaArgument &, const LuaArgument &);

public:
    using TableListType = std::vector<LuaArgument>;
    using TableMapType = std::unordered_map<LuaArgument, LuaArgument, LuaArgumentHash>;

    enum PointerType
    {
        POINTER_USERDATA = LuaArgumentType::USERDATA,
        POINTER_LIGHTUSERDATA = LuaArgumentType::LIGHTUSERDATA,
    };

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

    explicit LuaArgument(void *value, PointerType type = POINTER_USERDATA)
        :
        value(value),
        type(static_cast<LuaArgumentType>(type))
    {}

    explicit LuaArgument(int value)
        : value(new int(value)), type(LuaArgumentType::INTEGER)
    {}

    explicit LuaArgument(LuaObject value)
        : value(new LuaObject(std::move(value))), type(LuaArgumentType::OBJECT)
    {}

    explicit LuaArgument(const TableListType &value)
        : value(new TableListType(value)), type(LuaArgumentType::TABLE_LIST)
    {}

    explicit LuaArgument(const TableMapType &value)
        : value(new TableMapType(value)),
          type(LuaArgumentType::TABLE_MAP)
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

    // TODO: return reference
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

    TableMapType toMap() const
    {
        if (this->type == LuaArgumentType::TABLE_MAP) {
            return *reinterpret_cast<TableMapType *>(value);
        }
        if (this->type != LuaArgumentType::TABLE_LIST) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::TABLE_LIST, this->type);
        }

        const auto &original = *reinterpret_cast<TableListType *>(value);
        TableMapType result;
        for (int i = 0; i < original.size(); i++) {
            result[LuaArgument(i + 1.)] = original[i];
        }
        return result;
    }

    TableListType toList() const
    {
        if (this->type == LuaArgumentType::TABLE_LIST) {
            return *reinterpret_cast<TableListType *>(value);
        }
        if (this->type != LuaArgumentType::TABLE_MAP) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::TABLE_MAP, this->type);
        }

        const auto &original = *reinterpret_cast<TableMapType *>(value);
        TableListType result(original.size());
        for (int i = 0; i < original.size(); i++) {
            try {
                result[i] = original.at(LuaArgument(i + 1.));
            } catch (const std::out_of_range &) {
                throw LuaCannotTransformArgumentToList();
            }
        }
        return result;
    }

    /**
     * @brief Getter for userdata or lightuserdata
     * @return void pointer
     */
    void *toPointer() const
    {
        if (!(
            this->type == LuaArgumentType::LIGHTUSERDATA
                || this->type == LuaArgumentType::USERDATA
        )) {
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
        if (this->type == LuaArgumentType::OBJECT) {
            return *reinterpret_cast<LuaObject *>(this->value);
        }

        if (!(this->type == LuaArgumentType::USERDATA || this->type == LuaArgumentType::LIGHTUSERDATA)) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LIGHTUSERDATA, this->type);
        }

        // Do not need to clear memory

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

bool operator==(const LuaArgument &left, const LuaArgument &right);
