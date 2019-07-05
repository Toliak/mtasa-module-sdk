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

/**
 * @brief Provides hash functional for LuaArgument
 */
class LuaArgumentHash
{
public:
    size_t operator()(const LuaArgument &argument) const;
};

// TODO: split to cpp
/**
 * @brief Lua dynamic type object
 *
 */
class LuaArgument
{
    friend LuaArgumentHash;
    friend bool operator==(const LuaArgument &, const LuaArgument &);

public:
    using TableListType = std::vector<LuaArgument>;
    using TableMapType = std::unordered_map<LuaArgument, LuaArgument, LuaArgumentHash>;

    /**
     * @brief Nil constructor
     */
    explicit LuaArgument()
        : value(nullptr), type(LuaArgumentType::LueTypeNil)
    {}

    /**
     * @brief Boolean constructor
     * @param value Initial boolean
     */
    explicit LuaArgument(bool value)
        : value(new bool(value)), type(LuaArgumentType::LuaTypeBoolean)
    {}

    /**
     * @brief Number constructor
     * @param value Initial double
     */
    explicit LuaArgument(double value)
        : value(new double(value)), type(LuaArgumentType::LuaTypeNumber)
    {}

    // TODO: make c-style string constructor

    /**
     * @brief String constructor
     * @param value Initial string
     */
    explicit LuaArgument(std::string value)
        : value(new std::string(std::move(value))), type(LuaArgumentType::LuaTypeString)
    {}

    /// Constructor pointer meaning
    enum PointerType
    {
        POINTER_USERDATA = LuaArgumentType::LuaTypeUserdata,
        POINTER_LIGHTUSERDATA = LuaArgumentType::LuaTypeLightUserdata,
    };

    /**
     * @brief Userdata (or lightuserdata) constructor
     * @param value Initial pointer
     * @param type Pointer meaning
     */
    explicit LuaArgument(void *value, PointerType type = POINTER_USERDATA)
        :
        value(value),
        type(static_cast<LuaArgumentType>(type))
    {}

    /**
     * @brief Integer constructor
     * @param value Initial int
     */
    explicit LuaArgument(int value)
        : value(new int(value)), type(LuaArgumentType::LuaTypeInteger)
    {}

    /**
     * @brief MTASA Object (userdata special case) constructor
     * @param value Initial LuaObject
     */
    explicit LuaArgument(LuaObject value)
        : value(new LuaObject(std::move(value))), type(LuaArgumentType::LuaTypeObject)
    {}

    /**
     * @brief List (table special case) constructor
     * @param value Initial vector of LuaArgument
     */
    explicit LuaArgument(const TableListType &value)
        : value(new TableListType(value)), type(LuaArgumentType::LuaTypeTableList)
    {}

    /**
     * @brief Map (table special case) constructor
     * @param value Initial map of LuaArgument
     */
    explicit LuaArgument(const TableMapType &value)
        : value(new TableMapType(value)), type(LuaArgumentType::LuaTypeTableMap)
    {}

    /**
     * @brief Copy constructor
     */
    LuaArgument(const LuaArgument &argument)
        : type(argument.type), value(nullptr)
    {
        this->copy(argument);
    }

    /**
     * @brief Move constructor
     */
    LuaArgument(LuaArgument &&obj) noexcept
        : value(obj.value), type(obj.type)
    {
        obj.value = nullptr;
        obj.type = LuaArgumentType::LueTypeNil;
    }

    /**
     * @brief Copy assignment
     */
    LuaArgument &operator=(const LuaArgument &argument)
    {
        this->destroy();
        this->copy(argument);

        return *this;
    }

    LuaArgument &operator=(LuaArgument &&argument) noexcept
    {
        // Move value and type
        this->value = argument.value;
        this->type = argument.type;

        // TODO: make clear method
        // Clear
        argument.value = nullptr;
        argument.type = LuaArgumentType::LueTypeNil;

        return *this;
    }

