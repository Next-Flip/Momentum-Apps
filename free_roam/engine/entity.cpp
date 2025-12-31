#include "engine/entity.hpp"
#include "engine/game.hpp"
#include "engine/sprite3d.hpp"

Entity::Entity(
    const char *name,
    EntityType type,
    Vector position,
    Vector size,
    const uint8_t *sprite_data,
    const uint8_t *sprite_left_data,
    const uint8_t *sprite_right_data,
    void (*start)(Entity *, Game *),
    void (*stop)(Entity *, Game *),
    void (*update)(Entity *, Game *),
    void (*render)(Entity *, Draw *, Game *),
    void (*collision)(Entity *, Entity *, Game *),
    Sprite3DType sprite_3d_type)
{
    this->name = name;
    this->type = type;
    this->position = position;
    this->old_position = position;
    this->direction = Vector(1, 0);
    this->plane = Vector(0, 0);
    this->size = size;
    this->sprite = sprite_data;
    this->sprite_left = sprite_left_data;
    this->sprite_right = sprite_right_data;
    this->_start = start;
    this->_stop = stop;
    this->_update = update;
    this->_render = render;
    this->_collision = collision;
    this->is_active = false;
    this->is_visible = true;
    this->is_player = false;

    // initialize additional properties
    this->state = ENTITY_IDLE;
    this->start_position = position;
    this->end_position = position;
    this->move_timer = 0;
    this->elapsed_move_timer = 0;
    this->radius = this->size.x / 2;
    this->speed = 0;
    this->attack_timer = 0;
    this->elapsed_attack_timer = 0;
    this->strength = 0;
    this->health = 0;
    this->max_health = 0;
    this->level = 0;
    this->xp = 0;
    this->health_regen = 0;
    this->elapsed_health_regen = 0;

    // Initialize 3D sprite properties
    this->sprite_3d = nullptr;
    this->sprite_3d_type = sprite_3d_type;
    this->sprite_rotation = 0.0f;
    this->sprite_scale = 1.0f;

    // Create 3D sprite if type is specified
    if (sprite_3d_type != SPRITE_3D_NONE)
    {
        create3DSprite(sprite_3d_type);
    }
}

Entity::~Entity()
{
    // Clean up 3D sprite first
    destroy3DSprite();

    if (this->sprite != NULL)
    {
        delete this->sprite;
        this->sprite = NULL;
    }
    if (this->sprite_left != NULL)
    {
        delete this->sprite_left;
        this->sprite_left = NULL;
    }
    if (this->sprite_right != NULL)
    {
        delete this->sprite_right;
        this->sprite_right = NULL;
    }
}

void Entity::collision(Entity *other, Game *game)
{
    if (this->_collision != NULL)
    {
        this->_collision(this, other, game);
    }
}

bool Entity::hasChangedPosition() const
{
    return (this->position.x != this->old_position.x) || (this->position.y != this->old_position.y);
}

Vector Entity::position_get()
{
    return this->position;
}

void Entity::position_set(Vector value)
{
    this->old_position = this->position;
    this->position = value;

    // Automatically update 3D sprite position if it exists
    if (sprite_3d != nullptr)
    {
        sprite_3d->setPosition(position);
    }
}

void Entity::render(Draw *draw, Game *game)
{
    if (this->_render != NULL)
    {
        this->_render(this, draw, game);
    }
}

void Entity::start(Game *game)
{
    if (!game)
    {
        FURI_LOG_E("Entity", "Cannot start entity with NULL game pointer");
        return;
    }

    if (this->_start != NULL)
    {
        this->_start(this, game);
    }
    this->is_active = true;
}

void Entity::stop(Game *game)
{
    if (this->_stop != NULL)
    {
        this->_stop(this, game);
    }
    this->is_active = false;
}

void Entity::update(Game *game)
{
    if (this->_update != NULL)
    {
        this->_update(this, game);
    }

    // Update 3D sprite position if it exists
    if (has3DSprite())
    {
        update3DSpritePosition();
    }
}

