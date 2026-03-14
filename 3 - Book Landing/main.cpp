/**
* Author: Gabriel Valte
* Assignment: PROJECT 3: LUNAR LANDER
* Date due: 3/13/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

struct GameState
{
    Entity *player;
    Entity *illiterateChild;
    Entity *movingPlatform;
    Map *map;

    Music bgm;
    Sound jumpSound;

    Font font;
};

enum GameScreen { MENU, PLAYING, CRASHED, CHILD_CRASHED, GAMEWON, OUT_OF_BOUNDS };

GameScreen gGameState = MENU;

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char    BG_COLOUR[]      = "#C0897E";
constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  ATLAS_DIMENSIONS = { 4, 4 };

constexpr int   NUMBER_OF_TILES         = 20,
                NUMBER_OF_BLOCKS        = 3,
                MAX_LANDING_SPEED_X = 20.0f,
                MAX_LANDING_SPEED_Y = 70.0f;
constexpr float TILE_DIMENSION          = 45.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 800.0f,
                HORIZONTAL_THRUST = 30.0f,
                DRAG_FACTOR       = 5.0f,
                ENERGY_EXPENDETURE = 0.5f,
                MAX_ACCEL_X       = 20.0f;

float energy = 1000.0f,
      gChildDirection = 1.0f,
      ACCELERATION_OF_GRAVITY = 0.0f;

constexpr int LEVEL_WIDTH  = 36,
              LEVEL_HEIGHT = 16;
constexpr unsigned int LEVEL_DATA[] = { 
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 129, 129, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 130, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 130, 130, 129, 129, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 130, 130, 130, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 129, 129, 129, 129, 129, 129, 129, 0, 0, 0, 0, 0, 0, 0, 0, 141,
   141, 129, 129, 129, 129, 129, 129, 0, 0, 0, 0, 0, 0, 0, 129, 129, 129, 129, 129, 129, 130, 130, 130, 130, 130, 130, 130, 129, 129, 129, 129, 129, 129, 129, 129, 141,
   141, 130, 130, 130, 130, 130, 130, 0, 0, 0, 0, 0, 0, 0, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 141,
   141, 130, 130, 130, 130, 130, 130, 0, 0, 0, 0, 0, 0, 0, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 141,
   141, 130, 130, 130, 130, 130, 130, 0, 0, 0, 0, 0, 0, 0, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 141,
   141, 130, 130, 130, 130, 130, 130, 0, 0, 0, 0, 0, 0, 0, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 130, 141,
};

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;
Camera2D gCamera       = { 0 };

GameState gState;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void drawBackground()
{
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
        ColorFromHex("#94b3caff"),   
        ColorFromHex("#C0897E") 
    );
}

void displayMenu()
{
    ClearBackground(ColorFromHex(BG_COLOUR));

    DrawTextEx(gState.font, "BOOK LANDING",
        { SCREEN_WIDTH / 2 - 150, 200 }, 65, 2, WHITE);

    DrawTextEx(gState.font, "People aren't reading and are getting kinda dumb!",
        { SCREEN_WIDTH / 2 - 270, 300 }, 30, 2, BLACK);

    DrawTextEx(gState.font, "It's up to you to find a person and gently land",
        { SCREEN_WIDTH / 2 - 250, 330 }, 30, 2, BLACK);

    DrawTextEx(gState.font, "on them, maybe that'll inspire them to read",
        { SCREEN_WIDTH / 2 - 240, 360 }, 30, 2, BLACK);

    DrawTextEx(gState.font, "Press ENTER to play",
        { SCREEN_WIDTH / 2 - 120, 430 }, 35, 2, GREEN);
}

void displayOutOfBounds()
{
    ClearBackground(ColorFromHex(BG_COLOUR));
    DrawTextEx(gState.font, "Where u going man?", { SCREEN_WIDTH / 2 - 200, 200 }, 65, 2, YELLOW);
    DrawTextEx(gState.font, "You flew out of bounds", { SCREEN_WIDTH / 2 - 180, 300 }, 35, 2, WHITE);
    DrawTextEx(gState.font, "Press ENTER to try again", { SCREEN_WIDTH / 2 - 180, 400 }, 30, 2, GREEN);
}

void displayCrashed()
{
    ClearBackground(BLACK);
    if(energy < 5) DrawTextEx(gState.font, "YOU RAN OUT OF ENERGY AND CRASHED!", { SCREEN_WIDTH / 2 - 380, 200 }, 65, 2, RED);
    else DrawTextEx(gState.font, "YOU CRASHED!", { SCREEN_WIDTH / 2 - 150, 200 }, 65, 2, RED);
    DrawTextEx(gState.font, "Books are feeble, literacy declines ;(", { SCREEN_WIDTH / 2 - 180, 300 }, 35, 2, WHITE);
    DrawTextEx(gState.font, "Press ENTER to try again", { SCREEN_WIDTH / 2 - 180, 400 }, 30, 2, YELLOW);
}

void displayChildCrashed()
{
    ClearBackground(BLACK);
    DrawTextEx(gState.font, "YOU CRASHED INTO THE CHILD!", { SCREEN_WIDTH / 2 - 260, 200 }, 55, 2, RED);
    DrawTextEx(gState.font, "This child now has trauma and will never read a book again", { SCREEN_WIDTH / 2 - 300, 300 }, 30, 2, WHITE);
    DrawTextEx(gState.font, "Press ENTER to try again", { SCREEN_WIDTH / 2 - 180, 400 }, 30, 2, YELLOW);
}

void displayGameWon()
{
    ClearBackground(ColorFromHex(BG_COLOUR));
    DrawTextEx(gState.font, "LITERACY RESTORED!", { SCREEN_WIDTH / 2 - 220, 200 }, 55, 2, GREEN);
    DrawTextEx(gState.font, "The child has been inspired to read!", { SCREEN_WIDTH / 2 - 240, 300 }, 30, 2, WHITE);
    DrawTextEx(gState.font, "Press ENTER to play again", { SCREEN_WIDTH / 2 - 180, 400 }, 30, 2, YELLOW);
}

void displayStuff()
{
    static float displaySpeedX = 0.0f, displaySpeedY = 0.0f;
    static float updateTimer = 0.0f;
    updateTimer += GetFrameTime();

    if (updateTimer >= 0.2f)
    {
        displaySpeedX = gState.player->getVelocity().x;
        displaySpeedY = gState.player->getVelocity().y;
        updateTimer = 0.0f;
    }

    DrawRectangle(SCREEN_WIDTH - 165, 5, 160, 90, { 0, 0, 0, 120 });
    DrawRectangleLines(SCREEN_WIDTH - 165, 5, 160, 90, WHITE);

    if (fabs(displaySpeedX) > 20)
        DrawTextEx(gState.font, TextFormat("X Speed: %.1f", displaySpeedX), { SCREEN_WIDTH - 155, 15 }, 22, 2, RED);
    else
        DrawTextEx(gState.font, TextFormat("X Speed: %.1f", displaySpeedX), { SCREEN_WIDTH - 155, 15 }, 22, 2, GREEN);

    if (fabs(displaySpeedY) > 50)
        DrawTextEx(gState.font, TextFormat("Y Speed: %.1f", displaySpeedY), { SCREEN_WIDTH - 155, 40 }, 22, 2, RED);
    else
        DrawTextEx(gState.font, TextFormat("Y Speed: %.1f", displaySpeedY), { SCREEN_WIDTH - 155, 40 }, 22, 2, GREEN);

    if (energy >= 300)
        DrawTextEx(gState.font, TextFormat("Energy: %.1f", energy), { SCREEN_WIDTH - 155, 65 }, 22, 2, GREEN);
    else if (energy > 100)
        DrawTextEx(gState.font, TextFormat("Energy: %.1f", energy), { SCREEN_WIDTH - 155, 65 }, 22, 2, YELLOW);
    else
        DrawTextEx(gState.font, TextFormat("Energy: %.1f", energy), { SCREEN_WIDTH - 155, 65 }, 22, 2, RED);
}

void panCamera(Camera2D *camera, const Vector2 *targetPosition);

void panCamera(Camera2D *camera, const Vector2 *targetPosition)
{
    Vector2 positionDifference = Vector2Subtract(
        *targetPosition, 
        camera->target
    );

    camera->target = Vector2Add(
        camera->target, 
        Vector2Scale(positionDifference, 0.1f)
    ); // 0.1 = smoothing factor
}

void initialiseAudio(){
    InitAudioDevice();

    /*  
    ------------------------------------------------------
    I made this song! hope you like it, please don't steal
    ------------------------------------------------------
    */  

    gState.bgm = LoadMusicStream("assets/game/The Flying Puffin.mp3");
    SetMusicVolume(gState.bgm, 0.33f);
    PlayMusicStream(gState.bgm);

    //gState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Maps");
    
    gState.font = LoadFont("assets/game/GentiumBookPlus-Bold.ttf");

    /*
        ----------- MAP -----------
    */
    gState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
        (unsigned int *) LEVEL_DATA, // grid data
        "assets/game/ufeff_tiles_v2.png",   // texture filepath
        TILE_DIMENSION,              // tile size
        16, 16,                        // texture cols & rows
        ORIGIN                       // in-game origin
    );

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7, 8, 9, 10, 11, 12, 13, 14, 15 }},
        {LEFT,  {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }},
        {UP,    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }},
        {RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }},
    };

    float sizeRatio  = 48.0f / 64.0f;

    gState.player = new Entity(
        {ORIGIN.x - 300.0f, ORIGIN.y - 185.0f}, // position
        {150.0f * sizeRatio, 150.0f},           // scale
        "assets/game/flying book.png",          // texture file address
        ATLAS,                                  // single image or atlas?
        ATLAS_DIMENSIONS,                       // atlas dimensions
        playerAnimationAtlas,                  // actual atlas
        PLAYER                                  // entity type
    );

    gState.player->setFlyingPower(5.0f);

    gState.player->setColliderDimensions({
        gState.player->getScale().x / 2.5f,
        gState.player->getScale().y / 2.5f
    });
    gState.player->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    // Child
    gState.illiterateChild = new Entity(
        {ORIGIN.x + 550.0f, ORIGIN.y + 100.0f},
        {100.0f, 85.0f},
        "assets/game/kid.png",
        PLATFORM
    );
    
    gState.illiterateChild->setColliderDimensions({ 60.0f, 70.0f });

    /*
        ----------- CAMERA -----------
    */
    gCamera.target   = gState.player->getPosition(); // camera follows player
    gCamera.offset   = ORIGIN;                        // camera offset to center of screen
    gCamera.rotation = 0.0f;                          // no rotation
    gCamera.zoom     = 1.0f;                          // default zoom

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (IsKeyPressed(KEY_ENTER)) {
        
        if (gGameState == MENU){
            ACCELERATION_OF_GRAVITY = 98.1f;
            gGameState = PLAYING;
            gState.player->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY}); 
        }
        else if (gGameState == PLAYING){
            //gState.player->setPosition({ORIGIN.x - 300.0f, ORIGIN.y - 185.0f});
            ACCELERATION_OF_GRAVITY = 0.0f;
            gGameState = MENU;
        }
        else if (gGameState == CRASHED || gGameState == CHILD_CRASHED || gGameState == GAMEWON || gGameState == OUT_OF_BOUNDS)
        {
            delete gState.player;
            delete gState.illiterateChild;
            delete gState.map;
            energy = 1000.0f;
            ACCELERATION_OF_GRAVITY = 0.0f;
            initialise();
            gGameState = MENU;
        }
    }

    if(energy > 0){
        bool isMoving = IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_W);

        if (IsKeyDown(KEY_A))
        {
            energy -= ENERGY_EXPENDETURE;
            float newAccelX = gState.player->getAcceleration().x - HORIZONTAL_THRUST * FIXED_TIMESTEP;
            newAccelX = fmax(newAccelX, -MAX_ACCEL_X);
            gState.player->setAcceleration({ newAccelX, gState.player->getAcceleration().y });
            gState.player->setDirection(LEFT);
        }
        else if (IsKeyDown(KEY_D))
        {
            energy -= ENERGY_EXPENDETURE;
            float newAccelX = gState.player->getAcceleration().x + HORIZONTAL_THRUST * FIXED_TIMESTEP;
            newAccelX = fmin(newAccelX, MAX_ACCEL_X);
            gState.player->setAcceleration({ newAccelX, gState.player->getAcceleration().y });
            gState.player->setDirection(RIGHT);
        }
        else
        {
            float currentAccelX = gState.player->getAcceleration().x;
            float decayed = currentAccelX * (1.0f - DRAG_FACTOR * FIXED_TIMESTEP);

            if (fabs(decayed) < 0.01f) decayed = 0.0f;
            gState.player->setAcceleration({ decayed, gState.player->getAcceleration().y });
        }

        if (IsKeyDown(KEY_W)) {gState.player->fly(); energy -= ENERGY_EXPENDETURE;}

        gState.player->setIsMoving(isMoving);

    }
    
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    //if(gGameState != PLAYING) return;
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        UpdateMusicStream(gState.bgm);

        // gets the velocity before the collision sets it to 0
        Vector2 velocityBeforeUpdate = gState.player->getVelocity();

        gState.player->update(
            FIXED_TIMESTEP,
            nullptr,
            gState.map,
            gState.illiterateChild,
            1
        );

        deltaTime -= FIXED_TIMESTEP;

        Vector2 currentPlayerPosition = { gState.player->getPosition().x, ORIGIN.y };

        panCamera(&gCamera, &currentPlayerPosition);
        
        // check win or lose
        if(gGameState == PLAYING){
            if(gState.player->getPosition().y < -200.0f){
                gGameState = OUT_OF_BOUNDS;
            }
            if(gState.player->isCollidingWithTarget() && gState.player->isCollidingBottom())
            {
                if(fabs(velocityBeforeUpdate.y) < MAX_LANDING_SPEED_Y && fabs(velocityBeforeUpdate.x) < MAX_LANDING_SPEED_X)
                    gGameState = GAMEWON;
                else
                    gGameState = CHILD_CRASHED;
            }
            else if(gState.player->isCollidingWithTarget() && (gState.player->isCollidingLeft() || gState.player->isCollidingRight()))
            {
                gGameState = CHILD_CRASHED; 
            }
            else if(gState.player->isCollidingLeft() || gState.player->isCollidingRight())
            {
                gGameState = CRASHED;
            }
            else if(gState.player->isCollidingBottom() && fabs(velocityBeforeUpdate.y) > MAX_LANDING_SPEED_Y)
            {
                gGameState = CRASHED;
            }
        }
        
    }

    // moving 'platform'
    if(gState.illiterateChild->getPosition().x < ORIGIN.x + 470){
        gChildDirection *= -1;
        gState.illiterateChild->setDirection(RIGHT);
    }
    else if(gState.illiterateChild->getPosition().x > ORIGIN.x + 630){
        gChildDirection *= -1;
        gState.illiterateChild->setDirection(LEFT);
    }

    Vector2 childPos = gState.illiterateChild->getPosition();
    childPos.x += 60.0f * gChildDirection * FIXED_TIMESTEP;
    gState.illiterateChild->setPosition(childPos);

    //print speeds
    //printf("vx: %.2f | vy: %.2f\n", gState.player->getVelocity().x, gState.player->getVelocity().y);
}

