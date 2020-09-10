#include "PipelineManager.h"

// Instantiate static variables
boost::ptr_map<std::string, GraphicsPipeline> PipelineManager::_graphicsPipelines; // NOLINT(cert-err58-cpp)
bool PipelineManager::_isInit = false;
CreateGraphicsPipelineInfo PipelineManager::_createInfo;

void PipelineManager::init(CreateGraphicsPipelineInfo createInfo) {
    _createInfo = createInfo;
    _isInit = true;
}

void PipelineManager::createPipeline(std::string name, PipelineInfo info) {
    if (!_isInit) {
        throw std::invalid_argument("You cannot register pipelines before vulkan has initialised!");
    }

    spdlog::info("[Pipeline Manager] Loading pipeline '" + name + "'...");

    // Check that this name is unique
    if (_graphicsPipelines.find(name) == _graphicsPipelines.end()) {
        auto* pipeline = new GraphicsPipeline(info);
        _graphicsPipelines.insert(name, pipeline);
        pipeline->create(_createInfo);
    } else {
        spdlog::error("[Pipeline Manager] Could not add the pipeline, a pipeline of this name already exists.");
        throw std::invalid_argument("Could not add the pipeline, a pipeline of this name already exists.");
    }
}

GraphicsPipeline *PipelineManager::getPipeline(const std::string& name) {
    if (_graphicsPipelines.find(name) == _graphicsPipelines.end()) {
        return nullptr;
    }

    return _graphicsPipelines.find(name)->second;
}

void PipelineManager::cleanup(DestroyGraphicsPipelineInfo info) {
    for (boost::ptr_map<std::string, GraphicsPipeline>::iterator e = _graphicsPipelines.begin(); e != _graphicsPipelines.end(); e++) {
        e->second->destroy(info);
    }
}
