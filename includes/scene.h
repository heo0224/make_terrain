#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "common.h"

class Scene {
public:
    map<Model*, vector<Entity*>> entities;

    Scene() {
        this->entities = map<Model*, vector<Entity*>>();
    }

    void addEntity(Entity* entity) {
        if (entities.find(entity->model) == entities.end()) {
            entities[entity->model] = vector<Entity*>();
        }
        entities[entity->model].push_back(entity);
    }
};

#endif