#pragma once

#include <atomic>
#include <LE/Common/Defs.hpp>
#include <LE/Common/UID.hpp>

namespace le
{
    class Resource
    {
        friend class ResourceManager;
    public:
        template<typename T>
        using ID = UID;

        Resource();
        virtual ~Resource() = 0;
        LE_NO_COPY(Resource);
    protected:
        UID m_uid;
        std::atomic_bool m_deleted = false;
        size_t m_framesSinceDeletion = 0;
    };
}