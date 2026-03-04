#include "engine/entity.hpp"
#include "engine/game.hpp"
#include "engine/level.hpp"

// Default Constructor
Level::Level()
    : name(""),
      gameRef(nullptr),
      size(Vector(0, 0)),
      entity_count(0),
      entities(nullptr),
      _start(nullptr),
      _stop(nullptr)
{
}

// Parameterized Constructor
Level::Level(const char *name, const Vector &size, Game *game, void (*start)(Level &), void (*stop)(Level &))
    : name(name),
      gameRef(game),
      size(size),
      entity_count(0),
      entities(nullptr),
      _start(start),
      _stop(stop)
{
}

// Destructor
Level::~Level()
{
    clear();
}

// Clear all entities
void Level::clear()
{
    for (int i = 0; i < entity_count; i++)
    {
        if (entities[i] != nullptr)
        {
            entities[i]->stop(this->gameRef);
            // Only delete entities that are not players (players are managed externally)
            if (!entities[i]->is_player)
            {
                delete entities[i];
            }
            entities[i] = nullptr;
        }
    }
    // Free the dynamic array
    delete[] entities;
    entities = nullptr;
    entity_count = 0;
}

// Get list of collisions for a given entity
Entity **Level::collision_list(Entity *entity, int &count) const
{
    count = 0;
    if (entity_count == 0)
    {
        return nullptr;
    }

    Entity **result = new Entity *[entity_count];
    for (int i = 0; i < entity_count; i++)
    {
        if (entities[i] != nullptr &&
            entities[i] != entity && // Skip self
            is_collision(entity, entities[i]))
        {
            result[count++] = entities[i];
        }
    }
    return result;
}

// Add an entity to the level
void Level::entity_add(Entity *entity)
{
    if (!entity)
    {
        FURI_LOG_E("Level", "Cannot add NULL entity to level");
        return;
    }

    if (!this->gameRef)
    {
        FURI_LOG_E("Level", "Level has no game associated with it");
        return;
    }

    // Allocate a new array with size one greater than the current count
    Entity **newEntities = new Entity *[entity_count + 1];
    if (!newEntities)
    {
        FURI_LOG_E("Level", "Failed to allocate memory for entities array");
        return;
    }

    // Copy the existing entity pointers (if any)
    for (int i = 0; i < entity_count; i++)
    {
        newEntities[i] = entities[i];
    }
    newEntities[entity_count] = entity;

    // Delete the old array
    delete[] entities;
    entities = newEntities;
    entity_count++;

    // Start the new entity
    entity->start(this->gameRef);
    entity->is_active = true;
}

// Remove an entity from the level
void Level::entity_remove(Entity *entity)
{
    if (entity_count == 0)
        return;

    int remove_index = -1;
    for (int i = 0; i < entity_count; i++)
    {
        if (entities[i] == entity)
        {
            remove_index = i;
            break;
        }
    }
    if (remove_index == -1)
        return;

    // Stop and delete the entity (only if it's not a player - players are managed externally)
    entities[remove_index]->stop(this->gameRef);
    if (!entities[remove_index]->is_player)
    {
        delete entities[remove_index];
    }

    // Allocate a new array with one fewer slot (if any remain)
    Entity **newEntities = (entity_count - 1 > 0) ? new Entity *[entity_count - 1] : nullptr;
    // Copy over all pointers except the removed one
    for (int i = 0, j = 0; i < entity_count; i++)
    {
        if (i == remove_index)
            continue;
        newEntities[j++] = entities[i];
    }

    // Free the old array and update state
    delete[] entities;
    entities = newEntities;
    entity_count--;
}

// Check if any entity has collided with the given entity
bool Level::has_collided(Entity *entity) const
{
    for (int i = 0; i < entity_count; i++)
    {
        if (entities[i] != nullptr &&
            entities[i] != entity &&
            is_collision(entity, entities[i]))
        {
            return true;
        }
    }
    return false;
}

// Determine if two entities are colliding
bool Level::is_collision(const Entity *a, const Entity *b) const
{
    return a->position.x < b->position.x + b->size.x &&
           a->position.x + a->size.x > b->position.x &&
           a->position.y < b->position.y + b->size.y &&
           a->position.y + a->size.y > b->position.y;
}

