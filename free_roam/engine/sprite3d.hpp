#pragma once
#include "furi.h"
#include "engine/vector.hpp"
#include <math.h>

#define MAX_TRIANGLES_PER_SPRITE 28

static void scale_vertex(float &x, float &y, float &z, float scale_factor)
{
    x *= scale_factor;
    y *= scale_factor;
    z *= scale_factor;
}

static void rotateY_vertex(float &x, float &z, float angle)
{
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    float orig_x = x; // Store original x
    x = orig_x * cos_a - z * sin_a;
    z = orig_x * sin_a + z * cos_a; // Use original x
}

static void translate_vertex(float &x, float &y, float &z, float dx, float dy, float dz)
{
    x += dx;
    y += dy;
    z += dz;
}

// 3D vertex structure
struct Vertex3D
{
    float x, y, z;

    Vertex3D() : x(0.0), y(0.0), z(0.0) {}
    Vertex3D(float x, float y, float z) : x(x), y(y), z(z) {}

    // Rotate vertex around Y axis (for sprite facing)
    Vertex3D rotateY(float angle) const
    {
        float cos_a = cosf(angle);
        float sin_a = sinf(angle);
        return Vertex3D(
            x * cos_a - z * sin_a,
            y,
            x * sin_a + z * cos_a);
    }

    // Translate vertex
    Vertex3D translate(float dx, float dy, float dz) const
    {
        return Vertex3D(x + dx, y + dy, z + dz);
    }

    // Scale vertex
    Vertex3D scale(float sx, float sy, float sz) const
    {
        return Vertex3D(x * sx, y * sy, z * sz);
    }

    // Subtraction operator for vector operations
    Vertex3D operator-(const Vertex3D &other) const
    {
        return Vertex3D(x - other.x, y - other.y, z - other.z);
    }
};

// 3D triangle structure
struct Triangle3D
{
    float x1 = 0.0, y1 = 0.0, z1 = 0.0, x2 = 0.0, y2 = 0.0, z2 = 0.0, x3 = 0.0, y3 = 0.0, z3 = 0.0;
    bool visible;
    float distance; // For depth sorting
    bool set;

    Triangle3D() : visible(true), distance(0), set(false) {}

    Triangle3D(const Vertex3D &v1, const Vertex3D &v2, const Vertex3D &v3)
        : visible(true), distance(0), set(true)
    {
        x1 = v1.x;
        y1 = v1.y;
        z1 = v1.z;
        x2 = v2.x;
        y2 = v2.y;
        z2 = v2.z;
        x3 = v3.x;
        y3 = v3.y;
        z3 = v3.z;
    }

    Triangle3D(float x1, float y1, float z1,
               float x2, float y2, float z2,
               float x3, float y3, float z3)
        : x1(x1), y1(y1), z1(z1),
          x2(x2), y2(y2), z2(z2),
          x3(x3), y3(y3), z3(z3),
          visible(true), distance(0), set(true) {}

    // Calculate triangle center for distance sorting
    Vertex3D getCenter() const
    {
        return Vertex3D(
            (this->x1 + this->x2 + this->x3) / 3.0f,
            (this->y1 + this->y2 + this->y3) / 3.0f,
            (this->z1 + this->z2 + this->z3) / 3.0f);
    }

    // Check if triangle is facing the camera (basic backface culling)
    bool isFacingCamera(const Vector &camera_pos) const
    {
        // Calculate triangle normal using cross product
        Vertex3D vert_0 = Vertex3D(this->x1, this->y1, this->z1);
        Vertex3D vert_1 = Vertex3D(this->x2, this->y2, this->z2);
        Vertex3D vert_2 = Vertex3D(this->x3, this->y3, this->z3);
        Vertex3D v1 = vert_1 - vert_0;
        Vertex3D v2 = vert_2 - vert_0;

        // Cross product to get normal (right-hand rule)
        Vertex3D normal = Vertex3D(
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x);

        // Vector from triangle center to camera
        Vertex3D center = getCenter();
        Vertex3D to_camera = Vertex3D(
            camera_pos.x - center.x,
            0.5f - center.y,        // Camera height
            camera_pos.y - center.z // camera_pos.y is Z in world space
        );

        // Dot product - if positive, triangle faces camera
        float dot = normal.x * to_camera.x + normal.y * to_camera.y + normal.z * to_camera.z;
        return dot > 0.0f;
    }
};

enum SpriteType
{
    SPRITE_HUMANOID = 0,
    SPRITE_TREE = 1,
    SPRITE_HOUSE = 2,
    SPRITE_PILLAR = 3,
    SPRITE_CUSTOM = 4
};

