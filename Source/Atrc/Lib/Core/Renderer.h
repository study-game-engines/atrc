#pragma once

#include <AGZUtils/Utils/Texture.h>

#include <Atrc/Lib/Core/Reporter.h>
#include <Atrc/Lib/Core/Sampler.h>
#include <Atrc/Lib/Core/Scene.h>
#include <Atrc/Lib/Core/TFilm.h>

namespace Atrc
{

// 原则上一个Renderer只应用于一次渲染
class Renderer
{
public:

    virtual ~Renderer() = default;

    virtual void Render(const Scene *scene, Sampler *sampler, Film *film, Reporter *reporter) = 0;

    virtual bool IsCompleted() const = 0;

    virtual void Join(Reporter *reporter) = 0;
	
	virtual void Stop() = 0;
};

} // namespace Atrc
