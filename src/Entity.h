#pragma once

#include <pch.h>
#include "core/Model.h"
#include "World.h"

class World;

class Entity {
public:
    Entity(World* world, Model* model, reactphysics3d::CollisionShape* shape, glm::vec3 position, glm::vec3 rotation);
    ~Entity();

    glm::vec3 getPosition() { return _position; }
    glm::vec3 getRotation() { return _rotation; }

    void setPosition(glm::vec3 position) { _position = position; }
    void setRotation(glm::vec3 rotation) { _rotation = rotation; }

    void render(Shader& shader);
    void update(long double delta);
    void updatePhysics(long double timeStep, long double accumulator);

private:
    glm::vec3 _position;
    glm::vec3 _rotation;

    Model* _model;
    World* _world;

    reactphysics3d::RigidBody* _rigidBody;
    reactphysics3d::Collider* _collider;

    glm::mat4 getModelMatrix() {
        glm::mat4 pos(1.0f);
        pos = glm::translate(pos, _position);
        //pos = glm::rotate(pos, _rotation);
        return pos;
    }

    reactphysics3d::Transform getPhysicsPosition() const {
        reactphysics3d::Vector3 position(_position.x, _position.y, _position.x);
        reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
        reactphysics3d::Transform transform(position, orientation);

        return transform;
    }
};
