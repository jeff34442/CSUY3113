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
//#include "Entity.h"
#include <ctime>
#include <cstdlib>
#include <SDL_mixer.h>
#include <vector>
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#define ENEMY_COUNT 20
//#define BULLET_COUNT 30
#include "Title.h"
#include "Util.h"
#include "Big_Level.h"



Scene* currentScene;
Scene* sceneList[2];
Mix_Chunk* gunSound;
//Mix_Music* music;
GameState state;
SDL_Window* displayWindow;
ShaderProgram program;
glm::mat4 viewMatrix, zombieMatrix, projectionMatrix, soldierMatrix;
bool gameIsRunning = true;
bool start = false;
bool restarting = false;
bool livesUpdate = false;

int bulletIndex = 0;
int bulletCount = 0;
int remainingBullets = 30;
int remainingLives = 3;

void SwitchToScene(Scene* scene){
    currentScene = scene;
    currentScene->Initialize();
}


void ProcessInput(){
    //state.player ->movement = glm::vec3(0.0f);
    
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_LEFT:
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_UP:
                        break;
                    case SDLK_DOWN:
                        break;
                    case SDLK_RETURN:
                        std::cout<<"pressing enter" << std::endl;
                        if(start == false){
                            currentScene->state.start = true;
                            start = true;
                        }
                        break;
                    case SDLK_z:
                        if (start == true && currentScene->state.gameFinished == false) {
                            if ((bulletCount < 3 || (bulletCount == 3 && currentScene->state.bullet[bulletIndex].isActive == false)) && currentScene->state.player->isDead == false) {
                                Mix_PlayChannel(-1, gunSound, 0);
                                currentScene->state.bullet[bulletIndex].position = currentScene->state.player->position;
                                currentScene->state.bullet[bulletIndex].isActive = true;
                                currentScene->state.bullet[bulletIndex].entityType = BULLET;
                                if (currentScene->state.player->direction == LEFT) {
                                    currentScene->state.bullet[bulletIndex].velocity = glm::vec3(-4.0f, 0.0f, 0);
                                    currentScene->state.bullet[bulletIndex].direction = LEFT;
                                }
                                else if (currentScene->state.player->direction == RIGHT) {
                                    currentScene->state.bullet[bulletIndex].velocity = glm::vec3(4.0f, 0.0f, 0);
                                    currentScene->state.bullet[bulletIndex].direction = RIGHT;
                                }
                                currentScene->state.bullet[bulletIndex].textureID = Util::LoadTexture("bullet.png");
                                currentScene->state.bullet[bulletIndex].height = 1.0f;
                                currentScene->state.bullet[bulletIndex].width = 1.0f;
                                bulletIndex += 1;
                                if (bulletIndex == 3) {
                                    bulletIndex = 0;
                                }
                                if (bulletCount != 3) {
                                    bulletCount += 1;
                                }
                            }
                        }
                        break;
                }
                break;
        }
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (start == true && currentScene->state.gameFinished == false) {
            if (keys[SDL_SCANCODE_LEFT] && currentScene->state.player->isDead == false) {
                currentScene->state.player->movement.x = -1.0f;
                currentScene->state.player->direction = LEFT;
            }
            else if (keys[SDL_SCANCODE_RIGHT] && currentScene->state.player->isDead == false) {
                currentScene->state.player->movement.x = 1.0f;
                currentScene->state.player->direction = RIGHT;
            }
            else if (keys[SDL_SCANCODE_DOWN] && currentScene->state.player->isDead == false) {
                currentScene->state.player->movement.y = -1.0f;
                currentScene->state.player->direction = DOWN;
            }
            else if (keys[SDL_SCANCODE_UP] && currentScene->state.player->isDead == false) {
                currentScene->state.player->movement.y = 1.0f;
                currentScene->state.player->direction = UP;
            }
            else {
                if (currentScene->state.start == false) {
                    currentScene->state.player->movement.x = 0.0f;
                }
            }
        }
        /*if(start == true && currentScene -> state.gameFinished == false){
            if(keys[SDL_SCANCODE_UP] && currentScene->state.player->isDead == false){
                if(currentScene ->state.player -> position.y >=3.27){
                    currentScene ->state.player->movement.y = 0.0f;
                    
                }else{
                    currentScene ->state.player->movement.y = 3.0f;
                    currentScene ->state.player->animIndicies = currentScene ->state.player->animUp;
                }
                currentScene ->state.player->direction = UP;
            }else if(keys[SDL_SCANCODE_DOWN] && currentScene->state.player->isDead == false){
                if(currentScene ->state.player -> position.y <= -3.27){
                
                    currentScene ->state.player->movement.y = 0.0f;
                }else{
                    currentScene ->state.player->movement.y = -3.0f;
                    currentScene ->state.player->animIndicies = currentScene ->state.player->animDown;
                }
                currentScene ->state.player->direction = DOWN;
            }else if(keys[SDL_SCANCODE_LEFT] && currentScene->state.player->isDead == false){
                if(currentScene ->state.player -> position.x <= -4.5f){
                        currentScene ->state.player -> movement.x = 0.0f;
                    }else{
                        currentScene ->state.player->movement.x = -3.0f;
                        currentScene ->state.player->animIndicies = currentScene ->state.player->animLeft;
                    }
                    currentScene ->state.player->direction = LEFT;
            }else if(keys[SDL_SCANCODE_RIGHT] && currentScene->state.player->isDead == false){
                if(currentScene ->state.player -> position.x >= 4.5f){
                        currentScene ->state.player->movement.x = 0.0f;
                    }else{
                        currentScene ->state.player -> movement.x = 3.0f;
                        currentScene ->state.player->animIndicies = currentScene ->state.player->animRight;
                    }
                    currentScene ->state.player->direction = LEFT;
                }
        }*/
        
        if(event.type == SDL_QUIT ||event.type == SDL_WINDOWEVENT_CLOSE){
            gameIsRunning = false;
        }
    }
}


