#pragma once

#include <LE/Common/Defs.hpp>

namespace le
{
    class Resource
    {
    public:
        Resource();
        virtual ~Resource() = 0;
        LE_NO_COPY(Resource);
    };
}