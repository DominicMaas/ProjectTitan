#include "Entity.h"
#include "core/managers/PipelineManager.h"
#include "core/Renderer.h"

Entity::Entity(World *world, Model *model, b2PolygonShape* shape, glm::vec2 position, glm::vec2 rotation) {
    this->_world = world;
    this->_model = model;
    this->_position = position;
    this->_rotation = rotation;

    // Setup physics (only if a shape is provided)
    if (shape != nullptr) {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(this->_position.x, this->_position.y);

        _physicsBody = world->getPhysicsWorld()->CreateBody(&bodyDef);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = shape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;

        _physicsBody->CreateFixture(&fixtureDef);
    }

    // ------------------ Create Uniform Buffer ------------------ //

    // Create the descriptor set to store the chunk position
    auto* pipeline = PipelineManager::getPipeline("basic");
    if (pipeline == nullptr) {
        throw std::invalid_argument("Unable to retrieve the specified pipeline ('basic')");
    }

    pipeline->createModelUBO(_uniformBuffer, _uniformAllocation, _descriptorSet);

    _ubo.model = getModelMatrix();

    // Copy this data across to the local memory
    // TODO: Maybe move this to the GPU memory?
    void* mappedData;
    vmaMapMemory(Renderer::Instance->Allocator, _uniformAllocation, &mappedData);
    memcpy(mappedData, &_ubo, sizeof(_ubo));
    vmaUnmapMemory(Renderer::Instance->Allocator, _uniformAllocation);
}

Entity::~Entity() {
    // Remove the physics body
    if (_physicsBody != nullptr) {
        _world->getPhysicsWorld()->DestroyBody(_physicsBody);
        _physicsBody = nullptr;
    }

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
    if (_physicsBody != nullptr) {
        auto position = _physicsBody->GetPosition();

        // Update the internal physics position
        setPosition(glm::vec2(position.x, position.y));

        // Rebuild the model matrix
        _ubo.model = getModelMatrix();

        // TODO: THIS IS UGLY

        void* mappedData;
        vmaMapMemory(Renderer::Instance->Allocator, _uniformAllocation, &mappedData);
        memcpy(mappedData, &_ubo, sizeof(_ubo));
        vmaUnmapMemory(Renderer::Instance->Allocator, _uniformAllocation);
    }
}
