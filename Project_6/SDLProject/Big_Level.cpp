//
//  Big_Level.cpp
//  SDLProject
//
//  Created by Jeffrey Wang on 8/16/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Big_Level.h"
#define ENEMY_COUNT  3
#define BULLET_COUNT 3
Mix_Music* music;

void Big_Level::Initialize(){
    state.nextScene = -1;
    state.start = false;
    
    state.player = new Entity();
    state.player -> entityType = PLAYER;
    state.player -> position = glm::vec3(0);
    state.player -> movement = glm::vec3(0);
    state.player -> speed = 1.0f;
    state.player -> textureID = Util::LoadTexture("SoldierSprite.png");
    state.player -> height = 1.0f;
    state.player -> width = 1.0f;
    state.player -> animRight = new int[1] {2};
    state.player->animLeft = new int[1] {0};
    state.player->animUp = new int[1] {3};
    state.player->animDown = new int[1] {1};
    state.player->animIndicies = state.player->animRight;
    state.player->animFrames = 1;
    state.player ->animIndex = 0;
    state.player ->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 1;
    state.player -> isActive = true;
    
    
    state.enemies = new Entity[ENEMY_COUNT];
    state.bullet = new Entity[BULLET_COUNT];
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("cant_touch_this.mp3");
    //loop music
    Mix_PlayMusic(music, -1);
    
    GLuint enemyTextureID = Util::LoadTexture("zombie.png");
    
    for(int i = 0; i <ENEMY_COUNT;i++){
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = CHASER;
        state.enemies[i].speed = 1;
        float r = -2.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(3.0f)));
        state.enemies[i].position = glm::vec3(r*3.0f + 1.0f, r*2.0f, 0.0f);
        state.enemies[i].animUp = new int[4] {0,1,2,3};
        state.enemies[i].animLeft = new int[4] {36,37,38,39};
        state.enemies[i].animRight = new int[4] {180,181,182,183};
        state.enemies[i].animDown = new int[4] {252,253,254,255};
        state.enemies[i].animFrames = 4;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 36;
        state.enemies[i].animRows = 8;
    }
    
}

void Big_Level::Update(float deltaTime){
    state.player -> Update(deltaTime, state.enemies, ENEMY_COUNT);
    for(int i = 0; i <ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.bullet, BULLET_COUNT);
    }
    //state.player->position = glm::vec3(0.0f,0.0f,0.0f);
    /*if(state.player->direction == UP){
        if(state.player->position.y >= 3.27){
            state.player -> movement.y = 0.0f;
        }else{
            state.player -> movement.y = 1.0f;
            state.player->animIndicies = state.player->animUp;
        }
    }else if(state.player->direction == DOWN){
        if(state.player->position.y <= -3.27){
            state.player->movement.y = 0.0f;
        }else{
            state.player->movement.y = -1.0f;
            state.player->animIndicies = state.player->animDown;
        }
    }else if(state.player->direction == LEFT){
        if(state.player->position.x <= -4.5){
            state.player->movement.x = 0.0f;
        }else{
            state.player->movement.x = -1.0f;
            state.player->animIndicies = state.player->animLeft;
        }
    }else if(state.player->direction == RIGHT){
        if(state.player->position.x >= 4.5){
            state.player->movement.x = 0.0f;
        }else{
            state.player->movement.x = 1.0f;
            state.player->animIndicies = state.player->animRight;
        }
    }*/
    
}

void Big_Level::Render(ShaderProgram* program){
    state.player->Render(program);
    for(int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
