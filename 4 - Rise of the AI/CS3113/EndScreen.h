#include "Scene.h"

#ifndef ENDSCREEN_H
#define ENDSCREEN_H

class EndScreen : public Scene
{
private:
    bool mPlayerWon = false;

public:
    EndScreen();
    EndScreen(Vector2 origin, const char *bgHexCode);
    ~EndScreen();

    void setWon(bool won) { mPlayerWon = won; }

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // ENDSCREEN_H
