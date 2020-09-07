#pragma once

#include "../pch.h"
#include "../Camera.h"
#include "Renderable.h"
#include <boost/concept_check.hpp>
#include <boost/ptr_container/ptr_map.hpp>

class Scene {
private:
    Camera* _mainCamera;
    boost::ptr_map<std::string, Renderable> _renderables;

public:
    //Scene(Camera* camera);
    Scene() {}

    void addRenderable(std::string name, Renderable* renderable, RenderableData renderableData);

    void render(vk::CommandBuffer &commandBuffer);
    //void update();

    void destroy(RenderableData renderableData);
};