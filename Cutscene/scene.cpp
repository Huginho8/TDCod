#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

bool inCutscene = false;

// Helper function for text wrapping
std::string wrapText(sf::Text& textObject, const std::string& string, float maxWidth, const sf::Font& font, unsigned int charSize) {
    std::string wrappedString;
    std::string currentLine;
    std::string word;
    sf::Text tempText; // Use a temporary Text object for measurements
    tempText.setFont(font);
    tempText.setCharacterSize(charSize);

    for (char character : string) {
        if (character == ' ' || character == '\n' || character == '\t') { // Treat tabs like spaces for word separation
            // Check if currentLine + word fits
            tempText.setString(currentLine + word);
            if (tempText.getLocalBounds().width > maxWidth && !currentLine.empty()) {
                wrappedString += currentLine + '\n'; // Add newline
                currentLine = word; // New line starts with current word
            } else {
                currentLine += word;
            }
            
            if (character == '\n') { // Handle explicit newlines
                 wrappedString += currentLine + '\n';
                 currentLine.clear();
            } else {
                 currentLine += " "; // Add the space
            }
            word.clear();
        } else {
            word += character;
        }
    }
    // Add remaining word and line
    tempText.setString(currentLine + word);
    if (tempText.getLocalBounds().width > maxWidth && !currentLine.empty()) {
        wrappedString += currentLine + '\n' + word;
    } else {
        wrappedString += currentLine + word;
    }
    
    // Trim leading/trailing newlines that might be added by mistake
    size_t first = wrappedString.find_first_not_of("\n \t");
    if (std::string::npos == first) {
        return "";
    }
    size_t last = wrappedString.find_last_not_of("\n \t");
    return wrappedString.substr(first, (last - first + 1));
}


