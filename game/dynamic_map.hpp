#pragma once
#include "furi.h"
#include "engine/vector.hpp"
#include "engine/draw.hpp"

#define MAX_MAP_WIDTH 64
#define MAX_MAP_HEIGHT 64
#define MAX_WALLS 100

enum TileType
{
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_DOOR = 2,
    TILE_TELEPORT = 3,
    TILE_ENEMY_SPAWN = 4,
    TILE_ITEM_SPAWN = 5
};

// Wall structure for dynamic walls
struct Wall
{
    Vector start;
    Vector end;
    TileType type;
    uint8_t height;
    bool is_solid;
};

class DynamicMap
{
private:
    uint8_t width;
    uint8_t height;
    // eventually we'll add a color property to each tile
    // and turn it in to a struct
    // but obviously redundant for the Flipper Zero
    // save for the fillIn option,
    // it doesnt look that well on the Flipper Zero
    TileType tiles[MAX_MAP_HEIGHT][MAX_MAP_WIDTH];
    Wall walls[MAX_WALLS];
    uint8_t wall_count;
    const char *name;
    bool fillIn;

public:
    // Constructor
    DynamicMap(const char *name, uint8_t w, uint8_t h, bool addBorder = true, bool fillIn = false) : width(w), height(h), wall_count(0), name(name), fillIn(fillIn)
    {
        memset(tiles, 0, sizeof(tiles));
        if (addBorder)
        {
            this->addBorderWalls();
        }
    }

    // Methods in alphabetical order
    void addBorderWalls()
    {
        // Add walls around the entire map border
        addHorizontalWall(0, width - 1, 0, TILE_WALL);          // Top border
        addHorizontalWall(0, width - 1, height - 1, TILE_WALL); // Bottom border
        addVerticalWall(0, 0, height - 1, TILE_WALL);           // Left border
        addVerticalWall(width - 1, 0, height - 1, TILE_WALL);   // Right border
    }

