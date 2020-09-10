#pragma once

#include <stb_image.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <spdlog/spdlog.h>
#include <vk_mem_alloc.h>

#include <optional>
#include <set>
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <tuple>

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

enum ColliderCategory {
    COLLIDER_GENERAL = 0x0001,
    COLLIDER_WORLD_GROUND = 0x0002
};

static const int CHUNK_HEIGHT = 64;
static const int CHUNK_WIDTH = 16;