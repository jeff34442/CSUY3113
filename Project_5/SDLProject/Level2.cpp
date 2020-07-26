//
//  Level2.cpp
//  SDLProject
//
//  Created by Jeffrey Wang on 7/22/20.
//  Copyright © 2020 ctg. All rights reserved.
//
#include "Level2.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8
#define ENEMY_COUNT 3
#define FIRE_COUNT 3

unsigned int level2_data[] =
{
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4,
3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 4,
3, 0, 0, 0, 1, 2, 1, 0, 0, 1, 2, 1, 0, 4,
3, 1, 1, 1, 2, 2, 2, 0, 0, 2, 2, 2, 1, 4,
3, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 4
};

void Level2::Initialize() {
    state.nextScene = -1;
    state.start = false;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 5, 1);

    // Initialize Game Objects

    //Initialize the charizard
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->direction = RIGHT;
    state.player->position = glm::vec3(2.0f, -1.5f, 0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->textureID = Util::LoadTexture("charizard.png");
    state.player->height = 1.0f;
    state.player->width = 1.5f;
    state.player->isActive = true;

    // Initializes enemies
    state.enemy = new Entity[ENEMY_COUNT];
    //Creating caterpie
    state.enemy[0].textureID = Util::LoadTexture("Caterpie.png");
    // Creating squirtle
    state.enemy[1].textureID = Util::LoadTexture("squirtle_squad.png");
    // Creating zubat
    state.enemy[2].textureID = Util::LoadTexture("zubat.png");
    state.enemy[0].position = glm::vec3(-3.5f, -1.2f, 0.0f);
    state.enemy[1].position = glm::vec3(9.5f, -3.0f, 0.0f);
    state.enemy[1].acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    state.enemy[2].position = glm::vec3(5.0f, -4.0f, 0.0f);
    state.enemy[0].aiType = RUNNER;
    state.enemy[1].aiType = JUMPER;
    state.enemy[2].aiType = FLYING;
    state.enemy[0].height = 1.8f;
    state.enemy[0].width = 0.8f;
    state.enemy[1].height = 1.8f;
    state.enemy[1].width = 0.8f;
    state.enemy[2].height = 1.8f;
    state.enemy[2].width = 0.8f;

    state.enemy[0].isActive = false;
    state.enemy[1].isActive = false;
    state.enemy[2].isActive = true;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemy[i].entityType = ENEMY;
    }

    state.fire = new Entity[FIRE_COUNT];
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.map, state.enemy, ENEMY_COUNT);
    if (state.player->position.x >= 12.5f) {
        state.nextScene = 3;
    }

    state.enemy[2].Update(deltaTime, state.map, state.fire, FIRE_COUNT);

}
void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
    state.enemy[2].Render(program);
}
