//
//  Level3.hpp
//  SDLProject
//
//  Created by Jeffrey Wang on 7/22/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#pragma once
#include "Scene.h"
class Level3 : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram* program) override;
};
