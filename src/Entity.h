#pragma once

#include <pch.h>
#include "core/resources/Model.h"
#include "World.h"

class World;

class Entity {
public:
    Entity(World* world, Model* model, b2PolygonShape* shape, glm::vec2 position, glm::vec2 rotation);
    ~Entity();

    glm::vec2 getPosition() { return _position; }
    glm::vec2 getRotation() { return _rotation; }

    void setPosition(glm::vec2 position) { _position = position; }
    void setRotation(glm::vec2 rotation) { _rotation = rotation; }

    void render(vk::CommandBuffer &commandBuffer);
    void update(float deltaTime);
    void updatePhysics(long double timeStep, long double accumulator);

private:
    vk::Buffer _uniformBuffer;
    VmaAllocation _uniformAllocation;
    vk::DescriptorSet _descriptorSet;

    ModelUBO _ubo {};

    glm::vec2 _position;
    glm::vec2 _rotation;

    Model* _model;
    World* _world;

    b2Body* _physicsBody = nullptr;

    glm::mat4 getModelMatrix() {
        glm::mat4 pos(1.0f);
        pos = glm::translate(pos, glm::vec3(_position, 0));
        //pos = glm::rotate(pos, _rotation);
        return pos;
    }
};
