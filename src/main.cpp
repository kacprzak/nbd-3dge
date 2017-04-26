//#include "Game.h"

#include "Engine.h"
#include "GameLogic.h"
#include "Game.h"

int main()
{
    Engine engine;
    GameLogic game;
    game.attachView(std::make_shared<Game>("nbd-3dge", 1024, 760, false));
    engine.mainLoop(&game);

    return 0;
}
