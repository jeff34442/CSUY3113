
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#include <vector>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

//#include "Entity.h"
#include "Util.h"
#include "Map.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Title.h"

Scene* currentScene;

Scene* sceneList[4];

Mix_Chunk* fireSound;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool start = false;
bool restarting = false;
bool livesUpdated = false;

int fireIndex = 0;
int fireCount = 0;
int remainingFire = 3;
int remLives = 3;


ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Project 5: Charizard's Adventure", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    fireSound = Mix_LoadWAV("Fireball.wav");
    

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sceneList[0] = new Title();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
}

// Function to add text
void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {

        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
             offset + (-0.5f * size), 0.5f * size,
             offset + (-0.5f * size), -0.5f * size,
             offset + (0.5f * size), 0.5f * size,
             offset + (0.5f * size), -0.5f * size,
             offset + (0.5f * size), 0.5f * size,
             offset + (-0.5f * size), -0.5f * size,
            });
        texCoords.insert(texCoords.end(), {
        u, v,
        u, v + height,
        u + width, v,
        u + width, v + height,
        u + width, v,
        u, v + height,
            });

    }

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void ProcessInput() {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_RETURN:
                if (start == false) {
                    currentScene->state.start = true;
                    start = true;
                }
                break;

            case SDLK_z:
                // Makes the player jump
                if (start == false) {

                }
                else {
                    if (currentScene->state.player->canJump == true && currentScene->state.player->isDead == false) {
                        currentScene->state.player->jump = true;
                    }
                }
                
                break;
            case SDLK_x:
                // Makes the player shoot a fireball

                if (start == true && currentScene->state.gameFinished == false) {
                    if ((fireCount < 3 || (fireCount == 3 && currentScene->state.fire[fireIndex].isActive == false)) && currentScene->state.player->isDead == false) {
                        Mix_PlayChannel(-1, fireSound, 0);
                        currentScene->state.fire[fireIndex].position = currentScene->state.player->position;
                        currentScene->state.fire[fireIndex].isActive = true;
                        currentScene->state.fire[fireIndex].entityType = FIRE;
                        if (currentScene->state.player->direction == LEFT) {
                            currentScene->state.fire[fireIndex].velocity = glm::vec3(-4.0f, 0.0f, 0);
                            currentScene->state.fire[fireIndex].direction = LEFT;
                        }
                        else if (currentScene->state.player->direction == RIGHT) {
                            currentScene->state.fire[fireIndex].velocity = glm::vec3(4.0f, 0.0f, 0);
                            currentScene->state.fire[fireIndex].direction = RIGHT;
                        }
                        currentScene->state.fire[fireIndex].textureID = Util::LoadTexture("fireball.png");
                        currentScene->state.fire[fireIndex].height = 1.0f;
                        currentScene->state.fire[fireIndex].width = 1.0f;
                        fireIndex += 1;
                        if (fireIndex == 3) {
                            fireIndex = 0;
                        }
                        if (fireCount != 3) {
                            fireCount += 1;
                        }
                    }
                }

                
                break;
            }
            
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    // Left and right keys move the player
    if (start == true && currentScene->state.gameFinished == false) {
        if (keys[SDL_SCANCODE_LEFT] && currentScene->state.player->isDead == false) {
            currentScene->state.player->velocity.x = -2.0f;
            currentScene->state.player->direction = LEFT;
        }
        else if (keys[SDL_SCANCODE_RIGHT] && currentScene->state.player->isDead == false) {
            currentScene->state.player->velocity.x = 2.0f;
            currentScene->state.player->direction = RIGHT;
        }
        else {
            if (currentScene->state.start == false) {
                currentScene->state.player->velocity.x = 0.0f;
            }
        }
    }
    
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        currentScene->Update(deltaTime);
        for (int i = 0; i < fireCount; i++) {
            currentScene->state.fire[i].playerX = currentScene->state.player->position.x;
            currentScene->state.fire[i].Update(deltaTime, currentScene->state.map, currentScene->state.enemy, 3);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    
    if (start == true && currentScene->state.start == false) {
        if (currentScene->state.player->isDead == true) {
            currentScene->state.player->textureID = Util::LoadTexture("charizard_sad.png");
            currentScene->state.player->height = 1.5f;
        }
        if (currentScene->state.enemy[0].isDead == true) {
            currentScene->state.enemy[0].textureID = Util::LoadTexture("sad_caterpie.png");
        }
        if (currentScene->state.enemy[1].isDead == true) {
            currentScene->state.enemy[1].textureID = Util::LoadTexture("sad_squirtle.png");
        }
        if (currentScene->state.enemy[2].isDead == true) {
            currentScene->state.enemy[2].textureID = Util::LoadTexture("sad_zubat.png");
        }

        if (livesUpdated == false) {
            currentScene->state.player->lives = remLives;
            livesUpdated = true;
        }
    }
    
    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    if (start == true && currentScene->state.start == false) {
        if (currentScene->state.player->position.x > 5) {
            viewMatrix = glm::translate(viewMatrix,
                glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        }
        else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);

    for (int i = 0; i < fireCount; i++) {
        currentScene->state.fire[i].Render(&program);
    }

    
    if (start == true && currentScene->state.start == false) {

        if (currentScene->state.player->isDead == true) {
            Util::DrawText(&program, Util::LoadTexture("Font.png"), "Game Over", 0.6f, -0.3f, glm::vec3(currentScene->state.player->position.x, -3.0f, 0.0f));
        }
        
        else if (currentScene->state.gameFinished == true) {
            Util::DrawText(&program, Util::LoadTexture("Font.png"), "You Win!", 0.6f, -0.3f, glm::vec3(currentScene->state.player->position.x, -3.0f, 0.0f));
        }
        
    }
    
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0) {
            if (start == true && currentScene->state.start == false) {
                remLives = currentScene->state.player->lives;
                livesUpdated = false;
            }
            
            SwitchToScene(sceneList[currentScene->state.nextScene]);
            fireIndex = 0;
            fireCount = 0;
            remainingFire = 3;
        }
        Render();
    }

    Shutdown();
    return 0;
}
