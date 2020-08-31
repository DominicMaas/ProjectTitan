#include "Entity.h"

Entity::Entity(World *world, Model *model, reactphysics3d::CollisionShape* shape, glm::vec3 position, glm::vec3 rotation) {
    this->_world = world;
    this->_model = model;
    this->_position = position;
    this->_rotation = rotation;

    // Setup physics
    _rigidBody = world->getPhysicsWorld()->createRigidBody(getPhysicsPosition());
    _rigidBody->setType(reactphysics3d::BodyType::DYNAMIC);

    // Setup the collider
    _collider = _rigidBody->addCollider(shape, reactphysics3d::Transform::identity());
}

Entity::~Entity() {
    // Remove collider
    _rigidBody->removeCollider(_collider);

    // Remove rigid body
    _world->getPhysicsWorld()->destroyRigidBody(_rigidBody);
}

void Entity::render(Shader &shader) {
    // Update the shader model
    shader.setMat4("model", getModelMatrix());

    // Render
    _model->render(shader);
}

void Entity::update(long double delta) {

}

void Entity::updatePhysics(long double timeStep, long double accumulator) {
    // Compute the time interpolation factor
    auto factor = accumulator / timeStep;

    // Get the updated transform of the body
    auto currTransform = _rigidBody->getTransform();
    auto prevTransform = getPhysicsPosition();

    setPosition(glm::vec3(currTransform.getPosition().x, currTransform.getPosition().y, currTransform.getPosition().z));

    // Compute the interpolated transform of the rigid body
    //auto interpolatedTransform = reactphysics3d::Transform::interpolateTransforms(prevTransform, currTransform, factor);
    //setPosition(glm::vec3(interpolatedTransform.getPosition().x, interpolatedTransform.getPosition().y, interpolatedTransform.getPosition().z));
}
