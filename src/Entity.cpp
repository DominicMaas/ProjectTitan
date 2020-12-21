#include "Entity.h"
#include "core/managers/PipelineManager.h"
#include "core/Renderer.h"

Entity::Entity(World *world, Model *model, reactphysics3d::CollisionShape* shape, glm::vec2 position, glm::vec2 rotation) {
    this->_world = world;
    this->_model = model;
    this->_position = position;
    this->_rotation = rotation;

    // Setup physics
    //_rigidBody = world->getPhysicsWorld()->createRigidBody(getPhysicsPosition());
    //_rigidBody->setType(reactphysics3d::BodyType::DYNAMIC);

    // Setup the collider
    //_collider = _rigidBody->addCollider(shape, reactphysics3d::Transform::identity());

    // ------------------ Create Uniform Buffer ------------------ //

    // Create the descriptor set to store the chunk position
    auto* pipeline = PipelineManager::getPipeline("basic");
    if (pipeline == nullptr) {
        throw std::invalid_argument("Unable to retrieve the specified pipeline ('basic')");
    }

    pipeline->createModelUBO(_uniformBuffer, _uniformAllocation, _descriptorSet);

    ModelUBO ubo {};
    ubo.model = getModelMatrix();

    // Copy this data across to the local memory
    // TODO: Maybe move this to the GPU memory?
    void* mappedData;
    vmaMapMemory(Renderer::Instance->Allocator, _uniformAllocation, &mappedData);
    memcpy(mappedData, &ubo, sizeof(ubo));
    vmaUnmapMemory(Renderer::Instance->Allocator, _uniformAllocation);
}

Entity::~Entity() {
    // Remove collider
    //_rigidBody->removeCollider(_collider);

    // Remove rigid body
    //_world->getPhysicsWorld()->destroyRigidBody(_rigidBody);

    vmaDestroyBuffer(Renderer::Instance->Allocator, _uniformBuffer, _uniformAllocation);
}

void Entity::render(vk::CommandBuffer &commandBuffer) {
    // Bind the descriptor set for the position
    auto* pipeline = PipelineManager::getPipeline("basic");
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->getPipelineLayout(), 1, 1, &_descriptorSet, 0, nullptr);

    // Render
    _model->render(commandBuffer, "basic");
}

void Entity::update(float deltaTime) {

}

void Entity::updatePhysics(long double timeStep, long double accumulator) {
    // Compute the time interpolation factor
    //auto factor = accumulator / timeStep;

    // Get the updated transform of the body
    //auto currTransform = _rigidBody->getTransform();
    //auto prevTransform = getPhysicsPosition();

    //setPosition(glm::vec3(currTransform.getPosition().x, currTransform.getPosition().y, currTransform.getPosition().z));

    // Compute the interpolated transform of the rigid body
    //auto interpolatedTransform = reactphysics3d::Transform::interpolateTransforms(prevTransform, currTransform, factor);
    //setPosition(glm::vec3(interpolatedTransform.getPosition().x, interpolatedTransform.getPosition().y, interpolatedTransform.getPosition().z));
}