    // TODO: return reference
    /**
     * @brief Boolean getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(bool, LuaArgumentType::LuaTypeBoolean, Bool)
        return *reinterpret_cast<bool *>(value);
    }

    /**
     * @brief Number getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(double, LuaArgumentType::LuaTypeNumber, Number)
        return *reinterpret_cast<double *>(value);
    }

    /**
     * @brief Integer getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(int, LuaArgumentType::LuaTypeInteger, Integer)
        return *reinterpret_cast<int *>(value);
    }

    /**
     * @brief String getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(std::string, LuaArgumentType::LuaTypeString, String)
        return *reinterpret_cast<std::string *>(value);
    }

    /**
     * @brief MTASA Object getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(LuaObject, LuaArgumentType::LuaTypeObject, Object)
        return *reinterpret_cast<LuaObject *>(value);
    }

    /**
     * @brief Map getter
     * @throws LuaUnexpectedArgumentType Type mismatch (expected TABLE_MAP or TABLE_LIST)
     * @return Result
     */
    TableMapType toMap() const
    {
        if (this->type == LuaArgumentType::LuaTypeTableMap) {
            return *reinterpret_cast<TableMapType *>(value);
        }
        if (this->type != LuaArgumentType::LuaTypeTableList) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LuaTypeTableList, this->type);
        }

        const auto &original = *reinterpret_cast<TableListType *>(value);
        TableMapType result;
        for (int i = 0; i < original.size(); i++) {
            result[LuaArgument(i + 1.)] = original[i];
        }
        return result;
    }

    /**
     * @brief Map getter
     * @throws LuaUnexpectedArgumentType Type mismatch (expected TABLE_MAP or TABLE_LIST)
     * @throws LuaCannotTransformArgumentToList
     * @return Result
     */
    TableListType toList() const
    {
        if (this->type == LuaArgumentType::LuaTypeTableList) {
            return *reinterpret_cast<TableListType *>(value);
        }
        if (this->type != LuaArgumentType::LuaTypeTableMap) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LuaTypeTableMap, this->type);
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
     * @brief Pointer getter
     * @throws LuaUnexpectedArgumentType Type mismatch (expected USERDATA or LIGHTUSERDATA)
     * @return void pointer
     */
    void *toPointer() const
    {
        if (!(
            this->type == LuaArgumentType::LuaTypeLightUserdata
                || this->type == LuaArgumentType::LuaTypeUserdata
        )) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LuaTypeLightUserdata, this->type);
        }

        return value;
    }

    /**
     * @brief Is object nil
     * @return true, if current object is nil
     */
    bool isNil() const
    {
        return this->type == LuaArgumentType::LueTypeNil;
    }

    /**
     * @brief Transforms userdata to MTASA Object
     * @throws LuaUnexpectedArgumentType Type mismatch (expected USERDATA, LIGHTUSERDATA or OBJECT)
     * @param stringClass Lua MTASA object's class (may be empty to autodetect)
     * @return Result MTASA Object reference
     */
    LuaObject &extractObject(const std::string &stringClass = "")
    {
        if (this->type == LuaArgumentType::LuaTypeObject) {
            return *reinterpret_cast<LuaObject *>(this->value);
        }

        if (!(this->type == LuaArgumentType::LuaTypeUserdata || this->type == LuaArgumentType::LuaTypeLightUserdata)) {
            throw LuaUnexpectedArgumentType(LuaArgumentType::LuaTypeLightUserdata, this->type);
        }

        // Do not need to clear memory

        ObjectId id(*reinterpret_cast<unsigned long *>(this->value));
        this->value = new LuaObject(
            id,
            stringClass
        );
        this->type = LuaArgumentType::LuaTypeObject;

        return *reinterpret_cast<LuaObject *>(this->value);
    }

    /**
     * @brief Type getter
     * @return Object's type
     */
    LuaArgumentType getType() const
    {
        return type;
    }

    /**
     * @brief Destructor
     */
    ~LuaArgument()
    {
        destroy();
    }

private:

    // TODO: make move method

    virtual void copy(const LuaArgument &argument);
    virtual void destroy() noexcept;

    void *value;                                ///< Pointer to allocated value (unknown type)
    LuaArgumentType type;                       ///< Object's type
};

bool operator==(const LuaArgument &left, const LuaArgument &right);
