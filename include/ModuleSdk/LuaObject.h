#pragma once

#include <string>

/**
 * @brief Contains MTASA object ID
 */
struct ObjectId
{
    explicit ObjectId(unsigned long id)
        : id(id)
    {}

    unsigned long id;
};

bool operator==(const ObjectId &left, const ObjectId &right);

/**
 * @brief MTASA object wrapper
 */
class LuaObject
{
public:
    /**
     * @brief Constructor
     * @param objectId MTASA Object ID
     * @param stringClass Lua MTASA class. Empty for autodetect
     */
    explicit LuaObject(ObjectId objectId, std::string stringClass = "")
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
    /**
     * @brief MTASA object ID
     * @details For example, CElementIDs::m_Elements contains elements ID
     */
    ObjectId objectId;

    std::string stringClass;        ///< Lua MTASA class. Empty for autodetect
};

bool operator==(const LuaObject &left, const LuaObject &right);
