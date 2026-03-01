/**
* Author: Gabriel Valte
* Assignment: Pong Clone
* Date due: 2/28/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

enum GameMode { HUMAN, AI };
enum GameState { MENU, PLAYING, GAMEOVER };
enum Player1Momentum { LEFT1, RIGHT1, STEADY1 };
enum Player2Momentum { LEFT2, RIGHT2, STEADY2 };

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 600,
              FPS           = 60,
              MAX_VELOCITY = 300,
              ACCEL_RATE = 2,
              FRICTION = 20,
              MAX_BALLS   = 10;

constexpr Vector2 ORIGIN             = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  P2_SIZE = { 80.0f, 20.0f },
                  P1_SIZE   = { 80.0f, 20.0f },
                  BALL_SIZE = { 20.0f, 20.0f },
                  P2_INIT_POS  = { ORIGIN.x, ORIGIN.y - 275.0f },
                  P1_INIT_POS    = { ORIGIN.x, ORIGIN.y + 275.0f },
                  BALL_INIT_POS = ORIGIN;

constexpr char P1_FP[]   = "assets/game/P1Texture.png";
constexpr char P2_FP[] = "assets/game/P2Texture.png";
constexpr char BALL_FP[] = "assets/game/BallTexture.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f,
          gP1Velocity         = 300,
          gP2Velocity         = 300,
          gP1Accel = 0.0f,
          gP1Score = 0.0f,
          gP2Score = 0.0f;

int gActiveBalls = 1;

struct Ball {
    Vector2 position;
    Vector2 movement;
    bool active;
};

Ball gBalls[MAX_BALLS];
          

Vector2 gP2Position = P2_INIT_POS,
        gP2Scale    = P2_SIZE,
        gP2Direction = { 0.0f, 0.0f },
        
        gP1Position = P1_INIT_POS,
        gP1Scale    = P1_SIZE,
        gP1Direction = { 0.0f, 0.0f },

        gBallScale = BALL_SIZE,
        gBallPosition = BALL_INIT_POS,
        gRandValue = { 1, 1 },
        
        gMouseScale = { 20.0f, 20.0f },
        gMousePosition = GetMousePosition();

Texture2D gP2Texture;
Texture2D gP1Texture;
Texture2D gBallTexture;

GameMode gGameMode = AI;
GameState gGameState = MENU;
Player1Momentum gP1Momentum = STEADY1;
Player2Momentum gP2Momentum = STEADY2;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions
void playerMovement(float deltaTime) {

    if (gP1Momentum == LEFT1) gP1Accel = -ACCEL_RATE;
    else if (gP1Momentum == RIGHT1) gP1Accel = ACCEL_RATE;
    else gP1Accel = 0.0f;

        // Dont let player 1 go past x borders
        if(gP1Position.x > SCREEN_WIDTH - P1_SIZE.x/1.5) gP1Position.x = SCREEN_WIDTH - P1_SIZE.x/1.5;
        if(gP1Position.x < P1_SIZE.x/1.5) gP1Position.x = P1_SIZE.x/1.5;

        // update p1 position
        gP1Position.x += gP1Velocity * gP1Direction.x * deltaTime;

        if(gGameMode == HUMAN) {
            // Dont let player 2 go past x borders
            if(gP2Position.x > SCREEN_WIDTH - P2_SIZE.x/1.5) gP2Position.x = SCREEN_WIDTH - P2_SIZE.x/1.5;
            if(gP2Position.x < P1_SIZE.x/1.5) gP2Position.x = P2_SIZE.x/1.5;

            // update p2 position
            gP2Position.x += gP2Velocity * gP2Direction.x * deltaTime;
        }

        if(gGameMode == AI) {
            // Dont let player 2 go past x borders
            if(gP2Position.x > SCREEN_WIDTH - P2_SIZE.x/1.5) gP2Position.x = SCREEN_WIDTH - P2_SIZE.x/1.5;
            if(gP2Position.x < P1_SIZE.x/1.5) gP2Position.x = P2_SIZE.x/1.5;

            // ai movement
            if(gBalls[0].position.x < gP2Position.x - P2_SIZE.x/2) gP2Direction.x = -1;
            else if(gBalls[0].position.x > gP2Position.x + P2_SIZE.x/2) gP2Direction.x = 1;

            // update p2 position
            gP2Position.x += gP2Velocity * gP2Direction.x * deltaTime;
        }

}

void ballsMove(float deltaTime){
    for (int i = 0; i < gActiveBalls; i++) {
            Ball& ball = gBalls[i];

            ball.position.x += ball.movement.x * deltaTime * gRandValue.x;
            ball.position.y += ball.movement.y * deltaTime * gRandValue.y;

            // Wall collision
            float halfW = gBallScale.x / 2.0f;
            float halfH = gBallScale.y / 2.0f;

            // left wall
            if (ball.position.x - halfW < 0.0f) {
                float overlap = 0.0f - (ball.position.x - halfW);
                ball.position.x += overlap;
                ball.movement.x = -ball.movement.x;
            }

            // right wall
            if (ball.position.x + halfW > SCREEN_WIDTH) {
                float overlap = (ball.position.x + halfW) - SCREEN_WIDTH;
                ball.position.x -= overlap;
                ball.movement.x = -ball.movement.x;
            }

            // top
            if (ball.position.y - halfH < 0.0f) {
                float overlap = 0.0f - (ball.position.y - halfH);
                ball.position.y += overlap;
                ball.movement.y = -ball.movement.y;
                gP1Score += 1;
            }

            // bottom
            if (ball.position.y + halfH > SCREEN_HEIGHT) {
                float overlap = (ball.position.y + halfH) - SCREEN_HEIGHT;
                ball.position.y -= overlap;
                ball.movement.y = -ball.movement.y;
                gP2Score += 1;
            }

            // Player collision
            if (isColliding(&gP1Position, &gP1Scale, &ball.position, &gBallScale)){
                float yDistance = fabs(ball.position.y - gP1Position.y);
                float yOverlap  = fabs(
                    yDistance - (gBallScale.y / 2.0f) - (gP1Scale.y / 2.0f)
                );

                if (ball.movement.y > 0.0f) {
                    ball.position.y -= yOverlap;
                }
                else if (ball.movement.y < 0.0f) {
                    ball.position.y += yOverlap;
                }

                ball.movement.y = -ball.movement.y;

                gRandValue.y = GetRandomValue(1, 3);
                gRandValue.x = GetRandomValue(1, 2);
            }
            if (isColliding(&gP2Position, &gP2Scale, &ball.position, &gBallScale)) {
                float yDistance = fabs(ball.position.y - gP2Position.y);
                float yOverlap  = fabs(
                    yDistance - (gBallScale.y / 2.0f) - (gP2Scale.y / 2.0f)
                );

                if (ball.movement.y > 0.0f) {
                    ball.position.y -= yOverlap;
                }
                else if (ball.movement.y < 0.0f) {
                    ball.position.y += yOverlap;
                }

                ball.movement.y = -ball.movement.y;

                gRandValue.y = GetRandomValue(1, 3);
                gRandValue.x = GetRandomValue(1, 2);
            }
        }
}

bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(postionA->x - positionB->x) - 
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(postionA->y - positionB->y) - 
                      ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position, const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PONG");

    gP2Texture = LoadTexture(P2_FP);
    gP1Texture = LoadTexture(P1_FP);
    gBallTexture = LoadTexture(BALL_FP);

    for (int i = 0; i < MAX_BALLS; i++) {
        gBalls[i].position = BALL_INIT_POS;

        // Slightly different speeds for each ball
        gBalls[i].movement = { 
            80.0f + 20.0f * i, 
            150.0f - 10.0f * i 
        };

        gBalls[i].active = (i == 0); // only first ball active initially
    }

    SetTargetFPS(FPS);

}

void processInput() 
{

    gMousePosition = GetMousePosition();

    if(gGameState == MENU) {
        // switch game mode
        if (IsKeyPressed(KEY_T)) { gGameMode = (gGameMode == AI) ? HUMAN : AI; }
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        if (gGameState == MENU) {
            gGameState = PLAYING;
        }
        else if (gGameState == PLAYING) {
            gGameState = MENU;
        }
        else if (gGameState == GAMEOVER) {
            gP1Score = 0;
            gP2Score = 0;
            gActiveBalls = 1;

            for (int i = 0; i < MAX_BALLS; i++) {
                gBalls[i].position = BALL_INIT_POS;
            }

            gGameState = PLAYING;
        }
    }

    if(IsKeyPressed(KEY_ONE)) gActiveBalls = 1;
    if(IsKeyPressed(KEY_TWO)) gActiveBalls = 2;
    if(IsKeyPressed(KEY_THREE)) gActiveBalls = 3;
    if(IsKeyPressed(KEY_MINUS)) gActiveBalls -= 1;
    if(IsKeyPressed(KEY_EQUAL)) gActiveBalls += 1;

    
    if(gGameState == PLAYING) {
        gP1Direction = { 0.0f, 0.0f };
        gP1Momentum = STEADY1;
        if      (IsKeyDown(KEY_A)) { gP1Direction.x = -1; gP1Momentum = LEFT1; }
        else if (IsKeyDown(KEY_D)) { gP1Direction.x =  1; gP1Momentum = RIGHT1; }
        
        if(gGameMode == HUMAN) {
            gP2Direction = { 0.0f, 0.0f };
            gP2Momentum = STEADY2;
            if      (IsKeyDown(KEY_LEFT)) { gP2Direction.x = -1; gP2Momentum = LEFT2; }
            else if (IsKeyDown(KEY_RIGHT)) { gP2Direction.x =  1; gP2Momentum = RIGHT2; }
        }
    }
    

    // to close the game
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if(gGameState == PLAYING) {
        ballsMove(deltaTime);
        playerMovement(deltaTime);

        if (gP1Score >= 5 || gP2Score >= 5)
        {
            gGameState = GAMEOVER;
            
        }
    }
    
}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    // render P2
    renderObject(&gP2Texture, &gP2Position, &gP2Scale);

    // render P1
    renderObject(&gP1Texture, &gP1Position, &gP1Scale);

    // render Ball
    for (int i = 0; i < gActiveBalls; i++) {
        renderObject(&gBallTexture, &gBalls[i].position, &gBallScale);
    }

    if (gGameState == MENU) {
        DrawText("Welcome to Gabe Pong", 250, 220, 30, WHITE);

        if (gGameMode == HUMAN) { DrawText("Current Mode: Player vs Player", 250, 270, 25, GREEN); }
        else DrawText("Current Mode: Player vs AI", 250, 270, 25, YELLOW);
            
        DrawText("Press T to change game mode", 260, 320, 20, WHITE);
        DrawText("Press ENTER to Start & Pause", 240, 360, 20, WHITE);


        DrawText("(Controls: Left & Right Arrow)", gP2Position.x - 100, gP2Position.y + 20, 20, WHITE);
        DrawText("(Controls: A & D)", gP1Position.x - 70, gP1Position.y - 40, 20, WHITE);
    }

    if(gGameState == GAMEOVER) {
        if(gP1Score >= 5){
            DrawText("Player 1 Wins!", SCREEN_WIDTH/2 - 300, SCREEN_HEIGHT/2 - 30, 25, RED);
            DrawText("Press ENTER to play again", SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 30, 25, PURPLE);
        }
        else if(gP2Score >= 5){
            DrawText("Player 2 Wins!", SCREEN_WIDTH/2 - 300, SCREEN_HEIGHT/2 - 30, 25, BLUE);
            DrawText("Press ENTER to play again", SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 30, 25, PURPLE);
        }
    }

    DrawText(TextFormat("%.0f", gP1Score), 30, SCREEN_HEIGHT/2 + 30, 15, RED);
    DrawText("---", 25, SCREEN_HEIGHT/2, 15, WHITE);
    DrawText(TextFormat("%.0f", gP2Score), 30, SCREEN_HEIGHT/2 - 30, 15, BLUE);
    

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
    UnloadTexture(gP1Texture);
    UnloadTexture(gP2Texture);
    UnloadTexture(gBallTexture);
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}