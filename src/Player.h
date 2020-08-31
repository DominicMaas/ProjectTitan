#pragma once

#include "Entity.h"
#include "Camera.h"

class Player : Entity {
public:
    Camera getCamera() { return _camera; }


private:
    Camera _camera;
};