void render()
{
    BeginDrawing();

    

    if(gGameState == MENU){
        displayMenu();
    }
    else if(gGameState == PLAYING){
        ClearBackground(ColorFromHex(BG_COLOUR));

        drawBackground();

        BeginMode2D(gCamera);

        gState.illiterateChild->render();
        gState.player->render();
        gState.map->render();

        DrawTextEx(gState.font, "Use WASD to move and fly", { -100, 355 }, 35, 2, WHITE);
        DrawTextEx(gState.font, "Monitor you're energy and be careful not to crash!", { -100, 380 }, 35, 2, WHITE);

        if(gState.player->getPosition().x > ORIGIN.x + 420 && gState.player->getPosition().y < 500){
            DrawTextEx(gState.font, "Holy guacamole that's a flying book!", { 800, 460 }, 35, 2, WHITE);
        };

        EndMode2D();

        displayStuff();
    }
    else if (gGameState == CRASHED){
        displayCrashed();
    }
    else if (gGameState == CHILD_CRASHED){
        displayChildCrashed();
    }
    else if (gGameState == GAMEWON){
        displayGameWon();
    }
    else if (gGameState == OUT_OF_BOUNDS){
        displayOutOfBounds();
    }
    

    EndDrawing();
}

void shutdown() 
{
    delete gState.player;
    delete gState.map;
    delete gState.illiterateChild;

    UnloadFont(gState.font);

    UnloadMusicStream(gState.bgm);
    UnloadSound(gState.jumpSound);

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();
    initialiseAudio();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}