void Initialize(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Zombie Survival!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

    #ifdef _WINDOWS
        glewInit();
    #endif
        
    glViewport(0, 0, 640, 480);
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    zombieMatrix = glm::mat4(1.0f);
    soldierMatrix = glm::mat4(1.0f);
     
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f,3.75f,-1.0f,1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    sceneList[0] = new Title();
    sceneList[1] = new Big_Level();
    SwitchToScene(sceneList[0]);
}



void Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);
    for(int i = 0; i < bulletCount; i++){
        currentScene->state.bullet[i].Render(&program);
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

#define FIXED_TIMESTEP 0.0166666f
float lastTicks;
float accumulator = 0.0f;
void Update(){
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if(deltaTime < FIXED_TIMESTEP){
        accumulator = deltaTime;
        return;
    }
    
    while(deltaTime >= FIXED_TIMESTEP){
        currentScene -> Update(deltaTime);
        for(int i = 0; i < bulletCount; i++){
            currentScene ->state.bullet[i].playerX = currentScene ->state.player->position.x;
            currentScene ->state.bullet[i].Update(deltaTime, state.enemies,3);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    
    if(start == true && currentScene->state.start == false){
        if (currentScene->state.player->isDead == true) {
                  currentScene->state.player->textureID = Util::LoadTexture("dead_soldier.png");
                  currentScene->state.player->height = 1.0f;
        }
        
        for(int i = 0; i < ENEMY_COUNT; i++){
            if(currentScene->state.enemies[i].isDead == true){
                currentScene -> state.enemies[i].textureID = Util::LoadTexture("dead_zombie.png");
            }
        }
        if (livesUpdate == false) {
            currentScene->state.player->lives = remainingLives;
            livesUpdate = true;
        }
    }
    
    accumulator = deltaTime;
    
    
    //for(int i = 0; i<ENEMY_COUNT; i++){
    //    state.enemies[i].Update(deltaTime, state.player);
    //}
    
    //state.player->Update(deltaTime, state.player);
    
    
}

void ShutDown(){
    SDL_Quit();
}


int main(int argc, char* argv[]) {
    
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0) {
            if (start == true && currentScene->state.start == false) {
                remainingLives = currentScene->state.player->lives;
                livesUpdate = false;
            }
            
            SwitchToScene(sceneList[currentScene->state.nextScene]);
            bulletIndex = 0;
            bulletCount = 0;
            remainingBullets = 3;
        }
        Render();
    }
    
    ShutDown();
    return 0;
}
