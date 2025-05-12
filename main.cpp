#include "Game.h"
#include "../Cutscene/scene.h" // Include the cutscene header

int main() {
    Game game;
    // Run the intro sequence first, using the game's window
    if (!runIntroSequence(game.getWindow())) {
        return 0; // Exit if cutscene was closed or didn't complete
    }
    game.run();
    return 0;
}