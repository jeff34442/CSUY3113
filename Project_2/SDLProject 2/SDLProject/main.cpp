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


SDL_Window* displayWindow;
ShaderProgram program;
bool gameIsRunning = true;
bool gameStart = false;

//Foundation for paddles and ball
GLuint paddleTextureID;
GLuint ballTextureID;

//Set initial positions
glm::mat4 viewMatrix, paddle1Matrix, paddle2Matrix, ballMatrix, projectionMatrix;
glm::vec3 paddle1Pos = glm::vec3(-4.8f, 0.0f, 0.0f);
glm::vec3 paddle2Pos = glm::vec3(4.8f, 0.0f, 0.0f);
glm::vec3 ballPos = glm::vec3(0.0f,0.0f,0.0f);

//Movement vectors
glm::vec3 paddle1Move = glm::vec3(0,0,0);
glm::vec3 paddle2Move = glm::vec3(0,0,0);
glm::vec3 ballMove = glm::vec3(0.0f,0.0f,0.0f);

float ballRotate = 0.0f;
float lastTicks = 0.0f;


//=======================================================================================================
GLuint LoadTexture(const char* filePath){
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if(image == NULL){
        cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
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
//========================================================================================================
void Initialize(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
        
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0,0,640,480);
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    paddle1Matrix = glm::mat4(1.0f);
    paddle2Matrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    glClearColor(0.0f,0.0f,0.0f,1.0f); // background
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    paddleTextureID = LoadTexture("paddle.png");
    ballTextureID = LoadTexture("Ball.png");
        
}
//========================================================================================================
void ProcessInput(){
    paddle1Move = glm::vec3(0);
    paddle2Move = glm::vec3(0);
    //Player 1: W = Up, S = Down
    //Player 2: up arrow = Up, down arrow = Down
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
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
                //Start game
                break;
            }
            break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    //Wait for the spacebar to be pressed to start the game
    if(gameStart == false){
        if(keys[SDL_SCANCODE_SPACE]){
            ballMove = glm::vec3(5.0f,0.0f,0.0f); //start by going to the right by default
            gameStart = true;
        }
    }
    
    
    //Move the paddles
    else if((ballPos.x < -5.0f || ballPos.x > 5.0f) == false){
        //If the ball is still in play then here are the movements for the paddles
        if(keys[SDL_SCANCODE_W]){
            if(paddle1Pos.y < 3.2f){
                paddle1Move.y = 3.0f;
            }else{
                paddle1Move.y = 0.0f;
            }
        }else if(keys[SDL_SCANCODE_S]){
            if(paddle1Pos.y > -3.2f){
                paddle1Move.y = -3.0f;
            }else{
                paddle1Move.y = 0.0f;
            }
        }
        if(keys[SDL_SCANCODE_UP]){
            if(paddle2Pos.y < 3.2f){
                paddle2Move.y = 3.0f;
            }else{
                paddle2Move.y = 0.0f;
            }
        }else if(keys[SDL_SCANCODE_DOWN]){
            if(paddle2Pos.y > -3.2f){
                paddle2Move.y = - 3.0f;
            }else{
                paddle2Move.y = 0.0f;
            }
        }
    }else{
        //if ball is not in play reset
        paddle1Move.y = 0.0f;
        paddle2Move.y = 0.0f;
        if(keys[SDL_SCANCODE_SPACE]){
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            if(ballPos.x < -5.0f){
                ballMove = glm::vec3(5.0f,0.0f,0.0f);
            }else if(ballPos.x > 5.0f){
                ballMove = glm::vec3(-5.0f,0.0f,0.0f);
            }
            //reset
            ballPos = glm::vec3(0.0f,0.0f,0.0f);
            paddle1Pos = glm::vec3(-4.8f,0,0);
            paddle2Pos = glm::vec3(4.8f,0,0);
        }
    }
    
}

double randomBounce(){
    double directions [] = { -1.25,-1.45,-1.6,-1.75, 1.25, 1.45, 1.6,1.75};
    //double directions [] = {0.0,0.1,0.2,0.3,0.5,0.6,0.7,3.0};
    
    int directionIndex;
    srand(unsigned(time(NULL)));
    directionIndex = rand() % 8;
    //cout << "direction index : " << directionIndex << " value: " << directions[directionIndex] << endl;
    return directions[directionIndex];
}



void Update(){
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    paddle1Pos += paddle1Move * deltaTime;
    paddle2Pos += paddle2Move * deltaTime;
    
    if((ballPos.x < -5.0f || ballPos.x > 5.0f) == false){
        ballRotate += 90.0f * deltaTime;
    }else{
        ballRotate += 0.0f;
    }
    
    //Transformations
    ballPos += ballMove * deltaTime;
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, ballPos);
    ballMatrix = glm::rotate(ballMatrix, glm::radians(ballRotate), glm::vec3(0.0f,0.0f,1.0f));
    
    
    paddle1Matrix = glm::mat4(1.0f);
    paddle1Matrix = glm::translate(paddle1Matrix, paddle1Pos);
    paddle1Matrix = glm::scale(paddle1Matrix, glm::vec3(1.0f,1.0f,1.0f));
    
    paddle2Matrix = glm::mat4(1.0f);
    paddle2Matrix = glm::translate(paddle2Matrix, paddle2Pos);
    paddle2Matrix = glm::scale(paddle2Matrix, glm::vec3(1.0f,1.0f,1.0f));
    
    
    //Collision detection
    if(ballMove.x < 0.0f){
        float xLoc = fabs(ballPos.x - paddle1Pos.x) - 0.88f;
        float yLoc = fabs(ballPos.y - paddle1Pos.y) - 1.0f;
        
        if(xLoc < 0 && yLoc < 0){
            ballMove.x = 5.0f;
            ballMove.y = randomBounce();
        }
    }else if(ballMove.x > 0.0f){
        float xLoc = fabs(ballPos.x - paddle2Pos.x) - 0.88f;
        float yLoc = fabs(ballPos.y - paddle2Pos.y) - 0.7f;
        
        if(xLoc < 0 && yLoc < 0){
            ballMove.x = -5.0f;
            ballMove.y = randomBounce();
        }
    }
    
    if(paddle1Pos.y >= 5){
        paddle1Move.y = -2.0f;
    }else if(paddle1Pos.y <= -3.3){
        paddle1Move.y = 2.0f;
    }
    if(paddle2Pos.y >= 3.3){
        paddle2Move.y = -2.0f;
    }else if(paddle2Pos.y <= -3.3){
        paddle2Move.y = 2.0f;
    }
    
    
    
    if(ballPos.y < -3.1f){
        ballMove.y *= -1.0;
        //ballMove.x *= -1;
        //ballMove.y = randomBounce();
    }else if(ballPos.y > 3.1f){
        ballMove.y *= -1.0;
        //ballMove.x *= -1;
        //ballMove.y = randomBounce();
    }
    
    //When someone wins, screen freezes and screen turns green
    if(ballPos.x < -5.0f || ballPos.x > 5.0f){
        ballMove.x = 0.0;
        ballMove.y = 0.0;
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    }
    
}

void drawBall(){
    program.SetModelMatrix(ballMatrix);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawPaddle1(){
    program.SetModelMatrix(paddle1Matrix);
    glBindTexture(GL_TEXTURE_2D, paddleTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawPaddle2(){
    program.SetModelMatrix(paddle2Matrix);
    glBindTexture(GL_TEXTURE_2D, paddleTextureID);
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
       
       drawBall();
       drawPaddle1();
       drawPaddle2();
       
       
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
