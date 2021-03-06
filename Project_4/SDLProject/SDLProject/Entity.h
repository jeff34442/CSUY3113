//
//  Entity.hpp
//  SDLProject
//
//  Created by Jeffrey Wang on 7/8/20.
//  Copyright © 2020 ctg. All rights reserved.
//

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

enum EntityType { PLAYER, BLOCK, ENEMY, FIRE };

enum AIType { ZUBAT, CATERPIE, SQUIRTLE };

enum Direction { LEFT, RIGHT };

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    Direction direction;
    glm::vec3 position;
    glm::vec3 acceleration;
    glm::vec3 velocity;

    float width = 0.5;
    float height = 0.5;
    int type;
    int enemyType;
    int timer = 0;

    GLuint textureID;

    glm::mat4 modelMatrix;

    bool isActive = true;
    bool isDead = false;

    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedBottom = false;
    bool platformTouched = false;
    bool jump = false;
    bool canJump = true;
    bool shoot = false;
    Entity* fire;

    Entity();

    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void Update(float deltaTime, Entity* object1, int object1Count, Entity* object2, int object2Count);
    void Render(ShaderProgram* program);

    void AI(float deltaTime, Entity* objects, int objectCount);
    void AIZubat(float deltaTime);
    void AISquirtle(float deltaTime, Entity* objects, int objectCount);
    void AICaterpie(float deltaTime, Entity* object);
};