void Entity::create3DSprite(Sprite3DType type, float height, float width, float rotation)
{
    // Clean up any existing sprite first
    destroy3DSprite();

    sprite_3d_type = type;
    sprite_rotation = rotation;

    switch (type)
    {
    case SPRITE_3D_HUMANOID:
        sprite_3d = new Sprite3D();
        sprite_3d->initializeAsHumanoid(position, height, rotation);
        break;

    case SPRITE_3D_TREE:
        sprite_3d = new Sprite3D();
        sprite_3d->initializeAsTree(position, height);
        break;

    case SPRITE_3D_HOUSE:
        sprite_3d = new Sprite3D();
        sprite_3d->initializeAsHouse(position, width, height, rotation);
        break;

    case SPRITE_3D_PILLAR:
        sprite_3d = new Sprite3D();
        sprite_3d->initializeAsPillar(position, height, width);
        break;

    case SPRITE_3D_CUSTOM:
    case SPRITE_3D_NONE:
    default:
        sprite_3d = nullptr;
        break;
    }
}

void Entity::destroy3DSprite()
{
    if (sprite_3d != nullptr)
    {
        delete sprite_3d;
        sprite_3d = nullptr;
    }
    sprite_3d_type = SPRITE_3D_NONE;
}

void Entity::update3DSpritePosition()
{
    if (sprite_3d != nullptr)
    {
        sprite_3d->setPosition(position);
    }
}

void Entity::set3DSpriteRotation(float rotation)
{
    sprite_rotation = rotation;
    if (sprite_3d != nullptr)
    {
        sprite_3d->setRotation(rotation);
    }
}

void Entity::set3DSpriteScale(float scale)
{
    sprite_scale = scale;
    if (sprite_3d != nullptr)
    {
        sprite_3d->setScale(scale);
    }
}

bool Entity::has3DSprite() const
{
    return sprite_3d != nullptr && sprite_3d_type != SPRITE_3D_NONE;
}

void Entity::render3DSprite(Draw *draw, Vector player_pos, Vector player_dir, Vector player_plane, float view_height) const
{
    if (!has3DSprite())
        return;

    // Get triangles from the 3D sprite and render them
    static Vector screen_points[3];
    static Vector screen_point;
    static Triangle3D triangle;
    //
    const uint8_t triangle_count = sprite_3d->getTriangleCount();
    for (uint8_t i = 0; i < triangle_count; i++)
    {
        triangle = sprite_3d->getTransformedTriangle(i, player_pos);
        if (!triangle.set)
            continue;

        // Only render triangles facing the camera
        if (triangle.isFacingCamera(player_pos))
        {
            // Project 3D vertices to 2D screen coordinates
            bool all_visible = true;

            for (uint8_t j = 0; j < 3; j++)
            {
                switch (j)
                {
                case 0:
                    screen_point = project3DTo2D(triangle.x1, triangle.y1, triangle.z1, player_pos, player_dir, player_plane, view_height);
                    break;
                case 1:
                    screen_point = project3DTo2D(triangle.x2, triangle.y2, triangle.z2, player_pos, player_dir, player_plane, view_height);
                    break;
                case 2:
                    screen_point = project3DTo2D(triangle.x3, triangle.y3, triangle.z3, player_pos, player_dir, player_plane, view_height);
                    break;
                };

                // Check if point is on screen
                if (screen_point.x < 0 || screen_point.x >= 128 || screen_point.y < 0 || screen_point.y >= 64)
                {
                    all_visible = false;
                    break;
                }

                screen_points[j] = screen_point;
            }

            if (all_visible)
            {
                // Fill the triangle
                fillTriangle(draw, screen_points[0], screen_points[1], screen_points[2]);
            }
        }
    }
}

