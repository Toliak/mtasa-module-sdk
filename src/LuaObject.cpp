#include "ModuleSdk/LuaObject.h"

bool operator==(const ObjectId &left, const ObjectId &right)
{
    return left.id == right.id;
}

bool operator==(const LuaObject &left, const LuaObject &right)
{
    return left.getObjectId() == right.getObjectId();
}