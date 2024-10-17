#include <cmath>

#define M_PI 3.14159265358979323846264338327950288419716939937510

struct Vector2 {
    float x;
    float y;

    // Constructor
    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    // Addition
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    // Subtraction
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    // Scalar multiplication
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    // Dot product
    float Dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Length
    float Length() const {
        return sqrt(x * x + y * y);
    }

    // Normalization
    Vector2 Normalize() const {
        float len = Length();
        return (len > 0) ? Vector2(x / len, y / len) : Vector2(0, 0);
    }
};


struct Vector3 {
    float x;
    float y;
    float z;

    // Constructor
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    // Addition
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Subtraction
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Scalar multiplication
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Dot product
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vector3 Cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Length
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalization
    Vector3 Normalize() const {
        float len = Length();
        return (len > 0) ? Vector3(x / len, y / len, z / len) : Vector3(0, 0, 0);
    }
};