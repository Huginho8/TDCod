#ifndef TDCOD_CUTSCENE_H
#define TDCOD_CUTSCENE_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>

namespace TDCod {

class Cutscene {
public:
    Cutscene();
    ~Cutscene();

    void run(sf::RenderWindow& window);

private:
    // Add member variables for cutscene state, assets, etc.
};

} // namespace TDCod

#endif // TDCOD_CUTSCENE_H