class Sprite3D
{
private:
    Triangle3D triangles[MAX_TRIANGLES_PER_SPRITE];
    uint8_t triangle_count;
    Vector position;
    float rotation_y;
    float scale_factor;
    SpriteType type;
    bool active;

public:
    Sprite3D() : triangle_count(0), position(Vector(0, 0)), rotation_y(0),
                 scale_factor(1.0f), type(SPRITE_CUSTOM), active(false) {}

    // Basic sprite operations
    void setPosition(Vector pos) { position = pos; }
    uint8_t getTriangleCount() const { return triangle_count; }
    Vector getPosition() const { return position; }
    void setRotation(float rot) { rotation_y = rot; }
    float getRotation() const { return rotation_y; }
    void setScale(float scale) { scale_factor = scale; }
    float getScale() const { return scale_factor; }
    void setActive(bool state) { active = state; }
    bool isActive() const { return active; }
    SpriteType getType() const { return type; }

    // Add triangle to sprite
    void addTriangle(const Triangle3D &triangle)
    {
        if (triangle_count < MAX_TRIANGLES_PER_SPRITE)
        {
            triangles[triangle_count] = triangle;
            triangle_count++;
        }
    }

    // Clear all triangles
    void clearTriangles()
    {
        triangle_count = 0;
    }

    // Initialize sprite with specific parameters for Entity class
    void initializeAsHumanoid(Vector pos, float height, float rot)
    {
        position = pos;
        rotation_y = rot;
        clearTriangles();
        type = SPRITE_HUMANOID;
        active = true;
        createHumanoid(height);
    }

    void initializeAsTree(Vector pos, float height)
    {
        position = pos;
        rotation_y = 0;
        clearTriangles();
        type = SPRITE_TREE;
        active = true;
        createTree(height);
    }

    void initializeAsHouse(Vector pos, float width, float height, float rot)
    {
        position = pos;
        rotation_y = rot;
        clearTriangles();
        type = SPRITE_HOUSE;
        active = true;
        createHouse(width, height);
    }

    void initializeAsPillar(Vector pos, float height, float radius)
    {
        position = pos;
        rotation_y = 0;
        clearTriangles();
        type = SPRITE_PILLAR;
        active = true;
        createPillar(height, radius);
    }

    Triangle3D getTransformedTriangle(uint8_t index, const Vector &camera_pos) const
    {
        if (index >= triangle_count)
            return Triangle3D();

        Triangle3D transformed = triangles[index];

        // Apply transformations to each vertex
        for (uint8_t v = 0; v < 3; v++)
        {
            switch (v)
            {
            case 0:
            {
                // Scale
                scale_vertex(transformed.x1, transformed.y1, transformed.z1, scale_factor);
                // Rotate around Y axis
                rotateY_vertex(transformed.x1, transformed.z1, rotation_y);
                // Translate to world position
                translate_vertex(transformed.x1, transformed.y1, transformed.z1, position.x, 0, position.y);
                break;
            }
            case 1:
            {
                // Scale
                scale_vertex(transformed.x2, transformed.y2, transformed.z2, scale_factor);
                // Rotate around Y axis
                rotateY_vertex(transformed.x2, transformed.z2, rotation_y);
                // Translate to world position
                translate_vertex(transformed.x2, transformed.y2, transformed.z2, position.x, 0, position.y);
                break;
            }
            case 2:
            {
                // Scale
                scale_vertex(transformed.x3, transformed.y3, transformed.z3, scale_factor);
                // Rotate around Y axis
                rotateY_vertex(transformed.x3, transformed.z3, rotation_y);
                // Translate to world position
                translate_vertex(transformed.x3, transformed.y3, transformed.z3, position.x, 0, position.y);
                break;
            }
            default:
                break;
            };
        }

        // Check if triangle should be rendered
        if (transformed.isFacingCamera(camera_pos))
        {
            // Calculate distance for sorting
            Vertex3D center = transformed.getCenter();
            float dx = center.x - camera_pos.x;
            float dz = center.z - camera_pos.y;
            transformed.distance = sqrtf(dx * dx + dz * dz);

            return transformed;
        }
        return Triangle3D(); // Return empty triangle if not facing camera
    }

