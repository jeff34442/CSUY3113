#include "Entity.h"
#include <string>


Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);

    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) {
        return false;
    }

    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) return true;

    return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                if ((entityType == PLAYER || entityType == ENEMY) && object->entityType == BLOCK) {
                    position.y -= penetrationY;
                    velocity.y = 0;
                }
                else if (entityType == PLAYER && object->entityType == ENEMY && object->isDead == false) {
                    lives -= 1;
                    if (lives > 0) {
                        position = glm::vec3(2.0f, -3.0f, 0);
                        velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        isDead = true;
                    }
                }
                else if (entityType == ENEMY && object->entityType == PLAYER && isDead == false) {
                    object->lives -= 1;
                    if (lives > 0) {
                        object->position = glm::vec3(2.0f, -3.0f, 0);
                        object->velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        object->isDead = true;
                    }
                }
                
            }
            else if (velocity.y < 0) {
                if (entityType == ENEMY && object->entityType == BLOCK) {
                    position.y += penetrationY;
                    velocity.y = 0;
                    canJump = true;
                }
                else if (entityType == PLAYER && object->entityType == ENEMY && object->isDead == false) {
                    lives -= 1;
                    if (lives > 0) {
                        position = glm::vec3(2.0f, -3.0f, 0);
                        velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        isDead = true;
                    }
                }
                else if (entityType == ENEMY && object->entityType == PLAYER && isDead == false) {
                    object->lives -= 1;
                    if (lives > 0) {
                        object->position = glm::vec3(2.0f, -3.0f, 0);
                        object->velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        object->isDead = true;
                    }
                }
                else if (entityType == ENEMY && object->entityType == ENEMY) {

                }
            }
            
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x >= 0) {
                if ((entityType == PLAYER || entityType == ENEMY) && object->entityType == BLOCK) {
                    position.x -= penetrationX;
                    velocity.x = 0;
                }
                else if (entityType== FIRE && object->entityType == ENEMY && object->isDead == false) {
                    isActive = false;
                    object->isDead = true;
                }
                else if (entityType == FIRE && object->entityType == BLOCK) {
                    isActive = false;
                }
                else if (entityType == PLAYER && object->entityType == ENEMY && object->isDead == false) {
                    lives -= 1;
                    if (lives > 0) {
                        position = glm::vec3(2.0f, -3.0f, 0);
                        velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        isDead = true;
                    }
                }
                else if (entityType == ENEMY && object->entityType == PLAYER && isDead == false) {
                    object->lives -= 1;
                    if (lives > 0) {
                        object->position = glm::vec3(2.0f, -3.0f, 0);
                        object->velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        object->isDead = true;
                    }
                }
                else if (entityType == ENEMY && object->entityType == ENEMY) {

                }
            }
            else if (velocity.x < 0) {
                if ((entityType == PLAYER || entityType == ENEMY) && object->entityType == BLOCK) {
                    position.x += penetrationX;
                    velocity.x = 0;
                }
                else if (entityType == FIRE && object->entityType == ENEMY && object->isDead == false) {
                    isActive = false;
                    object->isDead = true;
                }
                else if (entityType == FIRE && object->entityType == BLOCK) {
                    isActive = false;
                }
                else if (entityType == PLAYER && object->entityType == ENEMY && object->isDead == false) {
                    lives -= 1;
                    if (lives > 0) {
                        position = glm::vec3(2.0f, -3.0f, 0);
                        velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        isDead = true;
                    }
                }
                else if (entityType == ENEMY && object->entityType == PLAYER && isDead == false) {
                    object->lives -= 1;
                    if (lives > 0) {
                        object->position = glm::vec3(2.0f, -3.0f, 0);
                        object->velocity = glm::vec3(0.0f, 0.0f, 0);
                    }
                    else {
                        object->isDead = true;
                    }
                }
                else if (entityType == ENEMY && object->entityType == ENEMY) {
                    
                }
            }
        }
    }
}

void Entity::CheckCollisionsY(Map* map)
{
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        canJump = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        canJump = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        canJump = true;
    }
}