// Render all active entities
void Level::render(Game *game, CameraPerspective perspective, const CameraParams *camera_params)
{
    // on flipper we only need to clear the screen and render the entities
    game->draw->clear(Vector(0, 0), Vector(128, 64), game->bg_color);

    // If using third person perspective but no camera params provided, calculate them from player
    CameraParams calculated_camera_params;
    if (perspective == CAMERA_THIRD_PERSON && camera_params == nullptr)
    {
        // Find the player entity to calculate 3rd person camera
        Entity *player = nullptr;
        for (int i = 0; i < entity_count; i++)
        {
            if (entities[i] != nullptr && entities[i]->is_player)
            {
                player = entities[i];
                break;
            }
        }

        if (player != nullptr)
        {
            // Calculate 3rd person camera position behind the player
            // Use same parameters as Player class for consistency
            float camera_distance = 2.0f; // Closer distance for better visibility

            // Normalize direction vector to ensure consistent behavior
            float dir_length = sqrtf(player->direction.x * player->direction.x + player->direction.y * player->direction.y);
            if (dir_length < 0.001f)
            {
                // Fallback if direction is zero
                dir_length = 1.0f;
                player->direction = Vector(1, 0); // Default forward direction
            }
            Vector normalized_dir = Vector(player->direction.x / dir_length, player->direction.y / dir_length);

            calculated_camera_params.position.x = player->position.x - normalized_dir.x * camera_distance;
            calculated_camera_params.position.y = player->position.y - normalized_dir.y * camera_distance;
            calculated_camera_params.direction = normalized_dir;
            calculated_camera_params.plane = player->plane;
            calculated_camera_params.height = 1.6f;
            camera_params = &calculated_camera_params;
        }
    }

    for (int i = 0; i < entity_count; i++)
    {
        Entity *ent = entities[i];

        if (ent != nullptr && ent->is_active)
        {
            ent->render(game->draw, game);

            if (!ent->is_visible)
            {
                continue; // Skip rendering if entity is not visible
            }

            // Only draw the 2D sprite if it exists
            if (ent->sprite != nullptr)
            {
                game->draw->image(Vector(ent->position.x - game->pos.x, ent->position.y - game->pos.y), ent->sprite, ent->size);
            }

            // Render 3D sprite if it exists
            if (ent->has3DSprite())
            {
                if (perspective == CAMERA_FIRST_PERSON)
                {
                    // First person: render from player's own perspective (original behavior)
                    if (ent->is_player)
                    {
                        // Use entity's own direction and plane for rendering
                        ent->render3DSprite(game->draw, ent->position, ent->direction, ent->plane, 1.5f);
                    }
                    else
                    {
                        // For non-player entities, render from the player's perspective
                        // We need to find the player entity to get the view parameters
                        Entity *player = nullptr;
                        for (int j = 0; j < entity_count; j++)
                        {
                            if (entities[j] != nullptr && entities[j]->is_player)
                            {
                                player = entities[j];
                                break;
                            }
                        }

                        if (player != nullptr)
                        {
                            ent->render3DSprite(game->draw, player->position, player->direction, player->plane, 1.5f);
                        }
                    }
                }
                else if (perspective == CAMERA_THIRD_PERSON && camera_params != nullptr)
                {
                    // Third person: render ALL entities (including player) from the external camera perspective
                    ent->render3DSprite(game->draw, camera_params->position, camera_params->direction, camera_params->plane, camera_params->height);
                }
            }
        }
    }
}

// Start the level
void Level::start()
{
    if (_start != nullptr)
    {
        _start(*this);
    }
}

// Stop the level
void Level::stop()
{
    if (_stop != nullptr)
    {
        _stop(*this);
    }
}

// Update all active entities
void Level::update(Game *game)
{
    for (int i = 0; i < entity_count; i++)
    {
        Entity *ent = entities[i];

        if (ent != nullptr && ent->is_active)
        {
            ent->update(game);

            int count = 0;
            Entity **collisions = collision_list(ent, count);

            for (int j = 0; j < count; j++)
            {
                ent->collision(collisions[j], game);
            }
            delete[] collisions;
        }
    }
}
