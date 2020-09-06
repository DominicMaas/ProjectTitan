#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"
#include <glm/glm.hpp>

#include <optional>
#include <set>
#include <algorithm>
#include <map>
#include <string>
#include <vector>