//
//  Entity.hpp
//  SDLProject
//
//  Created by Jeffrey Wang on 8/15/20.
//  Copyright © 2020 ctg. All rights reserved.
//
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <string>

enum EntityType {PLAYER, ENEMY, BULLET};
enum Direction {LEFT, RIGHT, UP, DOWN};
enum AIType {CHASER};


class Entity{
public:
    EntityType entityType;
    AIType aiType;

    Direction direction;
    bool collidedTop = false;
    bool collidedBot = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    
    float width = 1;
    float height = 1;
    float playerX = 0;
    
    int lives = 3;
    bool isDead = false;
    bool shoot = false;
    bool isActive = true;
    Entity* bullet;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    int animCols = 0;
    int animRows = 0;
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;
    int *animIndicies = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    float speed;
    GLuint textureID;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    
    
    Entity();
    void Update(float deltaTime, Entity *player, int object2Count);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    
    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void AI(float deltaTime, Entity *player, int objectCount);
    void AIChaser(float deltaTime, Entity *player, int objectCount);
};