void Entity::CheckCollisionsX(Map* map)
{
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (entityType == PLAYER && map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    if (entityType == PLAYER && map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    if (entityType == FIRE && map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        isActive = false;
        velocity.x = 0;
        collidedLeft = true;
    }

    if (entityType == FIRE && map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        isActive = false;
        velocity.x = 0;
        collidedRight = true;
    }
}

void Entity::Update(float deltaTime, Map* map, Entity* object2, int object2Count)
{

    if (isActive == false) {
        return;
    }

    else {
        collidedTop = false;
        collidedBottom = false;
        collidedLeft = false;
        collidedRight = false;

        if (jump == true) {
            jump = false;
            canJump = false;
            if (entityType == PLAYER) {
                velocity.y += 6;
            }
            else if (entityType == ENEMY) {
                velocity.y += 8;
            }
            acceleration.y = -9.81f;
        }

        velocity.y += acceleration.y * deltaTime;
        position.y += velocity.y * deltaTime;

        CheckCollisionsY(map);
        if (entityType == PLAYER) {
            CheckCollisionsY(object2, object2Count);
        }

        position.x += velocity.x * deltaTime;
        if ((entityType == PLAYER && isDead == false) || entityType == FIRE) {
            CheckCollisionsX(map);
        }
        
        if (entityType == PLAYER || entityType == FIRE) {
            CheckCollisionsX(object2, object2Count);
        }
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    if (entityType == ENEMY) {
        switch (direction) {
        case LEFT:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(-2.0f, 2.0f, 1.0f));
            break;

        case RIGHT:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
            break;
        }
        if (isDead == false) {
            AI(deltaTime, object2, object2Count);
        }
        
        else {
            acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
            velocity.x = 0.0f;
        }
    }
    else if (entityType == PLAYER) {
        switch (direction) {
        case LEFT:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(-2.0f, 2.0f, 1.0f));
            break;

        case RIGHT:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
            break;
        }
    }

    else if (entityType == FIRE) {
        switch (direction) {
        case LEFT:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));
            if (fabs(playerX - position.x) > 5.5f) {
                isActive = false;
            }
            break;
        
        case RIGHT:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
            if (fabs(playerX - position.x) > 5.5f) {
                isActive = false;
            }
            break;
        }
    }
}

void Entity::AI(float deltaTime, Entity* objects, int objectCount) {
    switch (aiType) {
    case FLYING:
        AIFlying(deltaTime);
        break;

    case JUMPER:
        AIJumper(deltaTime, objects, objectCount);
        break;

    case RUNNER:
        AIRunner(deltaTime, objects);
        break;
    }

}


void Entity::AIFlying(float deltaTime) {
    if (isDead == false) {
        if (timer == 0) {
            acceleration.y = 0.0f;
            acceleration.x = 1.0f;
            direction = LEFT;
        }
        else if (timer == 50) {
            acceleration.y = 0.0f;
        }
        else if (timer == 100) {
            acceleration.y = 0.0f;
            acceleration.x = -1.0f;
        }
        else if (timer == 150) {
            acceleration.y = 0.0f;
        }
        else if (timer == 200) {
            acceleration.y = 0.0f;
            acceleration.x = -1.0f;
            direction = RIGHT;
        }
        else if (timer == 250) {
            acceleration.y = -1.0f;
        }
        else if (timer == 300) {
            acceleration.y = 0.0f;
            acceleration.x = 1.0f;
        }
        else if (timer == 350) {
            acceleration.y = 0.0f;
        }
        else if (timer == 400) {
            acceleration.y = 0.0f;
            acceleration.x = 1.0f;
            direction = LEFT;
            timer = 0;
        }
        timer += 1;
        velocity.y += acceleration.y * deltaTime;
        velocity.x += acceleration.x * deltaTime;
        position.y += velocity.y * deltaTime;
        position.x += velocity.x * deltaTime;
    }
}


void Entity::AIJumper(float deltaTime, Entity* objects, int objectCount) {
    if (isDead == false) {
        for (int i = 0; i < objectCount; i++)
        {
            Entity* object = &objects[i];
            float xdist = (position.x - object->position.x) - ((width + object->width) / 2.0f);
            if (object->direction == RIGHT && object->isActive == true && xdist <= 1.5f && xdist >= 0 && canJump == true) {
                jump = true;
                canJump = false;
            }
            else if (object->direction == LEFT && object->isActive == true && xdist <= -2.0f && xdist >= -3.0f && canJump == true) {
                jump = true;
                canJump = false;
            }
        }
    
        if (timer == 0) {
            velocity.x = -1.2f;
            direction = LEFT;
        }
        else if (timer == 60) {
            velocity.x = 1.2f;
            direction = LEFT;
        }
        else if (timer == 120) {
            velocity.x = -1.2f;
            timer = 0;
            direction = RIGHT;
        }
        position.x += velocity.x * deltaTime;
        timer += 1;
    }
}


void Entity::AIRunner(float deltaTime, Entity* object) {
    if (isDead == false) {
        float xdist = (position.x - object->position.x) - ((width + object->width) / 2.0f);
        if (timer == 0) {
            velocity.x = 0;
        }
        else if (timer == 30) {
            if (xdist <= 0) {
                velocity.x = 2.0f;
            }
            else if (xdist > 0) {
                velocity.x = -2.0f;
            }
        }
        else if (timer == 60) {
            velocity.x = 0;
            timer = 0;
        }
        position.x += velocity.x * deltaTime;
        timer += 1;
    }
    
}



void Entity::Render(ShaderProgram* program) {
    if (isActive == false) {
        return;
    }

    program->SetModelMatrix(modelMatrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
