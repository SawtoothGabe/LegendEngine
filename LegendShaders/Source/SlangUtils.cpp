#include "SlangUtils.hpp"

#include <stdexcept>

void SlangUtils::Diagnose(slang::IBlob* blob)
{
    if (!blob)
        return;

    throw std::runtime_error(static_cast<const char*>(blob->getBufferPointer()));
}