    // Create a humanoid character
    void createHumanoid(float height = 1.8f)
    {
        clearTriangles();
        type = SPRITE_HUMANOID;

        float head_radius = height * 0.12f;
        float torso_width = height * 0.20f;
        float torso_height = height * 0.35f;
        float leg_height = height * 0.45f;
        float arm_length = height * 0.25f;

        // Head (simplified as a cube) - positioned at top, wider
        createCube(0, height - head_radius, 0, head_radius * 2, head_radius * 2, head_radius * 1.5f);

        // Torso - positioned in middle, wider and deeper
        createCube(0, leg_height + torso_height / 2, 0, torso_width, torso_height, torso_width * 0.8f);

        // Arms - positioned at shoulder level
        float arm_width = torso_width * 0.35f;
        float arm_y = leg_height + torso_height - arm_length / 2;
        createCube(-torso_width * 0.8f, arm_y, 0, arm_width, arm_length, arm_width);
        createCube(torso_width * 0.8f, arm_y, 0, arm_width, arm_length, arm_width);

        // Legs - positioned so their bottoms touch ground (y=0)
        float leg_width = torso_width * 0.45f;
        createCube(-leg_width * 0.7f, leg_height / 2, 0, leg_width, leg_height, leg_width);
        createCube(leg_width * 0.7f, leg_height / 2, 0, leg_width, leg_height, leg_width);
    }

    // Create a simple tree
    void createTree(float height = 2.0f)
    {
        clearTriangles();
        type = SPRITE_TREE;

        float trunk_width = height * 0.18f;
        float trunk_height = height * 0.4f;
        float crown_width = height * 0.65f;
        float crown_height = height * 0.6f;

        // Trunk (simple cube) - positioned so bottom touches ground (y=0)
        createCube(0, trunk_height / 2, 0, trunk_width, trunk_height, trunk_width);

        // Crown (simple cube representing foliage) - positioned on top of trunk
        createCube(0, trunk_height + crown_height / 2, 0, crown_width, crown_height, crown_width);
    }

    // Create a simple house
    void createHouse(float width = 2.0f, float height = 2.5f)
    {
        clearTriangles();
        type = SPRITE_HOUSE;

        float wall_height = height * 0.7f;
        float roof_height = height * 0.3f;
        float house_width = width * 1.3f;
        float house_depth = width * 1.1f;

        // House base (cube)
        createCube(0, wall_height / 2, 0, house_width, wall_height, house_depth);

        // Roof (triangular prism)
        createTriangularPrism(0, wall_height + roof_height / 2, 0, house_width, roof_height, house_depth);
    }

    // Create a pillar
    void createPillar(float height = 3.0f, float radius = 0.3f)
    {
        clearTriangles();
        type = SPRITE_PILLAR;
        float pillar_radius = radius * 1.5f;

        // Main cylinder - 6 segments = 12 triangles
        createCylinder(0, height / 2, 0, pillar_radius, height, 6);

        // Base - 4 segments = 8 triangles
        createCylinder(0, pillar_radius * 0.4f, 0, pillar_radius * 1.4f, pillar_radius * 0.8f, 4);

        // Top - 4 segments = 8 triangles
        createCylinder(0, height - pillar_radius * 0.4f, 0, pillar_radius * 1.4f, pillar_radius * 0.8f, 4);
    }

private:
    void createCube(float x, float y, float z, float width, float height, float depth)
    {
        float hw = width * 0.5f;
        float hh = height * 0.5f;
        float hd = depth * 0.5f;

        // Render 4 most important faces (skip top and bottom to save triangles)
        // This gives 8 triangles per cube instead of 12

        // Front face (2 triangles)
        addTriangle(Triangle3D(
            x - hw, y - hh, z + hd,
            x + hw, y - hh, z + hd,
            x + hw, y + hh, z + hd));
        addTriangle(Triangle3D(
            x - hw, y - hh, z + hd,
            x + hw, y + hh, z + hd,
            x - hw, y + hh, z + hd));

        // Back face (2 triangles)
        addTriangle(Triangle3D(
            x + hw, y - hh, z - hd,
            x - hw, y - hh, z - hd,
            x - hw, y + hh, z - hd));
        addTriangle(Triangle3D(
            x + hw, y - hh, z - hd,
            x - hw, y + hh, z - hd,
            x + hw, y + hh, z - hd));

        // Right face (2 triangles)
        addTriangle(Triangle3D(
            x + hw, y - hh, z + hd,
            x + hw, y - hh, z - hd,
            x + hw, y + hh, z - hd));
        addTriangle(Triangle3D(
            x + hw, y - hh, z + hd,
            x + hw, y + hh, z - hd,
            x + hw, y + hh, z + hd));

        // Left face (2 triangles)
        addTriangle(Triangle3D(
            x - hw, y - hh, z - hd,
            x - hw, y - hh, z + hd,
            x - hw, y + hh, z + hd));
        addTriangle(Triangle3D(
            x - hw, y - hh, z - hd,
            x - hw, y + hh, z + hd,
            x - hw, y + hh, z - hd));
    }

