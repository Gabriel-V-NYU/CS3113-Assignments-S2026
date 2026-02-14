/**

* Author: [Gabriel Valte]

* Assignment: Simple 2D Scene

* Date due: [2/14/2026]

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

enum TILES { T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14 };

// Global Constants
constexpr int   SCREEN_WIDTH  = 1600 / 2,
                SCREEN_HEIGHT = 900 / 2,
                FPS           = 60,
                SIZE          = 200,
                FRAME_LIMIT   = 100;
constexpr float MAX_AMP       = 10.0f,
                RADIUS          = 150.0f, // radius of the orbit
                ORBIT_SPEED     = 0.015f;  // the speed at which the triangle will travel its orbit

constexpr char    BG_COLOUR[] = "#000000";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 TOP_LEFT      = { SCREEN_WIDTH / 8, SCREEN_HEIGHT / 4 };

constexpr Vector2 BASE_SIZE   = { (float) SIZE, (float) SIZE };

constexpr char SARDINES[] = "assets/sardines.png";
constexpr char WINE[] = "assets/portWine.png";
constexpr char TILES[] = "assets/portoTiles.png";
constexpr char PASTEL_DE_NATA[] = "assets/pastelDeNata.png";


// Global Variables
Texture2D gSardinesTexture;
Texture2D gWineTexture;
Texture2D gTilesTexture;
Texture2D gPastelTexture;

Vector2   gPosition      = ORIGIN;
Vector2   gWinePosition      = ORIGIN;
Vector2   gTilePosition      = TOP_LEFT;

Vector2   gScale         = BASE_SIZE;

AppStatus gAppStatus     = RUNNING;
float     gScaleFactor   = SIZE,
          gAngle         = 0.0f,
          gWineAngle = 0.0f,
          gPulseTime     = 0.0f,
          gMultiplier = 1.2f,
          gPreviousTicks = 0.0f,

          gCharCoordX = 0.0f,
          gCharCoordY = 0.0f,

          gTileCoordX = 0.0f,
          gTileCoordY = 0.0f,
          gCurrentTile = T1,
          gTileSize = 0.25,

          gOrbitLocation = 0.0f;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Welcome to Porto");

    gSardinesTexture = LoadTexture(SARDINES);
    gWineTexture = LoadTexture(WINE);
    gTilesTexture = LoadTexture(TILES);
    gPastelTexture = LoadTexture(PASTEL_DE_NATA);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // Calculate delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // Pulse
    gPulseTime += gMultiplier * deltaTime;
    gScale = {
        BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };
    //std::cout << gWineTexture.width << std::endl;

    // Switch Tile
    if(gPulseTime >= 2.5 || gPulseTime <= 0 ) {
        if(gCurrentTile == T1){ 
            gCurrentTile = T2; 
            gTileCoordX = static_cast<float>(gTilesTexture.width) / 4; 
            gTileCoordY = 0.0f; 
        }
        else if(gCurrentTile == T2) { 
            gCurrentTile = T3; 
            gTileCoordX = 0.0f; 
            gTileCoordY = static_cast<float>(gTilesTexture.height) / 4; 
        }
        else if(gCurrentTile == T3) { 
            gCurrentTile = T4; 
            gTileCoordX = static_cast<float>(gTilesTexture.width) / 4; 
            gTileCoordY = static_cast<float>(gTilesTexture.height) / 4; 
        }
        else if(gCurrentTile == T4) { 
            gCurrentTile = T1; 
            gTileCoordX = 0.0f; 
            gTileCoordY = 0.0f; 
        }
        gMultiplier = gMultiplier * -1 ;
    };

    //if(gTileSize <= )
    // Orbit
    gOrbitLocation += ORBIT_SPEED;
    gWinePosition.x = ORIGIN.x + RADIUS*cos(gOrbitLocation);
    gWinePosition.y = ORIGIN.y + RADIUS*sin(gOrbitLocation);

    gWineAngle += cos(gOrbitLocation);

    

}

void sardines()
{
    // What part of the texture is showing
    Rectangle TextureArea = {
            // where it starts drawing, from top left
            gCharCoordX, gCharCoordY,

            // how big it is
            static_cast<float>(gSardinesTexture.width),
            static_cast<float>(gSardinesTexture.height)
        };

    // Where the texture will show up on screen
    Rectangle destinationArea = {
        gPosition.x,
        gPosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(gScale.x) / 2 ,
        static_cast<float>(gScale.y) / 2
    };

    // Render the texture on screen
    DrawTexturePro(
        gSardinesTexture, 
        TextureArea, 
        destinationArea, 
        objectOrigin, 
        gAngle, 
        PURPLE
    );
}

void wine()
{
    // What part of the texture is showing
    Rectangle TextureArea = {
            // where it starts drawing, from top left
            gCharCoordX + cos(gPulseTime) , gCharCoordY + sin(gPulseTime),

            // how big it is
            static_cast<float>(gWineTexture.width),
            static_cast<float>(gWineTexture.height)
        };

    // Where the texture will show up on screen
    Rectangle destinationArea = {
        gWinePosition.x,
        gWinePosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(gScale.x) / 2 ,
        static_cast<float>(gScale.y) / 2
    };

    // Render the texture on screen
    DrawTexturePro(
        gWineTexture, 
        TextureArea, 
        destinationArea, 
        objectOrigin, 
        gWineAngle, 
        PURPLE
    );
}

void tiles()
{
    Rectangle TextureArea = {
        // where it starts drawing, from top left
        gTileCoordX, gTileCoordY,

        // how big it is
        static_cast<float>(gTilesTexture.width) * gTileSize,
        static_cast<float>(gTilesTexture.height) * gTileSize
    };

    // Where the texture will show up on screen
    Rectangle destinationArea = {
        gTilePosition.x,
        gTilePosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(gScale.x) / 2 ,
        static_cast<float>(gScale.y) / 2
    };

    // Render the texture on screen
    DrawTexturePro(
        gTilesTexture, 
        TextureArea, 
        destinationArea, 
        objectOrigin, 
        gAngle, 
        PURPLE
    );
}

void pastel() {
    Rectangle TextureArea = {
        // where it starts drawing, from top left
        gCharCoordX + 500, gCharCoordY + 500,

        // how big it is
        static_cast<float>(gPastelTexture.width),
        static_cast<float>(gPastelTexture.height)
    };

    // Where the texture will show up on screen
    Rectangle destinationArea = {
        gTilePosition.x,
        gTilePosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(gScale.x) / 2 ,
        static_cast<float>(gScale.y) / 2
    };

    // Render the texture on screen
    DrawTexturePro(
        gPastelTexture, 
        TextureArea, 
        destinationArea, 
        objectOrigin, 
        gAngle, 
        PURPLE
    );
}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    DrawText("Welcome to Porto, Portugal (its night time)", 200, 300, 25, WHITE);
    DrawText("Why is this so hard", 400, 400, 20, PURPLE);
    tiles();
    pastel();
    sardines();
    wine();


    

    EndDrawing();
}

void shutdown() { 
    CloseWindow(); 
    UnloadTexture(gSardinesTexture); 
    UnloadTexture(gWineTexture); 
    UnloadTexture(gTilesTexture);
    UnloadTexture(gPastelTexture);
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