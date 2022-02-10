#pragma once

#include <engineincl.h>

#include <glm/glm.hpp>

namespace Techless
{


    struct Vector2
    {
        Vector2() = default;
        Vector2(float x, float y)
            : X(x), Y(y) {}

        float X = 0.f;
        float Y = 0.f;

        inline operator glm::vec<2, float>() const { return { X, Y }; };
        inline operator glm::vec<3, float>() const { return glm::vec<3, float>((glm::vec<2, float>)*this, 0.f); }
        inline operator glm::vec<4, float>() const { return glm::vec<4, float>((glm::vec<2, float>)*this, 0.f, 0.f); }
    };

    struct Vector3
    {
        Vector3() = default;
        Vector3(float x, float y, float z)
            : X(x), Y(y), Z(z) {}

        float X = 0.f;
        float Y = 0.f;
        float Z = 0.f;

        inline operator glm::vec<2, float>() const { return { X, Y }; };
        inline operator glm::vec<3, float>() const { return { X, Y, Z }; };
        inline operator glm::vec<4, float>() const { return glm::vec<4, float>((glm::vec<3, float>)*this, 0.f); }
    };

    std::ostream& operator<<(std::ostream& stream, const Vector2& vector)
    {
        stream << vector.X << " " << vector.Y;
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const Vector3& vector)
    {
        stream << vector.X << " " << vector.Y << " " << vector.Z;
        return stream;
    }
}