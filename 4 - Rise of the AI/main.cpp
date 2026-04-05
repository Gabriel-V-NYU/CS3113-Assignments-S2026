/**
* Author: Gabriel Valte]]
* Assignment: Rise of the AI
* Date due: 2026-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/Menu.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/EndScreen.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 4,
              STARTING_LIVES   = 3;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus  gAppStatus      = RUNNING;
float      gPreviousTicks  = 0.0f,
           gTimeAccumulator = 0.0f;
int        gLives          = STARTING_LIVES;
int        gCurrentLevelID = 0; // tracks which level to restart on death

Sound    gDeathSound   = { 0 };
Sound    gWalkingSound = { 0 };
Music    gBGM          = { 0 };

Camera2D gCamera = { 0 };

Scene      *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

Menu       *gMenu       = nullptr;
LevelA     *gLevelA     = nullptr;
LevelB     *gLevelB     = nullptr;
LevelC     *gLevelC     = nullptr;
EndScreen  *gEndScreen  = nullptr;

void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{
    if (gCurrentScene != nullptr) gCurrentScene->shutdown();
    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (gCurrentScene->getState().soldier != nullptr)
        gCamera.target = gCurrentScene->getState().soldier->getPosition();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rise of the AI");
    InitAudioDevice();

    gDeathSound = LoadSound("assets/game/Tom Screaming.wav");
    SetSoundVolume(gDeathSound, 0.3f);

    gWalkingSound = LoadSound("assets/game/walking.wav");
    SetSoundVolume(gWalkingSound, 0.25f);

    gBGM = LoadMusicStream("assets/game/Tangerine Dog.mp3");
    SetMusicVolume(gBGM, 1.0f);
    PlayMusicStream(gBGM);

    gMenu      = new Menu(ORIGIN,      "#000000");
    gLevelA    = new LevelA(ORIGIN,    "#C0897E");
    gLevelB    = new LevelB(ORIGIN,    "#3d0620ff");
    gLevelC    = new LevelC(ORIGIN,    "#13011cff");
    gEndScreen = new EndScreen(ORIGIN, "#000000");

    gLevels.push_back(gMenu);       // index 0
    gLevels.push_back(gLevelA);     // index 1
    gLevels.push_back(gLevelB);     // index 2
    gLevels.push_back(gLevelC);     // index 3
    gLevels.push_back(gEndScreen);  // index 4

    switchToScene(gLevels[0]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    SetTargetFPS(FPS);
}

void processInput()
{
    Entity *soldier = gCurrentScene->getState().soldier;

    if (soldier != nullptr)
    {
        soldier->resetMovement();

        if      (IsKeyDown(KEY_A)) {
            soldier->moveLeft();
            if (!IsSoundPlaying(gWalkingSound)) PlaySound(gWalkingSound);
        }
        else if (IsKeyDown(KEY_D)){
            soldier->moveRight();
            if (!IsSoundPlaying(gWalkingSound)) PlaySound(gWalkingSound);
        }

        if (IsKeyPressed(KEY_W) && soldier->isCollidingBottom())
        {
            soldier->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (GetLength(soldier->getMovement()) > 1.0f)
            soldier->normaliseMovement();
    }

    if (IsKeyPressed(KEY_ZERO))  switchToScene(gLevels[0]);
    if (IsKeyPressed(KEY_ONE))   switchToScene(gLevels[1]);
    if (IsKeyPressed(KEY_TWO))   switchToScene(gLevels[2]);
    if (IsKeyPressed(KEY_THREE)) switchToScene(gLevels[3]);
    if (IsKeyPressed(KEY_FOUR))  switchToScene(gLevels[4]);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    UpdateMusicStream(gBGM);

    float ticks    = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        if (gCurrentScene->getState().soldier != nullptr)
        {
            Vector2 playerPos = { gCurrentScene->getState().soldier->getPosition().x, ORIGIN.y };
            panCamera(&gCamera, &playerPos);
        }

    }

    if (gCurrentScene->getState().playerDied)
    {
        PlaySound(gDeathSound);
        gLives--;

        if (gLives <= 0)
        {
            gLives = STARTING_LIVES;
            gEndScreen->setWon(false);
            switchToScene(gLevels[4]);
        }
        else
        {
            switchToScene(gLevels[gCurrentLevelID]); // restart current level
        }
        return;
    }

    int nextID = gCurrentScene->getState().nextSceneID;

    if (nextID == -1) 
    {
        gLives = STARTING_LIVES;
        gCurrentLevelID = 0;
        switchToScene(gLevels[0]);
    }
    else if (nextID == 4) // win
    {
        gEndScreen->setWon(true);
        switchToScene(gLevels[4]);
    }
    else if (nextID > 0)
    {
        gCurrentLevelID = nextID;
        switchToScene(gLevels[nextID]);
    }
}

void render()
{
    BeginDrawing();

    if (gCurrentScene->getState().soldier != nullptr)
    {
        BeginMode2D(gCamera);
        gCurrentScene->render();
        EndMode2D();

        const char *livesText = TextFormat("Lives: %d", gLives);
        DrawText(livesText, 20, 20, 28, WHITE);

        Vector2 pos = gCurrentScene->getState().soldier->getPosition();
        DrawText(TextFormat("x: %.1f  y: %.1f", pos.x, pos.y), 20, 56, 22, YELLOW);
    }
    else
    {
        gCurrentScene->render();
    }

    EndDrawing();
}

void shutdown()
{
    delete gMenu;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;
    delete gEndScreen;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    UnloadSound(gDeathSound);
    UnloadSound(gWalkingSound);
    UnloadMusicStream(gBGM);
    CloseAudioDevice();
    CloseWindow();
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
