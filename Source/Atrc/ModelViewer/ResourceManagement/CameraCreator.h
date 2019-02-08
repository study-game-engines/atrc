#pragma once

#include <Atrc/ModelViewer/ResourceManagement/ResourceManager.h>

void RegisterCameraCreators(ResourceManager &rscMgr);

class PinholeCameraCreator : public CameraCreator
{
public:

    PinholeCameraCreator() : CameraCreator("pinhole") { }

    std::shared_ptr<CameraInstance> Create(std::string name) const override;
};
