//
//  Entity.cpp
//  SDLProject
//
//  Created by Jeffrey Wang on 8/15/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Entity.h"
#include <string>

Entity::Entity(){
    position = glm::vec3(0);
    speed = 0;
    modelMatrix = glm::mat4(1.0f);
}

void Entity::Update(float deltaTime, Entity *player, int object2Count){
    if(isActive == false)return;
    if(entityType == ENEMY){
        AI(deltaTime, player, object2Count);
    }else if(entityType == BULLET){
        switch(direction){
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
            case UP:
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f,1.0f,1.0f));
                if (fabs(playerX - position.y) > 5.5f) {
                    isActive = false;
                }
                break;
            case DOWN:
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f,-1.0f,1.0f));
                if (fabs(playerX - position.y) > 5.5f) {
                    isActive = false;
                }
                break;
        }
    }
    
    if(animIndicies != NULL){
        animTime += deltaTime;
        if(animTime >= 0.05f){
            animTime = 0.0f;
            animIndex++;
            if(animIndex >= animFrames){
                animIndex = 0;
            }
        }
        
        //CheckCollision(object);
        //CheckCollisionsY(object);
        //velocity.x = movement.x * speed;
        //velocity += acceleration * deltaTime;
        
        //position.y += velocity.y * deltaTime;
        //CheckCollisionsY(player, objectCount);
        
        //position.x += velocity.y * deltaTime;
        //CheckCollisionsX(player, objectCount);
        
        position += movement * speed*deltaTime;
        //std::cout<<"Pos: " << position.y <<std::endl;
        
        
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
    }
    
    
    


}

void Entity::Render(ShaderProgram *program){
    
    
    if(isActive == false)return;
    
    program->SetModelMatrix(modelMatrix);
    if(animIndicies != NULL){
        DrawSpriteFromTextureAtlas(program, textureID, animIndicies[animIndex]);
        return;
    }
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

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index){

    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    float width = 1.0f / (float)animCols; float height = 1.0f / (float)animRows;
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v};
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}


bool Entity::CheckCollision(Entity* other){
    if(isActive == false || other -> isActive == false){
        return false;
    }
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0){
        return true;
    }

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
            //float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (position.y > 0) {
                 if (entityType == PLAYER && object->entityType == ENEMY && object->isDead == false) {
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
                if (entityType == PLAYER && object->entityType == ENEMY && object->isDead == false) {
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
                if (entityType== BULLET && object->entityType == ENEMY && object->isDead == false) {
                    isActive = false;
                    object->isDead = true;
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
                if (entityType == BULLET && object->entityType == ENEMY && object->isDead == false) {
                    isActive = false;
                    object->isDead = true;
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


void Entity::AIChaser(float deltaTime, Entity *player, int objectCount){
    if(isDead == false){
        for(int i = 0; i<objectCount; i++){
            Entity* object = &player[i];
            movement = glm::vec3(0.0f);
            if(player->position.x - position.x < 0.0f){
                if(player-> position.y - position.y > 0.0f){
                    movement.y += 0.5f;
                    movement.x -= 0.5f;
                }else if(player->position.y - position.y < 0.0f){
                    movement.y -= 0.5f;
                    movement.x -= 0.5f;
                }else{
                    movement.x = 0.0f;
                }
                
                animIndicies = animLeft;
            }else if(player -> position.x - position.x > 0.0f){
                if(player->position.y - position.y > 0.0f){
                    movement.y += 0.5f;
                    movement.x += 0.5f;
                }else if(player->position.y - position.y <0.0f){
                    movement.y -= 0.5f;
                    movement.x += 0.5f;
                }else{
                    movement.x = 0.0f;
                }
                animIndicies = animRight;
            }else{
                if(player->position.y - position.y >0.0f){
                    movement.y += 0.5f;
                }else if(player->position.y - position.y < 0.0f){
                    movement.y -= 0.5f;
                }else{
                    movement.x = 0.0f;
                }
                animIndicies = animUp;
            }
        }
    }
    
}

void Entity::AI(float deltaTime, Entity *player, int objectCount){
    switch(aiType){
        case CHASER:
            
            AIChaser(deltaTime, player, objectCount);
            break;
    }
}
