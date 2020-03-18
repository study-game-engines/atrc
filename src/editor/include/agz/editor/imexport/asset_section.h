#pragma once

#include <agz/editor/common.h>

AGZ_EDITOR_BEGIN

enum class AssetSectionType : uint8_t
{
    MaterialPool  = 0,
    MediumPool    = 1,
    GeometryPool  = 2,
    Texture2DPool = 3,
    Texture3DPool = 4,

    Entities      = 6,
    EnvirLight    = 7,

    GlobalSettings = 8,
    PreviewWindow  = 9,

    Renderer = 10,
};

AGZ_EDITOR_END
