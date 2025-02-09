/**
 * Work with Character objects as MODEL
 * @file CharacterModel.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "Model.h"

Character::Character() {

    std::cout << "[+] Character object created" << std::endl;
}

void Character::start_position(Position pos) {
    this->pos = pos;

    std::cout << "Start position set" << std::endl;
}

Direction Character::get_direction() {
    return this->dir;
}

void Character::set_direction(Direction dir) {
    this->dir = dir;

    //std::cout << "Direction set" << std::endl;
}   

Position Character::get_next_position() {
    switch (this->get_direction()) {
    case Direction::UP:
        return {this->get_position_x(), this->get_position_y()-1};
    case Direction::DOWN:
        return {this->get_position_x(), this->get_position_y()+1};
    case Direction::LEFT:
        return {this->get_position_x()-1, this->get_position_y()};
    case Direction::RIGHT:
        return {this->get_position_x()+1, this->get_position_y()};
    case Direction::NONE:
        return {this->get_position_x(), this->get_position_y()};
    }
    return {this->get_position_x(), this->get_position_y()};
}

int Character::get_position_x() {
    return this->pos.x;
}
int Character::get_position_y() {
    return this->pos.y;
}

void Character::update_position() {
    switch (this->dir) {
    case Direction::UP:
        this->pos.y--;
        this->inc_steps();
        break;
    case Direction::DOWN:
        this->pos.y++;
        this->inc_steps();
        break;
    case Direction::LEFT:
        this->pos.x--;
        this->inc_steps();
        break;
    case Direction::RIGHT:
        this->pos.x++;
        this->inc_steps();
        break;
    default:
        break;
    }
}

void Character::inc_steps() {
    this->steps++;
}

int Character::get_steps() {
    return this->steps;
}

Pacman::Pacman(int health) {
    this->health = health;

    std::cout << "[+] Pacman object created" << std::endl;
}

void Pacman::take_damage() {
    this->health--;

    std::cout << "Pacman took damage" << std::endl;
}

int Pacman::get_health() {
    return this->health;
}

Ghost::Ghost() {
    std::cout << "[+] Ghost object created" << std::endl;
}

void Ghost::calculate_direction(std::vector<std::vector<MapObject>> map_layout, Mapsize map_size) {
    srand((unsigned)time(NULL) << 16 | rand() ^ (unsigned)time(NULL) % 65536);

    Position current_pos = {this->get_position_x(), this->get_position_y()};

    std::vector<Direction> possible_directions;

    // push available direction into the vector
    if (current_pos.x+1 < map_size.width) {
        if (map_layout[current_pos.y][current_pos.x+1] != MapObject::WALL) {
            possible_directions.push_back(Direction::RIGHT);
        }
    }
    if (current_pos.x-1 >= 0) { 
        if (map_layout[current_pos.y][current_pos.x-1] != MapObject::WALL) {
            possible_directions.push_back(Direction::LEFT);
        }
    }
    if (current_pos.y+1 < map_size.height) {
        if (map_layout[current_pos.y+1][current_pos.x] != MapObject::WALL) {
            possible_directions.push_back(Direction::DOWN);
        }
    }
    if (current_pos.y-1 >= 0) {
        if (map_layout[current_pos.y-1][current_pos.x] != MapObject::WALL) {
            possible_directions.push_back(Direction::UP);
        }
    }
    if (possible_directions.size() == 0) {
        possible_directions.push_back(Direction::NONE);
        this->set_direction(Direction::NONE);
        return;
    }

    /* if ghost can move to the same direction as before,
       it will, to lower difficulty of the game */
    for (int i = 0; i < possible_directions.size(); i++) {
        if (this->get_direction() == possible_directions[i]) {
            this->set_direction(possible_directions[i]);
            return;
        }
    }

    /* if ghost can't move to the same direction, it will
       choose a random direction from the possible directions */
    if (possible_directions.size() >= 1) {
        int random_index = rand() % possible_directions.size();
        this->set_direction(possible_directions[random_index]);
    }
}
