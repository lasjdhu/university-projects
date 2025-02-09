/**
 * Work with Map objects as MODEL
 * @file MapModel.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "Model.h"

Map::Map() {
    std::cout << "[+] Map object created" << std::endl;
}

std::vector<std::vector<MapObject>> Map::get_layout() {
    return this->map_layout;
}

void Map::load_map(int width, int height, std::string map) {
    this->map_layout.resize(height);

    for (int i = 0; i < map.size(); ++i) {
        int pos_x = i % width;
        int pos_y = i / width;

        if (pos_x == 0) {
            this->map_layout[pos_y].resize(width);
        }

        this->map_layout[pos_y][pos_x] = char2mapObject(map[i]);
    }

    std::cout << "[+] Map loaded" << std::endl;
}

MapObject Map::get_object(int x, int y) {
    if (y >= 0 && y < map_layout.size() && x >= 0 && x < map_layout[y].size()) {
        return map_layout[y][x];
    } else {
        return MapObject::INVALID;
    }
}

MapObject Map::char2mapObject(char c) {
    switch (c) {
    case '.':
        return MapObject::EMPTY;
    case 'X':
        return MapObject::WALL;
    case 'S':
        return MapObject::START;
    case 'T':
        return MapObject::TARGET;
    case 'G':
        return MapObject::GHOST;
    case 'K':
        return MapObject::KEY;
    default:
        return MapObject::EMPTY;
    }
}

void Map::free_map_objects() {
    delete this->target_pos;
    delete this->key;
}

StaticMapObjects::StaticMapObjects(Position pos) {
    this->pos = pos;
}

int StaticMapObjects::get_position_x() {
    return this->pos.x;
}

int StaticMapObjects::get_position_y() {
    return this->pos.y;
}

Key::Key(Position pos) : StaticMapObjects(pos) {
    std::cout << "[+] Key object created on (" <<
        pos.x << ", " << pos.y << ")" << std::endl;
}

bool Key::is_collected() {
    return this->collected;
}

void Key::collect() {
    this->collected = true;
}

TargetPos::TargetPos(Position pos) : StaticMapObjects(pos) {
    std::cout << "[+] Target object created on (" <<
        pos.x << ", " << pos.y << ")" << std::endl;
}
