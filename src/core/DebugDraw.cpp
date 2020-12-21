#include "DebugDraw.h"

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {}
void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    std::vector<Vertex> debugVertices;

    for (int i = 0; i < vertexCount; ++i) {
        auto v = vertices[i];
        debugVertices.emplace_back(glm::vec3(v.x, v.y, 0));
    }

    _physicsDebugMesh.rebuild(debugVertices, std::vector<unsigned short>(), std::vector<Texture>());
    _physicsDebugMesh.render(_commandBuffer);
}
void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {}
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {}
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {}
void DebugDraw::DrawTransform(const b2Transform& xf) {}
void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {}