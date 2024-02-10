/**
 * Header file for all model classes
 * @file Model.h
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <QString>
#include <QTextBrowser>

struct Position {
    int x;
    int y; 
};

struct Mapsize {
    int width;
    int height;
};

typedef enum {
    EMPTY,
    WALL,
    KEY,
    START,
    TARGET,
    GHOST,
    INVALID
} MapObject;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
} Direction;

typedef enum {
    RUNNING,
    PAUSED,
    OVER,
    WIN
} GameState;

/**
 * Character class
 */
class Character {
private:
    Position pos;
    Direction dir;
    int steps = 0;
public:
    /**
     * Constructor for Character class
     */
    Character();

    /**
     * Function to set start position
     * @param pos start position
     */
    void start_position(Position pos);

    /**
     * Function to get direction
     * @return Direction
     */
    Direction get_direction();

    /**
     * Function to set direction
     * @param dir direction
     */
    void set_direction(Direction dir);

    /**
     * Function to get x position
     * @return int
     */
    int get_position_x();

    /**
     * Function to get y position
     * @return int
     */
    int get_position_y();

    /**
     * Function to get next position
     * @return Position
     */
    Position get_next_position();

    /**
     * Function to update position
     */
    void update_position();

    /**
     * Function to get steps
     * @return int
     */
    int get_steps();

    /**
     * Function to increment steps
     */
    void inc_steps();
};

/**
 * Pacman class
 */
class Pacman : public Character {
private:
    int health;
public:
    /**
     * Constructor for Pacman class
     * @param health health of pacman
     */
    Pacman(int health);

    /**
     * Function to get health
     * @return int
     */
    int get_health();

    /**
     * Function to decrement health
     */
    void take_damage();
};

/**
 * Ghost class
 */
class Ghost : public Character {
public:
    /**
     * Constructor for Ghost class
     */
    Ghost();

    /**
     * Function to calculate direction
     * @param map_layout map layout
     * @param map_size map size
     */
    void calculate_direction(std::vector<std::vector<MapObject>> map_layout, Mapsize map_size);
};

/**
 * StaticMapObjects class
 */
class StaticMapObjects {
private:
    Position pos;
public:
    /**
     * Constructor for StaticMapObjects class
     * @param pos position
     */
    StaticMapObjects(Position pos);

    /**
     * Function to get x position
     * @return int
     */
    int get_position_x();

    /**
     * Function to get y position
     * @return int
     */
    int get_position_y();
};

/**
 * Key class
 */
class Key : public StaticMapObjects {
private:
    bool collected = false;
public:
    /**
     * Constructor for Key class
     * @param pos position
     */
    Key(Position pos);

    /**
     * Function to check if key is collected
     * @return bool
     */
    bool is_collected();

    /**
     * Function to set key as collected
     */
    void collect();
};

/**
 * TargetPos class
 */
class TargetPos : public StaticMapObjects {
public:
    /**
     * Constructor for TargetPos class
     * @param pos position
     */
    TargetPos(Position pos);
};

/**
 * Map class
 */
class Map {
private:
    std::vector <std::vector <MapObject>> map_layout;
public:
    Key* key;
    TargetPos* target_pos;

    /**
     * Constructor for Map class
     */
    Map();

    /**
     * Function to get map layout
     * @return <vector<vector<MapObject>>>
     */
    std::vector<std::vector<MapObject>> get_layout();

    /**
     * Function to load map from input
     * @param width width of map
     * @param height height of map
     * @param map map as a string
     */
    void load_map(int width, int height, std::string map);

    /**
     * Function to convert char to MapObject
     * @param c char to convert
     * @return MapObject
     */
    MapObject char2mapObject(char c);

    /**
     * Function to get map object at position
     * @param x x position
     * @param y y position
     * @return MapObject
     */
    MapObject get_object(int x, int y);

    /**
     * Function to free memory of map objects
     */
    void free_map_objects();
};

/**
 * Game class
 */
class Game {
private:
    Mapsize map_size;
    GameState game_state;
    int ghost_count = 0;
    bool contains_key = false;
    void init_pacman(int health);
    void init_ghosts(std::vector<Position> ghost_positions);
public:
    Pacman* pacman;
    Ghost** ghosts;
    Map* map;

    /**
     * Constructor for Game class
     */
    Game();

    /**
     * Function to initialize map
     */
    void init_map();

    /**
     * Function to parse map from loaded file
     * @param content map as a string
     * @return int, zero on success, negative on error
     */
    int parse_map(QString &content);

    /**
     * Function to get width of the map
     * @return int
     */
    int get_width();

    /**
     * Function to get height of the map
     * @return int
     */
    int get_height();

    /**
     * Function to get number of ghosts
     * @return int
     */
    int get_ghost_count();

    /**
     * Function to increment ghost count
     */
    void inc_ghost_count();

    /**
     * Function to get state of the game
     * @return GameState
     */
    GameState get_gamestate();

    /**
     * Function to set state of the game
     * @param state GameState
     */
    void set_gamestate(GameState state);

    /**
     * Function to calculate player collisions and move the player
     */
    void player_collision();

    /**
     * Function to calculate ghost collisions and move the ghosts
     */
    void ghost_collision();

    /**
     * Function to free memory of game objects
     */
    void free_objects();
};

#endif // MODEL_H
