#include "EndScreen.h"

EndScreen::EndScreen()                                      : Scene { {0.0f}, nullptr   } {}
EndScreen::EndScreen(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

EndScreen::~EndScreen() { shutdown(); }

void EndScreen::initialise()
{
    mGameState.nextSceneID = 0;
    mGameState.playerDied  = false;
}

void EndScreen::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = -1; 
}

void EndScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (mPlayerWon)
    {
        DrawText("YOU WIN",
            screenWidth / 2 - MeasureText("YOU WIN", 80) / 2,
            screenHeight / 2 - 80,
            80, GREEN);
    }
    else
    {
        DrawText("YOU LOSE",
            screenWidth / 2 - MeasureText("YOU LOSE", 80) / 2,
            screenHeight / 2 - 80,
            80, RED);
    }

    DrawText("Press ENTER to return to menu",
        screenWidth / 2 - MeasureText("Press ENTER to return to menu", 20) / 2,
        screenHeight / 2 + 20,
        20, LIGHTGRAY);
}

void EndScreen::shutdown() {}
