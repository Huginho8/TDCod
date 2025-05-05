#ifndef CONFIG_H
#define CONFIG_H

namespace Config {

    // Game world dimensions
    constexpr float CELL_SIZE = 20.0f;
    constexpr float BODY_RADIUS = CELL_SIZE * 0.5f;
    constexpr unsigned int MAP_SIZE = 4000;

    // Math constants
    constexpr float PI = 3.1415926f;
    constexpr float PI2 = 6.2831854f;   // 2π
    constexpr float PIS2 = 1.5707963f;   // π/2
    constexpr float RADDEG = 57.2958f;     // Radians to Degrees
    constexpr float DEGRAD = 0.0174533f;   // Degrees to Radians

    // Fixed time step (for updates/physics)
    constexpr float DT = 0.016f; // ~60 FPS

}

#endif // CONFIG_H
