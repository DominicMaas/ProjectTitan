#pragma once

#include <pch.h>

struct ImageSet {
    vk::Image image;
    VmaAllocation allocation;
    vk::ImageView imageView;
};