    void addCorridor(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
    {
        // Simple L-shaped corridor
        if (x1 == x2)
        {
            // Vertical corridor
            uint8_t start_y = (y1 < y2) ? y1 : y2;
            uint8_t end_y = (y1 < y2) ? y2 : y1;
            for (uint8_t y = start_y; y <= end_y; y++)
            {
                setTile(x1, y, TILE_EMPTY);
            }
        }
        else if (y1 == y2)
        {
            // Horizontal corridor
            uint8_t start_x = (x1 < x2) ? x1 : x2;
            uint8_t end_x = (x1 < x2) ? x2 : x1;
            for (uint8_t x = start_x; x <= end_x; x++)
            {
                setTile(x, y1, TILE_EMPTY);
            }
        }
        else
        {
            // L-shaped corridor (horizontal then vertical)
            uint8_t start_x = (x1 < x2) ? x1 : x2;
            uint8_t end_x = (x1 < x2) ? x2 : x1;
            for (uint8_t x = start_x; x <= end_x; x++)
            {
                setTile(x, y1, TILE_EMPTY);
            }

            uint8_t start_y = (y1 < y2) ? y1 : y2;
            uint8_t end_y = (y1 < y2) ? y2 : y1;
            for (uint8_t y = start_y; y <= end_y; y++)
            {
                setTile(x2, y, TILE_EMPTY);
            }
        }
    }

    void addDoor(uint8_t x, uint8_t y)
    {
        setTile(x, y, TILE_DOOR);
    }

    void addHorizontalWall(uint8_t x1, uint8_t x2, uint8_t y, TileType type = TILE_WALL)
    {
        for (uint8_t x = x1; x <= x2; x++)
        {
            setTile(x, y, type);
        }
    }

    void addRoom(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool add_walls = true)
    {
        // Clear the room area
        for (uint8_t y = y1; y <= y2; y++)
        {
            for (uint8_t x = x1; x <= x2; x++)
            {
                setTile(x, y, TILE_EMPTY);
            }
        }

        // Add walls around the room
        if (add_walls)
        {
            addHorizontalWall(x1, x2, y1, TILE_WALL); // Top wall
            addHorizontalWall(x1, x2, y2, TILE_WALL); // Bottom wall
            addVerticalWall(x1, y1, y2, TILE_WALL);   // Left wall
            addVerticalWall(x2, y1, y2, TILE_WALL);   // Right wall
        }
    }

    void addVerticalWall(uint8_t x, uint8_t y1, uint8_t y2, TileType type = TILE_WALL)
    {
        for (uint8_t y = y1; y <= y2; y++)
        {
            setTile(x, y, type);
        }
    }

    void addWall(Vector start, Vector end, TileType type = TILE_WALL, uint8_t height = 255, bool solid = true)
    {
        if (wall_count < MAX_WALLS)
        {
            walls[wall_count].start = start;
            walls[wall_count].end = end;
            walls[wall_count].type = type;
            walls[wall_count].height = height;
            walls[wall_count].is_solid = solid;
            wall_count++;
        }
    }

    uint8_t getBlockAt(uint8_t x, uint8_t y) const
    {
        // Make sure we're checking within bounds of our actual map
        if (x >= width || y >= height)
        {
            return 0x0; // Out of bounds is always empty
        }

        TileType tile = tiles[y][x];
        switch (tile)
        {
        case TILE_WALL:
        case TILE_DOOR:
            return 0xF;
        default:
            return 0x0;
        }
    }

    bool getFillIn() const { return fillIn; }

    uint8_t getHeight() const { return height; }

    // Fill output[y][x] with tile values for the full 64x64 grid.
    // Values: 0 = empty, 1 = wall, 2 = door, 3 = teleport, 4 = enemy spawn, 5 = item spawn
    void getMiniMap(uint8_t output[MAX_MAP_HEIGHT][MAX_MAP_WIDTH]) const
    {
        for (uint8_t y = 0; y < MAX_MAP_HEIGHT; y++)
        {
            for (uint8_t x = 0; x < MAX_MAP_WIDTH; x++)
            {
                if (x < width && y < height)
                    output[y][x] = (uint8_t)tiles[y][x];
                else
                    output[y][x] = 0;
            }
        }
    }

    const char *getName() const { return name; }

    TileType getTile(uint8_t x, uint8_t y) const
    {
        if (x >= width || y >= height)
        {
            return TILE_EMPTY; // Out of bounds = empty (no walls)
        }
        return tiles[y][x];
    }

    uint8_t getWidth() const { return width; }

    void render(float view_height, Draw *const canvas, Vector player_pos, Vector player_dir, Vector player_plane) const
    {
        // render walls using existing raycasting
        Vector _pixel = Vector(0, 0);
        for (uint8_t x = 0; x < 128; x += 2) // SCREEN_WIDTH with RES_DIVIDER
        {
            float camera_x = 2 * (float)x / 128 - 1; // SCREEN_WIDTH
            float ray_x = player_dir.x + player_plane.x * camera_x;
            float ray_y = player_dir.y + player_plane.y * camera_x;
            uint8_t map_x = (uint8_t)player_pos.x;
            uint8_t map_y = (uint8_t)player_pos.y;

            // Prevent division by zero
            if (ray_x == 0)
                ray_x = 0.00001f;
            if (ray_y == 0)
                ray_y = 0.00001f;

            float delta_x = fabs(1 / ray_x);
            float delta_y = fabs(1 / ray_y);

            int8_t step_x;
            int8_t step_y;
            float side_x;
            float side_y;

            if (ray_x < 0)
            {
                step_x = -1;
                side_x = (player_pos.x - map_x) * delta_x;
            }
            else
            {
                step_x = 1;
                side_x = (map_x + (float)1.0 - player_pos.x) * delta_x;
            }

            if (ray_y < 0)
            {
                step_y = -1;
                side_y = (player_pos.y - map_y) * delta_y;
            }
            else
            {
                step_y = 1;
                side_y = (map_y + (float)1.0 - player_pos.y) * delta_y;
            }

            // Wall detection
            uint8_t depth = 0;
            bool hit = 0;
            bool side;

            // Follow the ray until we hit a wall or reach max depth
            while (!hit && depth < 12) // MAX_RENDER_DEPTH
            {
                // Cast the ray forward
                if (side_x < side_y)
                {
                    side_x += delta_x;
                    map_x += step_x;
                    side = 0;
                }
                else
                {
                    side_y += delta_y;
                    map_y += step_y;
                    side = 1;
                }

                // Always default to empty space
                uint8_t block = 0x0;

                // Check if the coordinates are within the map boundaries
                if (map_x < width && map_y < height)
                {
                    // Use the tile from our actual map data
                    TileType tile = tiles[map_y][map_x];
                    if (tile == TILE_WALL || tile == TILE_DOOR)
                    {
                        block = 0xF;
                    }
                }

                if (block == 0xF) // Hit a wall
                {
                    hit = 1;
                }

                depth++;
            }

            if (hit)
            {
                float distance;

                if (side == 0)
                {
                    distance = fmax(1, (map_x - player_pos.x + (1 - step_x) / 2) / ray_x);
                }
                else
                {
                    distance = fmax(1, (map_y - player_pos.y + (1 - step_y) / 2) / ray_y);
                }

                // rendered line height
                uint8_t line_height = 56 / distance;                                          // RENDER_HEIGHT
                int8_t start_y = (int8_t)(view_height / distance - line_height / 2 + 56 / 2); // RENDER_HEIGHT
                int8_t end_y = (int8_t)(view_height / distance + line_height / 2 + 56 / 2);

                // Clamp to screen bounds
                if (start_y < 0)
                    start_y = 0;
                if (end_y >= 64)
                    end_y = 63;

                // Draw vertical line
                uint8_t dots = end_y - start_y;
                if (fillIn)
                {
                    // Fill in walls pixel-by-pixel
                    for (int i = 0; i < dots; i++)
                    {
                        // Draw the outline pixels
                        _pixel.x = x;
                        _pixel.y = start_y + i;
                        canvas->drawPixel(_pixel, ColorBlack);
                        // Fill in the wall by drawing additional pixels to the right
                        if (x + 1 < 128) // Make sure we don't go out of bounds
                        {
                            _pixel.x = x + 1;
                            canvas->drawPixel(_pixel, ColorBlack);
                        }
                    }
                }
                else
                {
                    // draw the outline
                    for (int i = 0; i < dots; i++)
                    {
                        _pixel.x = x;
                        _pixel.y = start_y + i;
                        canvas->drawPixel(_pixel, ColorBlack);
                    }
                }
            }
        }
    }

    // Draw the minimap onto canvas at pixel position (pos) fitting inside (display_w x display_h) pixels.
    void renderMiniMap(Draw *const canvas, Vector pos, Vector size,
                       Vector player_pos, Vector player_dir = Vector(0, 0)) const
    {
        if (!canvas || size.x == 0 || size.y == 0)
            return;

        // Background
        canvas->fillRect(pos, size, ColorWhite);
        canvas->drawRect(pos, size, ColorBlack);

        // Scale factors: pixels per map tile
        float scale_x = (float)size.x / width;
        float scale_y = (float)size.y / height;

        Vector _pos = Vector(scale_x, scale_y);
        Vector _size = Vector(scale_x, scale_y);
        for (uint8_t ty = 0; ty < height; ty++)
        {
            for (uint8_t tx = 0; tx < width; tx++)
            {
                TileType tile = tiles[ty][tx];
                if (tile == TILE_EMPTY)
                    continue;

                _pos.x = (uint8_t)(pos.x + tx * scale_x);
                _pos.y = (uint8_t)(pos.y + ty * scale_y);
                _size.x = (uint8_t)(scale_x + 0.5f);
                _size.y = (uint8_t)(scale_y + 0.5f);
                if (_size.x < 1)
                    _size.x = 1;
                if (_size.y < 1)
                    _size.y = 1;

                canvas->fillRect(_pos, _size, ColorBlack);
            }
        }

        // Draw player dot + direction arrow
        if (player_pos.x >= 0 && player_pos.y >= 0)
        {
            int16_t ppx = (int16_t)(pos.x + player_pos.x * scale_x);
            int16_t ppy = (int16_t)(pos.y + player_pos.y * scale_y);

            // 3x3 white square so the dot is visible over walls
            _pos.x = ppx - 1;
            _pos.y = ppy - 1;
            _size.x = 3;
            _size.y = 3;
            canvas->fillRect(_pos, _size, ColorWhite);

            // Direction arrow: line from centre out 4px in facing direction
            if (player_dir.x != 0.0f || player_dir.y != 0.0f)
            {
                int16_t tip_x = ppx + (int16_t)(player_dir.x * 4.0f);
                int16_t tip_y = ppy + (int16_t)(player_dir.y * 4.0f);
                _pos.x = ppx;
                _pos.y = ppy;
                _size.x = tip_x;
                _size.y = tip_y;
                canvas->drawLine(_pos, _size, ColorBlack);
                // Arrowhead: two lines from tip back to flanking points
                int16_t base_x = tip_x - (int16_t)(player_dir.x * 2.0f);
                int16_t base_y = tip_y - (int16_t)(player_dir.y * 2.0f);
                int16_t perp_x = (int16_t)(player_dir.y * 2.0f);
                int16_t perp_y = (int16_t)(-player_dir.x * 2.0f);
                _pos.x = tip_x;
                _pos.y = tip_y;
                _size.x = base_x + perp_x;
                _size.y = base_y + perp_y;
                canvas->drawLine(_pos, _size, ColorBlack);
                _size.x = base_x - perp_x;
                _size.y = base_y - perp_y;
                canvas->drawLine(_pos, _size, ColorBlack);
            }

            // Centre dot on top
            _pos.x = ppx;
            _pos.y = ppy;
            canvas->drawPixel(_pos, ColorBlack);
        }
    }

    void setFillIn(bool fill) { fillIn = fill; }

    void setTile(uint8_t x, uint8_t y, TileType type)
    {
        if (x < width && y < height)
        {
            tiles[y][x] = type;
        }
    }
};
