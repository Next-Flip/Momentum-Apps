#pragma once
#include "engine/vector.hpp"
#include "engine/draw.hpp"

// Forward declarations
class Game;
class Sprite3D;
struct Vertex3D;
struct Triangle3D;

#define ENTITY_LEFT Vector(-1, 0)
#define ENTITY_RIGHT Vector(1, 0)
#define ENTITY_UP Vector(0, -1)
#define ENTITY_DOWN Vector(0, 1)

typedef enum
{
    ENTITY_IDLE,
    ENTITY_MOVING,
    ENTITY_MOVING_TO_START,
    ENTITY_MOVING_TO_END,
    ENTITY_ATTACKING,
    ENTITY_ATTACKED,
    ENTITY_DEAD
} EntityState;

typedef enum
{
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_ICON,
    ENTITY_NPC,
    ENTITY_3D_SPRITE
} EntityType;

typedef enum
{
    SPRITE_3D_NONE,
    SPRITE_3D_HUMANOID,
    SPRITE_3D_TREE,
    SPRITE_3D_HOUSE,
    SPRITE_3D_PILLAR,
    SPRITE_3D_CUSTOM
} Sprite3DType;

// Represents a game entity.
class Entity
{
public:
    const char *name;              // The name of the entity.
    Vector position;               // The position of the entity.
    Vector old_position;           // The old position of the entity.
    Vector direction;              // The direction the entity is facing.
    Vector plane;                  // The camera plane perpendicular to the direction.
    bool is_player;                // Indicates if the entity is the player.
    bool position_changed = false; // Indicates if the position of the entity has changed.
    Vector size;                   // The size of the entity.
    const uint8_t *sprite;         // The current displayed sprite of the entity.
    const uint8_t *sprite_left;    // The sprite to switch to when facing left.
    const uint8_t *sprite_right;   // The sprite to switch to when facing right.
    bool is_active;                // Indicates if the entity is active.
    bool is_visible;               // Indicates if the entity is visible (for rendering)
    EntityType type;               // Type of the entity

    // 3D Sprite properties
    Sprite3D *sprite_3d;         // 3D sprite representation (can be null for 2D entities)
    Sprite3DType sprite_3d_type; // Type of 3D sprite
    float sprite_rotation;       // Rotation of the 3D sprite
    float sprite_scale;          // Scale factor for the 3D sprite

    /*
        Additional properties an entity may have.
        These are not controlled by the engine.
    */
    EntityState state;          // Current state of the entity
    Vector start_position;      // Start position of the entity
    Vector end_position;        // End position of the entity
    float move_timer;           // Timer for the entity movement
    float elapsed_move_timer;   // Elapsed time for the entity movement
    float radius;               // Collision radius for the entity
    float speed;                // Speed of the entity
    float attack_timer;         // Cooldown duration between attacks
    float elapsed_attack_timer; // Time elapsed since the last attack
    float strength;             // Damage the entity deals
    float health;               // Health of the entity
    float max_health;           // Maximum health of the entity
    float level;                // Level of the entity
    float xp;                   // Experience points of the entity
    float health_regen;         // player health regeneration rate per second/frame
    float elapsed_health_regen; // time elapsed since last health regeneration

    Entity(
        const char *name,                                     // The name of the entity.
        EntityType type,                                      // The type of the entity.
        Vector position,                                      // The position of the entity.
        Vector size,                                          // The size of the entity.
        const uint8_t *sprite_data,                           // The sprite of the entity.
        const uint8_t *sprite_left_data = NULL,               // The sprite to switch to when facing left.
        const uint8_t *sprite_right_data = NULL,              // The sprite to switch to when facing right.
        void (*start)(Entity *, Game *) = NULL,               // The start function of the entity.
        void (*stop)(Entity *, Game *) = NULL,                // The stop function of the entity.
        void (*update)(Entity *, Game *) = NULL,              // The update function of the entity.
        void (*render)(Entity *, Draw *, Game *) = NULL,      // The render function of the entity.
        void (*collision)(Entity *, Entity *, Game *) = NULL, // The collision function of the entity.
        Sprite3DType sprite_3d_type = SPRITE_3D_NONE          // 3D sprite type (optional)
    );

    virtual ~Entity(); // Virtual destructor for proper inheritance

    virtual void collision(Entity *other, Game *game); // Handles the collision with another entity.
    Vector position_get();                             // Gets the position of the entity.
    void position_set(Vector value);                   // Sets the position of the entity.
    virtual void render(Draw *draw, Game *game);       // called every frame to render the entity.
    virtual void start(Game *game);                    // called when the entity is created.
    virtual void stop(Game *game);                     // called when the entity is destroyed.
    virtual void update(Game *game);                   // called every frame to update the entity.

    // 3D Sprite query and control methods
    bool has3DSprite() const;
    void set3DSpriteRotation(float rotation);
    void set3DSpriteScale(float scale);
    void render3DSprite(Draw *draw, Vector player_pos, Vector player_dir, Vector player_plane, float view_height) const;
    void update3DSpritePosition();

    bool hasChangedPosition() const; // Check if the entity's position has changed

private:
    // Internal 3D sprite management
    void create3DSprite(Sprite3DType type, float height = 2.0f, float width = 1.0f, float rotation = 0.0f);
    void destroy3DSprite();

    // Helper methods for 3D sprite rendering
    Vector project3DTo2D(float x, float y, float z, Vector player_pos, Vector player_dir, Vector player_plane, float view_height) const;
    void fillTriangle(Draw *const draw, Vector p1, Vector p2, Vector p3) const;

    void (*_start)(Entity *, Game *);
    void (*_stop)(Entity *, Game *);
    void (*_update)(Entity *, Game *);
    void (*_render)(Entity *, Draw *, Game *);
    void (*_collision)(Entity *, Entity *, Game *);
};
