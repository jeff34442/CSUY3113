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

//Following two lines used to load an image with STB_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

#include <vector>

SDL_Window* displayWindow;
ShaderProgram program;
bool gameIsRunning = true;

vector<ShaderProgram> progVector;
//vector<glm::mat4> modelMatrix;
//vector<GLuint> playerTextureIDs;
GLuint flintTextureID;
GLuint rockTextureID;
glm::mat4 viewMatrix, flintMatrix, projectionMatrix, rockMatrix1, rockMatrix2;




//Use this function to load a texture
GLuint LoadTexture(const char* filePath){
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if(image == NULL){
        cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID); //returns n texture names in textureID
    glBindTexture(GL_TEXTURE_2D, textureID); //Paramters glBindTexture( target, texture)
                                             //glBindTexture lets you create or use a named texture
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    //Allows elements of an image array to be read by shaders

    
    //texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //Parameters: glTexParameteri(texture, pname, param)
    //Function assigns the value or values in params to the texture parameter pname.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}


void ProcessInput(){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}
void Initialize(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Avalanche!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0,0,640,480);
    //ShaderProgram program;
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    //view matrix
    viewMatrix = glm::mat4(1.0f);
    flintMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    glClearColor(0.2f,0.2f,0.2f,1.0f); //black background
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    flintTextureID = LoadTexture("Flintstone.png");
    rockTextureID = LoadTexture("boulder.png");
    
    
}

float lastTicks = 0;
float boulder_rotate = 0;

float flintstone_x = 0;

float rock_move1 = 0;
float rock_move2 = 0;

void Update(){
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    
    lastTicks = ticks;
    flintstone_x += 1.0f * deltaTime;
    rock_move1 += 1.0f * deltaTime;
    rock_move2 += 1.0f * deltaTime;
    boulder_rotate -= 90.0f * deltaTime; //+ to go counter clockwise, - to go clockwise
    
    
    flintMatrix = glm::mat4(1.0f);
    flintMatrix = glm::translate(flintMatrix, glm::vec3(flintstone_x, 0.01f, 0.0f));
    
    rockMatrix1 = glm::mat4(1.0f);
    rockMatrix1 = glm::translate(rockMatrix1, glm::vec3(-2.0f, 0.0f, 0.0f));//make this start from -2 on x axis
    rockMatrix1 = glm::translate(rockMatrix1, glm::vec3(rock_move1, 0.01f, 0.0f));
    rockMatrix1 = glm::rotate(rockMatrix1, glm::radians(boulder_rotate), glm::vec3(0.0f,0.0f,1.0f));
    
    rockMatrix2 = glm::mat4(1.0f);
    rockMatrix2 = glm::translate(rockMatrix2, glm::vec3(rock_move2, 0.01f, 0.0f));
    rockMatrix2 = glm::translate(rockMatrix2, glm::vec3(-4.0f, 0.0f,0.0f));
    rockMatrix2 = glm::rotate(rockMatrix2, glm::radians(boulder_rotate), glm::vec3(0.0f,0.0f,1.0f));
    
    
    cout << "flintstone_x: " << flintstone_x << " rock_move1: " << rock_move1 << " rock_move2: " << rock_move2 << endl;
    if(flintstone_x > 4.8){
        flintstone_x = -4.8;
       }
    if(rock_move1 > 6.2){
        rock_move1 = -3.8;
    }
    if(rock_move2 > 7.2){
        rock_move2 = -2.8;
       }
    
    
}


void drawFlint(){
    program.SetModelMatrix(flintMatrix);
    glBindTexture(GL_TEXTURE_2D, flintTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawRock1(){
    program.SetModelMatrix(rockMatrix1);
    glBindTexture(GL_TEXTURE_2D, rockTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawRock2(){
    program.SetModelMatrix(rockMatrix2);
    glBindTexture(GL_TEXTURE_2D, rockTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render(){
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    drawFlint();
    drawRock1();
    drawRock2();
    
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    
    SDL_GL_SwapWindow(displayWindow);
    
}

void ShutDown(){
    SDL_Quit();
}


int main(int argc, char* argv[]) {
    Initialize();
    
    while(gameIsRunning){
        ProcessInput();
        Update();
        Render();

    }
    
    ShutDown();
    return 0;
}
