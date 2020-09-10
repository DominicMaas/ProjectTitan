#pragma once

#include <pch.h>
#include "../../GraphicsPipeline.h"
#include <boost/concept_check.hpp>
#include <boost/ptr_container/ptr_map.hpp>

class PipelineManager {
private:
    static boost::ptr_map<std::string, GraphicsPipeline> _graphicsPipelines;
    static CreateGraphicsPipelineInfo _createInfo;
    static bool _isInit;

public:
    static void init(CreateGraphicsPipelineInfo createInfo);

    static void createPipeline(std::string name, PipelineInfo info);

    static GraphicsPipeline* getPipeline(const std::string& name);

    static void cleanup(DestroyGraphicsPipelineInfo info);
};
