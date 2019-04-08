#pragma once

#include <Atrc/Editor/Fresnel/Fresnel.h>

namespace Atrc::Editor
{

class Schlick : public IFresnel
{
    float etaOut_ = 1;
    float etaIn_ = 1.5f;

public:

    using IFresnel::IFresnel;

    std::string Save() const override;

    void Load(const AGZ::ConfigGroup &params) override;

    std::string Export() const override;

    void Display() override;

    bool IsMultiline() const noexcept override;
};

DEFINE_DEFAULT_RESOURCE_CREATOR(IFresnelCreator, Schlick, "Schlick");

}; // namespace Atrc::Editor
