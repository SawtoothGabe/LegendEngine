#include "SlangUtils.hpp"

#include <print>
#include <stdexcept>

void SlangUtils::Diagnose(slang::IBlob* blob)
{
    if (!blob)
        return;

    std::println(stderr, "{}", static_cast<const char*>(blob->getBufferPointer()));
    throw std::runtime_error("Slang function returned non-null diagnosis");
}


