#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
//Following two lines used to load an image with STB_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
using namespace std;


#define BLOCK_COUNT 29
#define PLATFORM_COUNT 4
struct GameState{
    Entity* player;
    Entity* blocks;
    Entity* platform;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool restarting = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        //assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void Initialize(){
SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
glewInit();
#endif
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(-2.0f, 4.0f, 0);
    state.player->acceleration = glm::vec3(0, -0.04f, 0);
    state.player->textureID = LoadTexture("spaceship.png");
    state.player->height = 0.45f;
    state.player->width = 0.8f;

    // Initialize blocks and landing platform
    state.blocks = new Entity[BLOCK_COUNT];

    GLuint blockTextureID = LoadTexture("block.png");
    for (int i = 0; i < BLOCK_COUNT; i++) {
        state.blocks[i].textureID = blockTextureID;
        state.blocks[i].type = 0;
        if (i >= 0 && i < 8) {
            state.blocks[i].position = glm::vec3(-4.5f, float(i) - 3.25f, 0);
        }
        else if (i >= 8 && i < 16) {
            state.blocks[i].position = glm::vec3(4.5f, float(i - 8) - 3.25f, 0);
        }
        else if (i >= 16 && i < 21) {
            state.blocks[i].position = glm::vec3(float(i - 16) - 3.5f, 1.5f, 0);
        }
        else if (i >= 21 && i < 24) {
            state.blocks[i].position = glm::vec3(3.5f - float(i - 21), -1.25f, 0);
        }
        else if (i >= 24 && i < 27) {
            state.blocks[i].position = glm::vec3(float(i - 24) - 3.5f, -2.25f, 0);
        }
        else if (i == 27) {
            state.blocks[i].position = glm::vec3(3.5f, 1.5f, 0);
        }
        else if (i == 28) {
            state.blocks[i].position = glm::vec3(1.5f, -2.25f, 0);
        }
    }

    for (int i = 0; i < BLOCK_COUNT; i++) {
        state.blocks[i].Update(0, NULL, 0);
    }

    state.platform = new Entity[PLATFORM_COUNT];
    state.platform[0].textureID = LoadTexture("Platform.png");
    state.platform[0].type = 1;
    state.platform[0].position = glm::vec3(-0.75f,-3.0f,0);
    state.platform[1].textureID = LoadTexture("Platform.png");
    state.platform[1].type = 1;
    state.platform[1].position = glm::vec3(-0.15f, -3.0f,0);
    state.platform[2].textureID = LoadTexture("Platform.png");
    state.platform[2].type = 1;
    state.platform[2].position = glm::vec3(0.25f, -3.0f,0);
    state.platform[3].textureID = LoadTexture("Platform.png");
    state.platform[3].type = 1;
    state.platform[3].position = glm::vec3(0.75f, -3.0f,0);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Update(0, NULL, 0);
    }
}

// Function to add text
void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    vector<float> vertices;
    vector<float> texCoords;

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

    }     glm::mat4 modelMatrix = glm::mat4(1.0f);

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

            case SDLK_SPACE:
                // Some sort of action
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    // Left and right keys accelerate the lunar lander to the left and right
    if (state.player->collidedBottom == false && state.player->collidedLeft == false && state.player->collidedRight == false &&
        state.player->platformTouched == false) {
        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->acceleration.x = -1.0f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->acceleration.x = 1.0f;
        }
        else {
            state.player->acceleration.x = 0.0f;
        }
    }
    
    else {
        state.player->acceleration.x = 0;
        state.player->acceleration.y = 0;

        if (keys[SDL_SCANCODE_SPACE] && restarting == true) {
            state.player->position = glm::vec3(-2.0f, 4.0f, 0);
            restarting = false;
            state.player->collidedBottom = false;
            state.player->collidedLeft = false;
            state.player->collidedRight = false;
            state.player->platformTouched = false;
            state.player->acceleration = glm::vec3(0, -0.05f, 0);
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
        state.player->Update(FIXED_TIMESTEP, state.platform, PLATFORM_COUNT);
        state.player->Update(FIXED_TIMESTEP, state.blocks, BLOCK_COUNT);

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    if (state.player->collidedBottom == false && state.player->collidedLeft == false && state.player->collidedRight == false &&
        state.player->platformTouched == false) {
        restarting = true;
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < BLOCK_COUNT; i++) {
        state.blocks[i].Render(&program);
    }

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Render(&program);
    }

    state.player->Render(&program);

    if (restarting == true) {
        if (state.player->collidedBottom == true || state.player->collidedLeft == true || state.player->collidedRight == true) {
            DrawText(&program, LoadTexture("Font.png"), "Mission Failed", 0.6f, -0.3f, glm::vec3(-2.0f, 0.0f, 0));
        }
        else if (state.player->platformTouched == true) {
            DrawText(&program, LoadTexture("Font.png"), "Mission Successful", 0.6f, -0.3f, glm::vec3(-2.0f, 0.0f, 0));
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
        Render();
    }

    Shutdown();
    return 0;
}
