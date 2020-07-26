//
//  Title.cpp
//  SDLProject
//
//  Created by Jeffrey Wang on 7/22/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Title.h"
#include "ShaderProgram.h"

void Title::Initialize() {
    state.nextScene = -1;
    textureID = Util::LoadTexture("Title.png");
}

void Title::Update(float deltaTime)
{
    if (state.start == true) {
        state.nextScene = 1;
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(9.3f, 9.3f, 1.0f));
}

void Title::Render(ShaderProgram* program) {
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
