#include "ModuleSdk/LuaArgument.h"


size_t LuaArgumentHash::operator()(const LuaArgument &k) const
{
    return (
        std::hash<LuaArgumentType>()(k.type) ^ reinterpret_cast<uintptr_t>(k.value)
    );
}


void LuaArgument::copy(const LuaArgument &argument)
{
    if (!argument.value) {
        this->type = LuaArgumentType::NIL;
        this->value = nullptr;
        return;
    }

    this->type = argument.type;
    if (type == LuaArgumentType::BOOLEAN) {
        this->value = new bool(*reinterpret_cast<bool *>(argument.value));
    } else if (type == LuaArgumentType::NUMBER) {
        this->value = new double(*reinterpret_cast<double *>(argument.value));
    } else if (type == LuaArgumentType::INTEGER) {
        this->value = new int(*reinterpret_cast<int *>(argument.value));
    } else if (type == LuaArgumentType::STRING) {
        this->value = new std::string(*reinterpret_cast<std::string *>(argument.value));
    } else if (
        type == LuaArgumentType::LIGHTUSERDATA
            || type == LuaArgumentType::USERDATA) {
        this->value = argument.value;
    } else if (type == LuaArgumentType::OBJECT) {
        this->value = new LuaObject(*reinterpret_cast<LuaObject *>(argument.value));
    } else if (type == LuaArgumentType::TABLE_LIST) {
        this->value = new TableListType(*reinterpret_cast<TableListType *>(argument.value));
    } else if (type == LuaArgumentType::TABLE_MAP) {
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

    if (type == LuaArgumentType::BOOLEAN) {
        delete reinterpret_cast<bool *>(value);
    } else if (type == LuaArgumentType::NUMBER) {
        delete reinterpret_cast<double *>(value);
    } else if (type == LuaArgumentType::INTEGER) {
        delete reinterpret_cast<int *>(value);
    } else if (type == LuaArgumentType::STRING) {
        delete reinterpret_cast<std::string *>(value);
    } else if (type == LuaArgumentType::OBJECT) {
        delete reinterpret_cast<LuaObject *>(value);
    } else if (type == LuaArgumentType::TABLE_LIST) {
        delete reinterpret_cast<TableListType *>(value);
    } else if (type == LuaArgumentType::TABLE_MAP) {
        delete reinterpret_cast<TableMapType *>(value);
    }
}

bool operator==(const LuaArgument &left, const LuaArgument &right)
{
    return left.type == right.type
        && left.value == right.value;
}
