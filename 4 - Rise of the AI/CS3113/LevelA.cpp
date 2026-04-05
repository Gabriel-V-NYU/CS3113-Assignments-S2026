#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = 0;
   mGameState.playerDied  = false;

   mGameState.jumpSound = LoadSound("assets/game/boing.mp3");
   SetSoundVolume(mGameState.jumpSound, 0.3f);

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/game/ufeff_tiles_v2.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      16, 16,                      // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> soldierAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7 }},
      {LEFT,  {  8,  9, 10, 11, 12, 13, 14, 15 }},
      {UP,    { 24, 25, 26, 27, 28, 29, 30, 31 }},
      {RIGHT, { 40, 41, 42, 43, 44, 45, 46, 47 }},
   };

   float sizeRatio  = 48.0f / 64.0f;

   mGameState.soldier = new Entity(
      {mOrigin.x - 1500.0f, mOrigin.y - 100.0f}, // position
      {250.0f * sizeRatio, 250.0f},             // scale
      "assets/soldier/Soldier-Walk.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 1, 8 },                                 // atlas dimensions
      soldierAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.soldier->setJumpingPower(550.0f);
   mGameState.soldier->setColliderDimensions({
      mGameState.soldier->getScale().x / 6.0f,
      mGameState.soldier->getScale().y / 6.4f
   });
   mGameState.soldier->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- GOBLINS -----------
   */
   std::map<Direction, std::vector<int>> goblinAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7 }},
      {LEFT,  {  8,  9, 10, 11, 12, 13, 14, 15 }},
      {UP,    { 24, 25, 26, 27, 28, 29, 30, 31 }},
      {RIGHT, { 40, 41, 42, 43, 44, 45, 46, 47 }},
   };

   mGameState.enemies = new Entity[2];

   // Goblin 1
   mGameState.enemies[0].init(
      {mOrigin.x - 00.0f, mOrigin.y + 100.0f},
      {250.0f * sizeRatio, 250.0f},
      "assets/enemies/orc-Walk.png",
      ATLAS, { 1, 8 }, goblinAnimationAtlas, NPC
   );
   mGameState.enemies[0].setAIType(WANDERER);
   mGameState.enemies[0].setJumpingPower(550.0f);
   mGameState.enemies[0].setColliderDimensions({
      mGameState.enemies[0].getScale().x / 6.0f,
      mGameState.enemies[0].getScale().y / 6.4f
   });
   mGameState.enemies[0].setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   // Goblin 2
   mGameState.enemies[1].init(
      {mOrigin.x + 600.0f, mOrigin.y + 100.0f},
      {250.0f * sizeRatio, 250.0f},
      "assets/enemies/orc-Walk.png",
      ATLAS, { 1, 8 }, goblinAnimationAtlas, NPC
   );
   mGameState.enemies[1].setAIType(WANDERER);
   mGameState.enemies[1].setJumpingPower(550.0f);
   mGameState.enemies[1].setColliderDimensions({
      mGameState.enemies[1].getScale().x / 6.0f,
      mGameState.enemies[1].getScale().y / 6.4f
   });
   mGameState.enemies[1].setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

}

void LevelA::update(float deltaTime)
{
   mGameState.soldier->update(
      deltaTime,
      nullptr,
      mGameState.map,
      nullptr, 0
   );

   for (int i = 0; i < 2; i++) {
      mGameState.enemies[i].update(
         deltaTime,
         mGameState.soldier,
         mGameState.map,
         nullptr, 0
      );
   }

   if (mGameState.soldier->getPosition().y > 800.0f) mGameState.playerDied = true;

   if (mGameState.soldier->getPosition().x > 2000.0f) mGameState.nextSceneID = 2;

   for (int i = 0; i < 2; i++)
   {
      if (mGameState.soldier->isColliding(&mGameState.enemies[i]))
      {
         mGameState.playerDied = true;
         break;
      }
   }

}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.soldier->render();

   for (int i = 0; i < 2; i++) mGameState.enemies[i].render();

   mGameState.map->render();
}

void LevelA::shutdown()
{
   delete mGameState.soldier; mGameState.soldier = nullptr;
   delete[] mGameState.enemies; mGameState.enemies = nullptr;
   delete mGameState.map;     mGameState.map     = nullptr;

   UnloadSound(mGameState.jumpSound);
}