#pragma once

#include <pch.h>
#include "core/resources/Model.h"
#include "World.h"

class World;

class Entity {
public:
    Entity(World* world, Model* model, reactphysics3d::CollisionShape* shape, glm::vec2 position, glm::vec2 rotation);
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

    glm::vec2 _position;
    glm::vec2 _rotation;

    Model* _model;
    World* _world;

    reactphysics3d::RigidBody* _rigidBody;
    reactphysics3d::Collider* _collider;

    glm::mat4 getModelMatrix() {
        glm::mat4 pos(1.0f);
        pos = glm::translate(pos, glm::vec3(_position, 0));
        //pos = glm::rotate(pos, _rotation);
        return pos;
    }

    reactphysics3d::Transform getPhysicsPosition() const {
        reactphysics3d::Vector3 position(_position.x, _position.y, 0);
        reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
        reactphysics3d::Transform transform(position, orientation);

        return transform;
    }
};