    void createCylinder(float x, float y, float z, float radius, float height, uint8_t segments)
    {
        float hh = height * 0.5f;

        // Limit segments to prevent too many triangles
        if (segments > 6)
            segments = 6;

        // Only side faces - no caps to save triangles
        for (uint8_t i = 0; i < segments; i++)
        {
            float angle1 = (float)i * 2.0f * M_PI / segments;
            float angle2 = (float)(i + 1) * 2.0f * M_PI / segments;

            float x1 = x + radius * cosf(angle1);
            float z1 = z + radius * sinf(angle1);
            float x2 = x + radius * cosf(angle2);
            float z2 = z + radius * sinf(angle2);

            // Side face triangles only
            addTriangle(Triangle3D(
                x1, y - hh, z1,
                x2, y - hh, z2,
                x2, y + hh, z2));
            addTriangle(Triangle3D(
                x1, y - hh, z1,
                x2, y + hh, z2,
                x1, y + hh, z1));
        }
    }

    void createSphere(float x, float y, float z, float radius, uint8_t segments)
    {
        // limit segments for sphere to prevent triangle explosion
        if (segments > 4)
            segments = 4;

        for (uint8_t lat = 0; lat < segments / 2; lat++)
        {
            float theta1 = (float)lat * M_PI / (segments / 2);
            float theta2 = (float)(lat + 1) * M_PI / (segments / 2);

            for (uint8_t lon = 0; lon < segments; lon++)
            {
                float phi1 = (float)lon * 2.0f * M_PI / segments;
                float phi2 = (float)(lon + 1) * 2.0f * M_PI / segments;

                // Calculate vertices
                float x1 = x + radius * sinf(theta1) * cosf(phi1);
                float y1 = y + radius * cosf(theta1);
                float z1 = z + radius * sinf(theta1) * sinf(phi1);
                //
                float x2 = x + radius * sinf(theta1) * cosf(phi2);
                float y2 = y + radius * cosf(theta1);
                float z2 = z + radius * sinf(theta1) * sinf(phi2);
                //
                float x3 = x + radius * sinf(theta2) * cosf(phi1);
                float y3 = y + radius * cosf(theta2);
                float z3 = z + radius * sinf(theta2) * sinf(phi1);
                //
                float x4 = x + radius * sinf(theta2) * cosf(phi2);
                float y4 = y + radius * cosf(theta2);
                float z4 = z + radius * sinf(theta2) * sinf(phi2);

                // Add triangles
                if (lat > 0)
                {
                    addTriangle(Triangle3D(x1, y1, z1, x2, y2, z2, x3, y3, z3));
                }
                if (lat < segments / 2 - 1)
                {
                    addTriangle(Triangle3D(x2, y2, z2, x4, y4, z4, x3, y3, z3));
                }
            }
        }
    }

    void createTriangularPrism(float x, float y, float z, float width, float height, float depth)
    {
        float hw = width * 0.5f;
        float hh = height * 0.5f;
        float hd = depth * 0.5f;

        // Front triangle
        addTriangle(Triangle3D(
            x - hw, y - hh, z + hd,
            x + hw, y - hh, z + hd,
            x, y + hh, z + hd));

        // Back triangle
        addTriangle(Triangle3D(
            x + hw, y - hh, z - hd,
            x - hw, y - hh, z - hd,
            x, y + hh, z - hd));

        // Bottom face
        addTriangle(Triangle3D(
            x - hw, y - hh, z - hd,
            x + hw, y - hh, z - hd,
            x + hw, y - hh, z + hd));
        addTriangle(Triangle3D(
            x - hw, y - hh, z - hd,
            x + hw, y - hh, z + hd,
            x - hw, y - hh, z + hd));

        // Side faces
        addTriangle(Triangle3D(
            x - hw, y - hh, z + hd,
            x, y + hh, z + hd,
            x, y + hh, z - hd));
        addTriangle(Triangle3D(
            x - hw, y - hh, z + hd,
            x, y + hh, z - hd,
            x - hw, y - hh, z - hd));

        addTriangle(Triangle3D(
            x, y + hh, z + hd,
            x + hw, y - hh, z + hd,
            x + hw, y - hh, z - hd));
        addTriangle(Triangle3D(
            x, y + hh, z + hd,
            x + hw, y - hh, z - hd,
            x, y + hh, z - hd));
    }
};
