#pragma once

#include <string>

struct ObjectId
{
    explicit ObjectId(unsigned long id)
        : id(id)
    {}

    unsigned long id;
};

bool operator==(const ObjectId &left, const ObjectId &right);


class LuaObject
{
public:
    explicit LuaObject(ObjectId objectId, std::string  stringClass = "")
        : objectId(objectId), stringClass(std::move(stringClass))
    {}

    const ObjectId &getObjectId() const
    {
        return objectId;
    }

    void setObjectId(const ObjectId &newObjectId)
    {
        objectId = newObjectId;
    }

    const std::string &getStringClass() const
    {
        return stringClass;
    }

    void setStringClass(const std::string &newStringClass)
    {
        stringClass = newStringClass;
    }


private:
    ObjectId objectId;

    std::string stringClass;
};

bool operator==(const LuaObject &left, const LuaObject &right);
