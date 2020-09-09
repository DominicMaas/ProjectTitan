#pragma once

#include "../pch.h"

class Frustum {
public:
    enum Planes {
        LEFT = 0,
        RIGHT,
        BOTTOM,
        TOP,
        NEAR,
        FAR,
        Count,
        Combinations = Count * (Count - 1) / 2
    };

    glm::vec4 _planes[Count];
    glm::vec3 _points[8];

    // m = ProjectionMatrix * ViewMatrix
    static Frustum GetFrustum(glm::mat4 m) {
        static Frustum frustum;
        frustum.CalculateFrustum(m);
        return frustum;
    }

    void CalculateFrustum(glm::mat4 m) {
        m = glm::transpose(m);
        _planes[LEFT] = m[3] + m[0];
        _planes[RIGHT] = m[3] - m[0];
        _planes[BOTTOM] = m[3] + m[1];
        _planes[TOP] = m[3] - m[1];
        _planes[NEAR] = m[3] + m[2];
        _planes[FAR] = m[3] - m[2];

        glm::vec3 crosses[Combinations] = {
                glm::cross(glm::vec3(_planes[LEFT]), glm::vec3(_planes[RIGHT])),
                glm::cross(glm::vec3(_planes[LEFT]), glm::vec3(_planes[BOTTOM])),
                glm::cross(glm::vec3(_planes[LEFT]), glm::vec3(_planes[TOP])),
                glm::cross(glm::vec3(_planes[LEFT]), glm::vec3(_planes[NEAR])),
                glm::cross(glm::vec3(_planes[LEFT]), glm::vec3(_planes[FAR])),
                glm::cross(glm::vec3(_planes[RIGHT]), glm::vec3(_planes[BOTTOM])),
                glm::cross(glm::vec3(_planes[RIGHT]), glm::vec3(_planes[TOP])),
                glm::cross(glm::vec3(_planes[RIGHT]), glm::vec3(_planes[NEAR])),
                glm::cross(glm::vec3(_planes[RIGHT]), glm::vec3(_planes[FAR])),
                glm::cross(glm::vec3(_planes[BOTTOM]), glm::vec3(_planes[TOP])),
                glm::cross(glm::vec3(_planes[BOTTOM]), glm::vec3(_planes[NEAR])),
                glm::cross(glm::vec3(_planes[BOTTOM]), glm::vec3(_planes[FAR])),
                glm::cross(glm::vec3(_planes[TOP]), glm::vec3(_planes[NEAR])),
                glm::cross(glm::vec3(_planes[TOP]), glm::vec3(_planes[FAR])),
                glm::cross(glm::vec3(_planes[NEAR]), glm::vec3(_planes[FAR]))
        };

        _points[0] = intersection<LEFT, BOTTOM, NEAR>(crosses);
        _points[1] = intersection<LEFT, TOP, NEAR>(crosses);
        _points[2] = intersection<RIGHT, BOTTOM, NEAR>(crosses);
        _points[3] = intersection<RIGHT, TOP, NEAR>(crosses);
        _points[4] = intersection<LEFT, BOTTOM, FAR>(crosses);
        _points[5] = intersection<LEFT, TOP, FAR>(crosses);
        _points[6] = intersection<RIGHT, BOTTOM, FAR>(crosses);
        _points[7] = intersection<RIGHT, TOP, FAR>(crosses);
    }

    template<Planes i, Planes j>
    struct ij2k {
        enum {
            k = i * (9 - i) / 2 + j - 1
        };
    };

    template<Planes a, Planes b, Planes c>
    glm::vec3 intersection(const glm::vec3 *crosses) const {
        float D = glm::dot(glm::vec3(_planes[a]), crosses[ij2k<b, c>::k]);
        glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
                        glm::vec3(_planes[a].w, _planes[b].w, _planes[c].w);
        return res * (-1.0f / D);
    }

    bool isBoxVisible(glm::vec3 min, glm::vec3 max) {
        for (int i = 0; i < 6; i++) {
            if ((glm::dot(_planes[i], glm::vec4(min.x, min.y, min.z, 1.0f)) < 0.0) &&
                (glm::dot(_planes[i], glm::vec4(max.x, min.y, min.z, 1.0f)) < 0.0) &&
                (glm::dot(_planes[i], glm::vec4(min.x, max.y, min.z, 1.0f)) < 0.0) &&
                (glm::dot(_planes[i], glm::vec4(max.x, max.y, min.z, 1.0f)) < 0.0) &&
                (glm::dot(_planes[i], glm::vec4(min.x, min.y, max.z, 1.0f)) < 0.0) &&
                (glm::dot(_planes[i], glm::vec4(max.x, min.y, max.z, 1.0f)) < 0.0) &&
                (glm::dot(_planes[i], glm::vec4(min.x, max.y, max.z, 1.0f)) < 0.0) &&
                (glm::dot(_planes[i], glm::vec4(max.x, max.y, max.z, 1.0f)) < 0.0)) {
                return false;
            }
        }

        // check frustum outside/inside box
        int out;
        out = 0;
        for (int i = 0; i < 8; i++) out += ((_points[i].x > max.x) ? 1 : 0);
        if (out == 8) return false;
        out = 0;
        for (int i = 0; i < 8; i++) out += ((_points[i].x < min.x) ? 1 : 0);
        if (out == 8) return false;
        out = 0;
        for (int i = 0; i < 8; i++) out += ((_points[i].y > max.y) ? 1 : 0);
        if (out == 8) return false;
        out = 0;
        for (int i = 0; i < 8; i++) out += ((_points[i].y < min.y) ? 1 : 0);
        if (out == 8) return false;
        out = 0;
        for (int i = 0; i < 8; i++) out += ((_points[i].z > max.z) ? 1 : 0);
        if (out == 8) return false;
        out = 0;
        for (int i = 0; i < 8; i++) out += ((_points[i].z < min.z) ? 1 : 0);
        if (out == 8) return false;

        return true;
    }
};