#pragma once

#include "Exception.h"
#include "LuaArgumentType.h"
#include "LuaObject.h"
#include "lua/lua.h"
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


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

/**
 * @brief Lua dynamic type object
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
    LuaArgument() = default;

    /**
     * @brief Boolean constructor
     * @param value Initial boolean
     */
    LuaArgument(bool valueBool)
        : value(new bool(valueBool)), type(LuaArgumentType::LuaTypeBoolean)
    {}

    /**
     * @brief Number constructor
     * @param value Initial double
     */
    LuaArgument(double valueDouble)
        : value(new double(valueDouble)), type(LuaArgumentType::LuaTypeNumber)
    {}

    /**
     * @brief String constructor
     * @param value Initial string
     */
    LuaArgument(std::string valueString)
        : value(new std::string(std::move(valueString))), type(LuaArgumentType::LuaTypeString)
    {}

    /**
     * @brief const char * constructor
     * @param value C-style string
     */
    LuaArgument(const char *valueStringC)
        : value(new std::string(valueStringC)), type(LuaArgumentType::LuaTypeString)
    {}

    /// Constructor pointer meaning
    enum PointerType
    {
        PointerUserdata = LuaArgumentType::LuaTypeUserdata,
        PointerLightuserdata = LuaArgumentType::LuaTypeLightUserdata,
    };

    /**
     * @brief Userdata (or lightuserdata) constructor
     * @param value Initial pointer
     * @param type Pointer meaning
     */
    LuaArgument(void *valuePointer, PointerType type = PointerUserdata)
        : value(valuePointer), type(static_cast<LuaArgumentType>(type))
    {}

    /**
     * @brief Integer constructor
     * @param value Initial int
     */
    LuaArgument(int valueInt)
        : value(new int(valueInt)), type(LuaArgumentType::LuaTypeInteger)
    {}

    /**
     * @brief MTASA Object (userdata special case) constructor
     * @param value Initial LuaObject
     */
    LuaArgument(LuaObject valueObject)
        : value(new LuaObject(std::move(valueObject))), type(LuaArgumentType::LuaTypeObject)
    {}

    /**
     * @brief List (table special case) constructor
     * @param value Initial vector of LuaArgument
     */
    LuaArgument(TableListType valueList)
        : value(new TableListType(std::move(valueList))), type(LuaArgumentType::LuaTypeTableList)
    {}

    /**
     * @brief Map (table special case) constructor
     * @param value Initial map of LuaArgument
     */
    LuaArgument(TableMapType valueMap)
        : value(new TableMapType(std::move(valueMap))), type(LuaArgumentType::LuaTypeTableMap)
    {}

    /**
     * @brief Copy constructor
     */
    LuaArgument(const LuaArgument &argument)
        : type(argument.type)
    {
        this->copy(argument);
    }

    /**
     * @brief Move constructor
     */
    LuaArgument(LuaArgument &&argument) noexcept
    {
        this->move(std::forward<LuaArgument>(argument));
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
        this->destroy();
        this->move(std::forward<LuaArgument>(argument));

        return *this;
    }

    /**
     * @brief Boolean getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(bool &, LuaArgumentType::LuaTypeBoolean, Bool)
        return *reinterpret_cast<bool *>(this->value);
    }

    /**
     * @brief Number getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(double &, LuaArgumentType::LuaTypeNumber, Number)
        return *reinterpret_cast<double *>(this->value);
    }

    /**
     * @brief Integer getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(int &, LuaArgumentType::LuaTypeInteger, Integer)
        return *reinterpret_cast<int *>(this->value);
    }

    /**
     * @brief String getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(std::string &, LuaArgumentType::LuaTypeString, String)
        return *reinterpret_cast<std::string *>(this->value);
    }

    /**
     * @brief MTASA Object getter
     * @throws LuaUnexpectedArgumentType Type mismatch
     * @return Result
     */
    LUA_VM_ARGUMENT_GET_FUNCTION(LuaObject &, LuaArgumentType::LuaTypeObject, Object)
        return *reinterpret_cast<LuaObject *>(this->value);
    }

    /**
     * @brief Map getter
     * @throws LuaUnexpectedArgumentType Type mismatch (expected TABLE_MAP or TABLE_LIST)
     * @return Result
     */
    TableMapType toMap() const;

    /**
     * @brief Map getter
     * @throws LuaUnexpectedArgumentType Type mismatch (expected TABLE_MAP or TABLE_LIST)
     * @throws LuaCannotTransformArgumentToList
     * @return Result
     */
    TableListType toList() const;

    /**
     * @brief Pointer getter
     * @throws LuaUnexpectedArgumentType Type mismatch (expected USERDATA or LIGHTUSERDATA)
     * @return void pointer
     */
    void *toPointer() const;

    /**
     * @brief Is object nil
     * @return true, if current object is nil
     */
    bool isNil() const
    {
        return this->type == LuaArgumentType::LuaTypeNil;
    }

    /**
     * @brief Transforms userdata to MTASA Object
     * @throws LuaUnexpectedArgumentType Type mismatch (expected USERDATA, LIGHTUSERDATA or OBJECT)
     * @param stringClass Lua MTASA object's class (may be empty to autodetect)
     * @return Result MTASA Object reference
     */
    LuaObject &extractObject(const std::string &stringClass = "");

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
    virtual ~LuaArgument()
    {
        this->destroy();
    }

private:
    virtual void move(LuaArgument &&argument) noexcept;
    virtual void copy(const LuaArgument &argument);
    virtual void destroy() noexcept;

    void *value = nullptr;                                ///< Pointer to allocated value (unknown type)
    LuaArgumentType type = LuaTypeNil;                    ///< Object's type
};

bool operator==(const LuaArgument &left, const LuaArgument &right);
