#include "ModuleSdk/LuaArgument.h"

size_t LuaArgumentHash::operator()(const LuaArgument &argument) const
{
    size_t hashType = std::hash<LuaArgumentType>()(argument.type);         ///< Hashed type

    if (argument.type == LuaArgumentType::LuaTypeLightUserdata || argument.type == LuaArgumentType::LuaTypeUserdata) {
        return hashType ^ reinterpret_cast<uintptr_t>(argument.value);
    }
    if (argument.type == LuaArgumentType::LuaTypeBoolean) {
        bool boolean = *reinterpret_cast<bool *>(argument.value);
        return hashType ^ static_cast<unsigned int>(boolean);
    }
    if (argument.type == LuaArgumentType::LuaTypeNumber) {
        return hashType ^ static_cast<size_t>(*reinterpret_cast<double *>(argument.value));
    }
    if (argument.type == LuaArgumentType::LuaTypeInteger) {
        return hashType ^ static_cast<size_t>(*reinterpret_cast<int *>(argument.value));
    }
    if (argument.type == LuaArgumentType::LuaTypeString) {
        return hashType ^ std::hash<std::string>()(*reinterpret_cast<std::string *>(argument.value));
    }
    // TODO: hash for tables
    return hashType ^ reinterpret_cast<uintptr_t>(argument.value);

}

LuaArgument::TableMapType LuaArgument::toMap() const
{
    if (this->type == LuaArgumentType::LuaTypeTableMap) {
        return *reinterpret_cast<TableMapType *>(value);
    }
    if (this->type != LuaArgumentType::LuaTypeTableList) {
        throw LuaUnexpectedArgumentType(LuaArgumentType::LuaTypeTableList, this->type);
    }

    TableListType &original = *reinterpret_cast<TableListType *>(value);
    TableMapType result;
    for (size_t i = 0; i < original.size(); i++) {
        result[LuaArgument(i + 1.)] = original[i];
    }
    return result;
}

LuaArgument::TableListType LuaArgument::toList() const
{
    if (this->type == LuaArgumentType::LuaTypeTableList) {
        return *reinterpret_cast<TableListType *>(value);
    }
    if (this->type != LuaArgumentType::LuaTypeTableMap) {
        throw LuaUnexpectedArgumentType(LuaArgumentType::LuaTypeTableMap, this->type);
    }

    const auto &original = *reinterpret_cast<TableMapType *>(value);
    TableListType result(original.size());
    for (size_t i = 0; i < original.size(); i++) {
        try {
            result[i] = original.at(LuaArgument(i + 1.));
        } catch (const std::out_of_range &) {
            throw LuaCannotTransformArgumentToList();
        }
    }
    return result;
}

void *LuaArgument::toPointer() const
{
    if (!(
        this->type == LuaArgumentType::LuaTypeLightUserdata
            || this->type == LuaArgumentType::LuaTypeUserdata
    )) {
        throw LuaUnexpectedArgumentType(LuaArgumentType::LuaTypeLightUserdata, this->type);
    }

    return value;
}

LuaObject &LuaArgument::extractObject(const std::string &stringClass)
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

void LuaArgument::move(LuaArgument &&argument) noexcept
{
    // Move value and type
    this->value = argument.value;
    this->type = argument.type;

    // Clear old argument
    argument.value = nullptr;
    argument.type = LuaArgumentType::LuaTypeNil;
}

void LuaArgument::copy(const LuaArgument &argument)
{
    if (!argument.value) {                          // Nil
        this->type = LuaArgumentType::LuaTypeNil;
        this->value = nullptr;
        return;
    }

    this->type = argument.type;
    if (type == LuaArgumentType::LuaTypeBoolean) {
        this->value = new bool(*reinterpret_cast<bool *>(argument.value));

    } else if (type == LuaArgumentType::LuaTypeNumber) {
        this->value = new double(*reinterpret_cast<double *>(argument.value));

    } else if (type == LuaArgumentType::LuaTypeInteger) {
        this->value = new int(*reinterpret_cast<int *>(argument.value));

    } else if (type == LuaArgumentType::LuaTypeString) {
        this->value = new std::string(*reinterpret_cast<std::string *>(argument.value));

    } else if (
        type == LuaArgumentType::LuaTypeLightUserdata
            || type == LuaArgumentType::LuaTypeUserdata) {
        this->value = argument.value;

    } else if (type == LuaArgumentType::LuaTypeObject) {
        this->value = new LuaObject(*reinterpret_cast<LuaObject *>(argument.value));

    } else if (type == LuaArgumentType::LuaTypeTableList) {
        this->value = new TableListType(*reinterpret_cast<TableListType *>(argument.value));

    } else if (type == LuaArgumentType::LuaTypeTableMap) {
        this->value = new TableMapType(*reinterpret_cast<TableMapType *>(argument.value));

    } else {
        this->value = nullptr;
    }
}

void LuaArgument::destroy() noexcept
{
    if (!this->value) {
        return;
    }

    if (type == LuaArgumentType::LuaTypeBoolean) {
        delete reinterpret_cast<bool *>(value);

    } else if (type == LuaArgumentType::LuaTypeNumber) {
        delete reinterpret_cast<double *>(value);

    } else if (type == LuaArgumentType::LuaTypeInteger) {
        delete reinterpret_cast<int *>(value);

    } else if (type == LuaArgumentType::LuaTypeString) {
        delete reinterpret_cast<std::string *>(value);

    } else if (type == LuaArgumentType::LuaTypeObject) {
        delete reinterpret_cast<LuaObject *>(value);

    } else if (type == LuaArgumentType::LuaTypeTableList) {
        delete reinterpret_cast<TableListType *>(value);

    } else if (type == LuaArgumentType::LuaTypeTableMap) {
        delete reinterpret_cast<TableMapType *>(value);

    } else {
        // LuaTypeNil
        // LuaTypeLightUserdata
        // LuaTypeUserdata

        return;
    }
}

bool operator==(const LuaArgument &left, const LuaArgument &right)
{
    if (left.type != right.type) {
        return false;
    }

    if (left.type == LuaArgumentType::LuaTypeBoolean) {
        return *reinterpret_cast<bool *>(left.value) == *reinterpret_cast<bool *>(right.value);
    }
    if (left.type == LuaArgumentType::LuaTypeNumber) {
        return *reinterpret_cast<double *>(left.value) == *reinterpret_cast<double *>(right.value);
    }
    if (left.type == LuaArgumentType::LuaTypeInteger) {
        return *reinterpret_cast<int *>(left.value) == *reinterpret_cast<int *>(right.value);
    }
    if (left.type == LuaArgumentType::LuaTypeString) {
        return *reinterpret_cast<std::string *>(left.value) == *reinterpret_cast<std::string *>(right.value);
    }
    if (left.type == LuaArgumentType::LuaTypeObject) {
        return *reinterpret_cast<LuaObject *>(left.value) == *reinterpret_cast<LuaObject *>(right.value);
    }
    if (left.type == LuaArgumentType::LuaTypeTableList) {
        return *reinterpret_cast<LuaArgument::TableListType *>(left.value)
            == *reinterpret_cast<LuaArgument::TableListType *>(right.value);
    }
    if (left.type == LuaArgumentType::LuaTypeTableMap) {
        return *reinterpret_cast<LuaArgument::TableMapType *>(left.value)
            == *reinterpret_cast<LuaArgument::TableMapType *>(right.value);
    }
    return left.value == right.value;

}