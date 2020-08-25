#pragma once

#include <glad/glad.h>
#include "RenderEffect.h"

class ShadowMapping : public RenderEffect {
public:
	ShadowMapping(int width, int height);
	~ShadowMapping();
	void render(Camera* camera);

private:
	unsigned int _depthMapFBO;
	unsigned int _depthMap;
};