Vector Entity::project3DTo2D(float x, float y, float z, Vector player_pos, Vector player_dir, Vector /*player_plane*/, float view_height) const
{
    // Transform world coordinates to camera coordinates
    float world_dx = x - player_pos.x;
    float world_dz = z - player_pos.y; // player_pos.y is actually the Z coordinate in world space
    float world_dy = y - view_height;  // Height difference from camera

    // Create camera coordinate system
    float forward_x = player_dir.x;
    float forward_z = player_dir.y;
    float right_x = player_dir.y; // Perpendicular to forward
    float right_z = -player_dir.x;

    // Transform to camera space
    float cam_x = world_dx * right_x + world_dz * right_z;
    float cam_z = world_dx * forward_x + world_dz * forward_z;
    float cam_y = world_dy; // Height difference

    // Prevent division by zero and reject points behind camera
    if (cam_z <= 0.1f)
    {
        return Vector(-1, -1); // Invalid point (behind camera)
    }

    // Project to screen coordinates
    float fov_scale = 64.0f;                               // Match the scale used in raycasting
    float screen_x = (cam_x / cam_z) * fov_scale + 64.0f;  // Center at 64 (128/2)
    float screen_y = (-cam_y / cam_z) * fov_scale + 32.0f; // Center at 32 (64/2)

    return Vector(screen_x, screen_y);
}

void Entity::fillTriangle(Draw *const draw, Vector p1, Vector p2, Vector p3) const
{
    // Sort vertices by Y coordinate (p1.y <= p2.y <= p3.y)
    if (p1.y > p2.y)
    {
        Vector temp = p1;
        p1 = p2;
        p2 = temp;
    }
    if (p2.y > p3.y)
    {
        Vector temp = p2;
        p2 = p3;
        p3 = temp;
    }
    if (p1.y > p2.y)
    {
        Vector temp = p1;
        p1 = p2;
        p2 = temp;
    }

    int y1 = (int)p1.y, y2 = (int)p2.y, y3 = (int)p3.y;

    // Handle degenerate case (all points on same line)
    if (y1 == y3)
        return;

    static Vector triangleVect = {0, 0};
    // Fill the triangle using horizontal scanlines
    for (int y = y1; y <= y3; y++)
    {
        if (y < 0 || y >= 64)
            continue; // Skip lines outside screen bounds

        float x_left = 0, x_right = 0;
        bool has_left = false, has_right = false;

        // Find left edge intersection
        if (y3 != y1)
        {
            x_left = p1.x + (p3.x - p1.x) * (y - y1) / (y3 - y1);
            has_left = true;
        }

        // Find right edge intersection
        if (y <= y2)
        {
            // Upper part of triangle (from p1 to p2)
            if (y2 != y1)
            {
                float x_temp = p1.x + (p2.x - p1.x) * (y - y1) / (y2 - y1);
                if (!has_right)
                {
                    x_right = x_temp;
                    has_right = true;
                }
                else
                {
                    // We have both intersections, determine which is left/right
                    if (x_temp < x_left)
                    {
                        x_right = x_left;
                        x_left = x_temp;
                    }
                    else
                    {
                        x_right = x_temp;
                    }
                }
            }
        }
        else
        {
            // Lower part of triangle (from p2 to p3)
            if (y3 != y2)
            {
                float x_temp = p2.x + (p3.x - p2.x) * (y - y2) / (y3 - y2);
                if (!has_right)
                {
                    x_right = x_temp;
                    has_right = true;
                }
                else
                {
                    // We have both intersections, determine which is left/right
                    if (x_temp < x_left)
                    {
                        x_right = x_left;
                        x_left = x_temp;
                    }
                    else
                    {
                        x_right = x_temp;
                    }
                }
            }
        }

        // Draw horizontal line from x_left to x_right
        if (has_left && has_right)
        {
            int start_x = (int)fminf(x_left, x_right);
            int end_x = (int)fmaxf(x_left, x_right);

            // Clamp to screen bounds
            if (start_x < 0)
                start_x = 0;
            if (end_x >= 128)
                end_x = 127;

            for (int x = start_x; x <= end_x; x++)
            {
                triangleVect.x = x;
                triangleVect.y = y;
                draw->drawPixel(triangleVect, ColorBlack);
            }
        }
    }
}
