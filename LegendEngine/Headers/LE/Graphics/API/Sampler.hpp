#pragma once

namespace le
{
    class Sampler
    {
    public:
        enum class Filter
        {
            NEAREST,
            LINEAR,
        };

        enum class AddressMode
        {
            REPEAT,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER,
        };

        enum class BorderColor
        {
            TRANSPARENT_BLACK,
            OPAQUE_BLACK,
            OPAQUE_WHITE,
        };

        struct Info
        {
            Filter filter = Filter::NEAREST;
            AddressMode addressMode = AddressMode::CLAMP_TO_EDGE;
            BorderColor borderColor = BorderColor::OPAQUE_BLACK;
        };

        virtual ~Sampler() = default;
    };
}