int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Echo's of Valkyrie");

    // Load the background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Cutscene/Assets/Space Background.png")) {
        // Handle error
        return -1;
    }

    // Create a sprite for the background
    sf::Sprite backgroundSprite(backgroundTexture);

    // Load the font
    sf::Font font;
    if (!font.loadFromFile("Cutscene/Assets/Call of Ops Duty.otf")) {
        // Handle error
        return -1;
    }

    // Create the title text
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString("Echo's of Valkyrie");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    // Center the title text horizontally
    sf::FloatRect titleTextBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleTextBounds.left + titleTextBounds.width / 2.0f, titleTextBounds.top + titleTextBounds.height / 2.0f);
    titleText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 4.0f);

    // Create a rectangle shape for the title border
    sf::RectangleShape titleBorder;
    titleBorder.setSize(sf::Vector2f(titleTextBounds.width + 20, titleTextBounds.height + 20)); // Add padding
    titleBorder.setFillColor(sf::Color::Transparent);
    titleBorder.setOutlineColor(sf::Color::White);
    titleBorder.setOutlineThickness(2);
    titleBorder.setOrigin(titleBorder.getSize().x / 2.0f, titleBorder.getSize().y / 2.0f);
    titleBorder.setPosition(titleText.getPosition());

    // Create the start button text
    sf::Text startButtonText;
    startButtonText.setFont(font);
    startButtonText.setString("Start");
    startButtonText.setCharacterSize(40);
    startButtonText.setFillColor(sf::Color::White);
    // Center the start button text horizontally
    sf::FloatRect startButtonBounds = startButtonText.getLocalBounds();
    startButtonText.setOrigin(startButtonBounds.left + startButtonBounds.width / 2.0f, startButtonBounds.top + startButtonBounds.height / 2.0f);
    startButtonText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 1.5f);

    // Create a rectangle shape for the start button border
    sf::RectangleShape startButtonBorder;
    startButtonBorder.setSize(sf::Vector2f(startButtonBounds.width + 20, startButtonBounds.height + 20)); // Add padding
    startButtonBorder.setFillColor(sf::Color::Transparent);
    startButtonBorder.setOutlineColor(sf::Color::White);
    startButtonBorder.setOutlineThickness(2);
    startButtonBorder.setOrigin(startButtonBorder.getSize().x / 2.0f, startButtonBorder.getSize().y / 2.0f);
    startButtonBorder.setPosition(startButtonText.getPosition());

    // Load planet texture and create sprite
    sf::Texture planetTexture;
    if (!planetTexture.loadFromFile("Cutscene/Assets/planet.png")) {
        // Handle error
        return -1;
    }
    sf::Sprite planetSprite(planetTexture);
    // Position the planet on the right side of the full background, outside the initial window view
    planetSprite.setPosition(backgroundTexture.getSize().x - planetTexture.getSize().x - 50, window.getSize().y / 2.0f - planetTexture.getSize().y / 2.0f); // 50 pixels from the right edge of the full background

    // Load ship textures
    sf::Texture shipMoveTexture;
    if (!shipMoveTexture.loadFromFile("Cutscene/Assets/Corvette/Move.png")) {
        // Handle error
        return -1;
    }

    sf::Texture shipIdleTexture;
    if (!shipIdleTexture.loadFromFile("Cutscene/Assets/Corvette/Idle.png")) {
        // Handle error
        return -1;
    }

    // Create ship sprite and set initial position off-screen left
    sf::Sprite shipSprite(shipIdleTexture); // Use Idle texture initially
    shipSprite.setPosition(-shipSprite.getGlobalBounds().width, window.getSize().y / 2.0f - shipSprite.getGlobalBounds().height / 2.0f);

    // Clock for movement
    sf::Clock movementClock;
    sf::Clock animationClock; // Clock for animation
    sf::Clock fadeClock; // Clock for fading

    // Load sounds
    sf::SoundBuffer menuButtonBuffer;
    if (!menuButtonBuffer.loadFromFile("Cutscene/Assets/menubutton.mp3")) {
        return -1; // Handle error
    }
    sf::Sound menuButtonSound;
    menuButtonSound.setBuffer(menuButtonBuffer);

    sf::SoundBuffer menuClickBuffer;
    if (!menuClickBuffer.loadFromFile("Cutscene/Assets/menuclick.mp3")) {
        return -1; // Handle error
    }
    sf::Sound menuClickSound;
    menuClickSound.setBuffer(menuClickBuffer);

    // Load music
    sf::Music menuMusic;
    if (!menuMusic.openFromFile("Cutscene/Assets/01 - Damned.mp3")) {
        return -1; // Handle error
    }
    menuMusic.setLoop(true);
    menuMusic.play();

    // Dialogue System
    struct DialogueLine {
        std::string speaker;
        std::string line;
    };

    std::vector<DialogueLine> dialogueScript = {
        {"MC", "What the hell was that shake? That did not feel like turbulence."},
        {"Doctor", "It was not. The primary thruster cooling system just failed. We have got maybe five minutes before it overheats completely."},
        {"MC", "Can we fix it mid flight?"},
        {"Doctor", "Not a chance. We need to land right now. Find somewhere stable, or we are both going down with this thing."},
        {"MC", "Hold on... scanning for a surface. Found one. It is rough terrain, but it will have to do."},
        {"Doctor", "Then what are you waiting for? Get us down there before this ship turns into a fireball."}
    };

    int currentDialogueIndex = 0;
    int currentCharIndex = 0;
    sf::Text dialogueText;
    // Attempt to load a standard font for dialogue, fallback to original if not found
    sf::Font dialogueDisplayFont;
    if (!dialogueDisplayFont.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) { // Common path for Arial on macOS
        // Fallback to another common path or the original font if Arial isn't found
        if (!dialogueDisplayFont.loadFromFile("Arial.ttf")) { // Try simpler path
             dialogueDisplayFont = font; // Fallback to the original game font
        }
    }
    dialogueText.setFont(dialogueDisplayFont);
    dialogueText.setCharacterSize(24); // Adjust size as needed
    dialogueText.setFillColor(sf::Color::White);

    sf::Text skipPromptText;
    skipPromptText.setFont(dialogueDisplayFont);
    skipPromptText.setCharacterSize(18); // Smaller size for prompt
    skipPromptText.setFillColor(sf::Color(200, 200, 200)); // Light gray
    skipPromptText.setString("Press Space to Skip/Continue");
    
    sf::Clock dialogueClock;
    float timePerChar = 0.02f; // Seconds per character (quicker again)
    bool currentLineComplete = false;
    bool dialogueActive = false; // To activate dialogue when cutscene starts
    sf::Clock autoAdvanceClock;
    // Removed fixed autoAdvanceDelay, will calculate dynamically

    // Dialogue box
    sf::RectangleShape dialogueBox;
    dialogueBox.setSize(sf::Vector2f(window.getSize().x * 0.9f, window.getSize().y * 0.25f));
    dialogueBox.setFillColor(sf::Color(0, 0, 0, 180)); // Semi-transparent black interior
    dialogueBox.setOutlineColor(sf::Color::White);
    dialogueBox.setOutlineThickness(3.0f);
    // Center it horizontally, position on lower half
    dialogueBox.setOrigin(dialogueBox.getSize().x / 2.0f, dialogueBox.getSize().y / 2.0f);
    dialogueBox.setPosition(window.getSize().x / 2.0f, window.getSize().y * 0.8f);


    // Create a view for the camera
    sf::View cameraView(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    // bool cameraFollow = false; // Removed camera follow flag

    // Create a black rectangle for fading
    sf::RectangleShape fadeRect(sf::Vector2f(window.getSize().x, window.getSize().y));
    fadeRect.setFillColor(sf::Color(0, 0, 0, 0)); // Start fully transparent
    float fadeAlpha = 0.0f;
    float fadeDuration = 1.0f; // 1 second fade duration
    bool startFading = false;

    float normalShipSpeed = 150.0f;
    float fastShipSpeed = 400.0f; // Adjusted fast speed (slightly faster than 350)
    float currentShipSpeed = normalShipSpeed;
    // Removed canSpeedUpShip and shipHasBeenSpedUp variables

    // Main game loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (!inCutscene) {
                // Handle mouse clicks in GUI state
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        // Check if the click is within the start button bounds
                        if (startButtonText.getGlobalBounds().contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                            menuClickSound.play();
                            // Start button clicked, transition to cutscene
                            inCutscene = true;
                            dialogueActive = true; // Start dialogue
                            currentDialogueIndex = 0;
                            currentCharIndex = 0;
                            currentLineComplete = false;
                            dialogueClock.restart();
                            menuMusic.stop(); // Stop menu music
                            // cameraFollow = true; // Removed camera follow start
                            movementClock.restart(); // Start the clock for ship movement
                            animationClock.restart(); // Start the animation clock
                        }
                    }
                }
            } else if (dialogueActive && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                if (currentDialogueIndex < dialogueScript.size()) { // Ensure we are within script bounds
                    if (!currentLineComplete) {
                        // First press: Skip to end of current line
                        currentCharIndex = dialogueScript[currentDialogueIndex].line.length();
                        // The update logic will set currentLineComplete and restart autoAdvanceClock
                    } else {
                        // Second press (or press when line is complete): Advance to next line
                        currentDialogueIndex++;
                        if (currentDialogueIndex < dialogueScript.size()) {
                            currentCharIndex = 0;
                            currentLineComplete = false;
                            dialogueClock.restart();
                            autoAdvanceClock.restart();
                        } else {
                            dialogueActive = false; // End of dialogue
                            currentShipSpeed = fastShipSpeed; // Speed up automatically
                            skipPromptText.setString(""); // Clear prompt
                        }
                    }
                }
            }
            // Removed the separate space press logic for speed up after dialogue
        }

        // Update logic
        if (inCutscene) {
            // Move the ship
            float deltaTime = movementClock.restart().asSeconds();
            shipSprite.move(currentShipSpeed * deltaTime, 0); // Use currentShipSpeed

            // Simple animation between Idle and Move
            // if (animationClock.getElapsedTime().asSeconds() > 0.2f) {
            //     if (shipSprite.getTexture() == &shipMoveTexture) {
            //         shipSprite.setTexture(shipIdleTexture);
            //     } else {
            //         shipSprite.setTexture(shipMoveTexture);
            //     }
            //     animationClock.restart();
            // }

            // Update camera view to follow the ship, clamped to background bounds
            float cameraX = shipSprite.getPosition().x;
            float minCameraX = window.getSize().x / 2.0f;
            float maxCameraX = backgroundTexture.getSize().x - window.getSize().x / 2.0f;
            cameraX = std::max(minCameraX, std::min(cameraX, maxCameraX));
            cameraView.setCenter(cameraX, window.getSize().y / 2.0f);

            // Define a smaller collision area for the planet's center
            sf::FloatRect planetCenterBounds(
                planetSprite.getPosition().x + planetSprite.getGlobalBounds().width * 0.45f,
                planetSprite.getPosition().y + planetSprite.getGlobalBounds().height * 0.45f,
                planetSprite.getGlobalBounds().width * 0.1f,
                planetSprite.getGlobalBounds().height * 0.1f
            );

            // Use the entire planet bounds for fading collision
            sf::FloatRect planetBounds = planetSprite.getGlobalBounds();

            // Check if ship reached the center of the planet to make it disappear
            if (shipSprite.getGlobalBounds().intersects(planetCenterBounds)) {
                shipSprite.setColor(sf::Color::Transparent); // Make ship disappear
                // Optionally stop ship movement here as well
                // movementClock.restart(); // Stop ship movement by not getting delta time
            }

            // Check if ship reached the planet to start fading
            if (shipSprite.getGlobalBounds().intersects(planetBounds)) {
                // Start fading to black if not already fading
                if (!startFading) {
                    startFading = true;
                    fadeClock.restart();
                }
            }

            // Update fading
            if (startFading) {
                float elapsedTime = fadeClock.getElapsedTime().asSeconds();
                fadeAlpha = (elapsedTime / fadeDuration) * 255.0f;
                if (fadeAlpha > 255.0f) {
                    fadeAlpha = 255.0f;
                }
                fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeAlpha)));
            }

            // Update dialogue
            if (dialogueActive && currentDialogueIndex < dialogueScript.size()) {
                // Typewriter effect
                if (!currentLineComplete && dialogueClock.getElapsedTime().asSeconds() > timePerChar) {
                    if (currentCharIndex < dialogueScript[currentDialogueIndex].line.length()) {
                        currentCharIndex++;
                        dialogueClock.restart(); // Restart for next char
                    } else {
                        // Line finished typing
                        currentLineComplete = true;
                        autoAdvanceClock.restart(); // Start auto-advance timer
                    }
                }

                // Prepare and wrap text for display
                std::string currentSpeaker = dialogueScript[currentDialogueIndex].speaker;
                std::string lineContent = dialogueScript[currentDialogueIndex].line;
                // If currentLineComplete is true (either by typing or spacebar), show full line for wrapping
                std::string displayedLineContent = currentLineComplete ? lineContent : lineContent.substr(0, currentCharIndex);
                // Ensure speaker colon is definitely there
                std::string fullStringToDisplay = currentSpeaker + ": " + displayedLineContent;
                
                float dialogueBoxInnerWidth = dialogueBox.getSize().x - 40.0f; // e.g. 20px padding on each side
                // Use the loaded dialogueDisplayFont for wrapping measurement
                dialogueText.setString(wrapText(dialogueText, fullStringToDisplay, dialogueBoxInnerWidth, dialogueDisplayFont, dialogueText.getCharacterSize()));
                
                // Position text inside dialogue box
                sf::FloatRect dialogueBoxBounds = dialogueBox.getGlobalBounds();
                dialogueText.setPosition(
                    dialogueBoxBounds.left + 20.0f,
                    dialogueBoxBounds.top + 10.0f // Adjusted top padding
                );

                // Position skip prompt text
                sf::FloatRect skipPromptBounds = skipPromptText.getLocalBounds();
                skipPromptText.setPosition(
                    dialogueBoxBounds.left + dialogueBoxBounds.width - skipPromptBounds.width - 20.0f,
                    dialogueBoxBounds.top + dialogueBoxBounds.height - skipPromptBounds.height - 15.0f
                );

                // Auto-advance logic
                if (currentLineComplete && dialogueActive && currentDialogueIndex < dialogueScript.size()) {
                    // Calculate dynamic delay based on the length of the line just completed (tuned again)
                    float baseDelay = 0.3f; // Further reduced base delay
                    float charDelayFactor = 0.01f; // Further reduced per-character delay
                    float dynamicDelay = baseDelay + (dialogueScript[currentDialogueIndex].line.length() * charDelayFactor);
                    
                    if (autoAdvanceClock.getElapsedTime().asSeconds() > dynamicDelay) {
                        currentDialogueIndex++;
                        if (currentDialogueIndex < dialogueScript.size()) {
                            currentCharIndex = 0;
                            currentLineComplete = false;
                            dialogueClock.restart();
                            autoAdvanceClock.restart();
                        } else {
                            dialogueActive = false; // End of dialogue
                            currentShipSpeed = fastShipSpeed; // Speed up automatically
                            skipPromptText.setString(""); // Clear prompt
                        }
                    }
                }
            }


        } else {
            // Check for hover effect on the start button in GUI state
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            bool isHovering = startButtonText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            static bool wasHovering = false; // Keep track of previous hover state

            if (isHovering) {
                startButtonText.setFillColor(sf::Color::Yellow);
                startButtonBorder.setOutlineColor(sf::Color::Yellow);
                if (!wasHovering) { // Play sound only when hover starts
                    menuButtonSound.play();
                }
            } else {
                startButtonText.setFillColor(sf::Color::White);
                startButtonBorder.setOutlineColor(sf::Color::White);
            }
            wasHovering = isHovering; // Update previous hover state
        }

        // Clear screen
        window.clear();

        // Draw elements based on state
        if (!inCutscene) {
            // Draw GUI
            window.setView(window.getDefaultView()); // Use default view for GUI
            window.draw(backgroundSprite);
            window.draw(planetSprite); // Draw planet in GUI state
            window.draw(titleBorder);
            window.draw(titleText);
            window.draw(startButtonBorder);
            window.draw(startButtonText);
        } else {
            // Draw Cutscene elements with camera view
            window.setView(cameraView);
            window.draw(backgroundSprite); // Draw background first
            // backgroundSprite.setTextureRect(sf::IntRect(0, 0, backgroundTexture.getSize().x, backgroundTexture.getSize().y)); // Use full background
            // backgroundSprite position is already set correctly in GUI state
            window.draw(planetSprite);
            window.draw(shipSprite);

            // Draw the dialogue box with the default view
            window.setView(window.getDefaultView());
            if (dialogueActive) { // Only draw box if dialogue is active
                window.draw(dialogueBox);
                window.draw(dialogueText);
                window.draw(skipPromptText);
            }

            // Draw the fade rectangle with the default view to cover the entire window
            // window.setView(window.getDefaultView()); // Already set
            window.draw(fadeRect);
        }

        // Update the window
        window.display();
    }

    return 0;
}