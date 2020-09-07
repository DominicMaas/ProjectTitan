#include "Scene.h"

void Scene::addRenderable(std::string name, Renderable* renderable, RenderableData renderableData) {
    if (_renderables.find(name) == _renderables.end()) {
        _renderables.insert(name, renderable);
        renderable->build(renderableData);
    } else {
        throw std::invalid_argument("Could not add the renderable, a renderable of this name already exists.");
    }
}

void Scene::render(vk::CommandBuffer &commandBuffer) {
    for (boost::ptr_map<std::string, Renderable>::iterator e = _renderables.begin(); e != _renderables.end(); e++) {
        e->second->render(commandBuffer);
    }
}

void Scene::destroy(RenderableData renderableData) {
    for (boost::ptr_map<std::string, Renderable>::iterator e = _renderables.begin(); e != _renderables.end(); e++) {
        e->second->destroy(renderableData);
    }
}
