//
//  Title.hpp
//  SDLProject
//
//  Created by Jeffrey Wang on 7/22/20.
//  Copyright © 2020 ctg. All rights reserved.
//


#include "Scene.h"
class Title : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram* program) override;

    glm::mat4 modelMatrix;
    GLuint textureID;
    
};
