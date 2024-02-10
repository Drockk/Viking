//
// Created by batzi on 28.10.2023.
//

#ifndef VIKING_VK_TEXTURES_HPP
#define VIKING_VK_TEXTURES_HPP

#include "vk_types.hpp"
#include "vk_engine.hpp"

namespace vkutil {
    bool load_image_from_file(ViEngine& engine, const char* file, AllocatedImage& outImage);
}

#endif //VIKING_VK_TEXTURES_HPP
