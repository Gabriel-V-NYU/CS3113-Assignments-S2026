#include "Menu.h"

Menu::Menu()                                      : Scene { {0.0f}, nullptr   } {}
Menu::Menu(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Menu::~Menu() { shutdown(); }

void Menu::initialise()
{
    mGameState.nextSceneID = 0;
}

void Menu::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = 1;
}

void Menu::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawText("ESCAPE!!",
        screenWidth / 2 - MeasureText("ESCAPE!!", 60) / 2,
        screenHeight / 2 - 80,
        60, RED);

    DrawText("Press ENTER to Start",
        screenWidth / 2 - MeasureText("Press ENTER to Start", 24) / 2,
        screenHeight / 2 + 20,
        24, LIGHTGRAY);
}

void Menu::shutdown() {}
