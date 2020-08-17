//
//  Big_Level.hpp
//  SDLProject
//
//  Created by Jeffrey Wang on 8/16/20.
//  Copyright © 2020 ctg. All rights reserved.
//
#pragma once
#include "Scene.h"
class Big_Level : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram* program) override;
};
